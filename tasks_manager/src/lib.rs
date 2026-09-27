use std::{
    collections::BTreeSet,
    fs,
    io::{ErrorKind, Write},
    path::Path,
};

pub mod manifest;
mod registry;

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum Mode {
    Sync,
    Check,
}

#[derive(Debug, Default)]
pub struct Report {
    pub created: Vec<String>,
    pub added: Vec<String>,
    pub updated: Vec<String>,
    pub removed: Vec<String>,
    pub errors: Vec<String>,
    pub targets: usize,
    pub changed: bool,
}

pub fn run(manifest: &Path, mode: Mode) -> Result<Report, String> {
    let path = manifest
        .canonicalize()
        .map_err(|e| format!("{}: {e}", manifest.display()))?;
    let root = path
        .parent()
        .ok_or("Manifest must have a parent directory")?;
    let text = fs::read_to_string(&path).map_err(|e| format!("{}: {e}", path.display()))?;
    let tasks = manifest::parse(&text)?;
    let registry_path = root.join("tasks.cmake");
    ensure_within(root, &registry_path)?;
    let old = match fs::read_to_string(&registry_path) {
        Ok(text) => Some(text),
        Err(e) if e.kind() == ErrorKind::NotFound => None,
        Err(e) => return Err(format!("{}: {e}", registry_path.display())),
    };
    let mut report = Report::default();
    let mut valid = Vec::new();
    for task in tasks {
        match prepare_task(root, &task, mode, &mut report) {
            Ok(()) => valid.push(task),
            Err(TaskError::Invalid(error)) => report
                .errors
                .push(format!("{}: {error}; target excluded", task.directory)),
            Err(TaskError::Io(error)) => return Err(format!("{}: {error}", task.directory)),
        }
    }
    report.targets = valid.len();
    let new = registry::render(&valid);
    // Git's autocrlf may change generated files on checkout. Line endings alone
    // do not require synchronization; preserve the existing bytes and timestamp.
    report.changed = old.as_ref().map(|s| s.replace("\r\n", "\n")).as_deref() != Some(&new);
    registry::diff(old.as_deref().unwrap_or_default(), &new, &mut report);
    if report.changed {
        if mode == Mode::Check {
            report
                .errors
                .push("tasks.cmake is missing or out of sync; run tasks_manager sync".into());
        } else {
            let mut temp = tempfile::NamedTempFile::new_in(root).map_err(|e| e.to_string())?;
            temp.write_all(new.as_bytes()).map_err(|e| e.to_string())?;
            temp.as_file().sync_all().map_err(|e| e.to_string())?;
            temp.persist(&registry_path)
                .map_err(|e| format!("Cannot replace tasks.cmake: {e}"))?;
        }
    }
    Ok(report)
}

enum TaskError {
    Invalid(String),
    Io(String),
}

fn prepare_task(
    root: &Path,
    task: &manifest::Task,
    mode: Mode,
    report: &mut Report,
) -> Result<(), TaskError> {
    let directory = root.join(&task.directory);
    let entry = directory.join(task.entry());
    let opposite = directory.join(task.opposite_entry());
    ensure_within(root, &opposite).map_err(TaskError::Invalid)?;
    if opposite
        .try_exists()
        .map_err(|e| TaskError::Io(e.to_string()))?
    {
        return Err(TaskError::Invalid(format!(
            "found {}; include_tests={} requires only {} (main.cpp and tests.cpp are mutually exclusive)",
            task.opposite_entry(),
            task.include_tests,
            task.entry()
        )));
    }
    let entry_template = if task.include_tests {
        include_str!("../templates/tests.cpp")
    } else {
        include_str!("../templates/main.cpp")
    };
    let mut files = vec![(entry, entry_template.to_owned())];
    for source in &task.source_files {
        let source = directory.join(source);
        let header = source.with_extension("h");
        // Validated source paths have ASCII filenames; the header is a sibling.
        let header_name = header.file_name().unwrap().to_str().unwrap();
        let source_template = format!("#include \"{header_name}\"\n");
        files.push((source, source_template));
        files.push((header, "#pragma once\n".to_owned()));
    }

    // Validate every output before writing any of this task's templates.
    let planned_paths: BTreeSet<_> = files.iter().map(|(path, _)| path_key(path)).collect();
    for (path, _) in &files {
        for parent in path.ancestors().skip(1).take_while(|p| *p != root) {
            if planned_paths.contains(&path_key(parent)) {
                return Err(TaskError::Invalid(format!(
                    "{} is required as both a file and a directory",
                    parent.display()
                )));
            }
        }
        validate_template_path(root, path)?;
    }
    for (path, content) in &files {
        create_missing_template(root, path, content, mode, report)?;
    }
    Ok(())
}

fn path_key(path: &Path) -> String {
    path.to_string_lossy()
        .replace('\\', "/")
        .to_ascii_lowercase()
}

fn validate_template_path(root: &Path, path: &Path) -> Result<(), TaskError> {
    ensure_within(root, path).map_err(TaskError::Invalid)?;
    for ancestor in path
        .parent()
        .unwrap()
        .ancestors()
        .take_while(|p| *p != root)
    {
        if ancestor.exists() && !ancestor.is_dir() {
            return Err(TaskError::Invalid(format!(
                "{}: expected a directory",
                ancestor.display()
            )));
        }
    }
    if path.exists() && !path.is_file() {
        return Err(TaskError::Invalid(format!(
            "{}: expected a regular file",
            path.display()
        )));
    }
    Ok(())
}

fn create_missing_template(
    root: &Path,
    path: &Path,
    content: &str,
    mode: Mode,
    report: &mut Report,
) -> Result<(), TaskError> {
    if path
        .try_exists()
        .map_err(|e| TaskError::Io(e.to_string()))?
    {
        return Ok(());
    }
    let relative = path
        .strip_prefix(root)
        .unwrap()
        .to_string_lossy()
        .replace('\\', "/");
    if mode == Mode::Check {
        return Err(TaskError::Invalid(format!("missing {relative}")));
    }
    fs::create_dir_all(path.parent().unwrap()).map_err(|e| TaskError::Io(e.to_string()))?;
    let mut file = fs::OpenOptions::new()
        .write(true)
        .create_new(true)
        .open(path)
        .map_err(|e| TaskError::Io(format!("cannot create {}: {e}", path.display())))?;
    file.write_all(content.as_bytes())
        .map_err(|e| TaskError::Io(e.to_string()))?;
    report.created.push(relative);
    Ok(())
}

// Inspect every existing ancestor, including dangling links. Creating directories
// must never follow an existing link outside the manifest's project root.
fn ensure_within(root: &Path, path: &Path) -> Result<(), String> {
    if !path.starts_with(root) {
        return Err(format!("{}: path escapes project root", path.display()));
    }
    for ancestor in path.ancestors().take_while(|p| *p != root) {
        match fs::symlink_metadata(ancestor) {
            Ok(_) => {
                let resolved = ancestor
                    .canonicalize()
                    .map_err(|e| format!("{}: {e}", ancestor.display()))?;
                if !resolved.starts_with(root) {
                    return Err(format!("{}: link escapes project root", ancestor.display()));
                }
            }
            Err(e) if e.kind() == ErrorKind::NotFound => {}
            Err(e) => return Err(format!("{}: {e}", ancestor.display())),
        }
    }
    Ok(())
}

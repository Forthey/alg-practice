use std::{
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
    for ancestor in directory.ancestors().take_while(|p| *p != root) {
        if ancestor.exists() && !ancestor.is_dir() {
            return Err(TaskError::Invalid(format!(
                "{}: expected a directory",
                ancestor.display()
            )));
        }
    }
    for path in std::iter::once(&directory).chain([&entry, &opposite]) {
        ensure_within(root, path).map_err(TaskError::Invalid)?;
    }
    for source in &task.source_files {
        ensure_within(root, &directory.join(source)).map_err(TaskError::Invalid)?;
    }
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
    if !entry
        .try_exists()
        .map_err(|e| TaskError::Io(e.to_string()))?
    {
        if mode == Mode::Check {
            return Err(TaskError::Invalid(format!("missing {}", task.entry())));
        }
        fs::create_dir_all(&directory).map_err(|e| TaskError::Io(e.to_string()))?;
        let content = if task.include_tests {
            include_str!("../templates/tests.cpp")
        } else {
            include_str!("../templates/main.cpp")
        };
        let mut file = fs::OpenOptions::new()
            .write(true)
            .create_new(true)
            .open(&entry)
            .map_err(|e| TaskError::Io(format!("cannot create {}: {e}", entry.display())))?;
        file.write_all(content.as_bytes())
            .map_err(|e| TaskError::Io(e.to_string()))?;
        report
            .created
            .push(format!("{}/{}", task.directory, task.entry()));
    }
    for path in std::iter::once(entry).chain(task.source_files.iter().map(|s| directory.join(s))) {
        if !path.is_file() {
            return Err(TaskError::Invalid(format!(
                "missing source or not a regular file: {}",
                path.display()
            )));
        }
    }
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

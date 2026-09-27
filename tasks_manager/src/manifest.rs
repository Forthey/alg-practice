use std::collections::BTreeSet;

use noyalib::{Mapping, ParserConfig, Value};
use serde::Deserialize;

#[cfg(test)]
mod tests;

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Task {
    pub directory: String,
    pub name: String,
    pub include_tests: bool,
    pub source_files: Vec<String>,
}

impl Task {
    pub fn entry(&self) -> &'static str {
        if self.include_tests {
            "tests.cpp"
        } else {
            "main.cpp"
        }
    }

    pub fn opposite_entry(&self) -> &'static str {
        if self.include_tests {
            "main.cpp"
        } else {
            "tests.cpp"
        }
    }
}

#[derive(Deserialize)]
#[serde(deny_unknown_fields)]
struct Manifest {
    version: u32,
    tasks: Value,
}

pub fn parse(text: &str) -> Result<Vec<Task>, String> {
    let manifest: Manifest = noyalib::from_str_with_config(text, &ParserConfig::strict())
        .map_err(|e| format!("Invalid YAML manifest: {e}"))?;
    // Value mappings normalize scalar keys to strings. Inspect their original
    // tokens so an unquoted 02 cannot silently become directory 2.
    let document = noyalib::cst::parse_document_with_config(text, &ParserConfig::strict())
        .map_err(|e| format!("Invalid YAML manifest: {e}"))?;
    validate_key_types(&document, &manifest.tasks, "tasks")?;
    if manifest.version != 1 {
        return Err(format!(
            "Unsupported manifest version {}; expected 1",
            manifest.version
        ));
    }
    let mut tasks = Vec::new();
    visit_group(&manifest.tasks, "", &mut tasks)?;
    tasks.sort_by(|a, b| a.directory.cmp(&b.directory));
    let mut names = BTreeSet::new();
    for task in &tasks {
        let normalized = task.name.to_ascii_lowercase();
        if [
            "all",
            "all_build",
            "clean",
            "help",
            "test",
            "run_tests",
            "install",
            "preinstall",
            "package",
            "package_source",
            "edit_cache",
            "rebuild_cache",
            "zero_check",
            "list_install_components",
            "codegen",
            "gtest",
            "gtest_main",
            "gmock",
            "gmock_main",
            // cmake_parse_arguments interprets these even in quoted values.
            "name",
            "directory",
            "include_tests",
            "sources",
        ]
        .contains(&normalized.as_str())
        {
            return Err(format!(
                "{}: target name is reserved by CMake or GoogleTest",
                task.name
            ));
        }
        if !names.insert(normalized) {
            return Err(format!(
                "Target name collision: {} ({})",
                task.name, task.directory
            ));
        }
    }
    Ok(tasks)
}

fn validate_key_types(
    document: &noyalib::cst::Document,
    value: &Value,
    path: &str,
) -> Result<(), String> {
    if let Some(fields) = value.as_mapping() {
        for (key, value) in fields {
            validate_component(key).map_err(|e| format!("{path}: invalid key '{key}': {e}"))?;
            let child = format!("{path}['{key}']");
            let (start, end) = document
                .key_span(&child)
                .ok_or_else(|| format!("{child}: expected an explicit string key"))?;
            let token = &document.source()[start..end];
            let scalar: Value = noyalib::from_str(token).map_err(|e| e.to_string())?;
            if !matches!(scalar, Value::String(_)) {
                return Err(format!(
                    "{child}: directory/field keys must be strings; quote '{token}'"
                ));
            }
            validate_key_types(document, value, &child)?;
        }
    }
    Ok(())
}

fn mapping<'a>(value: &'a Value, path: &str) -> Result<&'a Mapping, String> {
    value
        .as_mapping()
        .ok_or_else(|| format!("{path}: expected a mapping"))
}

fn visit_group(value: &Value, parent: &str, tasks: &mut Vec<Task>) -> Result<(), String> {
    let group = mapping(value, if parent.is_empty() { "tasks" } else { parent })?;
    let mut siblings = BTreeSet::new();
    for (key, value) in group {
        validate_component(key).map_err(|e| format!("{parent}/{key}: {e}"))?;
        if !siblings.insert(key.to_ascii_lowercase()) {
            return Err(format!(
                "{parent}: directory names collide ignoring case: {key}"
            ));
        }
        if parent.is_empty()
            && ["build", "tasks_manager", "cmake", "utility", "docs"]
                .contains(&key.to_ascii_lowercase().as_str())
        {
            return Err(format!("{key}: reserved project directory"));
        }
        let directory = if parent.is_empty() {
            key.clone()
        } else {
            format!("{parent}/{key}")
        };
        let fields = mapping(value, &directory)?;
        if fields.contains_key("include_tests") || fields.contains_key("source_files") {
            tasks.push(parse_task(fields, directory)?);
        } else {
            visit_group(value, &directory, tasks)?;
        }
    }
    Ok(())
}

fn parse_task(fields: &Mapping, directory: String) -> Result<Task, String> {
    for key in fields.keys() {
        if key != "include_tests" && key != "source_files" {
            return Err(format!("{directory}: unknown task field '{key}'"));
        }
    }
    let include_tests = fields
        .get("include_tests")
        .and_then(Value::as_bool)
        .ok_or_else(|| format!("{directory}: include_tests must be a boolean"))?;
    let mut source_files = Vec::new();
    let mut seen = BTreeSet::new();
    if let Some(sources) = fields.get("source_files") {
        let sources = sources
            .as_sequence()
            .ok_or_else(|| format!("{directory}: source_files must be a list"))?;
        for source in sources {
            let source = source
                .as_str()
                .ok_or_else(|| format!("{directory}: source_files entries must be strings"))?;
            validate_source(source).map_err(|e| format!("{directory}: {e}"))?;
            if !seen.insert(source.to_ascii_lowercase()) {
                return Err(format!("{directory}: duplicate source '{source}'"));
            }
            source_files.push(source.to_owned());
        }
    }
    source_files.sort();
    Ok(Task {
        name: directory.replace('/', "__"),
        directory,
        include_tests,
        source_files,
    })
}

fn validate_component(component: &str) -> Result<(), String> {
    if component.is_empty()
        || component == "."
        || component == ".."
        || component.starts_with('.')
        || component.ends_with('.')
        || !component
            .bytes()
            .all(|c| c.is_ascii_alphanumeric() || b"_-.".contains(&c))
    {
        return Err("use a relative name containing letters, digits, '_', '-' or '.'".into());
    }
    let stem = component.split('.').next().unwrap().to_ascii_uppercase();
    if ["CON", "PRN", "AUX", "NUL"].contains(&stem.as_str())
        || ((stem.starts_with("COM") || stem.starts_with("LPT"))
            && stem.len() == 4
            && matches!(stem.as_bytes()[3], b'1'..=b'9'))
    {
        return Err("Windows device names are not valid file/directory names".into());
    }
    Ok(())
}

fn validate_source(source: &str) -> Result<(), String> {
    for component in source.split('/') {
        validate_component(component).map_err(|e| format!("invalid source '{source}': {e}"))?;
    }
    if ["main.cpp", "tests.cpp"].contains(&source.to_ascii_lowercase().as_str()) {
        return Err(format!(
            "{source} is selected by include_tests; do not list it in source_files"
        ));
    }
    let extension = source.rsplit('.').next().unwrap_or_default();
    if !["cpp", "cc", "cxx"].contains(&extension) {
        return Err(format!(
            "{source}: expected a C++ source (.cpp, .cc or .cxx)"
        ));
    }
    Ok(())
}

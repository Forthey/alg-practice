use std::{fs, path::Path};
use tasks_manager::{Mode, run};
use tempfile::tempdir;

fn manifest(root: &Path, entries: &str) -> std::path::PathBuf {
    let path = root.join("tasks.yaml");
    fs::write(&path, format!("version: 1\ntasks:\n{entries}\n")).unwrap();
    path
}

#[test]
fn scaffolds_both_modes_and_check_accepts_synced_registry() {
    let tmp = tempdir().unwrap();
    let path = manifest(
        tmp.path(),
        "  plain: {include_tests: false}\n  tested: {include_tests: true}",
    );
    let report = run(&path, Mode::Sync).unwrap();
    assert!(report.errors.is_empty());
    assert_eq!(report.targets, 2);
    assert_eq!(report.created.len(), 2);
    assert!(
        fs::read_to_string(tmp.path().join("plain/main.cpp"))
            .unwrap()
            .contains("int main()")
    );
    let tests = fs::read_to_string(tmp.path().join("tested/tests.cpp")).unwrap();
    assert!(tests.contains("TEST_P("));
    assert!(tests.contains("GTEST_SKIP()"));
    assert!(run(&path, Mode::Check).unwrap().errors.is_empty());
}

#[test]
fn repeated_sync_preserves_user_code_and_registry_timestamp() {
    let tmp = tempdir().unwrap();
    let path = manifest(tmp.path(), "  demo: {include_tests: false}");
    run(&path, Mode::Sync).unwrap();
    let entry = tmp.path().join("demo/main.cpp");
    fs::write(&entry, "// user code\nint main() { return 42; }\n").unwrap();
    let registry = tmp.path().join("tasks.cmake");
    let modified = fs::metadata(&registry).unwrap().modified().unwrap();
    let report = run(&path, Mode::Sync).unwrap();
    assert!(!report.changed);
    assert!(report.created.is_empty());
    assert!(report.added.is_empty());
    assert_eq!(
        fs::metadata(registry).unwrap().modified().unwrap(),
        modified
    );
    assert!(fs::read_to_string(entry).unwrap().contains("return 42"));
}

#[test]
fn windows_checkout_line_endings_do_not_make_registry_stale() {
    let tmp = tempdir().unwrap();
    let path = manifest(tmp.path(), "  demo: {include_tests: false}");
    run(&path, Mode::Sync).unwrap();
    let registry = tmp.path().join("tasks.cmake");
    let crlf = fs::read_to_string(&registry).unwrap().replace('\n', "\r\n");
    fs::write(&registry, &crlf).unwrap();
    let modified = fs::metadata(&registry).unwrap().modified().unwrap();
    assert!(run(&path, Mode::Check).unwrap().errors.is_empty());
    assert!(!run(&path, Mode::Sync).unwrap().changed);
    assert_eq!(fs::read_to_string(&registry).unwrap(), crlf);
    assert_eq!(
        fs::metadata(&registry).unwrap().modified().unwrap(),
        modified
    );
}

#[test]
fn check_is_read_only_and_reports_missing_entry_and_stale_registry() {
    let tmp = tempdir().unwrap();
    let path = manifest(tmp.path(), "  demo: {include_tests: true}");
    let report = run(&path, Mode::Check).unwrap();
    assert!(report.errors.iter().any(|e| e.contains("tests.cpp")));
    assert!(!tmp.path().join("demo").exists());
    assert!(!tmp.path().join("tasks.cmake").exists());
}

#[test]
fn conflict_removes_old_target_but_syncs_other_tasks() {
    let tmp = tempdir().unwrap();
    let path = manifest(tmp.path(), "  bad: {include_tests: true}");
    run(&path, Mode::Sync).unwrap();
    fs::write(tmp.path().join("bad/main.cpp"), "user main").unwrap();
    manifest(
        tmp.path(),
        "  bad: {include_tests: true}\n  good: {include_tests: false}",
    );
    let report = run(&path, Mode::Sync).unwrap();
    assert_eq!(report.targets, 1);
    assert_eq!(report.errors.len(), 1);
    assert_eq!(report.removed, ["bad"]);
    assert_eq!(report.added, ["good"]);
    assert!(tmp.path().join("bad/tests.cpp").exists());
    assert_eq!(
        fs::read_to_string(tmp.path().join("bad/main.cpp")).unwrap(),
        "user main"
    );
}

#[test]
fn opposite_entry_is_not_overwritten_or_given_conflicting_template() {
    let tmp = tempdir().unwrap();
    fs::create_dir(tmp.path().join("a")).unwrap();
    fs::write(tmp.path().join("a/main.cpp"), "original").unwrap();
    let path = manifest(tmp.path(), "  a: {include_tests: true}");
    assert_eq!(run(&path, Mode::Sync).unwrap().errors.len(), 1);
    assert!(!tmp.path().join("a/tests.cpp").exists());
}

#[test]
fn missing_sources_are_not_created_and_invalid_tasks_are_excluded() {
    let tmp = tempdir().unwrap();
    let path = manifest(
        tmp.path(),
        "  a: {include_tests: true, source_files: [solution.cpp]}",
    );
    let report = run(&path, Mode::Sync).unwrap();
    assert_eq!(report.targets, 0);
    assert!(report.errors.iter().any(|e| e.contains("solution.cpp")));
    assert!(!tmp.path().join("a/solution.cpp").exists());
}

#[test]
fn reports_source_changes_and_removals_without_deleting_files() {
    let tmp = tempdir().unwrap();
    let path = manifest(tmp.path(), "  a: {include_tests: false}");
    run(&path, Mode::Sync).unwrap();
    fs::write(tmp.path().join("a/solution.cpp"), "// implementation").unwrap();
    manifest(
        tmp.path(),
        "  a: {include_tests: false, source_files: [solution.cpp]}",
    );
    assert!(!run(&path, Mode::Check).unwrap().errors.is_empty());
    let report = run(&path, Mode::Sync).unwrap();
    assert_eq!(report.updated.len(), 1);
    assert!(report.updated[0].contains("solution.cpp"));
    fs::write(&path, "version: 1\ntasks: {}\n").unwrap();
    assert_eq!(run(&path, Mode::Sync).unwrap().removed, ["a"]);
    assert!(tmp.path().join("a/main.cpp").exists());
}

#[test]
fn malformed_manifest_does_not_change_registry_or_create_directories() {
    let tmp = tempdir().unwrap();
    let path = manifest(tmp.path(), "  a: {include_tests: false}");
    fs::write(tmp.path().join("tasks.cmake"), "keep this").unwrap();
    fs::write(
        &path,
        "version: 1\ntasks:\n  a: {include_tests: false}\n  b: {include_tests: 3}",
    )
    .unwrap();
    assert!(run(&path, Mode::Sync).is_err());
    assert!(!tmp.path().join("a").exists());
    assert_eq!(
        fs::read_to_string(tmp.path().join("tasks.cmake")).unwrap(),
        "keep this"
    );
}

#[test]
fn directories_cannot_masquerade_as_entry_files() {
    let tmp = tempdir().unwrap();
    fs::create_dir_all(tmp.path().join("a/main.cpp")).unwrap();
    let path = manifest(tmp.path(), "  a: {include_tests: false}");
    let report = run(&path, Mode::Sync).unwrap();
    assert_eq!(report.targets, 0);
    assert!(!report.errors.is_empty());
}

#[test]
fn a_file_in_place_of_task_directory_does_not_abort_other_tasks() {
    let tmp = tempdir().unwrap();
    fs::write(tmp.path().join("bad"), "keep this file").unwrap();
    let path = manifest(
        tmp.path(),
        "  bad: {include_tests: false}\n  good: {include_tests: false}",
    );
    let report = run(&path, Mode::Sync).expect("invalid directory is a task error");
    assert_eq!(report.targets, 1);
    assert_eq!(report.errors.len(), 1);
    assert!(tmp.path().join("good/main.cpp").is_file());
    assert_eq!(
        fs::read_to_string(tmp.path().join("bad")).unwrap(),
        "keep this file"
    );
}

fn link_directory(link: &Path, target: &Path) {
    #[cfg(unix)]
    std::os::unix::fs::symlink(target, link).unwrap();
    #[cfg(windows)]
    {
        // Junctions don't require Windows Developer Mode / symlink privilege.
        let output = std::process::Command::new("powershell")
            .args(["-NoProfile", "-NonInteractive", "-Command",
                "New-Item -ItemType Junction -Path $env:TASK_LINK -Target $env:TASK_TARGET -ErrorAction Stop | Out-Null"])
            .env("TASK_LINK", link).env("TASK_TARGET", target).output().unwrap();
        assert!(
            output.status.success(),
            "{}",
            String::from_utf8_lossy(&output.stderr)
        );
    }
}

#[test]
fn directory_links_cannot_create_entry_files_outside_project() {
    let tmp = tempdir().unwrap();
    let outside = tempdir().unwrap();
    link_directory(&tmp.path().join("escape"), outside.path());
    let path = manifest(
        tmp.path(),
        "  escape: {include_tests: false}\n  good: {include_tests: false}",
    );
    let report = run(&path, Mode::Sync).unwrap();
    assert_eq!(report.targets, 1);
    assert!(
        report
            .errors
            .iter()
            .any(|e| e.contains("escapes project root"))
    );
    assert!(!outside.path().join("main.cpp").exists());
}

#[test]
fn source_links_outside_project_exclude_the_task() {
    let tmp = tempdir().unwrap();
    let outside = tempdir().unwrap();
    fs::create_dir(tmp.path().join("a")).unwrap();
    fs::write(outside.path().join("solution.cpp"), "int value = 1;").unwrap();
    link_directory(&tmp.path().join("a/external"), outside.path());
    let path = manifest(
        tmp.path(),
        "  a: {include_tests: true, source_files: [external/solution.cpp]}",
    );
    let report = run(&path, Mode::Sync).unwrap();
    assert_eq!(report.targets, 0);
    assert!(
        report
            .errors
            .iter()
            .any(|e| e.contains("escapes project root"))
    );
}

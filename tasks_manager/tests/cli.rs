use std::{fs, process::Command};
use tempfile::tempdir;

fn cli() -> Command {
    let mut cmd = Command::new(env!("CARGO_BIN_EXE_tasks_manager"));
    cmd.env("NO_COLOR", "1");
    cmd
}

#[test]
fn cli_uses_manifest_directory_even_when_started_elsewhere() {
    let root = tempdir().unwrap();
    let elsewhere = tempdir().unwrap();
    let manifest = root.path().join("tasks.yaml");
    fs::write(
        &manifest,
        "version: 1\ntasks:\n  a: {include_tests: false}\n",
    )
    .unwrap();
    let output = cli()
        .current_dir(elsewhere.path())
        .arg("sync")
        .arg(&manifest)
        .output()
        .unwrap();
    assert!(
        output.status.success(),
        "{}",
        String::from_utf8_lossy(&output.stderr)
    );
    assert!(root.path().join("a/main.cpp").is_file());
    assert!(!elsewhere.path().join("tasks.cmake").exists());
    let stdout = String::from_utf8(output.stdout).unwrap();
    assert!(stdout.contains("CREATE"));
    assert!(!stdout.contains('\x1b'));
    assert!(
        cli()
            .arg("check")
            .arg(manifest)
            .output()
            .unwrap()
            .status
            .success()
    );
}

#[test]
fn cli_returns_failure_for_partial_sync_and_check_errors() {
    let root = tempdir().unwrap();
    let manifest = root.path().join("tasks.yaml");
    fs::create_dir(root.path().join("bad")).unwrap();
    fs::write(root.path().join("bad/main.cpp"), "int main() {}").unwrap();
    fs::write(
        &manifest,
        "version: 1\ntasks:\n  bad: {include_tests: true}\n  good: {include_tests: true}\n",
    )
    .unwrap();
    let output = cli().arg("sync").arg(&manifest).output().unwrap();
    assert_eq!(output.status.code(), Some(1));
    assert!(String::from_utf8_lossy(&output.stderr).contains("bad"));
    assert!(root.path().join("good/tests.cpp").is_file());
    assert_eq!(
        cli()
            .arg("check")
            .arg(manifest)
            .output()
            .unwrap()
            .status
            .code(),
        Some(1)
    );
}

#[test]
fn cli_help_and_invalid_arguments_have_distinct_exit_codes() {
    assert!(cli().arg("--help").output().unwrap().status.success());
    assert_eq!(cli().arg("delete").output().unwrap().status.code(), Some(2));
    assert_eq!(cli().output().unwrap().status.code(), Some(2));
}

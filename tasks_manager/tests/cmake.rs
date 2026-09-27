use std::{fs, path::Path, process::Command};
use tasks_manager::{Mode, run};
use tempfile::tempdir;

fn checked(command: &mut Command) -> String {
    let output = command
        .output()
        .expect("CMake, Ninja and a C++23 compiler must be installed");
    assert!(
        output.status.success(),
        "{command:?}\n{}\n{}",
        String::from_utf8_lossy(&output.stdout),
        String::from_utf8_lossy(&output.stderr)
    );
    String::from_utf8_lossy(&output.stdout).into_owned()
}

fn configure(root: &Path) {
    let mut command = Command::new("cmake");
    command
        .args(["-S"])
        .arg(root)
        .arg("-B")
        .arg(root.join("build"))
        .args(["-G", "Ninja"]);
    if let Some(source) = std::env::var_os("TASKS_GTEST_SOURCE") {
        command.arg(format!(
            "-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST={}",
            Path::new(&source).display()
        ));
    }
    checked(&mut command);
}

#[test]
#[ignore = "requires CMake, Ninja, C++23, and GoogleTest download or TASKS_GTEST_SOURCE"]
fn new_manifest_tasks_build_and_discover_parameterized_cases() {
    let tmp = tempdir().unwrap();
    let root = tmp.path();
    fs::create_dir(root.join("cmake")).unwrap();
    fs::write(
        root.join("cmake/AddTask.cmake"),
        include_str!("../../cmake/AddTask.cmake"),
    )
    .unwrap();
    fs::write(
        root.join("install_googletest.cmake"),
        include_str!("../../install_googletest.cmake"),
    )
    .unwrap();
    fs::write(
        root.join("CMakeLists.txt"),
        include_str!("../../CMakeLists.txt"),
    )
    .unwrap();
    let manifest = root.join("tasks.yaml");
    fs::write(&manifest, "version: 1\ntasks:\n  examples:\n    plain: {include_tests: false, source_files: [helpers/start.cpp, helpers/support.cc]}\n    tested: {include_tests: true, source_files: [solution.cpp]}\n").unwrap();
    assert!(run(&manifest, Mode::Sync).unwrap().errors.is_empty());
    configure(root);
    checked(
        Command::new("cmake")
            .arg("--build")
            .arg(root.join("build"))
            .args(["--parallel", "4"]),
    );
    let exe = root
        .join("build")
        .join(format!("examples__plain{}", std::env::consts::EXE_SUFFIX));
    checked(&mut Command::new(exe));
    let output = checked(
        Command::new("ctest")
            .arg("--test-dir")
            .arg(root.join("build"))
            .arg("--output-on-failure"),
    );
    assert!(
        output.contains("Skipped"),
        "new test template must not claim to test a solution: {output}"
    );

    // Populate a real solution and verify that source_files reaches the linker.
    fs::write(
        root.join("examples/tested/solution.h"),
        "#pragma once\nint twice(int);\n",
    )
    .unwrap();
    fs::write(
        root.join("examples/tested/solution.cpp"),
        "#include \"solution.h\"\nint twice(int x) { return 2 * x; }\n",
    )
    .unwrap();
    fs::write(
        root.join("examples/tested/tests.cpp"),
        r#"
#include <gtest/gtest.h>
#include "solution.h"
struct Case { int input; int expected; };
const Case cases[] = {{2, 4}, {-3, -6}};
using TwiceTest = testing::TestWithParam<Case>;
TEST_P(TwiceTest, ReturnsExpected) { auto c = GetParam(); EXPECT_EQ(c.expected, twice(c.input)); }
INSTANTIATE_TEST_SUITE_P(Examples, TwiceTest, testing::ValuesIn(cases));
"#,
    )
    .unwrap();
    fs::write(&manifest, "version: 1\ntasks:\n  examples:\n    plain: {include_tests: false, source_files: [helpers/start.cpp, helpers/support.cc]}\n    tested: {include_tests: true, source_files: [solution.cpp, extra.cxx]}\n").unwrap();
    let report = run(&manifest, Mode::Sync).unwrap();
    assert!(report.errors.is_empty());
    assert_eq!(report.updated.len(), 1);
    checked(
        Command::new("cmake")
            .arg("--build")
            .arg(root.join("build"))
            .args(["--parallel", "4"]),
    );
    let list = checked(
        Command::new("ctest")
            .arg("--test-dir")
            .arg(root.join("build"))
            .arg("-N"),
    );
    assert!(list.contains("Total Tests: 2"), "{list}");
    let output = checked(
        Command::new("ctest")
            .arg("--test-dir")
            .arg(root.join("build"))
            .arg("--output-on-failure"),
    );
    assert!(!output.contains("Skipped"));
    assert!(run(&manifest, Mode::Check).unwrap().errors.is_empty());
}

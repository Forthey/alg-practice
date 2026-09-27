use super::parse;

#[test]
fn reads_tree_and_defaults_sources_without_losing_numeric_directory_names() {
    let tasks = parse("version: 1\ntasks:\n  itmo:\n    '02':\n      sum:\n        include_tests: true\n        source_files: [solution.cpp]\n  demo:\n    include_tests: false\n").unwrap();
    assert_eq!(tasks.len(), 2);
    let sum = tasks.iter().find(|t| t.directory == "itmo/02/sum").unwrap();
    assert_eq!(sum.name, "itmo__02__sum");
    assert!(sum.include_tests);
    assert_eq!(sum.source_files, ["solution.cpp"]);
    assert!(
        tasks
            .iter()
            .find(|t| t.directory == "demo")
            .unwrap()
            .source_files
            .is_empty()
    );
}

#[test]
fn rejects_invalid_manifest_shapes_and_duplicate_keys() {
    for yaml in [
        "version: 2\ntasks: {}",
        "version: 1\ntask: {}",
        "version: 1\ntasks: []",
        "version: 1\ntasks:\n  a:\n    include_tests: 'true'",
        "version: 1\ntasks:\n  a:\n    include_tests: true\n    typo: false",
        "version: 1\ntasks:\n  a:\n    source_files: [a.cpp]",
        "version: 1\ntasks:\n  02:\n    include_tests: true",
        "version: 1\ntasks:\n  a:\n    include_tests: true\n    source_files: a.cpp",
        "version: 1\ntasks:\n  a:\n    include_tests: true\n    include_tests: false",
        "version: 1\ntasks:\n  a: {include_tests: true}\n  a: {include_tests: false}",
    ] {
        assert!(parse(yaml).is_err(), "accepted: {yaml}");
    }
}

#[test]
fn rejects_entry_files_duplicates_and_unsafe_source_paths() {
    for source in [
        "main.cpp",
        "tests.cpp",
        "MAIN.CPP",
        "../a.cpp",
        "/a.cpp",
        "C:/a.cpp",
        "a;bad.cpp",
        "${oops}.cpp",
        "a.txt",
        "foo//a.cpp",
        "foo/./a.cpp",
        "foo\\a.cpp",
    ] {
        let yaml = format!(
            "version: 1\ntasks:\n  a:\n    include_tests: true\n    source_files: ['{source}']"
        );
        assert!(parse(&yaml).is_err(), "accepted: {source}");
    }
    assert!(
        parse(
            "version: 1\ntasks:\n  a:\n    include_tests: true\n    source_files: [a.cpp, A.cpp]"
        )
        .is_err()
    );
}

#[test]
fn rejects_unsafe_directory_names_and_colliding_targets() {
    for name in [
        "..",
        "a/b",
        "a;b",
        "CON",
        "nul",
        "a.",
        "a b",
        "build",
        "tasks_manager",
        "cmake",
        "utility",
    ] {
        let yaml = format!("version: 1\ntasks:\n  '{name}':\n    include_tests: false");
        assert!(parse(&yaml).is_err(), "accepted: {name}");
    }
    for yaml in [
        "version: 1\ntasks:\n  a__b: {include_tests: false}\n  a:\n    b: {include_tests: true}",
        "version: 1\ntasks:\n  A: {include_tests: false}\n  a: {include_tests: true}",
        "version: 1\ntasks:\n  A:\n    x: {include_tests: false}\n  a:\n    y: {include_tests: true}",
    ] {
        assert!(parse(yaml).is_err());
    }
}

#[test]
fn empty_catalog_is_valid_for_removing_last_target() {
    assert!(parse("version: 1\ntasks: {}").unwrap().is_empty());
}

#[test]
fn rejects_cmake_and_googletest_reserved_target_names() {
    for name in [
        "all",
        "test",
        "help",
        "install",
        "clean",
        "gtest",
        "gtest_main",
        "gmock",
        "gmock_main",
        "ALL_BUILD",
        "ZERO_CHECK",
    ] {
        let yaml = format!("version: 1\ntasks:\n  {name}: {{include_tests: false}}");
        assert!(parse(&yaml).is_err(), "CMake cannot create target {name}");
    }
}

#[test]
fn rejects_names_that_cmake_would_parse_as_helper_keywords() {
    for name in ["NAME", "DIRECTORY", "INCLUDE_TESTS", "SOURCES"] {
        let yaml = format!("version: 1\ntasks:\n  {name}: {{include_tests: false}}");
        assert!(
            parse(&yaml).is_err(),
            "CMake interprets {name} as a keyword"
        );
    }
}

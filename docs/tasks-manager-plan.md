# Tasks Manager Implementation Plan

> For agentic workers: use superpowers:executing-plans, with final independent review.

Goal: replace recursive CMake files and custom testing with a YAML-managed registry.
Architecture: parse and validate manifest, inspect filesystem, create missing
templates, render deterministic declarations, report semantic differences.
Tech stack: Rust, serde/noyalib, owo-colors, tempfile, CMake 3.22, C++23, GoogleTest.
Spec: `docs/tasks-manager-design.md`.

Global constraints: preserve existing solution code and user changes; no generated
entry overwrites; no algorithm changes except proven defects affecting acceptance.

Review focus: duplicate YAML keys, path/target collisions, symlink escapes,
partial sync errors retaining stale targets, byte-preserving repeated sync.

## Task 1: Rust manager
- [x] Create Cargo package and contract tests with failing API stubs.
- [x] `manifest.rs`: parse(&str) -> Result<Vec<Task>, String>, recursive mappings,
      strict leaf schema, source/name/path validation and collision detection.
- [x] `templates/`: commented main and skipped parameterized GoogleTest template.
- [x] `registry.rs`: deterministic render and semantic ADD/UPDATE/REMOVE diff.
- [x] `lib.rs`: run(path, Mode) -> Result<Report, String>; real file validation,
      safe create-new and atomic registry replacement, check with no writes.
- [x] `main.rs`: CLI arguments, colors, summaries, error exit codes.
- [x] Tests: parsing, invalid schemas/paths, conflict/missing sources, partial sync,
      scaffold both modes, preserve edits, diff/remove, repeat sync and check.
- [x] Run cargo test, fmt, clippy and build.

## Task 2: Migrate project
- [x] Fill manifest with all 18 remaining tasks.
- [x] Extract legacy custom harness solutions and preserve parameter tables in tests.
- [x] Rename existing GoogleTest files and homework implementation files.
- [x] Give hw2_task5 a public declaration and meaningful parameterized tests.
- [x] Add CMake helper, replace root traversal, remove nested CMakeLists and old helpers.
- [x] Sync/check registry and configure/build/CTest.

## Task 3: End-to-end and review
- [x] Test generated main and GoogleTest targets in temporary project via CMake/CTest.
- [x] Document commands and semantics, verify no nested project CMakeLists remain.
- [x] Independent review, address important findings with regressions.
- [x] Final cargo test/clippy/fmt, sync/check, C++ build/CTest, report evidence.

Execution notes: user explicitly authorized implementation of the agreed design;
no further planning approval gate. Work in the current dirty checkout to preserve
and build on supplied changes; do not stage/commit unrelated user work. Rust is
installed at `%USERPROFILE%/.cargo/bin` but absent from the inherited PATH.

# Tasks manager design

Approved in conversation on 2026-09-27. The manifest is the source of truth.

- Rust CLI in `tasks_manager/`, using noyalib/serde and owo-colors.
- `tasks.yaml`: version 1, recursive `tasks` mapping; string directory keys;
  task leaves require `include_tests: bool`, optional `source_files: [string]`.
- Sources are relative C++ compilation units, excluding main.cpp/tests.cpp.
- `sync` creates missing directories and entry templates, validates individual
  tasks, and generates `tasks.cmake` from valid tasks. Conflicting entry files
  exclude a task. Missing declared sources exclude a task, but aren't created.
- `check` writes nothing and reports invalid tasks and stale generated output.
- Invalid manifest structure aborts before any writes. Task errors allow other
  tasks to sync but return a nonzero exit code. Filesystem write failures abort.
- No deletion of solution files, no overwriting existing entry files. Removed
  manifest tasks lose only their CMake declarations. Repeat sync is a no-op.
- `tasks.cmake` is generated; `cmake/AddTask.cmake` implements target setup.
  One executable per task, GoogleTest discovery prefixed by target name.
- New test templates contain parameterized, explicitly skipped placeholders.
- Migrate the 18 currently present tasks, custom TestSuite to GoogleTest;
  remove nested project CMakeLists and obsolete build helpers.
- Keep the user's existing deletion of f3_find_sub_matrix_min.
- Paths must stay within the project, including after symlink resolution;
  reject ambiguous/colliding Windows paths and target names.
- Sync is explicit, before CMake configuration. Rust is not invoked by CMake.

Acceptance: cargo build/test/clippy/fmt; sync and check the real manifest;
configure/build all C++ tasks and run CTest; exercise newly scaffolded main
and parameterized-test tasks in a temporary project.

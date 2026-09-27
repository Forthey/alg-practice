use std::{env, path::PathBuf, process::ExitCode};

use owo_colors::{OwoColorize, Stream};
use tasks_manager::{Mode, run};

fn line(label: &str, message: &str) {
    let label = format!("{label:>7}");
    println!(
        "{}  {message}",
        label.if_supports_color(Stream::Stdout, |s| s.cyan())
    );
}

fn main() -> ExitCode {
    let mut args = env::args_os().skip(1);
    let Some(command) = args.next() else {
        eprintln!("Usage: tasks_manager <sync|check> [tasks.yaml]");
        return ExitCode::from(2);
    };
    if command == "--help" || command == "-h" {
        println!(
            "Usage: tasks_manager <sync|check> [tasks.yaml]\n\n  sync   Create missing entry templates and synchronize tasks.cmake\n  check  Validate files and registry without making changes\n\nPaths are relative to the manifest directory. Removed targets keep their files."
        );
        return ExitCode::SUCCESS;
    }
    let mode = match command.to_str() {
        Some("sync") => Mode::Sync,
        Some("check") => Mode::Check,
        _ => {
            eprintln!(
                "Unknown command: {}. Expected sync or check.",
                command.to_string_lossy()
            );
            return ExitCode::from(2);
        }
    };
    let path = args
        .next()
        .map(PathBuf::from)
        .unwrap_or_else(|| "tasks.yaml".into());
    if args.next().is_some() {
        eprintln!("Too many arguments. Usage: tasks_manager <sync|check> [tasks.yaml]");
        return ExitCode::from(2);
    }
    match run(&path, mode) {
        Err(error) => {
            eprintln!(
                "{}  {error}",
                "ERROR".if_supports_color(Stream::Stderr, |s| s.red())
            );
            ExitCode::FAILURE
        }
        Ok(report) => {
            if mode == Mode::Check && report.changed {
                line(
                    "STALE",
                    "Required registry changes (check makes no changes):",
                );
            }
            for (label, entries) in [
                ("CREATE", &report.created),
                ("ADD", &report.added),
                ("UPDATE", &report.updated),
                ("REMOVE", &report.removed),
            ] {
                for entry in entries {
                    line(label, entry);
                }
            }
            for error in &report.errors {
                eprintln!(
                    "{}  {error}",
                    "ERROR".if_supports_color(Stream::Stderr, |s| s.red())
                );
            }
            println!(
                "\n{} targets | +{} ~{} -{} | {} files created | {} errors",
                report.targets,
                report.added.len(),
                report.updated.len(),
                report.removed.len(),
                report.created.len(),
                report.errors.len()
            );
            if report.errors.is_empty() {
                line(
                    "OK",
                    if mode == Mode::Check {
                        "Manifest and registry are valid."
                    } else if report.changed {
                        "tasks.cmake synchronized."
                    } else {
                        "Already synchronized."
                    },
                );
                ExitCode::SUCCESS
            } else {
                ExitCode::FAILURE
            }
        }
    }
}

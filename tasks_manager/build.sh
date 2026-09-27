#!/usr/bin/env bash
set -euo pipefail

profile="${1:-release}"
if [[ $# -gt 1 || ( "$profile" != release && "$profile" != debug ) ]]; then
    printf 'Usage: bash tasks_manager/build.sh [release|debug]\n' >&2
    exit 2
fi

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
project_root="$(dirname -- "$script_dir")"

if command -v cargo >/dev/null 2>&1; then
    cargo_path="$(command -v cargo)"
else
    cargo_path="${CARGO_HOME:-$HOME/.cargo}/bin/cargo"
    if [[ ! -x "$cargo_path" ]]; then
        printf 'Cargo not found. Install Rust or add its bin directory to PATH.\n' >&2
        exit 1
    fi
fi

# Read Cargo's JSON instead of assuming a target directory or target triple.
python_path=''
for candidate in python3 python; do
    if command -v "$candidate" >/dev/null 2>&1 &&
        "$candidate" -c 'import sys; sys.exit(sys.version_info.major != 3)' >/dev/null 2>&1; then
        python_path="$(command -v "$candidate")"
        break
    fi
done
if [[ -z "$python_path" ]]; then
    printf 'Python 3 not found. It is required to read Cargo build output.\n' >&2
    exit 1
fi

cargo_args=(build --locked --bin tasks_manager
    --manifest-path "$script_dir/Cargo.toml"
    --message-format=json-render-diagnostics)
if [[ "$profile" == release ]]; then
    cargo_args+=(--release)
fi

messages="$(mktemp)"
trap 'rm -f -- "$messages"' EXIT
"$cargo_path" "${cargo_args[@]}" > "$messages"

artifact="$("$python_path" -c '
import json
import sys

artifact = None
with open(sys.argv[1], encoding="utf-8") as stream:
    for line in stream:
        message = json.loads(line)
        if (message.get("reason") == "compiler-artifact"
                and message.get("target", {}).get("name") == "tasks_manager"
                and message.get("executable")):
            artifact = message["executable"]
if artifact is None:
    sys.exit("Cargo completed without reporting the tasks_manager executable.")
print(artifact)
' "$messages")"
# Native Windows Cargo/Python can also be used from Git Bash.
artifact="${artifact%$'\r'}"
if command -v cygpath >/dev/null 2>&1; then
    artifact="$(cygpath -u "$artifact")"
fi

filename=tasks-manager
if [[ "$artifact" == *.exe ]]; then
    filename=tasks_manager.exe
fi
destination="$project_root/$filename"
cp -- "$artifact" "$destination"
chmod +x -- "$destination"
printf 'Ready: %s\n' "$destination"

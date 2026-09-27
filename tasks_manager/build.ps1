param(
    [ValidateSet('release', 'debug')]
    [string]$Profile = 'release'
)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$cargoCommand = Get-Command cargo -CommandType Application -ErrorAction SilentlyContinue
if ($cargoCommand) {
    $cargoPath = $cargoCommand.Source
} else {
    $cargoRoot = if ($env:CARGO_HOME) { $env:CARGO_HOME } else { Join-Path $env:USERPROFILE '.cargo' }
    $cargoPath = Join-Path $cargoRoot 'bin/cargo.exe'
    if (-not (Test-Path -LiteralPath $cargoPath -PathType Leaf)) {
        throw 'Cargo not found. Install Rust or add its bin directory to PATH.'
    }
}

$cargoArgs = @(
    'build', '--locked', '--bin', 'tasks_manager',
    '--manifest-path', (Join-Path $PSScriptRoot 'Cargo.toml'),
    '--message-format=json-render-diagnostics'
)
if ($Profile -eq 'release') {
    $cargoArgs += '--release'
}

# Cargo reports the actual executable path, including custom target directories.
$messages = @(& $cargoPath @cargoArgs)
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
$artifact = $messages | ForEach-Object { $_ | ConvertFrom-Json } |
    Where-Object { $_.reason -eq 'compiler-artifact' -and $_.target.name -eq 'tasks_manager' -and $_.executable } |
    Select-Object -Last 1
if (-not $artifact) {
    throw 'Cargo completed without reporting the tasks_manager executable.'
}

# The Unix name must differ from the existing tasks_manager/ directory.
$filename = if ([System.IO.Path]::GetExtension($artifact.executable) -eq '.exe') { 'tasks_manager.exe' } else { 'tasks-manager' }
$destination = Join-Path $projectRoot $filename
Copy-Item -LiteralPath $artifact.executable -Destination $destination -Force
Write-Host "Ready: $destination"

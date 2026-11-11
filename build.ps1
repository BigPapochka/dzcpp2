$ErrorActionPreference = 'Stop'
$taskCompilerCommand = Get-Command g++.exe -ErrorAction SilentlyContinue
$taskCompilerPath = $null
if ($taskCompilerCommand) {
    $taskCompilerPath = $taskCompilerCommand.Source
} else {
    $taskPackageRoot = Join-Path $env:LOCALAPPDATA 'Microsoft\WinGet\Packages'
    if (Test-Path -LiteralPath $taskPackageRoot) {
        $taskCompilerPath = Get-ChildItem -LiteralPath $taskPackageRoot -Directory -Filter 'BrechtSanders.WinLibs*' |
            Sort-Object LastWriteTime -Descending |
            ForEach-Object {
                $taskCandidate = Join-Path $_.FullName 'mingw64\bin\g++.exe'
                if (Test-Path -LiteralPath $taskCandidate) { $taskCandidate }
            } | Select-Object -First 1
    }
}
if (-not $taskCompilerPath) {
    throw 'g++ was not found. Install a C++17 compiler or add it to PATH.'
}
Push-Location -LiteralPath $PSScriptRoot
try {
    & $taskCompilerPath -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Werror main.cpp -o main.exe
    if ($LASTEXITCODE -ne 0) { throw 'Compilation failed.' }
} finally {
    Pop-Location
}
Write-Output ("Built: " + (Join-Path $PSScriptRoot 'main.exe'))

# PowerShell script to run semantic tests
# Usage: .\run_tests.ps1

$compiler = "..\..\..\cmake-build-debug\scala_lexer.exe"
$testsDir = $PSScriptRoot

Write-Host "=== Running Semantic Tests ===" -ForegroundColor Cyan
Write-Host ""

# Valid tests (should have no errors)
Write-Host "--- Valid Tests ---" -ForegroundColor Green

$validTests = Get-ChildItem -Path "$testsDir\valid\*.scala"
foreach ($test in $validTests) {
    $name = $test.Name
    Write-Host -NoNewline "Testing $name... "

    $output = & $compiler $test.FullName 2>&1

    if ($output -match "Error") {
        Write-Host "FAILED (unexpected errors)" -ForegroundColor Red
        Write-Host $output -ForegroundColor Yellow
    } else {
        Write-Host "PASSED" -ForegroundColor Green
    }
}

Write-Host ""

# Error tests (should have expected errors)
Write-Host "--- Error Tests ---" -ForegroundColor Yellow

$errorTests = Get-ChildItem -Path "$testsDir\errors\*.scala"
foreach ($test in $errorTests) {
    $name = $test.BaseName
    $expectedFile = "$testsDir\expected\$name.txt"

    Write-Host -NoNewline "Testing $name... "

    $output = & $compiler $test.FullName 2>&1

    if (Test-Path $expectedFile) {
        $expected = Get-Content $expectedFile -Raw
        if ($output -match "Error") {
            Write-Host "PASSED (errors detected)" -ForegroundColor Green
        } else {
            Write-Host "FAILED (expected errors but none found)" -ForegroundColor Red
        }
    } else {
        if ($output -match "Error") {
            Write-Host "OK (errors detected, no expected file)" -ForegroundColor Yellow
            Write-Host "  Output: $output" -ForegroundColor Gray
        } else {
            Write-Host "FAILED (expected errors)" -ForegroundColor Red
        }
    }
}

Write-Host ""
Write-Host "=== Tests Complete ===" -ForegroundColor Cyan

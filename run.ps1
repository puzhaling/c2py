# Script to run c2py translator with interactive file selection
Write-Host "=== C to Python Translator ===" -ForegroundColor Cyan
Write-Host ""

# Get path to executable
$buildDir = ".\build"
$exeName = "c2py.exe"
$exePath = Join-Path $buildDir $exeName

# Check if executable exists
if (-not (Test-Path $exePath)) {
    Write-Host "Compiling project..." -ForegroundColor Yellow
    
    # Create build directory
    if (-not (Test-Path $buildDir)) {
        New-Item -ItemType Directory -Path $buildDir | Out-Null
    }
    
    # Source files to compile
    $srcs = @(
        "src/expr_translator.cpp",
        "src/lexer.cpp",
        "src/parser.cpp",
        "src/ast.cpp",
        "src/code_generator.cpp",
        "src/semantic.cpp",
        "src/symbol_table.cc",
        "src/main.cpp"
    )
    
    $cppflags = @("-iquote", "include", "-g", "-Wall", "-Wextra")
    
    # Compile source files
    $compileCmd = @("c++") + $cppflags + $srcs + @("-o", $exePath)
    
    & $compileCmd[0] @($compileCmd[1..($compileCmd.Length-1)])
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Compilation error!" -ForegroundColor Red
        exit 1
    }
    
    Write-Host "Compilation successful!" -ForegroundColor Green
    Write-Host ""
}

# Find all C files in examples/
$examplesDir = ".\examples"
if (-not (Test-Path $examplesDir)) {
    Write-Host "Directory ./examples not found!" -ForegroundColor Red
    exit 1
}

$cFiles = Get-ChildItem -Path $examplesDir -Filter "*.c" | Select-Object -ExpandProperty Name

if ($cFiles.Count -eq 0) {
    Write-Host "No C files found in ./examples/" -ForegroundColor Red
    exit 1
}
# File selection menu
Write-Host "Select a file to translate:" -ForegroundColor Green
Write-Host ""

# Show menu
$index = 1
$cFiles | ForEach-Object {
    Write-Host "[$index] $_" -ForegroundColor Cyan
    $index++
}

Write-Host "[0] Exit" -ForegroundColor Yellow
Write-Host ""
$choice = Read-Host "Enter file number"

# Check for exit
if ($choice -eq "0") {
    Write-Host "Goodbye!" -ForegroundColor Green
    exit 0
}

# Validate choice
if (-not ($choice -match '^\d+$') -or ([int]$choice -lt 1 -or [int]$choice -gt $cFiles.Count)) {
    Write-Host "Invalid choice!" -ForegroundColor Red
    exit 1
}

$selectedFile = $cFiles[[int]$choice - 1]
$inputPath = Join-Path $examplesDir $selectedFile

Write-Host ""
Write-Host "Translating file: $selectedFile" -ForegroundColor Yellow
Write-Host ""

# Run translator
& $exePath $inputPath

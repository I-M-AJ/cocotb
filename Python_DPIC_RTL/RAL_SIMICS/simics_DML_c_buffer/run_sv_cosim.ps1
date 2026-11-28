$ErrorActionPreference = "Stop"
$projDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$svDir = Join-Path $projDir "sv"
$previousName = $env:SIMICS_SV_BUFFER_NAME
$env:SIMICS_SV_BUFFER_NAME = "Local\simics_sv_$([guid]::NewGuid().ToString('N'))"
$vsimProc = $null

try {
    Write-Host "=== Building Simics device ==="
    cmake --build "$projDir\build" --target adder
    if ($LASTEXITCODE -ne 0) { throw "Simics device build failed" }

    Write-Host "=== Compiling SystemVerilog and DPI-C ==="
    Set-Location $svDir
    if (-not (Test-Path "$svDir\work")) {
        vlib work
        if ($LASTEXITCODE -ne 0) { throw "vlib failed" }
    }
    vlog -sv -dpiheader "$svDir\dpi_generated.h" adder.sv tb_top.sv dpi_sv_server.c
    if ($LASTEXITCODE -ne 0) { throw "vlog compilation failed" }

    $vsimProc = Start-Process -FilePath "vsim.exe" `
        -ArgumentList "-c", "-voptargs=+acc", "tb_top", "-do", "`"run -all; quit -f`"" `
        -WorkingDirectory $svDir -PassThru -NoNewWindow
    $null = $vsimProc.Handle

    $ready = $false
    for ($i = 0; $i -lt 100; $i++) {
        if ($vsimProc.HasExited) { throw "QuestaSim exited before the bridge started" }
        try {
            $map = [System.IO.MemoryMappedFiles.MemoryMappedFile]::OpenExisting($env:SIMICS_SV_BUFFER_NAME)
            $view = $map.CreateViewAccessor(0, 4)
            $ready = $view.ReadUInt32(0) -eq 0x53564231
            $view.Dispose()
            $map.Dispose()
            if ($ready) { break }
        } catch [System.IO.FileNotFoundException] {}
        Start-Sleep -Milliseconds 100
    }
    if (-not $ready) { throw "Timed out waiting for the SV shared-memory bridge" }

    Set-Location $projDir
    Write-Host "=== Running Simics buffer tests ==="
    & "$projDir\bin\test-runner.bat" --project-only
    if ($LASTEXITCODE -ne 0) { throw "Simics buffer tests failed" }

    Write-Host "=== Running Simics Python example ==="
    & "$projDir\simics.bat" --batch-mode --no-win "$projDir\run_adder_sv_cli.simics"
    if ($LASTEXITCODE -ne 0) { throw "Simics Python example failed" }
    if (-not $vsimProc.WaitForExit(10000)) { throw "QuestaSim did not stop after shutdown" }
    if ($vsimProc.ExitCode -ne 0) { throw "QuestaSim failed with exit code $($vsimProc.ExitCode)" }
}
finally {
    if ($vsimProc -and -not $vsimProc.HasExited) {
        Stop-Process -Id $vsimProc.Id
    }
    $env:SIMICS_SV_BUFFER_NAME = $previousName
    Set-Location $projDir
}

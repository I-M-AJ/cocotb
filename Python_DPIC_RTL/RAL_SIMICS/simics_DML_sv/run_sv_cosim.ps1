$ErrorActionPreference = "Stop"
$projDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$svDir = Join-Path $projDir "sv"
$port = if ($env:SIMICS_SV_PORT) { [int]$env:SIMICS_SV_PORT } else { 5555 }

Write-Host "=== 1. Compiling SystemVerilog RTL + DPI-C server in QuestaSim ==="
Set-Location $svDir
if (-not (Test-Path "$svDir\work")) {
    vlib work
}
vlog -sv -dpiheader "$svDir\dpi_generated.h" adder.sv tb_top.sv dpi_sv_server.c
if ($LASTEXITCODE -ne 0) { throw "vlog compilation failed" }

Write-Host "=== 2. Starting QuestaSim SV server on 127.0.0.1:$port ==="
$vsimProc = Start-Process -FilePath "vsim.exe" `
    -ArgumentList "-c", "-voptargs=+acc", "-wlf", "waves.wlf", "-ldflags", "-lws2_32", "tb_top", "-do", "`"log -r /*; run -all; quit -f`"" `
    -WorkingDirectory $svDir `
    -PassThru -NoNewWindow

try {
    # Wait for TCP port to open (up to 15 seconds)
    $connected = $false
    for ($i = 0; $i -lt 30; $i++) {
        try {
            $tcp = New-Object System.Net.Sockets.TcpClient("127.0.0.1", $port)
            $stream = $tcp.GetStream()
            $writer = New-Object System.IO.StreamWriter($stream)
            $writer.WriteLine("QUIT")
            $writer.Flush()
            $tcp.Close()
            $connected = $true
            break
        } catch {
            Start-Sleep -Milliseconds 500
        }
    }
    if (-not $connected) { throw "Timed out waiting for QuestaSim DPI-C server on port $port" }

    Set-Location $projDir
    Write-Host "=== 3. Running Simics unit tests (test-runner.bat) ==="
    & "$projDir\bin\test-runner.bat"
    if ($LASTEXITCODE -ne 0) { throw "Simics test-runner failed" }

    Write-Host "=== 4. Running Simics CLI script (run_adder_sv_cli.simics) ==="
    & "$projDir\simics.bat" --batch-mode --no-win "$projDir\run_adder_sv_cli.simics"
    if ($LASTEXITCODE -ne 0) { throw "Simics CLI run failed" }
}
finally {
    Write-Host "=== 5. Shutting down QuestaSim SV server ==="
    try {
        $tcp = New-Object System.Net.Sockets.TcpClient("127.0.0.1", $port)
        $stream = $tcp.GetStream()
        $writer = New-Object System.IO.StreamWriter($stream)
        $writer.WriteLine("SHUTDOWN")
        $writer.Flush()
        $tcp.Close()
    } catch {}
    if ($vsimProc -and -not $vsimProc.HasExited) {
        $vsimProc.WaitForExit(5000) | Out-Null
        if (-not $vsimProc.HasExited) { $vsimProc.Kill() }
    }
    Set-Location $projDir
}

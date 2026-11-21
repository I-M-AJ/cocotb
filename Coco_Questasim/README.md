# QuestaSim cocotb example

An 8-bit synchronous counter with reset and enable. The cocotb test checks
reset priority, counting, holding the value, and wraparound.

## Run on Windows 11

Use 64-bit Python 3.9-3.13, a matching 64-bit QuestaSim installation, and a
valid simulator license. Python 3.15 is not supported by cocotb 2.0. In
PowerShell, from this directory:

```powershell
py -3.12 -m venv .venv
.\.venv\Scripts\python.exe -m pip install -r requirements.txt
.\.venv\Scripts\python.exe run_sim.py
```

Replace `-3.12` with an installed, supported Python version if necessary.
If Python 3.12 is not installed, install it first, or use `uv` to download it
and set up the environment:

```powershell
uv venv --python 3.12 .venv
uv pip install --python .\.venv\Scripts\python.exe -r requirements.txt
.\.venv\Scripts\python.exe run_sim.py
```

Ensure QuestaSim's `win64` directory is on `PATH`, so `vsim`, `vlog`, and
`vlib` can be found. For example, for the default installation on this machine:

```powershell
$env:PATH = "C:\questasim64_2024.1\win64;" + $env:PATH
```

A successful run shows `test_counter` as PASS and exits with code 0. Build
files and the cocotb results XML file go into `sim_build`. If license checkout
fails, configure the license using your QuestaSim installation's licensing
instructions.

The current simulator transcript is `sim_build\transcript`. A `transcript`
in the project directory may be left over from an older run and is not updated
by this runner.

## Run on Linux or WSL

Use 64-bit Python 3.9-3.13 with the `venv` module, a 64-bit **Linux**
installation of QuestaSim, and a valid license accessible from Linux. WSL
requires the Linux edition of QuestaSim; the Windows executables and Windows
virtual environment are not suitable for this flow.

In a Linux or WSL shell, change to the project directory. For example, in
WSL, this Windows work area is normally mounted at:

```bash
cd /mnt/f/Anamoly_detection/cocotb/Coco_Questasim
```

Add your Linux QuestaSim binary directory to `PATH` (replace the example
path with your installation path), and confirm the tools are available:

```bash
export PATH="/path/to/questasim/linux_x86_64:$PATH"
command -v vsim vlog vlib
vsim -version
```

Create a separate Linux virtual environment and run the test:

```bash
python3.12 -m venv .venv-linux
.venv-linux/bin/python -m pip install -r requirements.txt
.venv-linux/bin/python run_sim.py
```

Replace `python3.12` with another supported Python executable if needed. If
creating the environment fails because `venv` is missing, install the
corresponding Python venv package for your Linux distribution. Configure the
QuestaSim license according to your Linux installation's licensing
instructions.

The current transcript and results are in `sim_build/transcript` and
`sim_build/results.xml`. When switching between Windows and Linux, remove the
generated `sim_build` directory before running on the other platform so that
QuestaSim rebuilds its library. This also removes previous simulation results.

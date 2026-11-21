# VCS and Verdi cocotb example

This is the same 8-bit counter and test as `../Coco_Questasim`. The test
checks reset priority, counting, holding and wraparound. VCS is the simulator;
Verdi is the waveform/debug viewer, not a simulator.

## Run with VCS on a company Linux machine

Python 3.11 is supported. You do not need admin rights to install the Python
packages: the virtual environment is private to your user. You do need a
64-bit Linux VCS installation, access to a valid VCS license, and a
VCS-supported C compiler and build tools provided by your site. The virtual
environment does not install VCS, Verdi, their licenses, or system tools.

Work in a directory where you can create files. From this `Coco_VCS`
directory, configure VCS and licensing using your company's setup instructions
(for example, their environment module), then check the prerequisites:

```bash
python3.11 --version
command -v vcs
```

If your site supplies VCS through a binary directory instead, add that
directory to `PATH` using its instructions. Do not copy the license settings
from another machine; use the settings provided by your site.

Create the environment and install the project requirements into it:

```bash
python3.11 -m venv .venv-linux
.venv-linux/bin/python --version
.venv-linux/bin/python -m pip install -r requirements.txt
.venv-linux/bin/python -m cocotb_tools.config --version
.venv-linux/bin/python run_sim.py
```

No activation, `sudo`, or `pip install --user` is needed. The requirements
install cocotb 2.0.x and its Python dependencies inside `.venv-linux`.
If `python3.11` has a different name at your site, use the full path to its
Python 3.11 executable when creating the environment.

The default run does not require Verdi or produce waveforms. A successful run
reports `test_counter` as PASS; results and build files are in
`sim_build/vcs/`. If `venv` or pip is missing, ask your site support for a
Python 3.11 installation that includes them. If cocotb reports that it cannot
find `libpython`, ask for the matching shared Python library and the site's
recommended environment setup; a virtual environment alone may not supply it.
If VCS cannot be found, fails to check out a license, or cannot find its
compiler, use your company's tool setup or contact site support.

If the company machine cannot access a package index, use an approved internal
Python package mirror, or prepare a wheelhouse on a networked machine with the
same OS, CPU architecture and Python 3.11:

```bash
python3.11 -m pip download -d wheelhouse -r requirements.txt
```

Transfer `wheelhouse` to the company machine only if company policy permits,
then, from this directory, install without accessing an index:

```bash
.venv-linux/bin/python -m pip install --no-index --find-links /path/to/wheelhouse -r requirements.txt
```

For WSL, use Linux VCS and a license accessible from Linux; Windows simulator
binaries cannot be used by this flow.

### Record FSDB and open it in Verdi

For waveform recording, configure a compatible Verdi installation and its
license. Set `VERDI_HOME` according to your installation and add Verdi's
`bin` directory to `PATH`. Supply the directory containing the VCS-specific
`novas.tab` and `pli.a` files; the path below is a common Linux layout, but
may differ for your installation:

```bash
export VERDI_HOME=/path/to/verdi
export PATH="$VERDI_HOME/bin:$PATH"
.venv-linux/bin/python run_sim.py --pli-dir "$VERDI_HOME/share/PLI/VCS/LINUX64"
cd sim_build/vcs
verdi -ssf counter.fsdb
```

The runner enables `-kdb` and links the supplied Verdi PLI files; the RTL
then dumps `sim_build/vcs/counter.fsdb`. Verdi can load that file to inspect
waveforms; use the generated VCS KDB to browse the compiled design if desired.
PLI setup and licensing can vary with VCS/Verdi versions; follow your site's
installation instructions if linking or FSDB loading fails. Re-running
overwrites generated build files and waveforms.

## Try it without VCS

Icarus Verilog 11 or newer can run the same cocotb test locally, without VCS
or Verdi. Install Icarus and use the same Python environment as above:

```bash
command -v iverilog
.venv-linux/bin/python run_sim.py --sim icarus
```

Its results are in `sim_build/icarus/`. This checks the counter and testbench,
but does **not** validate the VCS or FSDB/Verdi integration.

## EDA Playground

[EDA Playground](https://www.edaplayground.com/) offers browser-based simulators
and [cocotb examples](https://eda-playground.readthedocs.io/en/latest/code-examples/cocotb.html).
Simulator availability, versions, access and custom-script support depend on
the site and account. This example requires cocotb 2.0.x; the site's
[settings documentation](https://eda-playground.readthedocs.io/en/latest/settings.html)
lists older versions, so check the actual available version rather than
assuming compatibility.

If a VCS environment with cocotb 2.0.x and **Use run.bash shell script** is
available, select Python + Verilog/SystemVerilog and VCS, then:

1. Add `counter.sv`, `test_counter.py` and `run_sim.py` as files in the
   playground. Leave the default `design.sv` and `testbench.py` tabs empty,
   so they do not duplicate the design or tests.
2. Add a `run.bash` tab containing:

   ```bash
   python3 run_sim.py
   ```

3. Enable **Use run.bash shell script** and run. Look for `test_counter` PASS
   in the log. This uses the VCS runner without FSDB.

This browser flow has not been verified against a live playground. If its
installed cocotb is older or the custom script/VCS combination is unavailable,
use a licensed Linux machine or run the Icarus fallback instead. EPWave is the
site's waveform viewer, not Verdi; FSDB/Verdi availability on the site has not
been established. Do not upload proprietary code unless permitted.

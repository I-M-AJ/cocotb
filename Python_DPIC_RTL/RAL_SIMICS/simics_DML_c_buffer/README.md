# Simics to SystemVerilog C-buffered adder

This Windows example uses a Simics `adder` object and a pure SystemVerilog
testbench; cocotb and sockets are not used. Simics and Questa run in separate
processes, so DPI-C and the Simics device share a one-word input mailbox and a
one-word result mailbox through named Windows shared memory. The **1,024-word
FIFO is in C (`sv/dpi_sv_server.c`)**, not in SystemVerilog or shared memory.
The DPI-C layer drains the input mailbox into the C ring buffer, feeds
`tb_top.sv` / `adder.sv` one word at a time, and returns each RTL sum in order.
A full result mailbox stalls the testbench and eventually fills the C FIFO and
input mailbox.

Initialize the project once with Simics project-setup and configure CMake. For
example, using the existing sibling project's Simics installation and Python:

```powershell
..\simics_DML_sv\bin\project-setup.bat --no-venv --python ..\simics_DML_sv\.venv\Scripts\python.exe --ignore-existing-files --mingw-dir F:\TDM-GCC-64 .
cmake -S . -B build -G "MinGW Makefiles" "-DCMAKE_MAKE_PROGRAM=F:\TDM-GCC-64\bin\mingw32-make.exe" "-DCMAKE_C_COMPILER=F:\TDM-GCC-64\bin\gcc.exe" "-DCMAKE_CXX_COMPILER=F:\TDM-GCC-64\bin\g++.exe"
```

Adjust the paths to match your installation. Then run `.\run_sv_cosim.bat`
from this project directory. It builds the Simics
module, compiles and starts Questa, runs the Simics tests and Python example,
and shuts down Questa. The script assigns a unique shared-memory name using
`SIMICS_SV_BUFFER_NAME`; both processes must use the same name when started
separately. Without that variable the name is `Local\simics_sv_buffer`.
Only one SV testbench and one Simics producer should use a name at a time.

The Simics `adder` register bank provides 32-bit accesses:

| Offset | Access | Meaning |
| --- | --- | --- |
| `0x00` | write | Input: operand A in bits 7:0, B in bits 15:8. Other bits are ignored by the RTL. |
| `0x04` | read | Pop the next 9-bit sum. Read only when status bit 1 is set. |
| `0x08` | read | Bit 31: bridge connected; bit 0: input mailbox free; bit 1: result available; bits 12:2: last observed C FIFO occupancy (0–1024). |
| `0x0c` | write | Bit 0 requests immediate testbench shutdown; pending data may be discarded. |

Poll bit 0 before every input write and bit 1 before every result read. Writing
while the input mailbox is occupied, reading an empty result, and accessing an
unavailable bridge are reported as errors. The FIFO count is a snapshot from
the testbench, not an acknowledgement that a particular input was processed.
See `run_buffer.py` for Python using the Simics object and register bank.

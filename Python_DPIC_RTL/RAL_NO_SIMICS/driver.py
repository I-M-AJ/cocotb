import ctypes
import re
import os


if os.name == "nt":
    dpi = ctypes.CDLL("vsim_auto_compile.dll", winmode=0)
else:
    dpi = ctypes.CDLL(None)
# Load DPI-C C symbols
dpi.c_write_port.argtypes = [ctypes.c_char_p, ctypes.c_uint64]
dpi.c_read_port.argtypes  = [ctypes.c_char_p]
dpi.c_read_port.restype   = ctypes.c_uint64
dpi.c_step_time.argtypes  = [ctypes.c_int]

class Port:
    """Represents a single RTL port with metadata and DPI-C binding."""
    def __init__(self, name: str, width: int, direction: str):
        self.name = name
        self.width = width
        self.direction = direction  # "input" or "output"
        self._mask = (1 << width) - 1
        self._c_name = name.encode("utf-8")

    @property
    def value(self) -> int:
        return dpi.c_read_port(self._c_name) & self._mask

    @value.setter
    def value(self, val: int):
        if self.direction != "input":
            raise PermissionError(f"Cannot drive output port '{self.name}'")
        masked_val = val & self._mask
        dpi.c_write_port(self._c_name, masked_val)

class Clock:
    """Drives a clock Port object for a given period."""
    def __init__(self, port: Port, period_ns: int = 10):
        self.port = port
        self.half_period = period_ns // 2
        self.port.value = 0

    def tick(self, cycles: int = 1):
        for _ in range(cycles):
            self.port.value = 0
            dpi.c_step_time(self.half_period)
            self.port.value = 1
            dpi.c_step_time(self.half_period)

class Reset:
    """Drives a reset Port object (active-high or active-low)."""
    def __init__(self, port: Port, active_high: bool = True):
        self.port = port
        self.active_val = 1 if active_high else 0
        self.inactive_val = 0 if active_high else 1

    def assert_reset(self, clk: Clock, cycles: int = 2):
        self.port.value = self.active_val
        clk.tick(cycles)
        self.port.value = self.inactive_val
        clk.tick(1)

class DUT:
    """Builds Port objects dynamically from RTL port definitions."""
    def __init__(self, sv_file: str):
        self.ports = {}
        self._parse_rtl_ports(sv_file)

    def _parse_rtl_ports(self, sv_file: str):
        # Automatically extracts input/output ports and bit-widths from adder.sv
        pattern = re.compile(r'\b(input|output)\s+(?:logic|wire|reg)?\s*(?:\[(\d+):(\d+)\])?\s*(\w+)')
        with open(sv_file, 'r') as f:
            for line in f:
                match = pattern.search(line)
                if match:
                    direction, msb, lsb, name = match.groups()
                    width = (int(msb) - int(lsb) + 1) if msb is not None else 1
                    port_obj = Port(name, width, direction)
                    self.ports[name] = port_obj
                    setattr(self, name, port_obj)  # Access as dut.a, dut.sum, etc.

# --- User Testbench Execution ---
def run_stimulus():
    # 1. Instantiate DUT object directly from RTL file metadata
    dut = DUT("adder.sv")
    print(f"[Python] Loaded ports: { {k: (v.direction, v.width) for k, v in dut.ports.items()} }")

    # 2. Wrap clk and rst ports in helper objects
    clk = Clock(dut.clk, period_ns=10)
    rst = Reset(dut.rst, active_high=True)

    # 3. Apply Reset using object method
    print("[Python] Applying Reset...")
    rst.assert_reset(clk, cycles=2)

    # 4. Drive data via Port objects and advance Clock object
    for a_val, b_val in [(15, 25), (100, 155), (255, 1)]:
        dut.a.value = a_val
        dut.b.value = b_val
        clk.tick(1)
        print(f"[Python] a={dut.a.value}, b={dut.b.value} -> sum={dut.sum.value}")

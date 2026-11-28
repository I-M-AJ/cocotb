"""Send packed operands through a Simics object to the C FIFO."""

import time

import dev_util
import simics

device = simics.SIM_create_object("adder", "adder0", [])
input_reg = dev_util.Register_LE(device.bank.regs, 0x00, size=4)
result_reg = dev_util.Register_LE(device.bank.regs, 0x04, size=4)
status_reg = dev_util.Register_LE(device.bank.regs, 0x08, size=4)
control_reg = dev_util.Register_LE(device.bank.regs, 0x0c, size=4)


def wait_for(mask):
    deadline = time.monotonic() + 10
    while time.monotonic() < deadline:
        status = status_reg.read()
        if not status & 0x80000000:
            raise RuntimeError("SV shared-memory bridge is unavailable")
        if status & mask:
            return
        time.sleep(0.001)
    raise TimeoutError(f"Timed out waiting for status mask 0x{mask:x}")


for a, b in [(25, 17), (255, 255), (0, 0)]:
    wait_for(1)
    input_reg.write(a | (b << 8))
    wait_for(2)
    result = result_reg.read()
    if result != a + b:
        raise AssertionError(f"{a} + {b}: received {result}")
    print(f"{a} + {b} = {result}")

control_reg.write(1)

import cocotb
from cocotb.triggers import Timer


async def cycle(dut, reset, enable, expected):
    dut.clk.value = 0
    dut.reset.value = reset
    dut.enable.value = enable
    await Timer(5, unit="ns")
    dut.clk.value = 1
    await Timer(5, unit="ns")
    actual = int(dut.count.value)
    assert actual == expected, f"expected {expected:#04x}, got {actual:#04x}"


@cocotb.test()
async def test_counter(dut):
    await cycle(dut, reset=1, enable=0, expected=0)
    await cycle(dut, reset=0, enable=1, expected=1)
    await cycle(dut, reset=0, enable=0, expected=1)
    await cycle(dut, reset=1, enable=1, expected=0)

    for expected in range(1, 256):
        await cycle(dut, reset=0, enable=1, expected=expected)
    await cycle(dut, reset=0, enable=1, expected=0)
    await cycle(dut, reset=0, enable=0, expected=0)

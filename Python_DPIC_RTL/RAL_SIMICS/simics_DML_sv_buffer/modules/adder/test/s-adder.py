# © 2026 Intel Corporation

import time

import dev_util
import stest
import adder_common

dev = adder_common.create_adder()
input_reg = dev_util.Register_LE(dev.bank.regs, 0x00, size=4)
result_reg = dev_util.Register_LE(dev.bank.regs, 0x04, size=4)
status_reg = dev_util.Register_LE(dev.bank.regs, 0x08, size=4)
partial_input = dev_util.Register_LE(dev.bank.regs, 0x00, size=1)
partial_result = dev_util.Register_LE(dev.bank.regs, 0x04, size=1)


def wait_status(predicate, description):
    deadline = time.monotonic() + 20
    while time.monotonic() < deadline:
        status = status_reg.read()
        if not status & 0x80000000:
            stest.fail("SV bridge is unavailable")
        if predicate(status):
            return status
        time.sleep(0.001)
    stest.fail("Timed out waiting for " + description)


def push(word):
    wait_status(lambda status: status & 1, "input mailbox")
    input_reg.write(word)


def pop():
    wait_status(lambda status: status & 2, "RTL result")
    return result_reg.read()


for a, b in [(25, 17), (255, 255), (0, 0), (1, 255)]:
    push(a | (b << 8))
    stest.expect_equal(pop(), a + b)

with stest.expect_log_mgr(dev.bank.regs, "error"):
    partial_input.write(7)
push(7 | (9 << 8))
wait_status(lambda status: status & 2, "RTL result")
with stest.expect_log_mgr(dev.bank.regs, "error"):
    partial_result.read()
stest.expect_equal(pop(), 16)

# Stall result consumption to fill the SV FIFO and the input handoff.
words = [(i & 255) | (((i * 3) & 255) << 8) for i in range(1027)]
for word in words:
    push(word)
wait_status(lambda status: ((status >> 2) & 0x7ff) == 1024 and not status & 1,
            "a full FIFO and input mailbox")
with stest.expect_log_mgr(dev.bank.regs, "error"):
    input_reg.write(0x1234)

for word in words:
    stest.expect_equal(pop(), (word & 255) + ((word >> 8) & 255))

wait_status(lambda status: ((status >> 2) & 0x7ff) == 0 and not status & 2,
            "an empty FIFO")

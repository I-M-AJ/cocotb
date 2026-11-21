# © 2026 Intel Corporation

import dev_util
import stest
import adder_common

# Create an instance of the device to test
dev = adder_common.create_adder()

reg_a = dev_util.Register_LE(dev.bank.regs, 0x00, size=4)
reg_b = dev_util.Register_LE(dev.bank.regs, 0x04, size=4)
reg_sum = dev_util.Register_LE(dev.bank.regs, 0x08, size=4)
reg_ctrl = dev_util.Register_LE(dev.bank.regs, 0x0C, size=4)

# Initial state
stest.expect_equal(reg_a.read(), 0)
stest.expect_equal(reg_b.read(), 0)
stest.expect_equal(reg_sum.read(), 0)

# Basic addition
reg_a.write(25)
reg_b.write(17)
stest.expect_equal(reg_sum.read(), 42)

# 8-bit masking & 9-bit sum overflow (255 + 255 = 510)
reg_a.write(0x1FF)
reg_b.write(0xFF)
stest.expect_equal(reg_a.read(), 0xFF)
stest.expect_equal(reg_sum.read(), 510)

# Read-only SUM should ignore writes and log spec-viol
with stest.expect_log_mgr(dev.bank.regs, 'spec-viol'):
    reg_sum.write(123)
stest.expect_equal(reg_sum.read(), 510)

# Reset via CTRL bit 0
reg_ctrl.write(1)
stest.expect_equal(reg_a.read(), 0)
stest.expect_equal(reg_b.read(), 0)
stest.expect_equal(reg_sum.read(), 0)
stest.expect_equal(reg_ctrl.read(), 0)

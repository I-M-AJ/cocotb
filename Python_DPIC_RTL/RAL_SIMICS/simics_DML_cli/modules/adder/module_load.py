# © 2026 Intel Corporation

import cli

class_name = 'adder'


def get_info(obj):
    return [("Adder",
             [("Description", "8-bit memory-mapped adder (A[7:0] + B[7:0] -> SUM[8:0])"),
              ("Register Map", "0x00: A, 0x04: B, 0x08: SUM (RO), 0x0C: CTRL (bit 0 = reset)")])]


cli.new_info_command(class_name, get_info)


def get_status(obj):
    return [("Registers",
             [("A", f"0x{obj.regs_a:02x} ({obj.regs_a})"),
              ("B", f"0x{obj.regs_b:02x} ({obj.regs_b})"),
              ("SUM", f"0x{obj.regs_sum:03x} ({obj.regs_sum})"),
              ("CTRL", f"0x{obj.regs_ctrl:08x}")])]


cli.new_status_command(class_name, get_status)

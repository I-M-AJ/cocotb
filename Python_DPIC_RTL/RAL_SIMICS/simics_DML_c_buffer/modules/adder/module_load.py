# © 2026 Intel Corporation

import cli


def get_info(obj):
    return [("C-buffered SV adder",
             [("Input", "0x00: packed operands A[7:0], B[15:8]"),
              ("Result", "0x04: pop 9-bit sum"),
              ("Status", "0x08: ready, FIFO count, input/result flags"),
              ("Control", "0x0c: bit 0 stops the SV simulation")])]


def get_status(obj):
    return [("Bridge", [("Status", "Read register 0x08 for live FIFO and mailbox status")])]


cli.new_info_command("adder", get_info)
cli.new_status_command("adder", get_status)

# © 2026 Intel Corporation

import simics

# Extend this function if your device requires any additional attributes to be
# set. It is often sensible to make additional arguments to this function
# optional, and let the function create mock objects if needed.
def create_adder(name = None):
    '''Create a new adder object'''
    adder = simics.pre_conf_object(name, 'adder')
    simics.SIM_add_configuration([adder], None)
    return simics.SIM_get_object(adder.name)

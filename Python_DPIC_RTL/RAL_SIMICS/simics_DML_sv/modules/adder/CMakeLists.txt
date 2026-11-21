# © 2026 Intel Corporation

simics_add_module(adder
  CLASSES adder
  SOURCES adder.dml sv_client.c module_load.py
  SIMICS_API 7
)
target_link_libraries(adder PRIVATE ws2_32)
add_subdirectory(test)

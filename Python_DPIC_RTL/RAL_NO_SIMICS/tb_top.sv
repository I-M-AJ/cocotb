`timescale 1ns/1ps
module tb_top;
    logic       clk;
    logic       rst;
    logic [7:0] a, b;
    logic [8:0] sum;

    adder dut (.*);

    // 1. Generic Write Task (Python -> RTL Port)
    export "DPI-C" task sv_write_port;
    task sv_write_port(input string port_name, input longint unsigned val);
        case (port_name)
            "clk": clk <= val[0];
            "rst": rst <= val[0];
            "a":   a   <= val[7:0];
            "b":   b   <= val[7:0];
            default: $error("[SV] Unknown input port: %s", port_name);
        endcase
    endtask

    // 2. Generic Read Task (RTL Port -> Python)
    export "DPI-C" task sv_read_port;
    task sv_read_port(input string port_name, output longint unsigned val);
        #0; // Ensure delta-cycle updates settle before reading
        case (port_name)
            "clk": val = {63'd0, clk};
            "rst": val = {63'd0, rst};
            "a":   val = {56'd0, a};
            "b":   val = {56'd0, b};
            "sum": val = {55'd0, sum};
            default: $error("[SV] Unknown port: %s", port_name);
        endcase
    endtask

    // 3. Generic Time-Step Task (Python controls the clock period)
    export "DPI-C" task sv_step_time;
    task sv_step_time(input int delay_ns);
        #(delay_ns * 1ns);
    endtask

    import "DPI-C" context task run_python_test();

    initial begin
        run_python_test();
        $display("[SV] Simulation complete.");
        $finish;
    end
endmodule

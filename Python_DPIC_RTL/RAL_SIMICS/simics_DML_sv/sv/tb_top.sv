`timescale 1ns/1ps
module tb_top;
    logic       clk;
    logic       rst;
    logic [7:0] a;
    logic [7:0] b;
    logic [8:0] sum;

    adder dut (
        .clk (clk),
        .rst (rst),
        .a   (a),
        .b   (b),
        .sum (sum)
    );

    task automatic pulse_clk();
        clk = 1'b0;
        #5ns;
        clk = 1'b1;
        #5ns;
        clk = 1'b0;
        #1ns;
    endtask

    export "DPI-C" task sv_write_a;
    task sv_write_a(input int unsigned val);
        a = val[7:0];
        pulse_clk();
    endtask

    export "DPI-C" task sv_write_b;
    task sv_write_b(input int unsigned val);
        b = val[7:0];
        pulse_clk();
    endtask

    export "DPI-C" task sv_read_sum;
    task sv_read_sum(output int unsigned val);
        #0;
        val = {23'd0, sum};
    endtask

    export "DPI-C" task sv_reset;
    task sv_reset();
        rst = 1'b1;
        a   = 8'd0;
        b   = 8'd0;
        pulse_clk();
        rst = 1'b0;
        pulse_clk();
    endtask

    import "DPI-C" context task sv_dpi_server_run(input int port);

    initial begin
        clk = 1'b0;
        rst = 1'b1;
        a   = 8'd0;
        b   = 8'd0;
        pulse_clk();
        rst = 1'b0;
        pulse_clk();

        $display("[SV] Starting DPI-C TCP server for Simics...");
        sv_dpi_server_run(5555);
        $display("[SV] Simulation complete.");
        $finish;
    end
endmodule

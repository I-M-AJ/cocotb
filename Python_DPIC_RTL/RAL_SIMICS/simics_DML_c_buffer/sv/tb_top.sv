`timescale 1ns/1ps
module tb_top;
    bit clk = 0;
    logic rst = 1;
    logic [7:0] a = 0;
    logic [7:0] b = 0;
    logic [8:0] sum;

    adder dut (.clk(clk), .rst(rst), .a(a), .b(b), .sum(sum));
    always #5ns clk = ~clk;

    import "DPI-C" function int sv_bridge_start();
    import "DPI-C" function int sv_bridge_poll(output int unsigned word,
                                               input int can_take);
    import "DPI-C" function int sv_bridge_publish(input int unsigned word);
    import "DPI-C" function void sv_bridge_close();

    initial begin : drive_buffer
        int rc;
        int unsigned word;
        bit result_pending;
        result_pending = 0;
        if (sv_bridge_start() != 0) $fatal(1, "Cannot start shared-memory bridge");
        repeat (2) @(negedge clk);
        rst = 0;
        forever begin
            @(negedge clk);
            if (result_pending) begin
                rc = sv_bridge_publish({23'd0, sum});
                if (rc < 0) $fatal(1, "Cannot publish RTL result");
                if (rc == 1) result_pending = 0;
            end

            rc = sv_bridge_poll(word, !result_pending);
            if (rc == -2) break;
            if (rc < 0) $fatal(1, "Shared-memory bridge failed");
            if (rc == 1) begin
                a = word[7:0];
                b = word[15:8];
                result_pending = 1;
            end
        end
        sv_bridge_close();
        $finish;
    end
endmodule

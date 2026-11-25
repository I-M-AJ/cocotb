`timescale 1ns/1ps
module tb_top;
    localparam int FIFO_DEPTH = 1024;
    bit clk = 0;
    logic rst = 1;
    logic [7:0] a = 0;
    logic [7:0] b = 0;
    logic [8:0] sum;
    int unsigned input_fifo[$:FIFO_DEPTH-1];

    adder dut (.clk(clk), .rst(rst), .a(a), .b(b), .sum(sum));
    always #5ns clk = ~clk;

    import "DPI-C" function int sv_bridge_start();
    import "DPI-C" function int sv_bridge_poll(output int unsigned word,
                                               input int can_take,
                                               input int unsigned fifo_count);
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
            rc = sv_bridge_poll(word, input_fifo.size() < FIFO_DEPTH, input_fifo.size());
            if (rc == -2) break;
            if (rc < 0) $fatal(1, "Shared-memory bridge failed");
            if (rc == 1) input_fifo.push_back(word);

            if (result_pending) begin
                rc = sv_bridge_publish({23'd0, sum});
                if (rc < 0) $fatal(1, "Cannot publish RTL result");
                if (rc == 1) result_pending = 0;
            end
            if (!result_pending && input_fifo.size() != 0) begin
                word = input_fifo.pop_front();
                a = word[7:0];
                b = word[15:8];
                result_pending = 1;
            end
        end
        sv_bridge_close();
        $finish;
    end
endmodule

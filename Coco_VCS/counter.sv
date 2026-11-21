`timescale 1ns/1ps

module counter (
    input  logic       clk,
    input  logic       reset,
    input  logic       enable,
    output logic [7:0] count
);
    always_ff @(posedge clk) begin
        if (reset)
            count <= 8'h00;
        else if (enable)
            count <= count + 8'h01;
    end

`ifdef DUMP_FSDB
    initial begin
        $fsdbDumpfile("counter.fsdb");
        $fsdbDumpvars(0, counter);
    end
`endif
endmodule

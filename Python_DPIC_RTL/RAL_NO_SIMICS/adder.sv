module adder (
    input  logic       clk,
    input  logic       rst,
    input  logic [7:0] a,
    input  logic [7:0] b,
    output logic [8:0] sum
);
    always_ff @(posedge clk) begin
        if (rst) sum <= 9'd0;
        else     sum <= a + b;
    end
endmodule

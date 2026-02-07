`timescale 1ps/1ps

module regs(input clk,
    input [3:0]raddr0_, output [15:0]rdata0,
    input [3:0]raddr1_, output [15:0]rdata1,
    input wen, input [3:0]waddr, input [15:0]wdata);

    reg [15:0]data[0:15];

    reg [3:0]raddr0;
    reg [3:0]raddr1;

    assign rdata0 = data[raddr0];
    assign rdata1 = data[raddr1];

    always @(posedge clk) begin
        raddr0 <= raddr0_;
        raddr1 <= raddr1_;
        // $display("Reading %d from %d", rdata0, raddr0);
        // $display("Reading %d from %d", rdata1, raddr1);
        if (wen) begin
            // $display("Writing %d to %d", wdata, waddr);
            data[waddr] <= wdata;
        end
    end

endmodule

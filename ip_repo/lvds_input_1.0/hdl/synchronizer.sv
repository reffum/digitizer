//
// Simple synchronizer
//

module synchronizer(clk, resetn, in, out);
   parameter SIZE = 8;

   input clk;
   input resetn;
   input [SIZE-1:0] in;
   output [SIZE-1:0] out;

   // Synchronizer registers
   logic [SIZE-1:0] in_d1, in_d2;

   always_ff @(posedge clk)
     if(!resetn) begin
	in_d1 <= {SIZE{1'd0}};
     	in_d2 <= {SIZE{1'd0}};
     end else begin
	in_d1 <= in;
	in_d2 <= in_d1;
     end

   assign out = in_d2;
   
endmodule // synchronizer

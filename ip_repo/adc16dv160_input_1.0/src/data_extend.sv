//
// This module receive input 16-bit data on 2 clock and
// output 32-bit data word on each second clock
//

module data_extend
  (
   input 	 resetn,
   input 	 clk,

   input [15:0]  indata,
   output [31:0] outdata,
   output 	 outvalid
   );

   //
   // State
   //
   enum   logic {S0, S1} state_cs, state_ns ;

   //
   // Registers
   //
   logic [31:0] data_cs, data_ns;

   always_ff @(posedge clk, negedge resetn) begin : STATE_REGISTER
      if(!resetn)
	state_cs <= S0;
      else
	state_cs <= state_ns;
   end

   always_comb begin : STATE_LOGIC
      state_ns <= state_cs;

      case(state_cs)
	S0: state_ns <= S1;
	S1: state_ns <= S0;
      endcase // case (state_cs)
   end

   always_ff @(posedge clk, negedge resetn) begin : DATA_REGISTERS
      if(!resetn)
	data_cs <= 0;
      else
	data_cs <= data_ns;
   end

   always_comb begin : DATA_LOGIC
      data_ns <= data_cs;

      case(state_cs)
	S0:
	  data_ns[15:0] <= indata;
	S1:
	  data_ns[31:16] <= indata;
      endcase // case (case_cs)
   end

   //
   // Outputs
   //
   assign outdata = data_cs;
   assign outvalid = state_cs == S0 ? 1'b1 : 1'b0;
   
endmodule // data_extend


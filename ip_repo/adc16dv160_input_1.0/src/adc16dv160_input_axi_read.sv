//
// AXI Lite read logic for adc_input module.
//

import adc16dv160_input_common::*;

module adc16dv160_input_axi_read
  (
   input 	       ACLK,
   input 	       ARESETN,

   input [31:0]        ARADDR,
   input 	       ARVALID,
   output logic        ARREADY,

   output logic [31:0] RDATA,
   output logic [1:0]  RRESP,
   output logic        RVALID,
   input 	       RREADY,

   input [31:0]        dsize,
   input 	       cr_test, 
   input 	       sr_pc
   );

   //
   // State register
   //
   enum  logic [1:0] {S0, S1, S2} state_cs, state_ns;

   //
   // Functions
   //
   function [31:0] ReadReg(logic [31:0] addr);
      automatic logic [31:0] value = 0;

      case(addr[7:0])
	AXI_ADDR_CR:
	  if(cr_test)
	    value = _CR_TEST;
	AXI_ADDR_SR:
	  if(sr_pc)
	    value = _SR_PC;
	AXI_ADDR_DSIZE:
	  value = dsize;
	default:
	  value = 0;
      endcase // case (addr)

      return value;
   endfunction // ReadReg

   //
   // State logic
   //

   always_ff @(posedge ACLK, negedge ARESETN) begin : STATE_REGISTER
      if(!ARESETN)
	state_cs <= S0;
      else
	state_cs <= state_ns;
   end

   always_comb begin : STATE_LOGIC
      state_ns <= state_cs;

      case(state_cs)
	S0:
	  if(ARVALID)
	    state_ns <= S1;
	S1:
	  state_ns <= S2;
	S2:
	  if(RREADY)
	    state_ns <= S0;
      endcase // case (state_cs)
   end // block: STATE_LOGIC

   //
   // Outputs logic
   //
   always_comb begin : OUTPUTS
      ARREADY <= 1'b0;
      RVALID <= 1'b0;
      RDATA <= 32'hXXXXXXXX;
      RRESP <= 2'b00;

      case(state_cs)
	S1:
	  ARREADY <= 1'b1;
	S2: begin
	   RDATA <= ReadReg(ARADDR);
	   RVALID <= 1'b1;
	end

	default: ;
      endcase // case (state_cs)
   end // block: OUTPUTS
   
endmodule 

//
// AXI Lite write logic for adc_input
//

import adc16dv160_input_common::*;

module adc16dv160_input_write
  (
   input 	      ACLK,
   input 	      ARESETN,

   input [31:0]       AWADDR,
   input 	      AWVALID,
   output logic       AWREADY,

   input [31:0]       WDATA,
   input [3:0] 	      WSTRB,
   input 	      WVALID,
   output logic       WREADY,

   output logic [1:0] BRESP,
   output logic       BVALID,
   input 	      BREADY,

   output [31:0]      dsize,
   output logic       cr_test, cr_start, cr_rt, cr_ls,
   output [15:0]      ls_start_thr,
   output [15:0]      ls_stop_thr,
   output [31:0]      ls_n_start,
   output [31:0]      ls_n_stop
   );

   //
   // State register
   //
   enum   logic [1:0] {S0, S1, S2} state_cs, state_ns;

   //
   // Data registers
   //
   logic [31:0] dsize_cs, dsize_ns;
   logic 	cr_test_ns, cr_test_cs;
   logic 	cr_rt_ns, cr_rt_cs;
   logic 	cr_ls_ns, cr_ls_cs;
   logic [15:0] ls_start_thr_ns, ls_start_thr_cs;
   logic [15:0] ls_stop_thr_ns, ls_stop_thr_cs;
   logic [31:0] ls_n_start_ns, ls_n_start_cs;
   logic [31:0] ls_n_stop_ns, ls_n_stop_cs;

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
	  if(AWVALID && WVALID)
	    state_ns <= S1;
	S1:
	  state_ns <= S2;
	S2:
	  if(BREADY)
	    state_ns <= S0;
      endcase // case (state_cs)
   end // block: STATE_LOGIC

   //
   // Data registers logic
   //
   always_ff @(posedge ACLK, negedge ARESETN) begin : DATA_REGISTERS
      if(!ARESETN) begin
	 dsize_cs <= 0;
	 cr_test_cs <= 1'b0;
	 cr_rt_cs <= 1'b0;
	 cr_ls_cs <= 1'b0;
	 ls_start_thr_cs <= 0;
	 ls_stop_thr_cs <= 0;
	 ls_n_start_cs <= 0;
	 ls_n_stop_cs <= 0;
      end else begin
	 dsize_cs <= dsize_ns;
	 cr_test_cs <= cr_test_ns;
	 cr_rt_cs <= cr_rt_ns;
	 cr_ls_cs <= cr_ls_ns;
	 ls_start_thr_cs <= ls_start_thr_ns;
	 ls_stop_thr_cs <= ls_stop_thr_ns;
	 ls_n_start_cs <= ls_n_start_ns;
	 ls_n_stop_cs <= ls_n_stop_ns;
      end
   end

   always_comb begin : DATA_LOGIC
      dsize_ns <= dsize_cs;
      cr_test_ns <= cr_test_cs;
      cr_start <= 1'b0;
      cr_rt_ns <= cr_rt_cs;
      cr_ls_ns <= cr_ls_cs;
      ls_start_thr_ns <= ls_start_thr_cs;
      ls_stop_thr_ns <= ls_stop_thr_cs;
      ls_n_start_ns <= ls_n_start_cs;
      ls_n_stop_ns <= ls_n_stop_cs;
      
      case(state_cs)
	S1:
	  case(AWADDR[7:0])
	    AXI_ADDR_CR: begin
	       cr_start <= WDATA[0];
	       cr_test_ns <= WDATA[1];
	       cr_rt_ns <= WDATA[2];
	       cr_ls_ns <= WDATA[3];
	    end
	    
	    AXI_ADDR_DSIZE:
	      dsize_ns <= WDATA;

	    AXI_ADDR_LS_START_THR:
	      ls_start_thr_ns <= WDATA[15:0];

	    AXI_ADDR_LS_STOP_THR:
	      ls_stop_thr_ns <= WDATA[15:0];

	    AXI_ADDR_LS_N_START:
	      ls_n_start_ns <= WDATA;

	    AXI_ADDR_LS_N_STOP:
	      ls_n_stop_ns <= WDATA;
	    
	    default: ;
	  endcase // case (AWADDR)
      endcase
   end // block: DATA_LOGIC

   //
   // Outputs
   //
   always_comb begin : OUTPUTS
      AWREADY <= 1'b0;
      WREADY <= 1'b0;
      BRESP <= 2'b00;
      BVALID <= 1'b0;

      case(state_cs)
	S1: begin
	   WREADY <= 1'b1;
	   AWREADY <= 1'b1;
	end

	S2: begin
	   BVALID <= 1'b1;
	end
	default: ;
      endcase // case (state_cs)
   end // block: OUTPUTS

   //
   // Nets assignment
   //
   assign dsize = dsize_cs;
   assign cr_test = cr_test_cs;
   assign cr_rt = cr_rt_cs;
   assign cr_ls = cr_ls_cs;
   assign ls_start_thr = ls_start_thr_cs;
   assign ls_stop_thr = ls_stop_thr_cs;
   assign ls_n_start = ls_n_start_cs;
   assign ls_n_stop = ls_n_stop_cs;
   
endmodule // adc_input_write

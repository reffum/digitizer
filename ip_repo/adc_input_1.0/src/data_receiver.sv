//
// Receive data from ADC and transmit it to AXI Stream
//

module data_receiver
  (
   // ADC inputs
    input 		 adc_clk,
    input [15:0] 	 adc_data,

    // AXI STREAM MASTER interface
    input wire 		 m00_axis_aclk,
    input wire 		 m00_axis_aresetn,
    output wire 	 m00_axis_tvalid,
    output wire [15 : 0] m00_axis_tdata,
    output wire [1 : 0]  m00_axis_tkeep,
    output wire 	 m00_axis_tlast,
    input wire 		 m00_axis_tready,

   // Control signals

   // Packet size
    input [31:0] 	 dsize,

   // Test mode
    input 		 test,

   // Start
    input 		 start,

   // Packet transmittion complete flag
    output logic 	 sr_pc
   
   );

   //
   // Constants
   //
   localparam INIT_COUNTER = 1024;
   
   
   //
   // Nets
   //
   logic  ACLK, ARESETN, TREADY, TVALID, TLAST;
   logic [1:0] TKEEP;
   logic [15:0] TDATA;
   
   
   /* FIFO signals */
   logic fifo_empty, fifo_full;
   logic fifo_almost_empty, fifo_almost_full;
   logic [15:0] fifo_do,  fifo_di;
   logic [9:0] fifo_rdcount, fifo_wrcount;
   logic fifo_rderr, fifo_wrerr;
   logic fifo_rdclk, fifo_wrclk;
   logic fifo_rst;
   logic fifo_rden, fifo_wren;
   
   //
   // State
   //
   enum  logic [2:0] {INIT, S0, S1, S2, S3} state_ns, state_cs;

   //
   // Registers
   //
   integer unsigned counter_cs, counter_ns;

   //
   // Functions
   //

   // Return data word for current AXI stream transaction.
   function logic [15:0] GetData();
      if(!test)
	return fifo_do;
      else
	return counter_cs[15:0];
   endfunction // GetData

   //
   // State logic
   //
   always_ff @(posedge ACLK, negedge ARESETN) begin : STATE_REGISTER
      if(!ARESETN)
	state_cs <= INIT;
      else
	state_cs <= state_ns;
   end

   always_comb begin : STATE_LOGIC
      state_ns <= state_cs;

      case(state_cs)
	INIT:
	  if(counter_cs == INIT_COUNTER)
	    state_ns <= S0;
	S0:
	  if(start)
	    state_ns <= S1;
	S1:
	  if(test || fifo_almost_full)
	    state_ns <= S2;
	S2:
	  if(TREADY) 
	    if(counter_cs <= 1)
	      state_ns <= S3;
	S3:
	  if(TREADY)
	    state_ns <= S0;
      endcase // case (state_cs)
   end // block: STATE_LOGIC

   //
   // Data registers logic
   //
   always_ff @(posedge ACLK, negedge ARESETN) begin : DATA_REGISTERS
      if(!ARESETN)
	counter_cs <= 0;
      else
	counter_cs <= counter_ns;
   end

   always_comb begin : DATA_LOGIC
      counter_ns <= counter_cs;

      case(state_cs)
	INIT:
	  counter_ns <= counter_cs + 1;
	S1:
	  counter_ns <= dsize;
	S2:
	  if(TREADY)
	    if(counter_cs > 1)
	      counter_ns <= counter_cs - 1;
	default:
	  counter_ns <= counter_cs;
      endcase // case (state_cs)
   end // block: DATA_LOGIC

   always_comb begin : OUTPUTS_AND_CONTROL
      TVALID <= 1'b0;
      TDATA <= 16'd0;
      TKEEP <= 2'b11;
      TLAST <= 1'b0;
      sr_pc <= 1'b0;
      fifo_rden <= 1'b0;
      fifo_rst <= 1'b0;
      fifo_wren <= 1'b1;
      
      
      case(state_cs)
	INIT: begin
	   fifo_rst <= 1'b1;
	   fifo_wren <= 1'b0;
	end
	
	S0: begin
	   sr_pc <= 1'b1;
	   fifo_wren <= 1'b0;
	end

	S2: begin
	   TDATA <= GetData();
	   TVALID <= 1'b1;

	   if(TREADY)
	     fifo_rden <= 1'b1;
	end

	S3: begin
	   TDATA <= GetData();
	   TVALID <= 1'b1;
	   TLAST <= 1'b1;
	end
	default: ;
      endcase // case (state_cs)
   end // block: OUTPUTS_AND_CONTROL

   //
   // Modules instant
   //
	  

   // FIFO_DUALCLOCK_MACRO: Dual Clock First-In, First-Out (FIFO) RAM Buffer
   //                       Artix-7
   // Xilinx HDL Language Template, version 2019.2

   /////////////////////////////////////////////////////////////////
   // DATA_WIDTH | FIFO_SIZE | FIFO Depth | RDCOUNT/WRCOUNT Width //
   // ===========|===========|============|=======================//
   //   37-72    |  "36Kb"   |     512    |         9-bit         //
   //   19-36    |  "36Kb"   |    1024    |        10-bit         //
   //   19-36    |  "18Kb"   |     512    |         9-bit         //
   //   10-18    |  "36Kb"   |    2048    |        11-bit         //
   //   10-18    |  "18Kb"   |    1024    |        10-bit         //
   //    5-9     |  "36Kb"   |    4096    |        12-bit         //
   //    5-9     |  "18Kb"   |    2048    |        11-bit         //
   //    1-4     |  "36Kb"   |    8192    |        13-bit         //
   //    1-4     |  "18Kb"   |    4096    |        12-bit         //
   /////////////////////////////////////////////////////////////////

   FIFO_DUALCLOCK_MACRO  #(
      .ALMOST_EMPTY_OFFSET(9'h010), // Sets the almost empty threshold
      .ALMOST_FULL_OFFSET(9'h010),  // Sets almost full threshold
      .DATA_WIDTH(16),   // Valid values are 1-72 (37-72 only valid when FIFO_SIZE="36Kb")
      .DEVICE("7SERIES"),  // Target device: "7SERIES" 
      .FIFO_SIZE ("18Kb"), // Target BRAM: "18Kb" or "36Kb" 
      .FIRST_WORD_FALL_THROUGH ("TRUE") // Sets the FIFO FWFT to "TRUE" or "FALSE" 
   ) FIFO_DUALCLOCK_MACRO_inst (
      .ALMOSTEMPTY(fifo_almost_empty),// 1-bit output almost empty
      .ALMOSTFULL(fifo_almost_full),  // 1-bit output almost full
      .DO(fifo_do),                   // Output data, width defined by DATA_WIDTH parameter
      .EMPTY(fifo_empty),             // 1-bit output empty
      .FULL(fifo_full),               // 1-bit output full
      .RDCOUNT(fifo_rdcount),         // Output read count, width determined by FIFO depth
      .RDERR(fifo_rderr),             // 1-bit output read error
      .WRCOUNT(fifo_wrcount),         // Output write count, width determined by FIFO depth
      .WRERR(fifo_wrerr),             // 1-bit output write error
      .DI(fifo_di),                   // Input data, width defined by DATA_WIDTH parameter
      .RDCLK(fifo_rdclk),             // 1-bit input read clock
      .RDEN(fifo_rden),               // 1-bit input read enable
      .RST(fifo_rst),                 // 1-bit input reset
      .WRCLK(fifo_wrclk),             // 1-bit input write clock
      .WREN(fifo_wren)                // 1-bit input write enable
   );


   //
   // Nets assignment
   //
   assign ACLK = m00_axis_aclk,
     ARESETN = m00_axis_aresetn,
     TREADY = m00_axis_tready,
     m00_axis_tvalid = TVALID,
     m00_axis_tkeep = TKEEP,
     m00_axis_tlast = TLAST;

   assign m00_axis_tdata = TDATA;
   
   assign fifo_di = adc_data;
   assign fifo_wrclk = adc_clk;
   assign fifo_rdclk = ACLK;
   
   
endmodule // data_receiver

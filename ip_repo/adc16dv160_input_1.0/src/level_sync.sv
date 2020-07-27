//
// level_sync.sv
//
// Generate packet synchronization signal depend on input data.
// If given data samples number is less then threashold, set sync output to 1.
// If given data samples number is more then threashold, set sync output to 0.
//
module level_sync
  (
   input clk,
   input resetn,
   
   // Two ADC data samples
   input unsigned [31:0] adc_data,

   // Start threashold
   input unsigned [15:0] start_threashold,

   // Stop threashold
   input unsigned [15:0] stop_threashold,

   // Start samples number
   input [31:0] start_samples_number,

   // Stop samples number
   input [31:0] stop_samples_number,

   // Packet synchronization output
   output sync
   );

   // State registers
   enum   logic {S0, S1} state_cs, state_ns;
   
   //
   // Data registers
   //

   // Samples counter
   int unsigned samples_counter_cs, samples_counter_ns;

   always_ff @(posedge clk, negedge resetn) begin : STATE_REGISTER
      if(!resetn)
	state_cs <= S0;
      else
	state_cs <= state_ns;
   end

   always_comb begin : STATE_LOGIC
      state_ns <= state_cs;

      case(state_cs)
	S0:
	  if(samples_counter_cs > start_samples_number)
	    state_ns <= S1;
	S1:
	  if(samples_counter_cs > stop_samples_number)
	    state_ns <= S0;
      endcase // case (state_cs)
   end // block: STATE_LOGIC

   always_ff @(posedge clk, negedge resetn) begin: DATA_REGISTER
     if(~resetn)
       samples_counter_cs <= 0;
     else
       samples_counter_cs <= samples_counter_ns;
   end

   always_comb begin : DATA_LOGIC
      samples_counter_ns <= samples_counter_cs;

      case(state_cs)
	S0: begin
	   if(state_ns == S1)
	     samples_counter_ns <= 0;
	   else if( adc_data[31:16] < start_threashold &&
		    adc_data[15:0] < start_threashold)
	     samples_counter_ns <= samples_counter_cs + 1;
	end

	S1: begin
	   if(state_ns == S0)
	     samples_counter_ns <= 0;
	   else if(adc_data[31:16] > stop_threashold &&
		   adc_data[15:0] > stop_threashold)
	     samples_counter_ns <= samples_counter_cs + 1;
	end

      endcase // case (state_cs)
   end // block: DATA_LOGIC
   

   //
   // Outputs
   //
   assign sync = (state_cs == S0) ? 1'b0 : 1'b1;
   

endmodule // level_sync

   

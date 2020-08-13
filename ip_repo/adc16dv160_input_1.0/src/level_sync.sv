//
// level_sync.sv
//
// Generate packet synchronization signal depend on input data.
// If it receive at least one sample less then threashold, set sync to 1.
// If it receive n_sample samples more then threashold, set sync to 0.
//
module level_sync
  (
   input 		 clk,
   input 		 resetn,
   
   // Two ADC data samples
   input unsigned [31:0] adc_data,
   input 		 adc_data_valid,

   // Parameters
   input unsigned [15:0] threashold,
   input unsigned [31:0] n_sample,

   // Packet synchronization output
   output 		 sync
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
	S0: begin
	   if(adc_data_valid) 
	      if(adc_data[31:16] < threashold ||
		 adc_data[15:0] < threashold)
		state_ns <= S1;
	end

	S1: begin
	   if(samples_counter_cs > n_sample)
	     state_ns <= S0;
	end
      endcase // case (state_cs)
   end // block: STATE_LOGIC

   always_ff @(posedge clk, negedge resetn) begin : DATA_REGISTER
      if(!resetn)
	samples_counter_cs <= 0;
      else
	samples_counter_cs <= samples_counter_ns;
   end

   always_comb begin : DATA_LOGIC
      samples_counter_ns <= samples_counter_cs;

      case(state_cs)
	S0: samples_counter_ns <= 0;
	
	S1: begin
	   if(adc_data_valid)
	     if(adc_data[31:16] < threashold ||
		adc_data[15:0] < threashold)
	       samples_counter_ns <= 0;
	     else
	       samples_counter_ns <= samples_counter_cs + 2;
	end
      endcase // case (state_cs)
   end // block: DATA_LOGIC


   //
   // Outputs
   //
   assign sync = (state_cs == S0) ? 1'b0 : 1'b1;
   

endmodule // level_sync

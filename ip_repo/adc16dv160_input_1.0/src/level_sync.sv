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
   input [31:0] adc_data; 

   // Start threashold
   input [15:0] start_threashold,

   // Stop threashold
   input [15:0] stop_threashold,

   // Start samples number
   input [31:0] start_samples_number,

   // Stop samples number
   input [31:0] stop_samples_number,

   // Packet synchronization output
   output sync
   );

endmodule // level_sync

   

//
// ADC16DV160 chip simulation model
//
`timescale 1ns / 1ps

module adc16dv160
  (
   output outclk_p, outclk_n,
   // Outputs
   // D0/1 - d[0], D16/15 - d[7]
   output [7:0] d_p, d_n
   );

   parameter FREQ = 160e6;
   
   localparam realtime PERIOD = 1s / FREQ;
   localparam realtime TSU = 1ns;
   localparam realtime TH = 1ns;
   

   logic [15:0] OutputData[];
   logic 	start = 1'b0;

   logic 	outclk;
   logic [7:0] 	d;

   assign outclk_p = outclk;
   assign outclk_n = ~outclk;

   assign d_p = d;
   assign d_n = ~d;
   
   task Start(logic [15:0] data[]);
      OutputData = data;
      start = 1'b1;
   endtask // SetData

   initial begin
      automatic logic [15:0] w;
      
      outclk = 1'b1;
      d = 8'd0;
      
      @(start == 1);

      forever begin
	 foreach(OutputData[i]) begin
	    w = OutputData[i];

	    d = 8'hXX;
	    
	    #(PERIOD/2 - TSU - TH) d = {w[15], w[13], w[11], w[9], w[7], w[5], w[3], w[1]};
	    #(TSU) outclk = 1'b0;
	    #(TH) d = 8'hXX;
	    #(PERIOD/2 - TSU - TH) d = {w[14], w[12], w[10], w[8], w[6], w[4], w[2], w[0]};
	    #(TSU) outclk = 1'b1;
	    #(TH) d = 8'hXX;
	 end // foreach (OutputData[i])
      end // forever begin
   end // initial begin

endmodule // adc16dv160


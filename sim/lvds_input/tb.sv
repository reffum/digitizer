//
// lvds testbench
//
timeunit 1ns;
timeprecision 100ps;

import lvds_input_common::*;


module tb;

   localparam real     ADC_FREQ = 25e6;
   localparam realtime ADC_CLK_PERIOD = 1.0s/ADC_FREQ;
   localparam realtime AXI_CLK_PERIOD = 10ns;
   localparam realtime SYNC_TIME = 176us;
   localparam realtime SYNC_PERIOD = 4ms;

   //
   // UUT ports
   //
   
   // ADC logics
   logic 	       adc_clk;
   logic [3:0] 	       adc_data_p, adc_data_n;
   logic 	       sync = 1'b0;
   
   // AXI-lite slave interface
   logic [31:0] 	 s_axi_awaddr = 32'h0;
   logic 		 s_axi_awvalid = 1'b0;
   logic 		 s_axi_awready;
   
   logic [31:0] 	 s_axi_wdata = 32'd0;
   logic [3:0] 		 s_axi_wstrb = 4'hf;
   logic 		 s_axi_wvalid = 1'b0;
   logic 		 s_axi_wready;

   logic [1:0] 		 s_axi_bresp;
   logic 		 s_axi_bvalid;
   logic 		 s_axi_bready = 1'b1;

   logic [31:0] 	 s_axi_araddr = 32'd0;
   logic 		 s_axi_arvalid = 1'b0;
   logic 		 s_axi_arready;
   
   logic [31:0] 	 s_axi_rdata;
   logic [1:0] 		 s_axi_rresp;
   logic 		 s_axi_rvalid;
   logic 		 s_axi_rready = 1'b1;
   
   // AXI STREAM MASTER interface
   logic  		 m00_axis_aclk = 1'b0;
   logic  		 m00_axis_aresetn = 1'b0;
   logic 		 m00_axis_tvalid;
   logic [31 : 0] 	 m00_axis_tdata;
   logic [3 : 0] 	 m00_axis_tkeep;
   logic 		 m00_axis_tlast;
   logic 		 m00_axis_tready;
   
   lvds_input_v1_0 UUT
     (
      .*
      );

   axi_stream_receiver axi_stream_receiver_inst
     (
      .ACLK(m00_axis_aclk),
      .ARESETN(m00_axis_aresetn),
      .TVALID(m00_axis_tvalid),
      .TDATA(m00_axis_tdata),
      .TKEEP(m00_axis_tkeep),
      .TLAST(m00_axis_tlast),
      .TREADY(m00_axis_tready)
      );
   

   initial begin : LVDS_CLK_GEN
      adc_clk = 1'b0;
      forever #(ADC_CLK_PERIOD/2) adc_clk = ~adc_clk;
   end
   
   initial begin : AXI_CLK_GEN
      m00_axis_aclk = 1'b0;
      forever #(AXI_CLK_PERIOD) m00_axis_aclk = ~m00_axis_aclk;
   end

   initial begin : TEST_DATA_GENERATION
      automatic logic [3:0] data = 4'd0;

      adc_data_p = 4'd0;
      
      forever begin
	 @(sync === 1'b1);

	 do begin
	   @(posedge adc_clk);
	    adc_data_p = data++;
	 end 
	 while(sync === 1'b1);
      end
   end // block: TEST_DATA_GENERATION

   assign adc_data_n = ~adc_data_p;
   
   
   initial begin : TEST
      integer i;
      logic [31:0] cr, r;
      
      // Reset
      m00_axis_aresetn = 1'b0;
      repeat(20) @(posedge m00_axis_aclk);
      m00_axis_aresetn = 1'b1;
      repeat(20) @(posedge m00_axis_aclk);
      
      // Start Real-time mode and 
      WriteReg(AXI_ADDR_CR, _CR_RT);

      // Check, that it bit is set
      ReadReg(AXI_ADDR_CR, cr);
      assert(cr == (_CR_RT)) else $stop;

      #100us;
      repeat(5) begin
      	 sync = 1'b1;
      	 #(SYNC_TIME/2) sync = 1'b0;
      	 #(SYNC_PERIOD);
      end
      #100us;
      
      $finish;
   end

   default clocking cb @(posedge m00_axis_aclk);
      default input #1step output negedge;
      
      output s_axi_awaddr;
      output s_axi_awvalid;
      input  s_axi_awready;
      output s_axi_wdata;
      output s_axi_wstrb;
      output s_axi_wvalid;
      input  s_axi_wready;
      input  s_axi_bresp;
      input  s_axi_bvalid;
      output s_axi_bready;
      output s_axi_araddr;
      output s_axi_arvalid;
      input  s_axi_arready;
      input  s_axi_rdata;
      input  s_axi_rresp;
      input  s_axi_rvalid;
      output s_axi_rready;
   endclocking // cb
   
   
   task WriteReg(input [32:0] address, input [32:0] data);
      cb.s_axi_awaddr <= address;
      cb.s_axi_awvalid <= 1'b1;
      cb.s_axi_wdata <= data;
      cb.s_axi_wvalid <= 1'b1;
      @(cb.s_axi_awready && cb.s_axi_wready);
      @(cb.s_axi_bvalid);
      assert(cb.s_axi_bresp == 2'b00) else $stop;
      cb.s_axi_awvalid <= 1'b0;
      cb.s_axi_wvalid <= 1'b0;
      ##1;
   endtask // WriteReg

   task ReadReg(input [31:0] address, output [31:0] data);
      cb.s_axi_araddr <= address;
      cb.s_axi_arvalid <= 1'b1;
      @(cb.s_axi_arready);
      @(cb.s_axi_rvalid);
      assert(cb.s_axi_rresp == 2'b00) else $stop;
      data = cb.s_axi_rdata;
      cb.s_axi_arvalid <= 1'b0;
   endtask // ReadReg
      
endmodule // tb

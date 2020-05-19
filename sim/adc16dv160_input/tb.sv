//
// adc16dv160_input testbench
//
timeunit 1ns;
timeprecision 100ps;

module tb;

   localparam realtime REF_CLK_DELAY_PERIOD = 5ns;
   localparam real     ADC_FREQ = 160e6;
   localparam realtime ADC_CLK_PERIOD = 1.0s/ADC_FREQ;
   localparam realtime AXI_CLK_PERIOD = 10ns;
   localparam realtime SYNC_TIME = 1ms;
   localparam realtime SYNC_PERIOD = 4ms;
   
   
   // Addres of registers
   localparam CR = 0;
   localparam SR = 4;
   localparam DSIZE = 8;

   localparam _CR_RT = 32'h4;
   
   
   
   //
   // UUT ports
   //
   
   // ADC logics
   logic 	       adc_clk_p, adc_clk_n;
   logic [7:0] 	       adc_data_p, adc_data_n;
   logic 	       sync = 1'b0;
   
   // REFCLK for IODEALYCTRL 
   logic 		 ref_clk_delay;

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
   logic 		 m00_axis_tready = 1'b1;
   
   adc16dv160_input_v1_0 UUT
     (
      .*
      );

   adc16dv160 adc16dv160_inst
     (
      .outclk_p(adc_clk_p),
      .outclk_n(adc_clk_n),
      .d_p(adc_data_p),
      .d_n(adc_data_n)
      );
   
   initial begin : REF_CLK_DELAY_GEN
      ref_clk_delay = 1'b0;
      forever #(REF_CLK_DELAY_PERIOD) ref_clk_delay = ~ref_clk_delay;
   end

   initial begin : AXI_CLK_GEN
      m00_axis_aclk = 1'b0;
      forever #(AXI_CLK_PERIOD) m00_axis_aclk = ~m00_axis_aclk;
   end

   initial begin : TEST
      integer i;
      logic [31:0] cr;
      automatic logic [15:0] AdcData[1024];

      for(i = 0; i < $size(AdcData); i = i + 1) begin
	 AdcData[i] = 16'hAAAA;
      end      

      adc16dv160_inst.Start(AdcData);
      
      // Reset
      m00_axis_aresetn = 1'b0;
      repeat(20) @(posedge m00_axis_aclk);
      m00_axis_aresetn = 1'b1;
      repeat(20) @(posedge m00_axis_aclk);

      // Start Real-time mode
      WriteReg(CR, _CR_RT);

      // Check, that it bit is set
      ReadReg(CR, cr);
      assert(cr == _CR_RT) else $stop;

      // Start transmit

      // Wait some times before assert sync
      #100us;


      repeat(5) begin
	 sync = 1'b1;
	 #(SYNC_TIME/2) m00_axis_tready = 1'b0;
	 #500ns m00_axis_tready = 1'b1;
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


`timescale 1 ns / 1 ps

module adc16dv160_input_v1_0
   (
    // ADC inputs
    input 		 adc_clk_p, adc_clk_n,
    input [7:0] 	 adc_data_p, adc_data_n,
    
    // REFCLK for IODEALYCTRL 
    input ref_clk_delay,

    // AXI-lite slave interface
    input [31:0] 	 s_axi_awaddr,
    input 		 s_axi_awvalid,
    output 		 s_axi_awready,
    
    input [31:0] 	 s_axi_wdata,
    input [3:0] 	 s_axi_wstrb,
    input 		 s_axi_wvalid,
    output 		 s_axi_wready,

    output [1:0] 	 s_axi_bresp,
    output 		 s_axi_bvalid,
    input 		 s_axi_bready,

    input [31:0] 	 s_axi_araddr,
    input 		 s_axi_arvalid,
    output 		 s_axi_arready,
    
    output [31:0] 	 s_axi_rdata,
    output [1:0] 	 s_axi_rresp,
    output 		 s_axi_rvalid,
    input 		 s_axi_rready,
    
    // AXI STREAM MASTER interface
    input wire 		 m00_axis_aclk,
    input wire 		 m00_axis_aresetn,
    output wire 	 m00_axis_tvalid,
    output wire [31 : 0] m00_axis_tdata,
    output wire [3 : 0]  m00_axis_tkeep,
    output wire 	 m00_axis_tlast,
    input wire 		 m00_axis_tready
    );

   
   //
   // Nets
   //
   logic 		 adc_clk, adc_clk_io;
   logic [15:0] 	 adc_data;
   
   // AXI DSIZE register
   logic [31:0] 	 dsize;
   logic 		 cr_test;
   logic 		 cr_start;
   logic 		 sr_pc;
   
   //
   // Modules instantiation
   //
   adc16dv160_input_data_receiver data_receiver_inst
     (
      .test(cr_test),
      .start(cr_start),
      
      .*
      );

   adc16dv160_input_axi_read adc16dv160_input_axi_read_inst
     (
      .ACLK(m00_axis_aclk),
      .ARESETN(m00_axis_aresetn),

      .ARADDR(s_axi_araddr),
      .ARVALID(s_axi_arvalid),
      .ARREADY(s_axi_arready),

      .RDATA(s_axi_rdata),
      .RRESP(s_axi_rresp),
      .RVALID(s_axi_rvalid),
      .RREADY(s_axi_rready),

      .*
      );

   adc16dv160_input_write  adc16dv160_input_write_inst
     (
      .ACLK(m00_axis_aclk),
      .ARESETN(m00_axis_aresetn),

      .AWADDR(s_axi_awaddr),
      .AWVALID(s_axi_awvalid),
      .AWREADY(s_axi_awready),

      .WDATA(s_axi_wdata),
      .WSTRB(s_axi_wstrb),
      .WVALID(s_axi_wvalid),
      .WREADY(s_axi_wready),

      .BRESP(s_axi_bresp),
      .BVALID(s_axi_bvalid),
      .BREADY(s_axi_bready),

      .*
      );

   //
   // Data input buffers and DDR logic
   //
   genvar 		 i;

   generate
      for(i = 0; i < 8; i = i + 1) begin : gen_adc_data
	 logic adc_data_i, adc_data_d1, q1, q2;
	 
	 IBUFDS 
	   #(
	     .DIFF_TERM("FALSE"),
	     .IBUF_LOW_PWR("FALSE"),
	     .IOSTANDARD("LVDS_25")
	     ) IBUFDS_JA 
	     (
	      .O(adc_data_i),
	      .I(adc_data_p[i]),
	      .IB(adc_data_n[i])
	      );

	 IDELAYE2 
	   #(
	     .CINVCTRL_SEL("FALSE"),          // Enable dynamic clock inversion (FALSE, TRUE)
	     .DELAY_SRC("IDATAIN"),           // Delay input (IDATAIN, DATAIN)
	     .HIGH_PERFORMANCE_MODE("TRUE"),  // Reduced jitter ("TRUE"), Reduced power ("FALSE")
	     .IDELAY_TYPE("FIXED"),           // FIXED, VARIABLE, VAR_LOAD, VAR_LOAD_PIPE
	     .IDELAY_VALUE(0),                // Input delay tap setting (0-31)
	     .PIPE_SEL("FALSE"),              // Select pipelined mode, FALSE, TRUE
	     .REFCLK_FREQUENCY(200.0),        // IDELAYCTRL clock input frequency in MHz (190.0-210.0, 290.0-310.0).
	     .SIGNAL_PATTERN("DATA")          // DATA, CLOCK input signal
	     )
	 IDELAYE2_ADC_DATA 
	   (
	    .CNTVALUEOUT(),            // 5-bit output: Counter value output
	    .DATAOUT(adc_data_d1),     // 1-bit output: Delayed data output
	    .C(1'b0),                  // 1-bit input: Clock input
	    .CE(1'b0),                 // 1-bit input: Active high enable increment/decrement input
	    .CINVCTRL(1'b0),           // 1-bit input: Dynamic clock inversion input
	    .CNTVALUEIN(5'd0),         // 5-bit input: Counter value input
	    .DATAIN(1'b0),             // 1-bit input: Internal delay data input
	    .IDATAIN(adc_data_i),   // 1-bit input: Data input from the I/O
	    .INC(1'b0),                // 1-bit input: Increment / Decrement tap delay input
	    .LD(1'b0),                 // 1-bit input: Load IDELAY_VALUE input
	    .LDPIPEEN(1'b0),           // 1-bit input: Enable PIPELINE register to load data input
	    .REGRST(1'b0)              // 1-bit input: Active-high reset tap-delay input
	    );

	 IDDR
	   #(
	     .DDR_CLK_EDGE("SAME_EDGE"),
	     .INIT_Q1(1'b0),
	     .INIT_Q2(1'b0),
	     .SRTYPE("SYNC")
	     )
	 IDDR_ADC_DATA
	   (
	    .Q1(q1),
	    .Q2(q2),
	    .C(adc_clk_io),
	    .CE(1'b1),
	    .D(adc_data_d1),
	    .R(1'b0),
	    .S(1'b0)
	    );

	 assign adc_data[2*i] = q1;
	 assign adc_data[2*i + 1] = q2;
      end // block: gen_adc_data      
   endgenerate
   
   IDELAYCTRL IDELAYCTRL_ADC_DATA (
      .RDY(),          // 1-bit output: Ready output
      .REFCLK(ref_clk_delay), // 1-bit input: Reference clock input
      .RST(1'b0)        // 1-bit input: Active high reset input
   );   
   
   // Adc clockc input 
   IBUFDS 
     #(
       .DIFF_TERM("FALSE"),
       .IBUF_LOW_PWR("FALSE"),
       .IOSTANDARD("LVDS_25")
       ) IBUFDS_ADC_CLK
       (
	.O(adc_clk_i),
	.I(adc_clk_p),
	.IB(adc_clk_n)
	);     

   BUFIO BUFIO_ADC_CLK
     (
      .I(adc_clk_i),
      .O(adc_clk_io)
      );

   BUFG BUFG_ADC_CLK
     (
      .I(adc_clk_i),
      .O(adc_clk)
      );
   
endmodule


`timescale 1 ns / 1 ps

module adc16dv160_input_v1_0
   (
    // ADC inputs
    input 		 adc_clk_p, adc_clk_n,
    input [7:0] 	 adc_data_p, adc_data_n,

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
   logic 		 adc_clk;
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
   logic [7:0] 		 adc_data_i;
   
   genvar 		 i;

   generate
      for(i = 0; i < 8; i = i + 1) begin : gen_adc_data
	 IBUFDS 
	      #(
		.DIFF_TERM("FALSE"),
		.IBUF_LOW_PWR("FALSE"),
		.IOSTANDARD("LVDS_25")
		) IBUFDS_JA 
	      (
	       .O(adc_data_i[i]),
	       .I(adc_data_p[i]),
	       .IB(adc_data_n[i])
	       );


	 logic q1, q2;
	 
	 IDDR
	   #(
	     .DDR_CLK_EDGE("OPPOSITE_EDGE"),
	     .INIT_Q1(1'b0),
	     .INIT_Q2(1'b0),
	     .SRTYPE("SYNC")
	     )
	 IDDR_ADC_DATA
	   (
	    .Q1(q1),
	    .Q2(q2),
	    .C(adc_clk),
	    .CE(1'b1),
	    .D(adc_data_i[i]),
	    .R(1'b0),
	    .S(1'b0)
	    );

	 assign adc_data[2*i] = q1;
	 assign adc_data[2*i + 1] = q2;
      end // block: gen_adc_data      
   endgenerate
   
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

   BUFG BUFG_ADC_CLK
     (
      .I(adc_clk_i),
      .O(adc_clk)
      );
   


   
endmodule


`timescale 1 ns / 1 ps

module adc_input_v1_0 
   (
    // ADC inputs
    input 		 adc_clk,
    input [15:0] 	 adc_data,

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
    output wire [15 : 0] m00_axis_tdata,
    output wire [1 : 0]  m00_axis_tkeep,
    output wire 	 m00_axis_tlast,
    input wire 		 m00_axis_tready
    );

   
   //
   // Nets
   //
   
   // AXI DSIZE register
   logic [31:0] 	 dsize;
   logic 		 cr_test;
   logic 		 cr_start;
   logic 		 sr_pc;
   
   //
   // Modules instantiation
   //
   data_receiver data_receiver_inst
     (
      .test(cr_test),
      .start(cr_start),
      
      .*
      );

   adc_input_axi_read adc_input_axi_read_inst
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

   adc_input_write  adc_input_write_inst
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
   
endmodule

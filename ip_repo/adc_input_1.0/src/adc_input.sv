
`timescale 1 ns / 1 ps

module adc_input_v1_0 
  #(
    parameter C_BASEADDR = 32'd0,
    parameter C_HIGHADDR = 32'd0
    )
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
    output wire [1 : 0]  m00_axis_tstrb,
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

   
endmodule

`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 07.02.2019 18:00:10
// Design Name: 
// Module Name: tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module tb;
   //
   // UUT ports
   //
   wire [14:0]DDR_addr;
   wire [2:0] DDR_ba;
   wire       DDR_cas_n;
   wire       DDR_ck_n;
   wire       DDR_ck_p;
   wire       DDR_cke;
   wire       DDR_cs_n;
   wire [3:0] DDR_dm;
   wire [31:0] DDR_dq;
   wire [3:0]  DDR_dqs_n;
   wire [3:0]  DDR_dqs_p;
   wire        DDR_odt;
   wire        DDR_ras_n;
   wire        DDR_reset_n;
   wire        DDR_we_n;
   wire        FIXED_IO_ddr_vrn;
   wire        FIXED_IO_ddr_vrp;
   wire [53:0] FIXED_IO_mio;
   wire        FIXED_IO_ps_clk;
   wire        FIXED_IO_ps_porb;
   wire        FIXED_IO_ps_srstb;
   wire        ja, jc, jd;
   wire        hdmi_rx_clk_n;
   wire        hdmi_rx_clk_p;
   wire        hdmi_rx_n;
   wire        hdmi_rx_p;
   wire        dphy_data_hs_n;
   wire        dphy_data_hs_p;
   wire        dphy_hs_clock_clk_n;
   wire        dphy_hs_clock_clk_p;
   wire        led;

   
   logic clk, resetn;

   assign FIXED_IO_ps_clk = clk;
   assign FIXED_IO_ps_porb = resetn,
	 FIXED_IO_ps_srstb = resetn;

   initial begin : CLK_GEN
	  clk = 1'b0;
	  forever #15ns clk = ~clk;
   end
   
`define A tb.design_1_i.design_1_i.processing_system7_0.inst
   initial begin : TEST
      logic [1:0] responce;
      
      $display("TEST start");
      resetn = 1'b0;
      repeat(20) @(posedge clk);
      resetn = 1'b1;
      repeat(5) @(posedge clk);

      `A.fpga_soft_reset(32'h1);
      `A.fpga_soft_reset(32'h0);

      #10us;

      `A.write_data(32'h6000_0000, 4, 32'h0000_0003, responce);
      
      
      $finish;
      
   end // block: TEST

  design_1_wrapper design_1_i
    (
     .*
     );
   

endmodule

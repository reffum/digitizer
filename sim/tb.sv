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
   localparam time ADC_CLK_PERIOD = 100ns;
   localparam DMA_ADDR = 32'h4040_0000;
   
   
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

   wire [3:0]  ja_p, ja_n;
   wire [3:0]  jb_p, jb_n; 
   wire [3:0]  jc_p, jc_n; 
   wire [3:0]  jd_p, jd_n; 
   wire        hdmi_clk_n, hdmi_clk_p;
   
   wire        led;

   
   logic       clk, resetn;

   logic       adc_clk_ex;
   

   assign FIXED_IO_ps_clk = clk;
   assign FIXED_IO_ps_porb = resetn,
     FIXED_IO_ps_srstb = resetn;

   assign jd_p[3] = adc_clk_ex;
   assign jd_n[3] = ~jd_p[3];
   

   initial begin : CLK_GEN
      clk = 1'b0;
      forever #15ns clk = ~clk;
   end

   initial begin : ADC_CLK_GEN
      adc_clk_ex = 1'b0;
      forever #(ADC_CLK_PERIOD/2) adc_clk_ex = ~adc_clk_ex;
   end
   
   
`define A tb.UUT.design_1_i.processing_system7_0.inst
   initial begin : TEST
      logic [1:0] responce;
      
      $display("TEST start");
      resetn = 1'b0;
      repeat(20) @(posedge clk);
      resetn = 1'b1;
      repeat(5) @(posedge clk);

      `A.fpga_soft_reset(32'h1);
      `A.fpga_soft_reset(32'h0);

      #20us;

      `A.write_data(DMA_ADDR + 'h30, 4, 32'h0000_0001, responce);
      assert(responce === 2'b00);

      `A.write_data(DMA_ADDR + 'h48, 4, 32'h0010_0000, responce);
      assert(responce === 2'b00);

      `A.write_data(DMA_ADDR + 'h58, 4, 32'h0000_1000, responce);
      assert(responce === 2'b00);

      
      // Set packet size
      `A.write_data(32'h6000_0008, 4, 32'h0000_0020, responce);
      assert(responce === 2'b00);

      // Set test mode and start
      `A.write_data(32'h6000_0000, 4, 32'h0000_0003, responce);
      assert(responce === 2'b00);
      
      #10us;
      
      $finish;
      
   end // block: TEST

   digitizer UUT
     (
      .*
      );
   

endmodule

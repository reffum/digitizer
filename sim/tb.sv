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
   localparam MEM_ADDRESS = 32'h0010_0000;
   localparam DMA_BUFFER_SIZE = 1024*1024*32;
   localparam DATA_SIZE = 2*1024;
   
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
   wire        je;
   wire        hdmi_clk_n, hdmi_clk_p;
   wire        hdmi_d_n, hdmi_d_p;
   wire [3:0]  led;

   
   logic       clk, resetn;

   logic       adc_clk_p, adc_clk_n;
   logic [7:0] adc_data_p, adc_data_n;

   assign FIXED_IO_ps_clk = clk;
   assign FIXED_IO_ps_porb = resetn,
     FIXED_IO_ps_srstb = resetn;

   // ADC data interface connected to
   // FPGA ports
   assign hdmi_clk_p = adc_clk_p;
   assign hdmi_clk_n = adc_clk_n;

   assign jd_p[0] = adc_data_p[3];
   assign jd_n[0] = adc_data_n[3];
   assign jd_p[1] = adc_data_p[2];
   assign jd_n[1] = adc_data_n[2];
   assign jd_p[2] = adc_data_p[1];
   assign jd_n[2] = adc_data_n[1];
   assign jd_p[3] = adc_data_p[0];
   assign jd_n[3] = adc_data_n[0];
   
   assign jc_p[0] = adc_data_p[7];
   assign jc_n[0] = adc_data_n[7];
   assign jc_p[1] = adc_data_p[6];
   assign jc_n[1] = adc_data_n[6];
   assign jc_p[2] = adc_data_p[5];
   assign jc_n[2] = adc_data_n[5];
   assign jc_p[3] = adc_data_p[4];
   assign jc_n[3] = adc_data_n[4];

   initial begin : CLK_GEN
      clk = 1'b0;
      forever #15ns clk = ~clk;
   end

   adc16dv160 adc16dv160_inst
     (
      .outclk_p(adc_clk_p),
      .outclk_n(adc_clk_n),
      .d_p(adc_data_p),
      .d_n(adc_data_n)
      );
   
   
`define A tb.UUT.design_1_i.processing_system7_0.inst
   initial begin : TEST
      logic [1:0] responce;
      logic [31:0] register;

      automatic logic [15:0] AdcData[1024];

      foreach(AdcData[i])
	AdcData[i] = i;

      adc16dv160_inst.Start(AdcData);
      
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

      `A.write_data(DMA_ADDR + 'h48, 4, MEM_ADDRESS, responce);
      assert(responce === 2'b00);

      `A.write_data(DMA_ADDR + 'h58, 4, DMA_BUFFER_SIZE, responce);
      assert(responce === 2'b00);

      
      // Set packet size
      `A.write_data(32'h6000_0008, 4, DATA_SIZE, responce);
      assert(responce === 2'b00);

      // Set start
      `A.write_data(32'h6000_0000, 4, 32'h0000_0001, responce);
      assert(responce === 2'b00);
      
      // Wait for end of data transmitt
      do begin
	 `A.read_data(DMA_ADDR + 'h34, 4, register, responce);
	 assert(responce === 2'b00);
      end while(!(register & 2));

      `A.peek_mem_to_file("data.bin", MEM_ADDRESS, DATA_SIZE);
      
      $finish;
      
   end // block: TEST

   digitizer UUT
     (
      .*
      );
   

endmodule

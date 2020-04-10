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
   localparam DATA_SIZE = 64*1024;
   
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
   
   wire        led;

   
   logic       clk, resetn;

   logic       adc_clk;
   logic [15:0] adc_data;

   assign FIXED_IO_ps_clk = clk;
   assign FIXED_IO_ps_porb = resetn,
     FIXED_IO_ps_srstb = resetn;

   assign jd_p[3] = adc_clk;
   assign ja_p[3:0] = adc_data[3:0];
   assign jb_p[3:0] = adc_data[7:4];
   assign jc_p[3:0] = adc_data[11:8];
   assign jd_p[2:0] = adc_data[14:12];
   assign ja_n[0] = adc_data[15];
   
   
   initial begin : CLK_GEN
      clk = 1'b0;
      forever #15ns clk = ~clk;
   end

   initial begin : ADC_CLK_GEN
      adc_clk = 1'b0;
      forever #(ADC_CLK_PERIOD/2) adc_clk = ~adc_clk;
   end

   clocking adc_ck @(posedge adc_clk);
      output negedge adc_data;
   endclocking // adc_ck

   initial begin : ADC_DATA_GENERATE
      automatic logic [15:0] value = 0;
      
      // Generate countinous incremented value
      adc_data = {$size(adc_data){1'b0}};

      forever begin
	 adc_ck.adc_data <= value;
	 value = value + 1;
	 @(adc_ck);
      end
   end // block: ADC_DATA_GENERATE
   
`define A tb.UUT.design_1_i.processing_system7_0.inst
   initial begin : TEST
      logic [1:0] responce;
      logic [31:0] register;
      
      
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
      `A.write_data(32'h6000_0000, 4, 32'h0000_0003, responce);
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

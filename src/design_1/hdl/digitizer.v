//Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2019.2 (win64) Build 2700185 Thu Oct 24 18:46:05 MDT 2019
//Date        : Sun Mar 22 18:44:44 2020
//Host        : DESKTOP-D2OREBE running 64-bit major release  (build 9200)
//Command     : generate_target design_1_wrapper.bd
//Design      : design_1_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module digitizer
  (DDR_addr,
   DDR_ba,
   DDR_cas_n,
   DDR_ck_n,
   DDR_ck_p,
   DDR_cke,
   DDR_cs_n,
   DDR_dm,
   DDR_dq,
   DDR_dqs_n,
   DDR_dqs_p,
   DDR_odt,
   DDR_ras_n,
   DDR_reset_n,
   DDR_we_n,
   FIXED_IO_ddr_vrn,
   FIXED_IO_ddr_vrp,
   FIXED_IO_mio,
   FIXED_IO_ps_clk,
   FIXED_IO_ps_porb,
   FIXED_IO_ps_srstb,
  
   ja_p, ja_n,
   jb_p, jb_n, 
   jc_p, jc_n,
   jd_p, jd_n,
  
   hdmi_clk_n, hdmi_clk_p,    
  
   led
   );
   inout [14:0]DDR_addr;
   inout [2:0] DDR_ba;
   inout       DDR_cas_n;
   inout       DDR_ck_n;
   inout       DDR_ck_p;
   inout       DDR_cke;
   inout       DDR_cs_n;
   inout [3:0] DDR_dm;
   inout [31:0] DDR_dq;
   inout [3:0] 	DDR_dqs_n;
   inout [3:0] 	DDR_dqs_p;
   inout 	DDR_odt;
   inout 	DDR_ras_n;
   inout 	DDR_reset_n;
   inout 	DDR_we_n;
   inout 	FIXED_IO_ddr_vrn;
   inout 	FIXED_IO_ddr_vrp;
   inout [53:0] FIXED_IO_mio;
   inout 	FIXED_IO_ps_clk;
   inout 	FIXED_IO_ps_porb;
   inout 	FIXED_IO_ps_srstb;
   
   input [3:0] 	ja_p, ja_n;
   input [3:0] 	jb_p, jb_n; 
   input [3:0] 	jc_p, jc_n; 
   input [3:0] 	jd_p, jd_n; 
   input 	hdmi_clk_n, hdmi_clk_p;
   
   output [3:0] led;

   wire [14:0] 	DDR_addr;
   wire [2:0] 	DDR_ba;
   wire 	DDR_cas_n;
   wire 	DDR_ck_n;
   wire 	DDR_ck_p;
   wire 	DDR_cke;
   wire 	DDR_cs_n;
   wire [3:0] 	DDR_dm;
   wire [31:0] 	DDR_dq;
   wire [3:0] 	DDR_dqs_n;
   wire [3:0] 	DDR_dqs_p;
   wire 	DDR_odt;
   wire 	DDR_ras_n;
   wire 	DDR_reset_n;
   wire 	DDR_we_n;
   wire 	FIXED_IO_ddr_vrn;
   wire 	FIXED_IO_ddr_vrp;
   wire [53:0] 	FIXED_IO_mio;
   wire 	FIXED_IO_ps_clk;
   wire FIXED_IO_ps_porb;
   wire FIXED_IO_ps_srstb;
   
   wire [7:0] adc_data_p, adc_data_n;
   wire       adc_clk_p, adc_n;
   
   reg 	       led_cs;  

  design_1 design_1_i
       (.DDR_addr(DDR_addr),
        .DDR_ba(DDR_ba),
        .DDR_cas_n(DDR_cas_n),
        .DDR_ck_n(DDR_ck_n),
        .DDR_ck_p(DDR_ck_p),
        .DDR_cke(DDR_cke),
        .DDR_cs_n(DDR_cs_n),
        .DDR_dm(DDR_dm),
        .DDR_dq(DDR_dq),
        .DDR_dqs_n(DDR_dqs_n),
        .DDR_dqs_p(DDR_dqs_p),
        .DDR_odt(DDR_odt),
        .DDR_ras_n(DDR_ras_n),
        .DDR_reset_n(DDR_reset_n),
        .DDR_we_n(DDR_we_n),
        .FIXED_IO_ddr_vrn(FIXED_IO_ddr_vrn),
        .FIXED_IO_ddr_vrp(FIXED_IO_ddr_vrp),
        .FIXED_IO_mio(FIXED_IO_mio),
        .FIXED_IO_ps_clk(FIXED_IO_ps_clk),
        .FIXED_IO_ps_porb(FIXED_IO_ps_porb),
        .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb),
        .adc_clk_p(adc_clk_p),
	.adc_clk_n(adc_clk_n),
        .adc_data_p(adc_data_p),
	.adc_data_n(adc_data_n));

   assign adc_data_p[3:0] = ja_p;
   assign adc_data_n[3:0] = ja_n;
   assign adc_data_p[7:4] = jb_p;
   assign adc_data_n[7:4] = jb_n;
   assign adc_clk_p = hdmi_clk_p;
   assign adc_clk_n = hdmi_clk_n;

   assign led[0] = 1'b1;
   
endmodule

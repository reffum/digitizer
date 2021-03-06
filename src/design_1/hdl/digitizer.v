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
   
   btn,
   
   ja_p, ja_n,

   jb1_p, jb1_n,
   jb2_p, jb2_n,
   jb3_p, jb3_n,
   jb4_p, jb4_n,
   
   jc_p, jc_n,
   jd_p, jd_n,
   je,
   je6, 
  
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
   
   inout [1:0] btn;
   
   input [3:0] ja_p, ja_n;

   output 	jb1_p, jb1_n;
   input 	jb2_p, jb2_n;
   input 	jb3_p, jb3_n;
   input 	jb4_p, jb4_n;
   
   input [3:0] 	jc_p, jc_n; 
   input [3:0] 	jd_p, jd_n;
   inout [5:0] je; 
   input je6;
   
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
   wire TTC0_WAVE1_OUT_0;
   wire [5:0] gpio_emio_i, gpio_emio_o, gpio_emio_t;
   wire sync;
   
   wire [7:0] adc_data_p, adc_data_n;
   wire       adc_clk_p, adc_n;
   (*keep*) wire     ADC_CLK_OUT;
   
   wire [3:0] lvds_data_p, lvds_data_n;
   wire lvds_clk;
   
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
	.adc_data_n(adc_data_n),
	.TTC0_WAVE1_OUT_0(TTC0_WAVE1_OUT_0),
	.ADC_CLK_OUT(ADC_CLK_OUT),
	.sync(sync),
	.GPIO_0_0_tri_i(gpio_emio_i),
	.GPIO_0_0_tri_o(gpio_emio_o),
	.GPIO_0_0_tri_t(gpio_emio_t),
     
	.lvds_data_p(lvds_data_p),
	.lvds_data_n(lvds_data_n),
	.lvds_clk(lvds_clk),
	.lvds_sync(lvds_sync)
     );
     
   assign lvds_sync = gpio_emio_o[5];
   
   assign adc_clk_p = jd_p[2];
   assign adc_clk_n = jd_n[2];
   
   assign adc_data_p[0] = jd_p[3];
   assign adc_data_n[0] = jd_n[3];
   assign adc_data_p[1] = jb2_p;
   assign adc_data_n[1] = jb2_n;
   assign adc_data_p[2] = jd_p[1];
   assign adc_data_n[2] = jd_n[1];   
   assign adc_data_p[3] = jd_p[0];
   assign adc_data_n[3] = jd_n[0];
   assign adc_data_p[4] = jc_p[3];
   assign adc_data_n[4] = jc_n[3];
   assign adc_data_p[5] = jc_p[2];
   assign adc_data_n[5] = jc_n[2];       
   assign adc_data_p[6] = jc_p[1];
   assign adc_data_n[6] = jc_n[1]; 
   assign adc_data_p[7] = jc_p[0];
   assign adc_data_n[7] = jc_n[0];        

   assign led[0] = 1'b1;
   assign led[3:1] = 3'h0;
   assign je[0] = TTC0_WAVE1_OUT_0;
   assign sync = ~je6;
   
   assign lvds_data_p = 4'd0;
   assign lvds_data_n = 4'd0;     
   
   //
   // GPIO EMIO
   //
  IOBUF GPIO_EMIO_JE2
       (.I(gpio_emio_o[0]),
        .IO(je[1]),
        .O(gpio_emio_i[0]),
        .T(gpio_emio_t[0])
        );  
        
  IOBUF GPIO_EMIO_JE3
       (.I(gpio_emio_o[1]),
        .IO(je[2]),
        .O(gpio_emio_i[1]),
        .T(gpio_emio_t[1])
        );
        
  IOBUF GPIO_EMIO_JE4
       (.I(gpio_emio_o[2]),
        .IO(je[3]),
        .O(gpio_emio_i[2]),
        .T(gpio_emio_t[2])
        );
        
  IOBUF GPIO_EMIO_JE7
       (.I(gpio_emio_o[3]),
        .IO(je[4]),
        .O(gpio_emio_i[3]),
        .T(gpio_emio_t[3])
        );  
        
  IOBUF GPIO_EMIO_JE8
       (.I(gpio_emio_o[4]),
        .IO(je[5]),
        .O(gpio_emio_i[4]),
        .T(gpio_emio_t[4])
        ); 
   
    // SELA and SELC
    assign btn[0] = TTC0_WAVE1_OUT_0;   // SELA
    assign btn[1] = TTC0_WAVE1_OUT_0;   // SELC       
           
        
   // LVDS_CLK
   assign jb1_p = lvds_clk;

endmodule

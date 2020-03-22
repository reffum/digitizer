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

module design_1_wrapper
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
    
    ja, jc, jd,
    
    hdmi_rx_clk_n, 
    hdmi_rx_clk_p,
    hdmi_rx_n,
    hdmi_rx_p,
    
    dphy_data_hs_n,
    dphy_data_hs_p,
    dphy_hs_clock_clk_n,
    dphy_hs_clock_clk_p,
    
    led
    );
  inout [14:0]DDR_addr;
  inout [2:0]DDR_ba;
  inout DDR_cas_n;
  inout DDR_ck_n;
  inout DDR_ck_p;
  inout DDR_cke;
  inout DDR_cs_n;
  inout [3:0]DDR_dm;
  inout [31:0]DDR_dq;
  inout [3:0]DDR_dqs_n;
  inout [3:0]DDR_dqs_p;
  inout DDR_odt;
  inout DDR_ras_n;
  inout DDR_reset_n;
  inout DDR_we_n;
  inout FIXED_IO_ddr_vrn;
  inout FIXED_IO_ddr_vrp;
  inout [53:0]FIXED_IO_mio;
  inout FIXED_IO_ps_clk;
  inout FIXED_IO_ps_porb;
  inout FIXED_IO_ps_srstb;
  
  input [7:0]ja, jc, jd;
  
  input hdmi_rx_clk_n, hdmi_rx_clk_p;
  input [2:0] hdmi_rx_n, hdmi_rx_p;
  
  
  input [1:0] dphy_data_hs_n, dphy_data_hs_p;
  input dphy_hs_clock_clk_n, dphy_hs_clock_clk_p;
  
  output [3:0] led;

  wire [14:0]DDR_addr;
  wire [2:0]DDR_ba;
  wire DDR_cas_n;
  wire DDR_ck_n;
  wire DDR_ck_p;
  wire DDR_cke;
  wire DDR_cs_n;
  wire [3:0]DDR_dm;
  wire [31:0]DDR_dq;
  wire [3:0]DDR_dqs_n;
  wire [3:0]DDR_dqs_p;
  wire DDR_odt;
  wire DDR_ras_n;
  wire DDR_reset_n;
  wire DDR_we_n;
  wire FIXED_IO_ddr_vrn;
  wire FIXED_IO_ddr_vrp;
  wire [53:0]FIXED_IO_mio;
  wire FIXED_IO_ps_clk;
  wire FIXED_IO_ps_porb;
  wire FIXED_IO_ps_srstb;

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
        .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb));
        
   wire [15:0] adc_data;
   wire adc_clk;
   reg led_cs;
   
   // Connect adc_data
   genvar i;
   
   generate
      for (i=0; i < 4; i = i + 1)
      begin: ja_adc_data
       IBUFDS #(
          .DIFF_TERM("FALSE"),
          .IBUF_LOW_PWR("FALSE"),
          .IOSTANDARD("LVDS_25")
       ) IBUFDS_inst (
          .O(adc_data[i]),
          .I(ja[i]),
          .IB(ja[i + 4])
       );
      end
   endgenerate 
   
   generate
      for (i=0; i < 4; i = i + 1)
      begin: jc_adc_data
       IBUFDS #(
          .DIFF_TERM("FALSE"),
          .IBUF_LOW_PWR("FALSE"),
          .IOSTANDARD("LVDS_25")
       ) IBUFDS_inst (
          .O(adc_data[i + 4]),
          .I(jc[i]),
          .IB(jc[i + 4])
       );
      end
   endgenerate  
   
   generate
      for (i=0; i < 4; i = i + 1)
      begin: jd_adc_data
       IBUFDS #(
          .DIFF_TERM("FALSE"),
          .IBUF_LOW_PWR("FALSE"),
          .IOSTANDARD("LVDS_25")
       ) IBUFDS_inst (
          .O(adc_data[i + 8]),
          .I(jd[i]),
          .IB(jd[i + 4])
       );
      end
   endgenerate
   
   generate 
    for(i = 0; i < 3; i = i + 1)
    begin: hdmi_rx_adc
        IBUFDS #(
          .DIFF_TERM("FALSE"),
          .IBUF_LOW_PWR("FALSE"),
          .IOSTANDARD("TMDS_33")
        ) IBUFDS_adc_clk (
          .O(adc_data[12 + i]),
          .I(hdmi_rx_p[i]),
          .IB(hdmi_rx_n[i])
        ); 
    end  
  endgenerate   
  
   IBUFDS #(
      .DIFF_TERM("FALSE"),
      .IBUF_LOW_PWR("FALSE"),
      .IOSTANDARD("LVDS_25")
    ) IBUFDS_adc_data15 (
      .O(adc_data[15]),
      .I(dphy_data_hs_p[0]),
      .IB(dphy_data_hs_n[0])
    );     
    
    IBUFDS #(
      .DIFF_TERM("FALSE"),
      .IBUF_LOW_PWR("FALSE"),
      .IOSTANDARD("LVDS_25")
    ) IBUFDS_adc_clk (
      .O(adc_clk),
      .I(dphy_hs_clock_clk_p),
      .IB(dphy_hs_clock_clk_n)
    );
    
    always @(posedge adc_clk) begin
        led_cs <= |adc_data;
    end
    
    assign led[0] = led_cs;
    
endmodule

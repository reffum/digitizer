
`timescale 1 ns / 1 ps

module lvds_input_v1_0
   (
    // ADC inputs
    input 		 adc_clk,
    input [3:0] 	 adc_data_p, adc_data_n,
    input 		 sync,

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
   logic [3:0] 		 adc_data;

   // Sync signal
   logic 		 sync_s;
   
   // Extern  sync
   logic 		 ext_sync;
   
   // AXI DSIZE register
   logic [31:0] 	 dsize;

   // CR register bits
   logic 		 cr_test;
   logic 		 cr_start;
   logic 		 cr_rt;
   
   // SR register bits
   logic 		 sr_pc;
   
   //
   // Modules instantiation
   //
   lvds_input_data_receiver data_receiver_inst
     (
      .sync(sync_s),
      .test(cr_test),
      .start(cr_start),
      .start_rt(cr_rt),
      .*
      );

   lvds_input_axi_read lvds_input_axi_read_inst
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

   lvds_input_write  lvds_input_write_inst
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

   synchronizer #(.SIZE(1)) synchronizer_sync
     (
      .clk(m00_axis_aclk),
      .resetn(m00_axis_aresetn),
      .in(sync),
      .out(ext_sync)
      );

   assign sync_s = ext_sync;
   
   //
   // Data input buffers and DDR logic
   //
   genvar 		 i;

   generate
      for(i = 0; i < $size(adc_data); i = i + 1) begin : gen_adc_data
	 
	 IBUFDS 
	   #(
	     .DIFF_TERM("FALSE"),
	     .IBUF_LOW_PWR("FALSE"),
	     .IOSTANDARD("LVDS_25")
	     ) IBUFDS_JA 
	     (
	      .O(adc_data[i]),
	      .I(adc_data_p[i]),
	      .IB(adc_data_n[i])
	      );

      end // block: gen_adc_data      
   endgenerate
      
   //TODO: Remove this code
   (*keep*) reg[15:0] dbg_cnt;
   
   always_ff @(posedge adc_clk)
    dbg_cnt <= dbg_cnt + 1;      
   
endmodule

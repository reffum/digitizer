//
// adc_input module common constants, functions etc
//

package adc16dv160_input_common;
   // AXI registers address map
   localparam AXI_ADDR_CR = 0;
   localparam AXI_ADDR_SR = 4;
   localparam AXI_ADDR_DSIZE = 8;
   localparam AXI_ADDR_LS_START_THR = 12;
   localparam AXI_ADDR_LS_STOP_THR = 16;
   localparam AXI_ADDR_LS_N_START = 20;
   localparam AXI_ADDR_LS_N_STOP = 24;
   
   // Bits in registers
   localparam _CR_START = 1;
   localparam _CR_TEST = 2;
   localparam _CR_RT = 4;
   localparam _CR_LS = 8;

   localparam _SR_PC = 1;
   

endpackage
   

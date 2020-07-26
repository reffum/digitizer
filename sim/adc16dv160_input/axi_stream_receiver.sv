//
// AXI STREAM receiver
// Receive AXI stream frames and save it to files
//

module axi_stream_receiver
  (
   input 	ACLK,
   input 	ARESETN,
   input 	TVALID,
   input [31:0] TDATA,
   input [3:0] 	TKEEP,
   input 	TLAST,
   output 	TREADY
   );
   
   assign TREADY = 1'b1;

   // Receive data and save it to file, on TLAST
   // Do not use TKEEP, because it must be always 1
   initial begin
      // Packet number
      automatic int PNUM = 0;
      automatic string FileName;
      automatic logic [15:0] ReceivedData [$];

      forever begin
	 @(posedge ACLK);
	 
	 if(TVALID) begin
	    ReceivedData.push_back(TDATA[31:16]);
	    ReceivedData.push_back(TDATA[15:0]);
	 end

	 if(TLAST) begin
	    $sformat(FileName, "%0d.dat", PNUM);
	    $writememh(FileName, ReceivedData);
	    ReceivedData.delete();
	    PNUM++;
	 end
      end
   end
   

endmodule // axi_stream_receiver

#include<hls_stream.h>
#include <ap_axi_sdata.h>
typedef ap_axiu<8,2,5,6> uint_8_side_channel;

void doHist(hls::stream<uint_8_side_channel> &inStream, int histo[256]);

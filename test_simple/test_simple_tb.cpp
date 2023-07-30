#include "test_simple.h"

int main(){

    hls::stream<data_t> mixin;
    hls::stream<data_t> mixout;

    data_t mixin_stream;

    data_t dataout;


    for(uint8_t j=0; j<128*4; ++j)
    {
    	mixin_stream.data = 0xa8184851; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x154f4b4f; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x19975771; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x7cef52db; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x5102240a; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x42dddb99; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0xb215e865; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x4eca0185; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0xc1c2dfc9; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x6ece31eb; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x3c6209d0; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0xbbc62b1d; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x74d075ae; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0xff1ffd18; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x204b4b69; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x8c93affd; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0xa8184851; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x154f4b4f; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x19975771; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x7cef52db; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x5102240a; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x42dddb99; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0xb215e865; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x4eca0185; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0xc1c2dfc9; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x6ece31eb; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x3c6209d0; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0xbbc62b1d; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x74d075ae; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0xff1ffd18; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x204b4b69; mixin_stream.last = 0; mixin.write(mixin_stream);
    	mixin_stream.data = 0x8c93affd; mixin_stream.last = 1; mixin.write(mixin_stream);
		}

		test_simple(mixin, mixout);
		for(unsigned int i = 0; i < 128*128; i++){
			dataout = mixout.read();
			printf("mixout[%d]:%x - TLAST: %u - KEEP: %u \r",
					i,
					dataout.data.to_uint(),
					dataout.last.to_uint(),
					dataout.keep.to_uint());
    }

}


#include "output_buffer.h"

namespace soclib { namespace caba {

#define tmpl(x) \
template < unsigned int WIDTH, typename IN_TYPE > \
x OutputBuffer<WIDTH, IN_TYPE>

tmpl(void)::transition () {

	if (p_resetn == false) {
		bufferFull = false;
		return;
	}

	if (p_output.wok) {
		bufferFull = false;
	}
	if (p_write) {
		buffer = p_input.read();
		bufferFull = true;
	}

}

tmpl(void)::genMealy () {

	p_output.data = (sc_uint<WIDTH>)buffer;
	p_output.w = bufferFull;
	p_writeok = !bufferFull || p_output.wok;

}

tmpl(/**/)::OutputBuffer(sc_module_name insname)
	: BaseModule(insname) {

	SC_METHOD (transition);
	dont_initialize();
	sensitive << p_clk.pos();

	SC_METHOD (genMealy);
	dont_initialize();
	sensitive << p_clk.neg()
		<< p_output.wok;
}

tmpl(/**/)::~OutputBuffer() {
}

}} // end of soclib::caba


/// Local Variables:
/// mode: hs-minor
/// c-basic-offset: 4
/// End:

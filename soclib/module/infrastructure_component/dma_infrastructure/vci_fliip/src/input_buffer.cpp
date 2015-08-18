
#include "input_buffer.h"

namespace soclib { namespace caba {

#define tmpl(x) \
template < unsigned int WIDTH, typename OUT_TYPE > \
x InputBuffer<WIDTH, OUT_TYPE>

tmpl(void)::genMealy () {

	p_output  = (OUT_TYPE) p_input.data.read();
	p_write   = p_input.rok;
	p_input.r = p_writeok;

}

tmpl(/**/)::InputBuffer(sc_module_name insname)
	: BaseModule(insname) {

	SC_METHOD (genMealy);
	dont_initialize();
	sensitive << p_input.data << p_input.rok << p_writeok;
}

tmpl(/**/)::~InputBuffer() {
}

}} // end of soclib::caba


/// Local Variables:
/// mode: hs-minor
/// c-basic-offset: 4
/// End:

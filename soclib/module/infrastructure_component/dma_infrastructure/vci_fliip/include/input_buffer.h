
#ifndef INPUT_BUFFER
#define INPUT_BUFFER

#include <systemc.h>
#include <base_module.h>
#include <fifo_ports.h>

namespace soclib { namespace caba {

template <unsigned int WIDTH, typename OUT_TYPE>
class InputBuffer : BaseModule {
public:
    sc_in  < bool > p_clk;
    sc_in  < bool > p_resetn;
	FifoInput< sc_uint< WIDTH > >  p_input;
	sc_out < OUT_TYPE > p_output;
	sc_in  < bool > p_writeok;
	sc_out < bool > p_write;

private:
	void genMealy();

protected:
    SC_HAS_PROCESS (InputBuffer);
  
public:
    InputBuffer (sc_module_name insname);

	~InputBuffer ();

};

}} // end of soclib::caba

#endif /* VCI_DWT */

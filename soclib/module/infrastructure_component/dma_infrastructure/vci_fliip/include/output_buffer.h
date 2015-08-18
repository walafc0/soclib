
#ifndef OUTPUT_BUFFER
#define OUTPUT_BUFFER

#include <systemc.h>
#include <base_module.h>
#include <fifo_ports.h>

namespace soclib { namespace caba {

template <unsigned int WIDTH, typename IN_TYPE>
class OutputBuffer : BaseModule {
public:
    sc_in  < bool > p_clk;
    sc_in  < bool > p_resetn;
	sc_in  < IN_TYPE > p_input;
	FifoOutput< sc_uint< WIDTH > >  p_output;
	sc_out  < bool > p_writeok;
	sc_in < bool > p_write;

private:
	void transition();
	void genMealy();

	bool bufferFull;
	IN_TYPE buffer;

protected:
    SC_HAS_PROCESS (OutputBuffer);
  
public:
    OutputBuffer (sc_module_name insname);

	~OutputBuffer ();

};

}} // end of soclib::caba

#endif /* VCI_DWT */

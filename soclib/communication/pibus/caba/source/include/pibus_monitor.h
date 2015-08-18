
#ifndef SOCLIB_CABA_PIBUS_MONITOR_PORTS_H
#define SOCLIB_CABA_PIBUS_MONITOR_PORTS_H

#include "pibus_signals.h"

namespace soclib { namespace caba {

class PibusMonitor
{
public:
	sc_in<sc_dt::sc_uint<32> >		a;	// address
	sc_in<sc_dt::sc_uint<4> >		opc;	// codop
	sc_in<bool>			read;	// read transaction
	sc_in<bool>			lock;	// burst transaction
	sc_in<sc_dt::sc_uint<2> >		ack;	// response code
	sc_in<sc_dt::sc_uint<32> >		d;	// data
	sc_in<bool>			tout;	// time_out

#define __ren(x) x((name+"_" #x).c_str())
    PibusMonitor(const std::string &name = sc_gen_unique_name("pibus_monitor"))
		: __ren(a),
          __ren(opc),
          __ren(read),
          __ren(lock),
          __ren(ack),
          __ren(d),
          __ren(tout)
	{}
#undef __ren

        void operator() (Pibus &sig)
        {
                a       (sig.a);
                opc     (sig.opc);
                read    (sig.read);
                lock    (sig.lock);
                ack     (sig.ack);
                d       (sig.d);
                tout    (sig.tout);
        }
};

}} // end of namespace

#endif

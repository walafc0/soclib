
#ifndef SOCLIB_CABA_VCI_MY_TARGET_H
#define SOCLIB_CABA_VCI_MY_TARGET_H

#include <inttypes.h>
#include <systemc>
#include "caba_base_module.h"
#include "vci_target.h"

namespace soclib {
namespace caba {

    using namespace sc_core;

template<typename    vci_param>
class VciTestelgTarget
    : public soclib::caba::BaseModule
{
private:
    typedef uint32_t addr_t;
    typedef uint32_t data_t;

    enum testelg_target_fsm_state_e {
                TARGET_IDLE     = 0,
                TARGET_RSP      = 1,
                TARGET_EOP      = 2,
                TARGET_EOPWRITE = 3
    };

public:
    sc_in<bool> p_clk;
    sc_in<bool> p_resetn;
    soclib::caba::VciTarget<vci_param>   p_vci;
    
private:

    // STRUCTURAL PARAMETERS

    // REGISTERS
    sc_signal<int>      r_testelg_target_fsm;
    sc_signal<int>      r_testelg_target_dec;
    sc_signal<int>      r_reg1;
    sc_signal<int>      ROM[1024];
	sc_signal<int>      r_srcid;
    sc_signal<int>      r_trdid;
    sc_signal<int>      r_pktid;
    
protected:
    SC_HAS_PROCESS(VciTestelgTarget);

public:
    VciTestelgTarget(
        sc_module_name insname );

    ~VciTestelgTarget();

private:
    void transition();
    void genMoore();
};

}}

#endif /* SOCLIB_CABA_VCI_MY_TARGET_H */

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4


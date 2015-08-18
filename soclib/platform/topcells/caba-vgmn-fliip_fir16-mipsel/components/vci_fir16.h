

#ifndef VCI_FIR16
#define VCI_FIR16

#include <caba_base_module.h>
#include <mwmr_fir16.h>
#include <loadword.h>
#include <vci_accelerator_dma.h>


namespace soclib { namespace caba {

template < typename vci_param >
class VciFir16 : BaseModule {

public:
    sc_in  < bool > p_clk;
    sc_in  < bool > p_resetn;

    VciTarget<vci_param> p_vci_target;
    VciInitiator<vci_param> p_vci_initiator;

    sc_out < bool > p_irq;

private:

	// subcomponents
	soclib::caba::VciAcceleratorDma<vci_param, 1, 1, 32, 2, 0, 60> dma;
	soclib::caba::Mwmr_fir16<vci_param> ip;
	soclib::caba::LoadWord<32, 16, true, true> lw0;
	soclib::caba::LoadWord<16, 32, true, true> lw1;

	// signals
    sc_signal<bool> s_clk;
    sc_signal<bool> s_resetn;

	VciSignals<vci_param> s_vci_target;
	VciSignals<vci_param> s_vci_initiator;
    sc_signal<bool> s_irq;
    sc_signal<bool> s_irq0;

	sc_signal<bool> s_dma_start;
    sc_signal<bool> s_en_ce;
    ACCELERATOR_ENBL_SIGNALS< 32, 2> s_dma_enbl;

	FifoSignals<sc_uint<32> > s_dma_p_toip_data_0___lw0_p_input;
	FifoSignals<sc_uint<32> > s_dma_p_frip_data_0___lw1_p_output;

	sc_signal<bool> ip_p_from_ctrl_rok__lw0_p_output_w;
	sc_signal<bool> ip_p_from_ctrl_r__lw0_p_output_wok;
	sc_signal<bool> ip_p_to_ctrl_wok__lw1_p_input_r;
	sc_signal<bool> ip_p_to_ctrl_w__lw1_p_input_rok;

	sc_signal<sc_uint<16> > ip_p_from_ctrl_data__lw0_p_output_data_0;
	sc_signal<uint32_t> ip_p_from_ctrl_data__lw0_p_output_data_1;
	sc_signal<sc_uint<16> > ip_p_to_ctrl_data__lw1_p_input_data_0;
	sc_signal<uint32_t> ip_p_to_ctrl_data__lw1_p_input_data_1;

	void genMealy();

protected:
    SC_HAS_PROCESS (VciFir16);
  
public:
    VciFir16(sc_module_name insname, const MappingTable& mt,
			const IntTab& srcid, const IntTab& tgtid, int burst_size);

	~VciFir16();

};

}} // end of soclib::caba

#endif /* VCI_FIR16 */


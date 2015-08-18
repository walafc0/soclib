

#include <vci_fir16.h>

extern std::vector<int> intArray( const int length, ... );

#define tmpl(x) \
template < typename vci_param > \
x VciFir16<vci_param>

namespace soclib { namespace caba {

tmpl(void)::genMealy() {
	ip_p_from_ctrl_data__lw0_p_output_data_1 =
			(uint32_t)ip_p_from_ctrl_data__lw0_p_output_data_0.read();
	ip_p_to_ctrl_data__lw1_p_input_data_0 =
			sc_uint<16>(ip_p_to_ctrl_data__lw1_p_input_data_1.read());
}

tmpl(/**/)::VciFir16(
		sc_module_name insname, const MappingTable& mt,
		const IntTab& srcid, const IntTab& tgtid, int burst_size)
	: BaseModule(insname), 
	p_clk("clk"),
	p_resetn("resetn"),
	p_vci_target("target"),
	p_vci_initiator("initiator"),
	p_irq("irq"),
	dma("dma", mt, srcid, tgtid, burst_size),
	ip("ip", intArray(16, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0)),
	lw0("lw0"),
	lw1("lw1") {

	// netlist

	//p_irq(s_irq0);

	dma.p_clk(p_clk);
	dma.p_resetn(p_resetn);
	dma.p_vci_target(p_vci_target);
	dma.p_vci_initiator(p_vci_initiator);
	dma.p_irq(s_irq);
	dma.p_start(s_dma_start);
	dma.p_enbl(s_dma_enbl);
	dma.p_toip_data[0](s_dma_p_toip_data_0___lw0_p_input);
	dma.p_frip_data[0](s_dma_p_frip_data_0___lw1_p_output);

	lw0.p_clk(p_clk);
	lw0.p_resetn(p_resetn);
	lw0.p_input(s_dma_p_toip_data_0___lw0_p_input);
	lw0.p_output.w(ip_p_from_ctrl_rok__lw0_p_output_w);
	lw0.p_output.wok(ip_p_from_ctrl_r__lw0_p_output_wok);
	lw0.p_output.data(ip_p_from_ctrl_data__lw0_p_output_data_0);

	lw1.p_clk(p_clk);
	lw1.p_resetn(p_resetn);
	lw1.p_output(s_dma_p_frip_data_0___lw1_p_output);
	lw1.p_input.r(ip_p_to_ctrl_wok__lw1_p_input_r);
	lw1.p_input.rok(ip_p_to_ctrl_w__lw1_p_input_rok);
	lw1.p_input.data(ip_p_to_ctrl_data__lw1_p_input_data_0);

	ip.p_clk(p_clk);
	ip.p_resetn(p_resetn);
	ip.p_from_ctrl.rok(ip_p_from_ctrl_rok__lw0_p_output_w);
	ip.p_from_ctrl.r(ip_p_from_ctrl_r__lw0_p_output_wok);
	ip.p_from_ctrl.data(ip_p_from_ctrl_data__lw0_p_output_data_1);
	ip.p_to_ctrl.wok(ip_p_to_ctrl_wok__lw1_p_input_r);
	ip.p_to_ctrl.w(ip_p_to_ctrl_w__lw1_p_input_rok);
	ip.p_to_ctrl.data(ip_p_to_ctrl_data__lw1_p_input_data_1);

	SC_METHOD (genMealy);
	dont_initialize();
	sensitive
		<< ip_p_from_ctrl_data__lw0_p_output_data_0
		<< ip_p_to_ctrl_data__lw1_p_input_data_1;

}

tmpl(/**/)::~VciFir16() {
}

}} // end of soclib::caba


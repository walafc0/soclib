
==================================
How to use the ``soclib_desc`` API
==================================

Let's do a step-by-step use of the :py:mod:`soclib_desc` API::

  >>> from soclib_desc import description_files

Let's take a ``vci_xcache_wrapper`` module from the index, and put it
in a variable::

  >>> vci_xcache_mod = description_files.get_module("caba:vci_xcache_wrapper")

Object pointed by ``vci_xcache_mod`` implements the
:py:class:`~soclib_desc.module.ModuleInterface` protocol. We can list
template parameters to specify in order to create a `Specialization`::

  >>> for p in vci_xcache_mod.get_template_parameters():
  ...    print p
  ... 
  <Parameter Module: vci_param>
  <Parameter Module: iss_t>

We can create the specialization::

  >>> vci_xcache_spec = vci_xcache_mod.specialize(
  ... cell_size = 4, plen_size = 8, addr_size = 32, rerror_size = 1,
  ... clen_size = 1, rflag_size = 1, srcid_size = 8, pktid_size = 4,
  ... trdid_size = 4, wrplen_size = 1,
  ... iss_t = "common:mips32el")
  >>> vci_xcache_spec.__class__
  <class sd_parser.specialization.Specialization at 0x101468e20>

``vci_xcache_spec`` implements the
:py:class:`~soclib_desc.specialization.SpecializationInterface`
protocol::

  >>> vci_xcache_spec.get_entity_name()
  'soclib::caba::VciXcacheWrapper<soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1 > ,soclib::common::Mips32ElIss > '
  >>> for spec in vci_xcache_spec.get_used_modules():
  ...    print spec.get_entity_name()
  ... 
  soclib::common::AddressDecodingTable<uint32_t,bool > 
  soclib::WriteBuffer
  soclib::common::Iss2
  soclib::common::AddressDecodingTable<uint64_t,int > 
  soclib::common::AddressMaskingTable<uint64_t > 
  sc_core::sc_in<sc_dt::sc_uint<32 >  > 
  sc_core::sc_in<sc_dt::sc_uint<1 >  > 
  soclib::common::AddressDecodingTable<uint32_t,int > 
  sc_core::sc_in<bool>
  soclib::caba::VciXcacheWrapper<soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1 > ,soclib::common::Mips32ElIss > 
  sc_core::sc_out<bool>
  sc_core::sc_in<sc_dt::sc_uint<8 >  > 
  soclib::common::AddressMaskingTable<uint32_t > 
  soclib::common::IntTab
  soclib::GenericCache<uint32_t > 
  soclib::common::MappingTable
  soclib::caba::BaseModule
  soclib::common::Mips32Iss
  soclib::caba::VciInitiator<soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1 >  > 
  soclib::common::BaseModule
  sc_core::sc_in<sc_dt::sc_uint<4 >  > 
  soclib::common::AddressDecodingTable<uint64_t,bool > 
  sc_core::sc_in<sc_dt::sc_uint<2 >  > 
  soclib::common::Segment
  soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1 > 
  soclib::common::Mips32ElIss
  sc_core::sc_in<bool>

And finally we can get the :py:class:`builder
<soclib_desc.component_builder.ComponentBuilderInterface>`, its
results :py:class:`~soclib_builder.bblock.BBlock` objects, and the
associated :py:class:`~soclib_builder.action.Action`::

  >>> b = vci_xcache_spec.builder()
  >>> for r in b.results():
  ...    print r.generator
  ... 
  <CxxCompile: ['{caba:vci_xcache_wrapper_7759636b764157b_soclib::caba::VciXcacheWrapper<soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1_>_,soclib::common::Mips32ElIss_>__vci_xcache_wrapper.cpp}'] -> ['{caba:vci_xcache_wrapper_7759636b764157b_soclib::caba::VciXcacheWrapper<soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1_>_,soclib::common::Mips32ElIss_>__vci_xcache_wrapper.o}'] + []>

We can also list the ports of the `Specialization`::

  >>> pl = vci_xcache_spec.port_list()
  >>> for name, portdecl in pl.items():
  ...   print name, portdecl.specialization.get_entity_name()
  ... 
  p_vci soclib::caba::VciInitiator<soclib::caba::VciParams<4,8,32,1,1,1,8,4,4,1 >  > 
  p_clk sc_core::sc_in<bool>
  p_resetn sc_core::sc_in<bool>
  p_irq sc_core::sc_in<bool>

And retrieve the corresponding headers::

  >>> for f in vci_xcache_spec.get_header_files():
  ...   print f
  /soclib/soclib/module/internal_component/vci_xcache_wrapper/caba/source/include/vci_xcache_wrapper.h

Or get the instance parameter list::

  >>> for p in vci_xcache_spec.get_instance_parameters():
  ...    print p
  ... 
  <Parameter Int: ident>
  <Parameter Module: mt>
  <Parameter IntTab: index>
  <Parameter Int: icache_ways>
  <Parameter Int: icache_sets>
  <Parameter Int: icache_words>
  <Parameter Int: dcache_ways>
  <Parameter Int: dcache_sets>
  <Parameter Int: dcache_words>

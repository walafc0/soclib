
import dsx
import soclib

def VgmnNoirqMono( nbcpu = 1,
                   nbram = 2,
                   with_gdb = False,
                   with_memchecker = False,
                   with_profiler = False,
                   icache_lines = 32,
                   icache_words = 8,
                   dcache_lines = 32,
                   dcache_words = 8 ):
    arch = soclib.Architecture(
                         cell_size = 4,
                         plen_size = 9,
                         addr_size = 32,
                         rerror_size = 1,
                         clen_size = 1,
                         rflag_size = 1,
                         srcid_size = 8,
                         pktid_size = 8,
                         trdid_size = 8,
                         wrplen_size = 1
                         )

    arch.nb_ram = nbram
    
    mt = arch.create('common:mapping_table',
              'mapping_table',
              addr_bits = [8],
              srcid_bits = [8],
              cacheability_mask = 0x300000)
    arch.create('common:loader', 'loader')

    vgmn = arch.create('caba:vci_vgmn', 'vgmn0', min_latency = 2, fifo_depth = 8, mt = mt)

    for i in range(nbcpu):
        add = dict(iss_t='common:mips32el')
        toreplace = 'iss_t'
        for doit, target_name, wrapper in (
            (with_memchecker, 'iss_memchecker_t', 'common:iss_memchecker'),
            (with_gdb, 'gdb_iss_t', 'common:gdb_iss'),
            (with_profiler, 'profiler_iss_t', 'common:iss2_profiler'),
            ):
            if doit:
                add[target_name] = add[toreplace]
                add[toreplace] = wrapper
        print add
        xcache = arch.create('caba:vci_xcache_wrapper',
                             'mips%d'%i,
                             ident = i,
                             icache_ways = 1,
                             icache_sets = icache_lines,
                             icache_words = icache_words,
                             dcache_ways = 1,
                             dcache_sets = dcache_lines,
                             dcache_words = dcache_words,
                             **add
                             )

        vgmn.to_initiator.new() // xcache.vci

    for task, name, addr, args in (
        ('tg', 'tg0', 0x70200000, {'FILENAME':'"plan.jpg"'}),
        ('ramdac', 'ramdac0', 0x71200000, {'WIDTH':"48",'HEIGHT':"48"}),
#       ('idct', 'idct0', 0x72200000, {"EXEC_TIME":"1024",'WIDTH':"48",'HEIGHT':"48"}),
        ):

        model = dsx.TaskModel.getByName(task).getImpl(soclib.HwTask)
        ctrl, coproc = model.instanciate(arch, name, name+'_ctrl', defines = args)
        ctrl.addSegment(name+'_ctrl', addr, 0x100, False)
        ctrl.vci_initiator // vgmn.to_initiator.new()
        ctrl.vci_target // vgmn.to_target.new()

    for i in range(nbram):
        ram = arch.create('caba:vci_ram', 'ram%d'%i)
        base = 0x10000000*i+0x10000000
        ram.addSegment('cram%d'%i, base, 0x100000, True)
        ram.addSegment('uram%d'%i, base + 0x200000, 0x100000, False)
        ram.vci // vgmn.to_target.new()
    ram.addSegment('boot', 0xbfc00000, 0x800,True)
    ram.addSegment('excep', 0x80000080, 0x800,True)

    tty = arch.create('caba:vci_multi_tty', 'tty0', names = ['tty0'])
    tty.addSegment('tty0', 0x90200000, 0x50, False)
    tty.vci // vgmn.to_target.new()

    return arch

if __name__ == '__main__':
    hard = VgmnNoirqMono(nbcpu = 5,
#                    with_gdb = True,
                     with_memchecker = True,
                     )
    hard.generate(soclib.PfDriver())
    

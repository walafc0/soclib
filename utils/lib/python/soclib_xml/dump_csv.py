
# SOCLIB_GPL_HEADER_BEGIN
# 
# This file is part of SoCLib, GNU GPLv2.
# 
# SoCLib is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# SoCLib is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with SoCLib; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
# 
# SOCLIB_GPL_HEADER_END
# 
# Copyright (c) UPMC, Lip6, SoC
#         Nicolas Pouillon <nipo@ssji.net>, 2010

from soclib_desc import parameter

class CsvDumper:
    def __init__(self, fd):
        self.__fd = fd
        self.warned = False
        
    def begin(self, zone):
        self.__zone = zone
        self.line('BEGIN '+self.__zone)
    def end(self):
        self.line('END '+self.__zone)
        self.line()

    def line(self, *args):
        print >>self.__fd, ','.join(args)

    def convert(self, m):
        module = m.getModuleName()
        
        self.begin('IDENTIFIER')
        self.line('vendor', 'library', 'name', 'version')
        self.line('lip6', 'soclib', module, 'svn')
        self.end()

        self.begin('VIEW')
        self.line('name', 'language', 'model_name', 'fileset_ref')
        self.line('SystemC_CABA', 'SystemC', module, 'sc_source')
        self.end()

        self.begin('TMPL_PARAMETERS')
        self.line('name', 'value')
        for p in m['tmpl_parameters']:
            self.line(p.name, '')
        self.end()

        sc_ports = []
        meta_ports = []
        for p in m['ports']:
            ptype, name, count = p.getMetaInfo()
            if isinstance(count, (parameter.Base, parameter.Constant)):
                print "%s: Cant serialize count reference in port '%s'"%(module, name)
                self.warned = True
                continue
            if count is not None:
                try:
                    print count, type(count)
                    count = count.resolve()
                except:
                    print "%s: Cant serialize count reference in port '%s'"%(module, name)
                    self.warned = True
                    continue
            # TODO: split base SC types from meta ones
            try:
                abs, _name = ptype.split(":", 1)
                mode, way = _name.split("_", 1)
            except:
                mode = "meta"
            if mode in ['bit', 'clock']:
                if count:
                    sc_ports.append((name, way, str(-1), '0'))
                else:
                    sc_ports.append((name, way, '', ''))
            else:
                meta_ports.append((name, ptype, 'master'))

        if meta_ports:
            self.begin('PORT_TLM')
            self.line('name', 'direction')
            for x in meta_ports:
                self.line(x[0], 'requires')
            self.end()

            self.begin('INTERFACES')
            self.line('interface_name', 'master', 'slave', 'system', 'logical_name', 'port_name')
            for x in meta_ports:
                self.line(x[1],
                          ['', '-'][x[2] == 'master'],
                          ['', '-'][x[2] == 'slave'],
                          ['', '-'][x[2] not in ['master', 'slave']],
                          x[0],
                          x[0])
            self.end()

        if sc_ports:
            self.begin('PORT_WIRE')
            self.line('name', 'direction')
            for x in sc_ports:
                self.line(*x)
            self.end()

        self.begin('PARAMETERS')
        self.line('parameter_name', 'parameter_id')
        for p in m['instance_parameters']:
            self.line(p.name, p.name)
        self.end()

        self.begin('FILE_SET')
        self.line('fileset_name', 'file_path', 'file_type', 'is_include')
        for s in m['implementation_files']:
            self.line('sc_source', s, 'systemCSource-2.1', 'false')
        for s in m['header_files']:
            self.line('sc_source', s, 'systemCSource-2.1', 'true')
        self.end()

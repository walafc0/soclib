
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

from soclib_desc import description_files
import os, os.path
from dump_csv import CsvDumper
import subprocess

_green = '\x1b[32m'
_yellow = '\x1b[33m'
_red = '\x1b[31m'
_normal = '\x1b[m'

def csv2xml(csv2ipxact, template, csv, xml, log):
    subprocess.Popen([csv2ipxact,
                      '-c', csv,
                      template,
                      '-o', xml,
                      '-log', log])

def do_convert(csv2ipxact, template, m):
    base = m.fileName().rsplit('.', 1)[0]
    csv = base+'.csv'
    xml = base+'.xml'
    log = base+'.txt'
    fd = open(csv, 'w')
    d = CsvDumper(fd)
    d.convert(m)
    fd.close()
    csv2xml(csv2ipxact, template, csv, xml, log)
    return d.warned

def convert_all(levels = "caba,tlmdt,common"):
    csv2ipxact = os.getenv("csv2ipxact")
    template = os.getenv("template")
    levels = levels.split(',')
    warned = ok = failed = 0
    
    for m in description_files.get_all_modules():
        if m['abstraction_level'] in levels:
            print m.getModuleName()+'...',
            try:
                w = do_convert(csv2ipxact, template, m)
                if w:
                    warned += 1
                    print _yellow+" warned"+_normal
                else:
                    ok += 1
                    print _green+" OK"+_normal
            except Exception, e:
                failed += 1
                print _red+" failed"+_normal+':', e
    print "%d modules"%(warned+ok+failed)
    print "OK: %d, warned: %d, failed: %d"%(ok, warned, failed)

if __name__ == "__main__":
    main()



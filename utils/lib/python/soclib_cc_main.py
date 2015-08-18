#!/usr/bin/env python

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

__author__ = 'Nicolas Pouillon, <nipo@ssji.net>'
__copyright__ = 'UPMC, Lip6, SoC, 2007-2010'
__license__ = 'GPL-v2'
__id__ = "$Id: soclib_cc_main.py 2381 2013-06-27 12:48:21Z porquet $"
__version__ = "$Revision: 2381 $"

import os, os.path
import sys
from optparse import OptionParser, OptionGroup, TitledHelpFormatter

import soclib_desc
import soclib_desc.description_files
import soclib_cc

def parse_args(available_configs):
    todb = []
    one_args = {}

    def buggy_callback(option, opt, value, parser):
        todb.append(value)
    def one_arg_callback(option, opt, value, parser):
        k, v = value.split('=', 1)
        try:
            v = int(v)
        except:
            pass
        one_args[k] = v

    epilog = """

In the above options, MODULE is always a module name in the
abstraction_level:module_name format (e.g. caba:vci_ram).

Run soclib-cc --examples to see some common command-line examples.
"""
    f = TitledHelpFormatter()
    f.format_epilog = lambda x: x
    try:
        parser = OptionParser(
            usage="%prog [ -m mode ] [ -t config ] [ -vqd ] [ -c -o output input | -p pf_desc ]",
            epilog = epilog,
            formatter = f)
    except TypeError:
        parser = OptionParser(usage="%prog [ -m mode ] [ -t config ] [ -vqd ] [ -c -o output input | -p pf_desc ]")

    parser.add_option('--examples', dest = 'examples',
                      action='store_true',
                      help="Show some example commands")

    group = OptionGroup(parser, "Pretty printing")
    parser.add_option_group(group)
    
    group.add_option('-q', '--quiet', dest = 'quiet',
                      action='store_true',
                      help="Print nothing but errors")
    group.add_option('-v', '--verbose', dest = 'verbose',
                      action='store_true',
                      help="Chat a lot")
    group.add_option('-d', '--debug', dest = 'debug',
                      action='store_true',
                      help="Print too much things")
    group.add_option('-P', '--progress_bar', dest = 'progress_bar',
                      action='store_true',
                      help="Print evolution with a progress bar")

    group = OptionGroup(parser, "Environment tweaks",
                        "These options can change the build flags or the "
                        "index of modules")
    parser.add_option_group(group)

    group.add_option('-I', dest = 'includes', metavar="DIR",
                      action='append', nargs = 1,
                      help="Append directory to .sd search path")
    group.add_option('-m', '--mode', dest = 'mode',
                      action='store', nargs = 1,
                      default = 'release',
                      help="Select mode: *release|debug|prof",
                      choices = ("release", "debug", "prof"))
    group.add_option('-t', '--type', dest = 'type',
                      action='store',
                      choices = available_configs,
                      help="Use a different configuration: <*%s>"%(', '.join(available_configs)))
    group.add_option('-b', '--buggy', nargs = 1, type = "string", metavar="MODULE",
                      action='callback', callback = buggy_callback,
                      help="Put MODULE in debug mode (disable opt, set SOCLIB_MODULE_DEBUG preprocessor variable)")

    group = OptionGroup(parser, "Information gathering",
                        "These options show various information "
                        "about the currently indexed modules "
                        "and SoCLib environment")
    parser.add_option_group(group)

    group.add_option('--getpath', dest = 'getpath',
                      action='store_true',
                      help="Print soclib path")
    group.add_option('-l', dest = 'list_descs',
                      action='store_const', const = "long",
                      help="List known descriptions == --list-descs=long")
    group.add_option('--list-descs', dest = 'list_descs', metavar = "FORMAT",
                      action='store', nargs = 1, choices = ("long", "names"),
                      help="List known descriptions. Format may be 'long' or 'names'")
    group.add_option('--list-files', dest = 'list_files', metavar = "MODULE",
                      action='store', nargs = 1, type = 'string',
                      help="List files belonging to a given module")
    group.add_option('--complete-name', dest = 'complete_name', metavar="MODULE",
                      action='store', nargs = 1, type = 'string',
                      help="Complete module name starting with ...")
    group.add_option('--complete-separator', dest = 'complete_separator', metavar="SEP",
                      action='store', nargs = 1, type = 'string',
                      default = ':',
                      help="Complete words splitted by this arg")
    group.add_option('--getflags', dest = 'getflags', metavar = "KIND",
                      action='store', choices = ("cflags",),
                      help="Get flags of some KIND <cflags>")
    group.add_option('--embedded-cflags', dest = 'embedded_cflags',
                      action='store_true',
                      help="Print software include directories C flags")

    group = OptionGroup(parser, "Compilation tweaks",
                        "These options change various behaviors of the compilation itself")
    parser.add_option_group(group)

    group.add_option('-j', '--jobs', dest = 'jobs', metavar = "N",
                      action='store', type = 'int',
                      default = 0,
                      help="Allow N parallel jobs")
    group.add_option('--work', dest = 'workpath', metavar = "DIR",
                      action='store',
                      help="When using ModelSim, use this work DIR")

    group = OptionGroup(parser, "What to do",
                        "These options tell soclib-cc what to do")
    parser.add_option_group(group)

    group.add_option('-c', '--compile', dest = 'compile',
                      action='store_true',
                      help="Do a simple compilation, not a linkage")
    group.add_option('-x', '--clean', dest = 'clean',
                      action='store_true',
                      help="Clean all outputs, only compatible with -p")
    group.add_option('-X', '--clean-cache', dest = 'clean_cache',
                      action='store_true',
                      help="Clean .desc file cache")
    group.add_option('-o', '--output', dest = 'output',
                      action='store', type = 'string',
                      help="Select output file")
    group.add_option('-F', '--format', dest = 'formatter',
                      action = 'store', nargs = 1, type = 'string',
                      help = 'Use a given formatter class name to format build actions')
    group.add_option('--tags', dest = 'tags',
                      action='store_true',
                      help="Ouput tags database, only compatible with -p")
    group.add_option('--tags-type', dest = 'tags_type', metavar = "FORMAT",
                      action='store', nargs = 1, choices = ("cscope", "ctags"),
                      default = "cscope",
                      help="Specify tags format: cscope or ctags [default is %default]")
    group.add_option('--tags-output', dest = 'tags_output', metavar = "FILE",
                      action='store', nargs = 1, type = "string",
                      help="Specify tags filename (e.g. 'cscope.out', 'tags', etc.)")

    group.add_option('-p', '--platform', dest = 'platform', metavar="PLATFORM_DESC",
                      action='store', type = 'string',
                      help="Use a platform description PLATFORM_DESC")
    group.add_option('-1', '--one-module', nargs = 1, type = "string", metavar="MODULE",
                      action='store', dest = 'one_module',
                      help="Only try to compile MODULE (try -a for the parameters)")
    group.add_option('-a', '--arg', nargs = 1, type = "string", metavar="KEY=VALUE",
                      action='callback', callback = one_arg_callback,
                      help="Specify arguments for one-module build")

    group = OptionGroup(parser, "Debugging")
    parser.add_option_group(group)

    group.add_option('--dump-config', dest = 'dump_config',
                      action='store_true',
                      help="Dump configuration")
    group.add_option('--bug-report', dest = 'bug_report',
                      action='store_true',
                      help="Create a bug-reporting log")
    group.add_option('--auto-bug-report', dest = 'auto_bug_report', metavar = "METHOD",
                      action='store', nargs = 1,
                      help="Auto report bug. Methods allowed: openbrowser, *none",
                      choices = ("openbrowser", "none"))

    parser.set_defaults(auto_bug_report = "none",
                        includes = [],
                        workpath = 'work',
                        embedded_cflags = False)
    opts, args = parser.parse_args()

    return opts, args, todb, one_args, parser

def main():
    from soclib_cc.config import config

    opts, args, todb, one_args, parser = parse_args(config.available())

    if opts.examples:
        print """
Some common command lines:

* Compile a complete platform described in platform_desc

  - with default configuration

    $ soclib-cc -P -o system.x -p platform_desc

  - with debug mode

    $ soclib-cc -P -o system.x -p platform_desc -m debug

  - with some_config (if "some_config" exists in your configuration)

    $ soclib-cc -P -o system.x -p platform_desc -t some_config

  - compiling with only the caba:vci_xcache_wrapper in debug mode

    $ soclib-cc -P -o system.x -p platform_desc -b caba:vci_xcache_wrapper

* Compile just the FifoReader to fifo_reader.o

  $ soclib-cc -v -c -o fifo_reader.o -1 caba:fifo_reader -a word_t=int32_t

* Looking for the vci_ram implementation code ?

  $ soclib-cc --list-files=caba:vci_ram
"""
        return 0

    from soclib_cc import bugreport
    bugreport.bootstrap(opts.bug_report, opts.auto_bug_report)

    if opts.getpath:
        print config.path
        return 0

    # options for which it would be safer to remove the current directory, so
    # that it doesn't scan .sd files that don't belong to soclib
    if ( opts.list_descs
            or opts.list_files
            or opts.complete_name):
        config.desc_paths.remove(os.getcwd())

    config = setup_config(opts)

    if opts.dump_config:
        print str(config)
        return 0

    for value in todb:
        soclib_desc.description_files.get_module(value).set_debug_mode()

    ## Done with setup

    if opts.clean_cache:
        soclib_desc.description_files.cleanup()
        return 0
    
    if opts.one_module:
        return compile_one_module(opts.output, opts.one_module, one_args, opts)
    
    if opts.list_files:
        m = soclib_desc.description_files.get_module(opts.list_files)
        for h in m.related_files():
            print h
        return 0
    
    if opts.list_descs is not None:
        return list_descs(opts.list_descs)
    
    if opts.complete_name is not None:
        return complete_name(opts.complete_name, opts.complete_separator)
    
    if opts.getflags:
        if opts.getflags == 'cflags':
            print ' '.join(config.getCflags())
        return 0

    if opts.platform:
        return compile_platform(opts.platform, opts)
    
    if opts.compile:
        from soclib_cc.actions.cxx import CxxCompile
        CxxCompile(config.output, args[0]).process()
        return 0
    
    parser.print_help()
    return 1

def compile_one_module(output, one_module, one_args, opts):
    from soclib_builder.todo import ToDo
    from soclib_cc.actions.cxx import CxxMkobj, CCompile
    todo = ToDo()
    class foo:
        def fullyQualifiedModuleName(self, d):
            return d
        def putArgs(self, d):
            pass
    out = []
    f = foo()
    for module in one_module.split(','):
        mod = soclib_desc.description_files.get_module(module)
        spec = mod.specialize(**one_args)
        for b in spec.get_used_modules():
            for o in b.builder().results():
                todo.add(o)
                out.append(o)
        if output:
            cobjs = filter(
                lambda x: isinstance(x.generator, CCompile),
                out)
            todo.add(CxxMkobj(output, cobjs).dests[0])
    return todo_do(todo, opts)

def complete_name(start, separator):
    completions = set()
    suffix = start
    for sep in separator:
        suffix = suffix.split(sep)[-1]
    prefix_len = len(start)-len(suffix)
    for mod in soclib_desc.description_files.get_all_modules():
        name = mod.name
        if name.startswith(start):
            client = name[prefix_len:]
            completions.add(client)
    print '\n'.join(completions)
    return 0

def list_descs(mode):
    if mode == 'long':
        for desc in soclib_desc.description_files.get_all_modules():
            print desc.name
            for f in desc.related_files():
                print ' -', f
    elif mode == 'names':
        for desc in soclib_desc.description_files.get_all_modules():
            print desc.name
    else:
        print "Please give arg 'long' or 'names'"
        return 1
    return 0

def build_tags(todo, opts):
    list_files = []
    for m in todo.get_used_modules():
        for h in m.get_header_files():
            list_files.append(h)
        for i in m.get_implementation_files():
            list_files.append(i)
    list_files = list(set(list_files)) # remove duplicates

    import subprocess
    tags_extra_arg = ""
    if opts.tags_output != None:
        tags_extra_arg = "-f" + opts.tags_output
    if opts.tags_type == "cscope":
        p = subprocess.Popen(["cscope", "-b", "-i-", tags_extra_arg], stdin = subprocess.PIPE)
    else:
        p = subprocess.Popen(["ctags", "-L -", tags_extra_arg], stdin = subprocess.PIPE)

    p.communicate("\n".join(list_files))
    p.wait()
    return 0

def compile_platform(platform, opts):
    import sd_parser.platform as pf
    from soclib_cc.config import config
    
    if not config.quiet and not opts.embedded_cflags:
        print "soclib-cc: Entering directory `%s'"%(
            os.path.abspath(os.getcwd()))
    todo = pf.parse(platform)
    return todo_do(todo, opts)

def todo_do(todo, opts):
    if opts.formatter:
        todo.format(opts.formatter, opts.output)
    elif opts.clean:
        todo.clean()
        soclib_desc.description_files.cleanup()
    elif opts.embedded_cflags:
        print todo.embedded_code_cflags()
    elif opts.tags:
        build_tags(todo, opts)
    else:
        from soclib_builder.action import ActionFailed
        try:
            todo.process()
        except ActionFailed, e:
            print "soclib-cc: *** Compilation action failed: %s. Stop."%str(e)
            return 2
    return 0

def setup_config(opts):
    from soclib_cc.config import config
    if opts.type:
        config.set_default(opts.type)

    for path in opts.includes:
        soclib_desc.description_files.add_path(path, False)

    config.set("mode", opts.mode)
    config.set("verbose", opts.verbose)
    config.set("debug", opts.debug)
    config.set("quiet", opts.quiet)
    config.set("workpath", os.path.abspath(opts.workpath))
    if opts.jobs:
        config.toolchain.set("max_processes", opts.jobs)

    if opts.progress_bar:
        # Dont put progress bar in emacs, it is ugly !
        if os.getenv('EMACS') == 't':
            print 'Progress-bar disabled, you look like in emacs'
        else:
            config.set("progress_bar", True)
            config.set("quiet", True)

    config.set("output", "system.x")

    if opts.compile:
        config.set("output", "mod.o")

    if opts.output:
        config.set("output", opts.output)

    return config

if __name__ == '__main__':
    sys.exit(main())

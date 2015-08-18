#!/usr/bin/env python

import csv
from StringIO import StringIO
import subprocess
import tempfile
import os, os.path
import pickle

class Plotter:
    def __init__(self, output, title = None, terminal = 'pdf'):
        self.__output = output
        self.__terminal = terminal
        self.__plots = []

    def add_datafile(self,
                     dataset, columns,
                     style,
                     title = None,
                     axes = None,
                     options = ""):
        self.__plots.append()

class Handler:
    def __init__(self, filename):
        self.__filename = filename

    def __csv(self):
        return csv.reader(open(self.__filename))

    def __fills_get(self, needed_mwmr):
        r = [(1,0)]
        for line in self.__csv():
            name, cycle, mwmr_name, srcid, op = line[:5]
            cycle = int(cycle)
            srcid = int(srcid)
            args = line[5:]
            if mwmr_name != needed_mwmr:
                continue
            if op != "write_usage":
                continue
            r.append((cycle, int(args[0])))
        return r

    def op_extract(self, name):
        items = self.__fills_get(name)
        acc = 0.
        values = {}
        for (begin, val), (end, foo) in zip(items, items[1:]):
            time = end - begin
            acc += float(val) * time
            values[val] = values.get(val, 0) + time
        begin = items[0][0]
        end = items[-2][0]
        print "Begin:", begin
        print "End:", end
        print "Acc:", acc
        print "Mean:", acc / (end - begin)
        for val, time in sorted(values.items()):
            print "Val", val, time, "cycles"

    def op_plot(self, name):
        items = self.__fills_get(name)
        for time, val in items:
            print time, val

    def op_ios(self, name, output):
        items = self.__fills_get(name)
        data = tempfile.NamedTemporaryFile()
        rcumul = 0.
        wcumul = 0.
        rbw = []
        wbw = []
        ribw, wibw = 0., 0.
        bwn, bwtot = 0, 0.
        last_read = last_write = items[0][0]
        def tx(t):
            d = sum(map(lambda x:x[0], t))
            if d:
                return sum(map(lambda x:x[1], t))/d
            return 0

        for (begin, before), (end, after) in zip(items, items[1:]):
            time = end - begin
            delta = after - before
            ribw = wibw = 0.

            if delta == 0:
                pass

            elif delta > 0:
                wcumul += delta
                wibw = float(delta) / (end - last_write)
                bwtot += wibw
                bwn += 1
                wbw.append((end - last_write, float(delta)))
                wbw = wbw[-10:]
                last_write = end

            else:
                delta = -delta

                rcumul += delta
                ribw = float(delta) / (end - last_read)
                bwtot += ribw
                bwn += 1
                rbw.append((end - last_read, float(delta)))
                rbw = rbw[-10:]
                last_read = end

            print >> data, begin, rcumul, rcumul/begin, wcumul, wcumul/begin, ribw, wibw, tx(rbw), tx(wbw)
#            print >> data, end-1, rcumul, rbw, wcumul, wbw
        data.flush()


        control = StringIO()
        print >> control, """
set terminal pdf
set output "%(output)s"
set title "Data through %(name)s"

set xlabel "Time (cycles)"
set xrange [0 : %(max)d]
set x2range [0 : %(max)d]
set y2range [0 : %(maxy2)f]
set ylabel "bytes"
set y2label "byte/cycle"
set ytics nomirror
set format x "%%.0s %%c"
set format y "%%.0s %%c"
set format y2 "%%.0se%%S"
#set key outside center bottom horizontal Left title 'Legend' box 3
set key outside center bottom horizontal
set y2tics
#set xtics out rotate by -30
set tics out
plot "%(data)s" using 1:2 with lines title 'Cumulative read' axes x1y1 \\
   , "%(data)s" using 1:4 with lines title 'Cumulative write' axes x1y1 \\
   , "%(data)s" using 1:8 with dots title 'Avg bw / last 10 reads' axes x2y2 \\
   , "%(data)s" using 1:9 with dots title 'Avg bw / last 10 writes' axes x2y2 \\
   , "%(data)s" using 1:5 with lines title 'Cumulative write bw' axes x2y2 \\
   , "%(data)s" using 1:3 with lines title 'Cumulative read bw' axes x2y2 \\

"""%dict(max = items[-1][0],
         maxy2 = wcumul/begin * 2,
         output = output,
         name = name,
         data = data.name,
         )

        proc = subprocess.Popen(["gnuplot"], stdin = subprocess.PIPE)
        proc.communicate(control.getvalue())
        data.close()
#        print control.getvalue()

    def op_fillness(self, name, output):
        items = self.__fills_get(name)
        data = tempfile.NamedTemporaryFile()
        control = StringIO()

        fillness = {}
        for (begin, before), (end, after) in zip(items, items[1:]):
            fillness[before] = fillness.get(before, 0.) + end - begin

        tot = sum(fillness.values())
        for bytes, cycles in sorted(fillness.items()):
            print >> data, bytes, cycles, '"%d (%.02f%%)"'%(cycles, 100*cycles/tot)

        print >> control, """
set terminal pdf
set output "%(output)s"
set title "Fillness of %(name)s"
set yrange [0 : %(maxy)d]
set xlabel "Usage (bytes)"
set ylabel "cycles"
set xtics (%(xtics)s) scale 0.
set format y "%%.0s %%c"
set ytics nomirror
set tics out
plot "%(data)s" using 1:2 with boxes notitle,\\
     "%(data)s" using 1:2:3 with labels offset 0,1 notitle 
"""%dict(output = output,
         name = name,
         maxy = 1.15*max(fillness.values()),
         data = data.name,
         xtics = ','.join(map(str, fillness.keys()))
         )

        data.flush()
        proc = subprocess.Popen(["gnuplot"], stdin = subprocess.PIPE)
        proc.communicate(control.getvalue())
        data.close()
#        print control.getvalue()

def main():
    import sys

    log = sys.argv[1]
    command = sys.argv[2]
    args = sys.argv[3:]
    
    handler = Handler(log)
    getattr(handler, 'op_'+command)(*args)

if __name__ == "__main__":
    main()

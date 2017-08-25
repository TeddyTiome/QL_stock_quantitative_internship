#!/usr/bin/python

import os, sys, math
import numpy as np
from scipy.stats.stats import pearsonr as pr
from optparse import OptionParser

parser = OptionParser()
parser.add_option('-s', '--startdate', dest='startdate', type='int', action='store', default='0')
(options, args) = parser.parse_args(sys.argv)
startdate = options.startdate

# check arg
if len(args) < 5:
    msg = """Useage: ./bcorr pnl_1 pnl_2 ... pnl_n column_number_for_pnl1 column_number_for_pnl_(2...n)
         column number for pnl_2 ... pnl_n should be the same"""
    print msg
    sys.exit(1)
#print args
mypnl_name = args[1]
otherpnl_name = args[2:len(args)-2]
col1 = int(args[len(args)-2])
col2 = int(args[len(args)-1])


# read mypnl
mypnl = {}
f = open(mypnl_name, 'r')
for line in f:
    line = line.strip('\n').split()
    if line[col1-1] == '0': # skip empty lines
        continue
    if int(line[0]) < startdate:
        continue
    mypnl[ line[0] ] = float(line[col1-1])
f.close()

#read other pnls
otherpnl = {}
for op in otherpnl_name:
    pnl = os.path.split(op)[-1]
    otherpnl[pnl] = {}
    f = open(op, 'r')
    for line in f:
        #line = line.strip('\n').split(' ') # problomatic when there are multiple spaces
        line = line.strip('\n').split() 
        if line[col2-1] == '0': # skip empty lines
            continue
        if int(line[0]) < startdate:
            continue
        otherpnl[pnl][ line[0] ] = float(line[col2-1])
    f.close()

# cal corr
for op in otherpnl_name:
    pnl = os.path.split(op)[-1]
    v1 = []
    v2 = []
    for date in mypnl.keys():
        if not date in otherpnl[pnl]:
            continue
        if np.isnan(mypnl[date]) or np.isnan(otherpnl[pnl][date]):
            continue
        v1.append(mypnl[date])
        v2.append(otherpnl[pnl][date])
    if len(v1) < 2:
        continue
    r, p = pr(v1, v2)
    sr = '%6f  %s' % (r, op)
    if r > 0:
        sr = '+%6f  %s' % (r, op)
    print sr

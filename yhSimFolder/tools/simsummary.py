#!/usr/bin/python

import os, sys, re, math, getopt

def doStats(XX, date, pnl, long, short, ret, sh_hld, sh_trd, b_share, t_share, longNum,shortNum):
  if (XX not in stats):
    stats[XX] = {}
    stats[XX]['dates'] = []
    stats[XX]['pnl'] = 0
    stats[XX]['long'] = 0
    stats[XX]['short'] = 0
    stats[XX]['sh_hld'] = 0
    stats[XX]['sh_trd'] = 0
    stats[XX]['avg_ret'] = 0
    stats[XX]['b_sh'] = 0
    stats[XX]['t_sh'] = 0
    stats[XX]['drawdown'] = 0
    stats[XX]['dd_start'] = 0
    stats[XX]['dd_end'] = 0
    stats[XX]['up_days'] = 0
    stats[XX]['days'] = 0
    stats[XX]['xsy'] = 0
    stats[XX]['xsyy'] = 0
    stats[XX]['longNum'] = 0
    stats[XX]['shortNum'] = 0



  stats[XX]['dates'].append(date)
  stats[XX]['pnl'] += pnl
  stats[XX]['long'] += long
  stats[XX]['short'] += short
  stats[XX]['sh_hld'] += sh_hld
  stats[XX]['sh_trd'] += sh_trd
  stats[XX]['avg_ret'] += ret
  stats[XX]['b_sh'] += b_share
  stats[XX]['t_sh'] += t_share
  #stats[XX]['drawdown'] = 0
  #stats[XX]['dd_start'] = 0
  #stats[XX]['dd_end'] = 0
  stats[XX]['xsy'] += ret
  stats[XX]['xsyy'] += ret * ret
  if (pnl > 0):
    #stats[XX]['up_days'] = 0
    stats[XX]['up_days'] += 1
  if (long != 0  or short != 0):
    #stats[XX]['days'] = 0
    stats[XX]['days'] += 1
  stats[XX]['longNum'] += longNum
  stats[XX]['shortNum'] += shortNum



# global variables
LONGSHORT_SCALE = 1e6
PNL_SCALE = 1e6
TRADE_DAYS = 242 # 250

DD_start = 0
DD_setst = 0
DD_sum = 0
stats = {}

pnlfile = ''
type = 'yearly'
sdate = -1
edate = -1
pattern = re.compile(r'\s+')

try:
  opts, args = getopt.getopt(sys.argv[1:], "s:e:p:t:", ["sdate=", "edate=", "pnl=", "type="])
  for o, a in opts:
    if o in ("-s", "--sdate"):
      sdate = a
    elif o in ("-e", "--edate"):
      edate = a
    elif o in ("-p", "--pnl"):
      pnlfile = os.path.abspath(a)
    elif o in ("-t", "--type"):
      type = a
  if pnlfile == '':
    pnlfile = os.path.abspath(args[0])
except getopt.GetoptError as err:
  print string(err)
  sys.exit(0)

if (os.path.isfile(pnlfile) == False):
  print "Pnl file '%s' no exists" % (pnlfile)
  sys.exit(0)

f = open(pnlfile, 'r')
for line in f:
  line = line.strip('\n')
  date, pnl, long, short, ret, sh_hld, sh_trd, b_share, t_share, longNum, shortNum = pattern.split(line, 11)
  date = date[:8]
  if (sdate > 0 and date < sdate) or (edate > 0 and date > edate):
    continue
  XX = date[0:4]
  if type == 'monthly':
    XX = date[0:6]
  doStats(XX, int(date), float(pnl), float(long), float(short), float(ret), float(sh_hld), float(sh_trd), float(b_share), float(t_share), 
            float(longNum), float(shortNum))
  doStats('ALL', int(date), float(pnl), float(long), float(short), float(ret), float(sh_hld), float(sh_trd), float(b_share), float(t_share),
            float(longNum), float(shortNum))

  # drawdown
  if (DD_setst == 1):
    DD_start = date
    DD_setst = 0
  DD_sum += float(pnl)
  if (DD_sum >= 0):
    DD_sum = 0
    DD_start = date
    DD_setst = 1

  if (DD_sum < stats[XX]['drawdown']):
    stats[XX]['drawdown'] = DD_sum
    stats[XX]['dd_start'] = DD_start
    stats[XX]['dd_end'] = date

  if (DD_sum < stats['ALL']['drawdown']):
    stats['ALL']['drawdown'] = DD_sum
    stats['ALL']['dd_start'] = DD_start
    stats['ALL']['dd_end'] = date

f.close()

# print head line
#print "%17s %7s %8s %7s %7s %7s %12s %5s %5s %6s %6s %7s" % ("dates", "long(M)", "short(M)", "pnl(M)", "%ret", "%tvr", "shrp (IR)", "%dd", "%win", "bpmrgn", "csmrgn", "fitness")
print "%17s %7s %8s %7s %7s %7s %12s %5s %5s %6s %10s %10s" % ("dates", "long(M)", "short(M)", "pnl(M)", "%ret", "%tvr", "shrp (IR)", "%dd", "%win", "bpmrgn", "longNum","shortNum")

for XX in sorted(stats):
  if XX == 'ALL': 
    print ''
  d = float(stats[XX]['days'])
  #print XX, d
  if (d < 1):
    continue
  long = stats[XX]['long'] / d
  short = stats[XX]['short'] / d
  ret = stats[XX]['avg_ret'] / d * TRADE_DAYS
  perwin = stats[XX]['up_days'] / d
  turnover = stats[XX]['sh_trd'] / stats[XX]['sh_hld'] if (stats[XX]['sh_hld'] > 0) else 0
  drawdown = stats[XX]['drawdown'] / long * -100 if (long > 0) else 0
  ir = 0
  if (d > 2):
    avg = stats[XX]['xsy'] / d
    std = math.sqrt((stats[XX]['xsyy'] - stats[XX]['xsy'] * stats[XX]['xsy'] / d) / (d - 1))
    ir = avg / std if (std > 0) else 0

  #fitness = ir / turnover if (turnover > 0) else 0
  fitness = ir * math.sqrt(TRADE_DAYS) * math.sqrt(math.fabs(ret/turnover)) if (turnover > 0) else 0
  margin = stats[XX]['pnl'] / stats[XX]['sh_trd'] * 10000 if (stats[XX]['sh_trd'] > 0) else 0
  margin_cs = stats[XX]['pnl'] / stats[XX]['t_sh'] * 100 if (stats[XX]['t_sh'] > 0) else 0
  
#  print "%8d-%8d %7.2f %8.2f %7.3f %7.2f %7.2f %6.2f(%4.2f) %5.2f %5.2f %6.2f %6.2f %7.2f" % (stats[XX]['dates'][0], stats[XX]['dates'][-1], long / LONGSHORT_SCALE, short / LONGSHORT_SCALE, stats[XX]['pnl'] / PNL_SCALE, ret * 100, turnover * 100, ir * math.sqrt(TRADE_DAYS), ir, drawdown, perwin, margin, margin_cs, fitness)
  print "%8d-%8d %7.2f %8.2f %7.3f %7.2f %7.2f %6.2f(%4.2f) %5.2f %5.2f %6.2f %10.1f %10.1f " % (stats[XX]['dates'][0], stats[XX]['dates'][-1], long / LONGSHORT_SCALE, short / LONGSHORT_SCALE, stats[XX]['pnl'] / PNL_SCALE, ret * 100, turnover * 100, ir * math.sqrt(TRADE_DAYS), ir, drawdown, perwin, margin
          , stats[XX]['longNum']/d,stats[XX]['shortNum']/d)



# -*- coding: utf-8 -*-
import re 
from prettytable import PrettyTable
import math
import sys
import os

class alpha_stat():
    
       
    def __init__(self):
        self.path = ""
        self.stats = {}
        self.pnl_scale = 1e6
        pass
 
    def stats_pnl(self, year_id, date, long_value, short_value, ret, turn_over, long_num, short_num):
        if (year_id not in self.stats):
            self.stats[year_id] = {}
            self.stats[year_id]['dates'] = []
            self.stats[year_id]['long'] = 0
            self.stats[year_id]['short'] = 0
            self.stats[year_id]['ret'] = 0
            self.stats[year_id]['ret2'] = 0
            self.stats[year_id]['long_num'] = 0
            self.stats[year_id]['short_num'] = 0
            self.stats[year_id]['drawdown'] = 0
            self.stats[year_id]['dd_start'] = 0
            self.stats[year_id]['dd_end'] = 0
            self.stats[year_id]['days'] = 0
            self.stats[year_id]['win_days'] = 0
            self.stats[year_id]['turnover'] = 0
            self.stats[year_id]['max_up_days'] = 0
            self.stats[year_id]['max_down_days'] = 0

        
        self.stats[year_id]['dates'].append(date)
        self.stats[year_id]['long'] += long_value
        self.stats[year_id]['short'] += short_value
        self.stats[year_id]['ret'] += ret
        self.stats[year_id]['ret2'] += ret * ret
        self.stats[year_id]['long_num'] += long_num
        self.stats[year_id]['short_num'] += short_num
        if (long_value != 0  or short_value != 0):
            self.stats[year_id]['days'] += 1
        self.stats[year_id]['turnover'] += turn_over

        if (ret > 0):
            self.stats[year_id]['win_days'] += 1
            
 

   
            
    def evaluate_pnl(self, pnl_path):
        pattern = re.compile(r'\s+')
        drawdown_start_day = 0
        drawdown_set = False
        drawdown_sum = 0
        max_up_days = 0
        max_down_days = 0
        file = open(pnl_path, 'r')
        for line in file:
            line = line.strip('\n')   
            date, pnl, long_value, short_value, ret, sh_hld, sh_trd, b_share, t_share, long_num, short_num = pattern.split(line, 11)
            turn_over = float(sh_trd)/float(sh_hld) if (float(sh_hld) > 0) else 0.0           
            year = date[0:4]
            self.stats_pnl(year, int(date), float(long_value), float(short_value), float(ret), turn_over, float(long_num), float(short_num))
            self.stats_pnl("sum", int(date), float(long_value), float(short_value), float(ret), turn_over, float(long_num), float(short_num))
            if (drawdown_set == True):
                drawdown_start_day = date
                drawdown_set = False
                
            drawdown_sum += float(pnl)
            if (drawdown_sum >= 0) or len(self.stats[year]["dates"]) == 1:
                drawdown_sum = 0
                drawdown_start_day = date
                drawdown_set = True
                
                
            if (drawdown_sum < self.stats[year]['drawdown']):
                self.stats[year]['drawdown'] = drawdown_sum
                self.stats[year]['dd_start'] = drawdown_start_day
                self.stats[year]['dd_end'] = date

            if (drawdown_sum < self.stats['sum']['drawdown']):
                self.stats['sum']['drawdown'] = drawdown_sum
                self.stats['sum']['dd_start'] = drawdown_start_day
                self.stats['sum']['dd_end'] = date  
                
            if float(ret) > 0:
                max_up_days += 1
                max_down_days = 0
            
            if float(ret) < 0:
                max_down_days +=1
                max_up_days = 0
                
            if max_up_days > self.stats[year]['max_up_days']:
                self.stats[year]['max_up_days'] = max_up_days
            if max_up_days > self.stats["sum"]['max_up_days']:
                self.stats['sum']['max_up_days'] = max_up_days
            if max_down_days > self.stats[year]['max_down_days']:
                self.stats[year]['max_down_days'] = max_down_days
            if max_down_days > self.stats["sum"]['max_down_days']:
                self.stats['sum']['max_down_days'] = max_down_days
                         
        file.close()
        
    def get_stats_table(self):
        x = PrettyTable(["dates", "long", "short", "ret","turnover", "ir","drawdown", "win", "long_num","short_num", "fitness", "up_days", "down_days"])
        x.align["策略"] = "l" 
        x.padding_width = 1
        for year in sorted(self.stats):                
            days = float(self.stats[year]["days"])
            if days < 1:
                continue
            long = self.stats[year]['long'] / days
            short = self.stats[year]['short'] / days
            ret = self.stats[year]['ret'] / days * 242
            win_rate = self.stats[year]['win_days'] / days
            turnover = self.stats[year]['turnover'] / days 
            max_drawdown = self.stats[year]['drawdown'] / long * -100 if (long > 0) else 0
            long_num = self.stats[year]['long_num'] / days
            short_num = self.stats[year]['short_num'] / days
            ir = 0
            if (days > 2):
                avg = self.stats[year]['ret'] / days
                std = math.sqrt((self.stats[year]['ret2'] - self.stats[year]['ret'] * self.stats[year]['ret'] / days) / (days - 1))
                ir = avg / std if (std > 0) else 0


            fitness = ir * math.sqrt(242) * math.sqrt(math.fabs(ret/turnover)) if (turnover > 0) else 0
            if year!= "sum":
                x.add_row(["%s-%s"%(self.stats[year]['dates'][0], self.stats[year]['dates'][-1]),
                       "%5.2f"%(long/self.pnl_scale),
                       "%5.2f"%(short/self.pnl_scale),
                       "%6.2f"%(ret*100),
                       "%6.2f"%(turnover*100),
                       "%4.2f"%(ir),  
                       "%5.2f(%s-%s)"%(max_drawdown,self.stats[year]['dd_start'],self.stats[year]['dd_end']), 
                       "%5.2f"%(win_rate), 
                       "%4.0f"%(long_num), 
                       "%4.0f"%(short_num),  
                       "%5.2f"%(fitness),
                       "%3.0f"%(self.stats[year]['max_up_days']),
                       "%3.0f"%(self.stats[year]['max_down_days'])               
                        ])
            else:
                x.add_row(["ALL",
                       "%5.2f"%(long/self.pnl_scale),
                       "%5.2f"%(short/self.pnl_scale),
                       "%6.2f"%(ret*100),
                       "%6.2f"%(turnover*100),
                       "%4.2f"%(ir),  
                       "%5.2f(%s-%s)"%(max_drawdown,self.stats[year]['dd_start'],self.stats[year]['dd_end']), 
                       "%5.2f"%(win_rate), 
                       "%4.0f"%(long_num), 
                       "%4.0f"%(short_num),  
                       "%5.2f"%(fitness),
                       "%3.0f"%(self.stats[year]['max_up_days']),
                       "%3.0f"%(self.stats[year]['max_down_days'])              
                        ])
        print x.get_string()
        
        
if __name__ == '__main__':	
	if len(sys.argv) == 2:
	   file_path = sys.argv[1]
	cls= alpha_stat()
	cls.path = os.path.abspath(file_path)
	cls.evaluate_pnl(cls.path)
	cls.get_stats_table()


  
# -*- coding: utf-8 -*-
import Queue
import threading
import xml.dom.minidom
import os,re,math
from prettytable import PrettyTable
from alpha_fit import *
import numpy
import csv




class ThreadSim(threading.Thread):
    def __init__(self, queue):
        threading.Thread.__init__(self)
        self.queue = queue


    def run(self):
        while True:
            _filename = self.queue.get()
            _cfgfile = give_para().config_path + _filename
            print "Sim::begin to handle "+_cfgfile+'\n'
            cmd = "./run "
            os.system(cmd+_cfgfile)
            print "Sim::"+_cfgfile+" done\n"
            self.queue.task_done()




class pnl_stat():     
    def __init__(self):
        self.path = "/home/yuhao/workspace/alphasystem/yhSimFolder/pnl/"
        self.stats = {}
        self.start_date = 20060101
        self.end_date = 20150101
        pass
 
    def stats_pnl(self, pnl_id, date, ret, turn_over):
        if (pnl_id not in self.stats):
            self.stats[pnl_id] = {}
            self.stats[pnl_id]['dates'] = []
            self.stats[pnl_id]['ret'] = 0
            self.stats[pnl_id]['ret2'] = 0
            self.stats[pnl_id]['turnover'] = 0
            self.stats[pnl_id]['days'] = 0

        if self.start_date <= date <= self.end_date:
            self.stats[pnl_id]['dates'].append(date)
            self.stats[pnl_id]['ret'] += ret
            self.stats[pnl_id]['ret2'] += ret * ret
            self.stats[pnl_id]['days'] += 1
            self.stats[pnl_id]['turnover'] += turn_over

            
    def evaluate_pnl(self, pnl_path):
        pnl_id = os.path.basename(pnl_path)
        pattern = re.compile(r'\s+')
        file = open(pnl_path, 'r')
        for line in file:
            line = line.strip('\n')   
            date, pnl, long_value, short_value, ret, sh_hld, sh_trd, b_share, t_share, long_num, short_num = pattern.split(line, 11)
            turn_over = float(sh_trd)/float(sh_hld) if (float(sh_hld) > 0) else 0.0           
            self.stats_pnl(pnl_id, int(date), float(ret), turn_over)                        
        file.close()
        for pnl_id in self.stats:                
            days = float(self.stats[pnl_id]["days"])
            if days > 0:
                ret = self.stats[pnl_id]['ret'] / days * 242
                turnover = self.stats[pnl_id]['turnover'] / days 
                avg = self.stats[pnl_id]['ret'] / days
                std = math.sqrt((self.stats[pnl_id]['ret2'] - self.stats[pnl_id]['ret'] * self.stats[pnl_id]['ret'] / days) / (days - 1)) if (days > 1) else 0
                ir = avg / std if (std > 0) else 0
                fitness = ir * math.sqrt(242) * math.sqrt(math.fabs(ret/turnover)) if (turnover > 0) else 0
                self.stats[pnl_id]["fitness"] = fitness
                self.stats[pnl_id]["sharpe"] = ir * math.sqrt(242)
                self.stats[pnl_id]["annual_ret"] = ret
                self.stats[pnl_id]["annual_turnover"] = turnover




    

    

class alpha_fitting():
    def __init__(self):
        self.config_path = give_para().config_path
        self.alpha_name = give_para().alpha_name
        self.config_name = "config_" + self.alpha_name + ".xml"
        self.para = give_para().para
        self.operations = give_para().operations
        pass
        
    def revise_one_alpha_config(self, config_name_new, tag_dict):
        dom = xml.dom.minidom.parse(self.config_name)
        root = dom.documentElement
        pf = root.getElementsByTagName("Portfolio")
        alpha = pf[0].getElementsByTagName("Alpha")[0]
        for tag_name in tag_dict:
            alpha.setAttribute(tag_name, tag_dict[tag_name])
        config_path_new = self.config_path + config_name_new
        new_dir_file = os.path.dirname(config_path_new)
        if not os.path.exists(new_dir_file):
            os.makedirs(new_dir_file)
        dom.writexml(open(config_path_new,'w'))

    def revise_one_operation_config(self, config_name_new, alpha_id, tag_dict):
        dom = xml.dom.minidom.parse(self.config_name)
        root = dom.documentElement
        pf = root.getElementsByTagName("Portfolio")
        alpha = pf[0].getElementsByTagName("Alpha")[0]
        alpha.setAttribute("id", alpha_id)
        operation = alpha.getElementsByTagName("Operations")[0].getElementsByTagName("Operation")
        for ope in operation:
            for module_id in tag_dict:
                if ope.getAttribute('moduleId') == module_id:
                    for tag_name in tag_dict[module_id]:
                        ope.setAttribute(tag_name, tag_dict[module_id][tag_name])
        config_path_new = self.config_path + config_name_new
        new_dir_file = os.path.dirname(config_path_new)
        if not os.path.exists(new_dir_file):
            os.makedirs(new_dir_file)
        dom.writexml(open(config_path_new,'w'))
            

    def revise_all_alpha_config(self):
        id_num = 0
        if len(self.para[0]) > 0:
            tag_dict = {}
            for i in range(len(self.para[0])):
                para1 = self.para[0][i]
                tag_dict["para1"] = str(para1)
                if len(self.para[1]) > 0:
                    for j in range(len(self.para[1])):
                        para2 = self.para[1][j]
                        tag_dict["para2"] = str(para2)
                        if len(self.para[2]) > 0:
                            for k in range(len(self.para[2])):
                                para3 = self.para[2][k]
                                tag_dict["para3"] = str(para3)
                                tag_dict["id"] = self.alpha_name + "_" + str(id_num)
                                config_name_new = "config_" + self.alpha_name + "_" + str(id_num) +  ".xml"
                                self.revise_one_alpha_config(config_name_new, tag_dict)
                                id_num = id_num + 1
                        else:
                            tag_dict["id"] = self.alpha_name + "_" + str(id_num)
                            config_name_new = "config_" + self.alpha_name + "_" + str(id_num) +  ".xml"
                            self.revise_one_alpha_config(config_name_new, tag_dict)
                            id_num = id_num + 1
                            
                else:
                    tag_dict["id"] = self.alpha_name + "_" + str(id_num)
                    config_name_new = "config_" + self.alpha_name + "_" + str(id_num) +  ".xml"
                    self.revise_one_alpha_config(config_name_new, tag_dict)
                    id_num = id_num + 1
        return id_num

               
    def revise_all_operation_config(self):
        id_num = 0
        tag_dict = {}
        tag_dict["AlphaOpPower"] = {}
        tag_dict["AlphaOpDecay"] = {}
        for i in range(len(self.operations["AlphaOpPower"])):
            for j in range(len(self.operations["AlphaOpDecay"])):
                tag_dict["AlphaOpPower"]["exp"] = str(self.operations["AlphaOpPower"][i])
                tag_dict["AlphaOpDecay"]["days"] = str(self.operations["AlphaOpDecay"][j])
                config_name_new = "config_" + self.alpha_name + "_" + str(id_num) +  ".xml"
                alpha_id = self.alpha_name + "_" + str(id_num)
                self.revise_one_operation_config(config_name_new, alpha_id, tag_dict)
                id_num = id_num + 1
        return id_num

    
        
    def run_all_alpha_config(self): 
        thread_max = 8
        queue = Queue.Queue() 
        id_num = self.revise_all_alpha_config()
        print id_num
        for i in range(id_num):
            queue.put("config_" + self.alpha_name + "_" + str(i) +  ".xml")       
        for j in range(thread_max):
            _thread = ThreadSim(queue)
            _thread.setDaemon(True)
            _thread.start()
        queue.join()
        print "all tasks complete"

    def run_all_operation_config(self): 
        thread_max = 8
        queue = Queue.Queue() 
        id_num = self.revise_all_operation_config()
        print id_num
        for i in range(id_num):
            queue.put("config_" + self.alpha_name + "_" + str(i) +  ".xml") 

        for j in range(thread_max):
            _thread = ThreadSim(queue)
            _thread.setDaemon(True)
            _thread.start()
        queue.join()
        
        print "all tasks complete"
        



class evaluate_pnl_performance():     
    def __init__(self):
        para = give_para()
        # para for whole perfomance
        self.fitness_door = para.fitness_door
        self.sharpe_door = para.sharpe_door
        self.tvr_door = para.tvr_door
        self.whole_test_period = para.whole_test_period
        # para for every year ret test
        self.year_list = para.year_list
        self.ret_door = para.ret_door
        # para for corr test
        self.corr_dict = para.corr_dict

        self.pnl_result = {}
        self.pnl_list = []
        self.id_num = para.id_num
        for i in range(self.id_num):
            pnl_id = para.alpha_name + "_" + str(i)
            self.pnl_list.append(pnl_id)





    def performance_test(self, pnl_id):  
        pnl_stat1 = pnl_stat()
        pnl_stat1.start_date = self.whole_test_period[0]
        pnl_stat1.end_date = self.whole_test_period[1]
        pnl_path = "/home/yuhao/workspace/alphasystem/yhSimFolder/pnl/" + pnl_id
        pnl_stat1.evaluate_pnl(pnl_path)
        performance_bad = False
        if pnl_stat1.stats[pnl_id]["fitness"] < self.fitness_door:
            performance_bad = True
        if pnl_stat1.stats[pnl_id]["sharpe"] < self.sharpe_door:
            performance_bad = True
        if pnl_stat1.stats[pnl_id]["annual_turnover"] > self.tvr_door:
            performance_bad = True
        if performance_bad == True:
            return 0.0
        else:
            return pnl_stat1.stats[pnl_id]["fitness"]


    def year_ret_test(self,pnl_id):  #cut for one_year_loss
        test_ev = True
        for year in self.year_list:
            pnl_stat1 = pnl_stat()
            pnl_stat1.start_date = year*10000 + 0101
            pnl_stat1.end_date = year*10000 + 1231
            pnl_path = "/home/yuhao/workspace/alphasystem/yhSimFolder/pnl/" + pnl_id
            pnl_stat1.evaluate_pnl(pnl_path)
            if abs(pnl_stat1.stats[pnl_id]["annual_ret"]) < self.ret_door:
                test_ev = False
        if test_ev == False:
            return 0.0
        else:
            return 1.0

    def corr_test(self,pnl_id):  
        corr_result = {}
        corCmd = "/data/alphaSystem/release/tools/cor ./pnl/%s"%(pnl_id)
        (result, output) = commands.getstatusoutput(corCmd)
        output1 = output.split('\n')
        while len(output1) < 5:
            (result, output) = commands.getstatusoutput(corCmd)
            output1 = output.split('\n')
        for line in output1:
            member = re.split("\s+", line)
            if len(member) > 1:
                if member[1] == "0.9":
                    corr_result["0.9"] = [float(member[2]), float(member[3])]
   
                if member[1] == "0.8":
                    corr_result["0.8"] = [float(member[2]), float(member[3])]
                if member[1] == "0.7":
                    corr_result["0.7"] = [float(member[2]), float(member[3])]
                if member[1] == "0.6":
                    corr_result["0.6"] = [float(member[2]), float(member[3])]
                if member[1] == "0.5":
                    corr_result["0.5"] = [float(member[2]), float(member[3])]
                if member[1] == "0.4":
                    corr_result["0.4"] = [float(member[2]), float(member[3])]
                if member[1] == "0.3":
                    corr_result["0.3"] = [float(member[2]), float(member[3])]
        corr_ev = False
        for id in corr_result:
            if id in self.corr_dict:
                if corr_result[id][1] > self.corr_dict[id]:
                    corr_ev = True
        if corr_ev == True:
            return 0.0
        else:
            return 1



    def evaluate_pnl_perform(self):  #cut for bad corr
        for pnl_id in self.pnl_list:   
            fitness_score = self.performance_test(pnl_id)
            ret_score = self.year_ret_test(pnl_id)
            corr_score = self.corr_test(pnl_id)
            print pnl_id, fitness_score, ret_score, corr_score
            if fitness_score != 0.0 and ret_score!= 0.0 and corr_score!= 0.0:
                self.pnl_result[pnl_id] = {}
                self.pnl_result[pnl_id]["fitness_score"] = fitness_score
                self.pnl_result[pnl_id]["ret_score"] = ret_score
                self.pnl_result[pnl_id]["corr_score"] = corr_score

        x = PrettyTable(["pnl_id", "fitness_score", "ret_score","corr_score"])
        x.align["\u7b56\u7565"] = "l" 
        x.padding_width = 1
        dict = sorted(self.pnl_result.iteritems(), key=lambda d:d[1].get('fitness_score',0), reverse = True)
        for info in dict:        
            x.add_row(["%s"%(info[0]),
                       "%s"%(info[1]["fitness_score"]),  
                       "%s"%(info[1]["ret_score"]),  
                       "%s"%(info[1]["corr_score"])            
                    ])
        contend = x.get_string()
        print contend
        pass 





    


  



        


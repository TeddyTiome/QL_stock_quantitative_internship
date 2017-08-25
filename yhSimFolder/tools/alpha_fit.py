# -*- coding: utf-8 -*-
import Queue
import threading
import xml.dom.minidom
import os,re,math,commands
from prettytable import PrettyTable
from alpha_fitclass import *

#pnl_stat : analysis pnl performance
#alpha_fitting: write xml files and get pnl files
    # 1. run_all_alpha_config() get alpha result for different paras
    # 2. run_all_operation_config() get alpha result for different operations
#pnl_stat : analysis pnl performance
#evaluate_pnl_performance
    # 1. evaluate_pnl_perform() analysis all pnl result

class give_para():     
    def __init__(self):

        # basic para
        self.config_path = "/home/yuhao/workspace/alphasystem/yhSimFolder/"
        self.alpha_name = "yuhao_t8"
        self.id_num = 70 # total alpha number

        # para for alpha and operation
        self.para = {}
        self.para[0] = []
        self.para[1] = []
        self.para[2] = []
        self.operations = {}
        self.operations["AlphaOpPower"] = [5,8,10,13,15,20,25,30,35,40]
        self.operations["AlphaOpDecay"] = [10,15,20,25,30,40,60]

        # para for pnl analysis
        self.fitness_door = 2.5
        self.sharpe_door = 3.0
        self.tvr_door = 0.45
        self.whole_test_period = [20060101,20150101]
        # para for every year ret test
        self.year_list = [2009,2010,2011,2012,2013,2014]
        self.ret_door = 0
        # para for corr test
        self.corr_dict = {"0.9":0, "0.8":0, "0.7": 0, "0.6":0,"0.5":0, "0.4":5, "0.3":20}





if __name__ == '__main__': 
    cls = alpha_fitting()
    cls.run_all_operation_config()
    cls = evaluate_pnl_performance()
    cls.evaluate_pnl_perform()


import pandas as pd
import math
import numpy as np
from matplotlib import pyplot as plt
import japanize_matplotlib

size_array = [1000,2000,4000,8000,16000,32000,64000,128000,256000]
def makefigure_DPUtime():
    x_axis = []
    result = []
    result_insert = []
    result_get= []
    result_total  = []
    df = pd.read_csv("data/requestnum.csv")
    result += df['DPU実行時間'].values.tolist()
    result_insert += df['insert平均サイクル'].values.tolist()
    result_get += df['get平均サイクル'].values.tolist()
    x_axis += df['挿入するキーの数'].values.tolist()
    plt.figure()
    plt.title("requestnum_DPUtime")
    plt.xlabel('num of requests')
    plt.ylabel('Time[s]')
    plt.plot(x_axis, result, label='total', marker='o')
    plt.ylim(0,)
    plt.xticks([1000]+x_axis[5:],["1000"]+[str(num) for num in x_axis[5:]])
    plt.grid()
    plt.savefig("graphs/requestnum_DPUtime.png")
    plt.show()

def makefigure_requestnum():
    x_axis = []
    result_insert = []
    result_get= []

    df = pd.read_csv("data/test.csv")
    #result += df['Time'].values.tolist()
    result_insert += df['cycles per insert'].values.tolist()
    result_get += df['cycles per get'].values.tolist()
    x_axis += df['num_of_requests'].values.tolist()
    plt.figure()
    plt.title("requestnum_averagecycle")
    plt.xlabel('num of requests')
    plt.ylabel('average DPU cycles')
    plt.plot(x_axis, result_insert, label='insert', marker='o')
    plt.plot(x_axis, result_get, label='get', marker='o')
    plt.ylim(0,)
    #plt.xscale('log')
    plt.xticks([1000]+x_axis[6:],["1000"]+[str(num) for num in x_axis[6:]])
    #plt.yscale('log')
    plt.legend()
    plt.grid()
    plt.savefig("graphs/test.png")
    plt.show()
    
def makefigure_taskletnum():
    x_axis = []
    throughput = []
    throughput_skew = []
    df = pd.read_csv("data/taskletnum_upmem_pointerdram.csv")
    #result += df['Time'].values.tolist()
    throughput += df[' throughput[OPS/s]'].values.tolist()
    for i in range (0, len(throughput)):
        throughput[i] = throughput[i]/1000000.0
        throughput_skew.append(0.040306)
    x_axis += df['num of tasklets'].values.tolist()
    fig = plt.figure()
    plt.rcParams["font.size"] = 18
    plt.rcParams["savefig.dpi"] = 300
    plt.title("スレッド数とスループットの関係")
    plt.xlabel('各DPUのスレッド数')
    plt.ylabel('スループット[MOPS/s]')
    plt.plot(x_axis, throughput, marker='o')
    plt.plot(x_axis, throughput_skew)
    plt.xlim(0,)
    plt.ylim(0,)
    #plt.xscale('log')
    #plt.xticks([1000]+x_axis[6:],["1000"]+[str(num) for num in x_axis[6:]])
    #plt.yscale('log')
    plt.grid()
    fig.subplots_adjust(bottom=0.13,left=0.15,top=0.93,right=0.99)
    plt.savefig("graphs/num_of_tasklets_pointerdram.png",transparent = True)
    plt.show()
    
def makefigure_taskletnum_time():
    x_axis = []
    time_sendreq = []
    time_dpuexe = []
    df = pd.read_csv("data/taskletnum_upmem_pointerdram.csv")
    time_sendreq += df[' time_sendrequests[s]'].values.tolist()
    time_dpuexe += df[' time_dpu_execution[s]'].values.tolist()
    x_axis += df['num of tasklets'].values.tolist()
    fig = plt.figure()
    plt.rcParams["font.size"] = 18
    plt.rcParams["savefig.dpi"] = 300
    plt.title("実行時間の内訳")
    plt.xlabel('各DPUのスレッド数')
    plt.ylabel('時間[s]')
    plt.bar(x_axis, time_sendreq, label="クエリ送信時間(CPU→DPU)",align="center", color = "c")
    plt.bar(x_axis, time_dpuexe, label="DPU実行時間",align="center", color = "b",bottom=time_sendreq)  
    plt.ylim(0,)
    #plt.xscale('log')
    #plt.xticks([1000]+x_axis[6:],["1000"]+[str(num) for num in x_axis[6:]])
    #plt.yscale('log')
    plt.legend()
    #plt.grid()
    fig.subplots_adjust(bottom=0.13,left=0.15,top=0.93,right=0.99)
    plt.savefig("graphs/num_of_tasklets_time_pointerdram.png",transparent = True)
    plt.show()
makefigure_taskletnum()
makefigure_taskletnum_time()
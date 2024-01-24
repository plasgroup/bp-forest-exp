import pandas as pd
import numpy as np
import os
import re
from matplotlib import pyplot as plt

# expno
expno = "2"
# base file names to convert from csv to graph
file_names = ["release_0_get","release_0_insert", "release_0.99_get","release_0.99_insert","release_0.2_get" ]
# ignore the first row (initialization batch)
first_batch = 1

result_dir = "./" + expno + "/result/"
graph_dir = "./" + expno + "/graphs/"
plus = lambda x, y : x + y

def make_csv(file_name):
    f_result = open(result_dir + file_name, 'r')
    f_csv = open(result_dir + file_name + ".csv", 'w')
    flag = 0
    header = "zipfian_const, NR_DPUS, NR_TASKLETS, batch_num, num_keys, max_query_num, preprocess_time1, preprocess_time2, migration_plan_time, migration_time, send_time, execution_time, receive_result_time, merge_time, batch_time, throughput"
    for s_line in f_result:
        if (flag == 0 and header in s_line):
            f_csv.write(s_line)
            flag = 1
        if (flag == 1 and '100000, ' in s_line):
            f_csv.write(s_line)

    
def makefigure_migration(file_name, first_batch):
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    throughput = []
    df = pd.read_csv(result_dir + file_name + ".csv")
    
    x_axis = list(range(len(df)))
    throughput += df[' throughput'].values.tolist()
    throughput_bpforest = max(throughput)
    throughput_PIM_skip_list=0.2
    plt.rcParams["savefig.dpi"] = 300
    plt.xlabel('batch iteration',fontsize=18)
    ax1.set_ylabel('throughput[MOP/s]',fontsize=18)
    ax1.plot(x_axis[first_batch:len(df)-1], throughput[first_batch:len(df)-1], marker='o')
    #plt.xticks(x_axis[first_batch:len(df)-1])
    plt.grid()
    fig.subplots_adjust(left=0.15)
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/throughput_" + file_name + ".png", transparent = False)
    
def max_query_num(file_name, first_batch):
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    send_size = []
    df = pd.read_csv(result_dir + file_name + ".csv")
    x_axis = list(range(len(df)))
    send_size += df[' max_query_num'].values.tolist()
    plt.rcParams["savefig.dpi"] = 300
    plt.xlabel('batch iteration',fontsize=18)
    ax1.set_ylabel('max # of queries for a DPU',fontsize=18)
    ax1.plot(x_axis[first_batch:len(df)-1], send_size[first_batch:len(df)-1], marker='o')
    #plt.xticks(x_axis[first_batch:len(df)-1])
    plt.grid()
    ax1.set_xlim(first_batch-0.5,len(df) - 1.5)
    ax1.set_ylim(0,)
    fig.subplots_adjust(left=0.2)
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/max_query_num" + file_name + ".png", transparent = False)
    
def time_bar_migration(file_name, first_batch):
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    throughput = []
    preprocess_time1 = []
    preprocess_time2 = []
    preprocess_time = []
    migration_time = []
    send_time = []
    execution_time = []
    recieve_result_time = []
    communication_time = []
    df = pd.read_csv(result_dir + file_name + ".csv")
    x_axis = list(range(len(df)))
    migration_time += df[' migration_time'].values.tolist()
    send_time += df[' send_time'].values.tolist()
    execution_time += df[' execution_time'].values.tolist()
    preprocess_time1 += df[' preprocess_time1'].values.tolist()
    preprocess_time2 += df[' preprocess_time2'].values.tolist()
    preprocess_time += map(plus, preprocess_time1, preprocess_time2)
    recieve_result_time += df[' receive_result_time'].values.tolist()
    communication_time += map(plus, send_time, recieve_result_time)
    throughput += df[' throughput'].values.tolist()
    plt.rcParams["savefig.dpi"] = 300
    plt.xlabel('batch iteration',fontsize=18)
    ax1.set_ylabel('time[s]',fontsize=18)
    ax1.bar(x_axis[first_batch:len(df) - 1], recieve_result_time[first_batch:len(df) - 1], bottom=[execution_time[i] + send_time[i] + migration_time[i] + preprocess_time[i] for i in range(first_batch, len(df) - 1)], label = "result aggregation", edgecolor='black', hatch="\\")
    ax1.bar(x_axis[first_batch:len(df) - 1], preprocess_time[first_batch:len(df) - 1], bottom=[execution_time[i] + send_time[i] + migration_time[i] for i in range(first_batch, len(df) - 1)], label = "preprocess", edgecolor='black', hatch="x")
    ax1.bar(x_axis[first_batch:len(df) - 1], migration_time[first_batch:len(df) - 1], bottom=[execution_time[i] + send_time[i] for i in range(first_batch, len(df) - 1)], label = "tree migration", edgecolor='black', hatch="+")
    ax1.bar(x_axis[first_batch:len(df) - 1], send_time[first_batch:len(df) - 1],bottom=execution_time[first_batch:len(df) - 1], label = "query deliver", edgecolor='black', hatch="-")
    ax1.bar(x_axis[first_batch:len(df) - 1], execution_time[first_batch:len(df) - 1], label = "query execution", edgecolor='black', hatch="/")
    #plt.xticks(x_axis[first_batch:len(df)-1])
    plt.grid()
    fig.subplots_adjust()
    plt.legend()
    ax1.set_xlim(first_batch-0.5,len(df) - 1.5)
    ax1.set_ylim(0,)
    fig.subplots_adjust()
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/breakdown_" + file_name + ".png", transparent = False)
    
def time_bar_migration_new(file_name, first_batch):
    plt.rcParams["font.size"] = 20
    df = pd.read_csv(result_dir + file_name + ".csv")
    colors = ["maroon", "brown", "lightcoral", "mistyrose"]
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    throughput = []
    preprocess_time1 = []
    preprocess_time2 = []
    preprocess_time = []
    migration_time = []
    send_time = []
    execution_time = []
    recieve_result_time = []
    communication_time = []
    df = pd.read_csv(result_dir + file_name + ".csv")
    x_axis = list(range(len(df)))
    migration_time += df[' migration_time'].values.tolist()
    send_time += df[' send_time'].values.tolist()
    execution_time += df[' execution_time'].values.tolist()
    preprocess_time1 += df[' preprocess_time1'].values.tolist()
    preprocess_time2 += df[' preprocess_time2'].values.tolist()
    preprocess_time += map(plus, preprocess_time1, preprocess_time2)
    recieve_result_time += df[' receive_result_time'].values.tolist()
    communication_time += map(plus, send_time, recieve_result_time)
    throughput += df[' throughput'].values.tolist()
    plt.rcParams["savefig.dpi"] = 300
    plt.xlabel('batch iteration',fontsize=18)
    ax1.set_ylabel('time[s]',fontsize=18)
    ax1.bar(x_axis[first_batch:len(df) - 1], preprocess_time[first_batch:len(df) - 1], bottom=[execution_time[i] + send_time[i] + migration_time[i] for i in range(first_batch, len(df) - 1)], label = "preprocess", color=colors[0])
    ax1.bar(x_axis[first_batch:len(df) - 1], migration_time[first_batch:len(df) - 1], bottom=[execution_time[i] + send_time[i] for i in range(first_batch, len(df) - 1)], label = "tree migration", color=colors[1])
    ax1.bar(x_axis[first_batch:len(df) - 1], communication_time[first_batch:len(df) - 1],bottom=execution_time[first_batch:len(df) - 1], label = "CPU-PIM communication", color=colors[2])
    ax1.bar(x_axis[first_batch:len(df) - 1], execution_time[first_batch:len(df) - 1], label = "PIM query execution", color=colors[3])
    #plt.xticks(x_axis[first_batch:len(df)-1])
    plt.grid()
    #plt.legend(framealpha=1)
    ax1.set_xlim(first_batch-0.5,len(df) - 1.5)
    ax1.set_ylim(0,)
    fig.subplots_adjust(left=0.19, bottom=0.15, right=0.98, top=0.98)
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/breakdown_" + file_name + "_new.svg", transparent = True)

def throughput_slides():
    plt.rcParams["savefig.dpi"] = 300
    plt.rcParams["font.size"]=18
    Naive = [41, 0.5, 36, 0.4]
    Proposed = [5.2, 3.3, 5.1, 1.1]
    PIM_tree = [15, 15, 5.1, 5.1]
    Brown = [18.8, 20.8, 2.7, 2.7]
    Bronson = [6.3, 7.1, 2.8, 2.9]
    labels = ['Naive_range_partition', 'Proposed', 'PIM_tree', 'Brown (a,b)-tree', 'Bronson BST']
    colors = ["maroon", "black", "brown", "lightcoral", "mistyrose"]
    x_labels = ['read\n α=0', 'read\n α=0.99', 'insert\n α=0', 'insert\n α=99']
    x = np.array([1, 2, 3, 4])
    data = [Naive, Proposed, PIM_tree, Brown, Bronson]
    # マージンを設定
    margin = 0.2  #0 <margin< 1
    total_width = 1 - margin
    # 棒グラフをプロット
    for i, h in enumerate(data):
        pos = x - total_width *( 1- (2*i+1)/len(data) )/2
        plt.bar(pos, h, width = total_width/len(data), label=labels[i], color=colors[i])

    # ラベルの設定
    plt.xticks(x, x_labels)
    plt.ylabel("throughput[MOP/s]")
    plt.subplots_adjust(left=0.15, bottom=0.15, right=0.95, top=0.95)
    #plt.xticks(x_axis[first_batch:len(df)-1])
    #plt.grid()
    #plt.legend(framealpha=1)
    plt.savefig(graph_dir + "throughput.svg", transparent = True)
    
    
    
# generate graphs
#for file_name in file_names:
    #make_csv(file_name)
    #makefigure_migration(file_name, first_batch)
    #max_query_num(file_name, first_batch)
    #time_bar_migration(file_name, first_batch)
    #time_bar_migration_new(file_name, first_batch)

throughput_slides()
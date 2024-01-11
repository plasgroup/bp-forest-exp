import pandas as pd
import numpy as np
import os
import re
from matplotlib import pyplot as plt

# expno
expno = "2"
# base file names to convert from csv to graph
file_names = ["release_0", "release_0.2", "release_0.4", "release_0.6", "release_0.8", "release_0.99", "release-flat_0", "release-flat_0.2", "release-flat_0.4", "release-flat_0.6", "release-flat_0.8", "release-flat_0.99"]
# ignore the first row (initialization batch)
first_batch = 1

result_dir = "./" + date + "/result/"
graph_dir = "./" + date + "/graphs/"
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
    
    plt.rcParams["savefig.dpi"] = 300
    plt.xlabel('batch iteration',fontsize=18)
    ax1.set_ylabel('throughput[MOP/s]',fontsize=18)
    ax1.plot(x_axis[first_batch:len(df)-1], throughput[first_batch:len(df)-1], marker='o')
    plt.xticks(x_axis[first_batch:len(df)-1])
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
    plt.xticks(x_axis[first_batch:len(df)-1])
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
    df = pd.read_csv(result_dir + file_name + ".csv")
    x_axis = list(range(len(df)))
    migration_time += df[' migration_time'].values.tolist()
    send_time += df[' send_time'].values.tolist()
    execution_time += df[' execution_time'].values.tolist()
    preprocess_time1 += df[' preprocess_time1'].values.tolist()
    preprocess_time2 += df[' preprocess_time2'].values.tolist()
    preprocess_time += map(plus, preprocess_time1, preprocess_time2)
    recieve_result_time += df[' receive_result_time'].values.tolist()
    throughput += df[' throughput'].values.tolist()
    plt.rcParams["savefig.dpi"] = 300
    plt.xlabel('batch iteration',fontsize=18)
    ax1.set_ylabel('time[s]',fontsize=18)
    ax1.bar(x_axis[first_batch:len(df) - 1], recieve_result_time[first_batch:len(df) - 1], bottom=[execution_time[i] + send_time[i] + migration_time[i] + preprocess_time[i] for i in range(first_batch, len(df) - 1)], label = "result aggregation", edgecolor='black', hatch="\\")
    ax1.bar(x_axis[first_batch:len(df) - 1], preprocess_time[first_batch:len(df) - 1], bottom=[execution_time[i] + send_time[i] + migration_time[i] for i in range(first_batch, len(df) - 1)], label = "preprocess", edgecolor='black', hatch="x")
    ax1.bar(x_axis[first_batch:len(df) - 1], migration_time[first_batch:len(df) - 1], bottom=[execution_time[i] + send_time[i] for i in range(first_batch, len(df) - 1)], label = "tree migration", edgecolor='black', hatch="+")
    ax1.bar(x_axis[first_batch:len(df) - 1], send_time[first_batch:len(df) - 1],bottom=execution_time[first_batch:len(df) - 1], label = "query deliver", edgecolor='black', hatch="-")
    ax1.bar(x_axis[first_batch:len(df) - 1], execution_time[first_batch:len(df) - 1], label = "query execution", edgecolor='black', hatch="/")
    plt.xticks(x_axis[first_batch:len(df)-1])
    plt.grid()
    fig.subplots_adjust()
    plt.legend()
    ax1.set_xlim(first_batch-0.5,len(df) - 1.5)
    ax1.set_ylim(0,0.16)
    fig.subplots_adjust()
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/breakdown_" + file_name + ".png", transparent = False)
    
# generate graphs
for file_name in file_names:
    make_csv(file_name)
    makefigure_migration(file_name, first_batch)
    max_query_num(file_name, first_batch)
    time_bar_migration(file_name, first_batch)

import pandas as pd
import numpy as np
import os
import re
from matplotlib import pyplot as plt

# expno
expno = "ppl_response_memory"
# base file names to convert from csv to graph
file_names = ["print-distribution_0_insert", "print-distribution_0.4_insert", "print-distribution_0.99_insert","print-distribution-50trees_0_insert", "print-distribution-50trees_0.4_insert", "print-distribution-50trees_0.99_insert"]

result_dir = "./" + expno + "/result/"
graph_dir = "./" + expno + "/graphs/"

def make_csv(file_name):
    f_result = open(result_dir + file_name, 'r')
    f_csv = open(result_dir + file_name + ".csv", 'w')
    flag = 0
    header = "batch, DPU, nqueries, nnodes"
    for s_line in f_result:
        if (flag == 0 and header in s_line):
            flag = 1
        if (flag == 1):
            f_csv.write(s_line)

def makefigure_histgram(file_name):
    plt.rcParams["savefig.dpi"] = 300
    num_queries = []
    df = pd.read_csv(result_dir + file_name + ".csv")
    batches = [0, 5, 49]
    fig = plt.figure()
    df_summary = df[df[' DPU'] == -1]
    max_num_queries = max(df_summary[' nqueries'].values.tolist())
    for i, batch in enumerate(batches):
        df_batch = df[df['batch'] == batch]
        num_queries = df_batch[' nqueries'].values.tolist()
        ax1 = fig.add_subplot(len(batches),1,i+1)
        ax1.set_xlim(0,max_num_queries)
        ax1.set_ylim(1,2500)
        ax1.set_ylabel("# of DPUs")
        ax1.set_title("batch " + str(batch))
        if (i == len(batches) - 1):
            ax1.set_xlabel("# of queries")
        ax1.hist(num_queries, bins=10, range= (0, max_num_queries), log=True)
    #plt.grid()
    #fig.subplots_adjust(left=0.15)
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/distribution" + file_name + ".png", transparent = False)
    
def makefigure_histgram_memory(file_name):
    plt.rcParams["savefig.dpi"] = 300
    plt.rcParams["font.family"] = "Times New Roman"
    num_elems = []
    df = pd.read_csv(result_dir + file_name + ".csv")
    batches = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 50, 100, 200, 499]
    fig = plt.figure()
    df_summary = df[df[' DPU'] == -1]
    max_num_kvpairs = max(df_summary[' nkvpairs'].values.tolist())
    for i, batch in enumerate(batches):
        df_batch = df[df['batch'] == batch]
        num_kvpairs = df_batch[' nkvpairs'].values.tolist()
        ax1 = fig.add_subplot(len(batches),1,i+1)
        ax1.set_xlim(0,max_num_kvpairs+10000)
        ax1.set_ylim(1,2500)
        ax1.set_yticks([])
        ax1.tick_params(left=False, labelleft=False)
        #ax1.set_ylabel("# of DPUs")
        #ax1.set_title("batch " + str(batch))
        ticks = list(ax1.get_xticks())
        if (i == len(batches) - 1):
            #ax1.set_xticks(ticks)
            ax1.set_xlabel("Number of key-value pairs")
        else:
            ax1.set_xticks([])
        ax1.hist(num_kvpairs, bins=10, range= (0, max_num_kvpairs), log=True, color="black")
    #plt.grid()
    #fig.subplots_adjust(left=0.15)
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/distribution_kvpairs" + file_name + ".svg", transparent = True)

def makefigure_histgram_memory_legend(file_name):
    plt.rcParams["savefig.dpi"] = 300
    plt.rcParams["font.family"] = "Times New Roman"
    num_elems = []
    df = pd.read_csv(result_dir + file_name + ".csv")
    batches = [10, 50, 100, 200, 499]
    fig = plt.figure()
    df_summary = df[df[' DPU'] == -1]
    max_num_kvpairs = max(df_summary[' nkvpairs'].values.tolist())
    for i, batch in enumerate(batches):
        df_batch = df[df['batch'] == batch]
        num_kvpairs = df_batch[' nkvpairs'].values.tolist()
        ax1 = fig.add_subplot(len(batches),1,i+1)
        ax1.set_xlim(0,max_num_kvpairs+10000)
        ax1.set_ylim(1,2500)
        ax1.set_yticks([])
        ax1.tick_params(left=False, labelleft=False)
        #ax1.set_ylabel("# of DPUs")
        #ax1.set_title("batch " + str(batch))
        ticks = list(ax1.get_xticks())
        if (i == len(batches) - 1):
            #ax1.set_xticks(ticks)
            ax1.set_xlabel("Number of key-value pairs")
        else:
            ax1.set_xticks([])
        ax1.hist(num_kvpairs, bins=10, range= (0, max_num_kvpairs), log=True, color="black")
    #plt.grid()
    #fig.subplots_adjust(left=0.15)
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/distribution_kvpairs_legend" + ".svg", transparent = True)
    
def makefigure_query_distribution(file_name):
    dpu_num = 2500
    plt.rcParams["savefig.dpi"] = 300
    plt.rcParams["font.size"] = 18
    num_queries = []
    df = pd.read_csv(result_dir + file_name + ".csv")
    batches = [0, 5, 10, 49]
    colors = ["maroon", "brown", "lightcoral", "mistyrose"]
    x_axis = list(range(1, dpu_num+1))
    df_summary = df[df[' DPU'] == -1]
    max_num_queries = max(df_summary[' nqueries'].values.tolist())
    fig = plt.figure()
    ax1 = fig.add_subplot(1,1,1)
    ax1.set_xlim(1,dpu_num+1)
    ax1.set_ylim(1,max_num_queries)
    ax1.set_xlabel("PIM node")
    ax1.set_ylabel("number of queries")
    ax1.set_xscale("log")
    ticks = list(ax1.get_yticks())
    ticks.remove(0)
    ticks.append(40)
    ax1.set_yticks(ticks)
    #ax1.set_yscale("log")
    for i, batch in enumerate(batches):
        df_batch = df[df['batch'] == batch]
        df_batch = df_batch[df_batch[' DPU'] != -1]
        num_queries = df_batch[' nqueries'].values.tolist()
        num_queries.sort(reverse=True)
        ax1.plot(x_axis, num_queries[:dpu_num], label="batch " + str(batch), lw=3, color = colors[i])
    ax1.plot(x_axis, [40]*dpu_num, label="perfectly balanced", color='black', lw=3, ls="--")
    plt.grid()
    plt.legend()
    #plt.grid()
    fig.subplots_adjust(left=0.15, bottom=0.15, right=0.95, top=0.95)
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/distribution_new.png", transparent = False)
    
def makefigure_max_nqueries(file_name, first_batch):
    plt.rcParams["savefig.dpi"] = 300
    df = pd.read_csv(result_dir + file_name + ".csv")
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    df_summary = df[df[' DPU'] == -1]
    send_size=[]
    send_size += df_summary[' nqueries'].values.tolist()
    x_axis = list(range(len(df_summary)))
    plt.rcParams["savefig.dpi"] = 300
    plt.xlabel('batch iteration',fontsize=18)
    ax1.set_ylabel('max # of queries for a DPU',fontsize=18)
    ax1.plot(x_axis[first_batch:len(df_summary)-1], send_size[first_batch:len(df_summary)-1], marker='o')
    #plt.xticks(x_axis[first_batch:len(df_summary)-1])
    #plt.grid()
    ax1.set_xlim(first_batch-0.5,len(df_summary) - 1.5)
    ax1.set_ylim(0,)
    fig.subplots_adjust(left=0.2)
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/max_query_num" + file_name + ".png", transparent = False)

def makefigure_max_nnodes(file_name, first_batch):
    plt.rcParams["savefig.dpi"] = 300
    plt.rcParams["font.size"] = 18
    df = pd.read_csv(result_dir + file_name + ".csv")
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    df_summary = df[df[' DPU'] == -1]
    nnodes=[]
    nnodes += df_summary[' nnodes'].values.tolist()
    x_axis = list(range(len(df_summary)))
    plt.rcParams["savefig.dpi"] = 300
    plt.xlabel('batch iteration',fontsize=18)
    ax1.set_ylabel('max # of nodes in a subtree',fontsize=18)
    ax1.plot(x_axis[first_batch:len(df_summary)-1], nnodes[first_batch:len(df_summary)-1], marker='o')
    #plt.xticks(x_axis[first_batch:len(df_summary)-1])
    #plt.grid()
    ax1.set_xlim(first_batch-0.5,len(df_summary) - 1.5)
    ax1.set_ylim(0,)
    fig.subplots_adjust(left=0.2)
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/max_node_num" + file_name + ".png", transparent = False)
    
# generate graphs
for file_name in file_names:
#     make_csv(file_name)
#     makefigure_query_distribution(file_name)
    makefigure_histgram_memory_legend(file_name)


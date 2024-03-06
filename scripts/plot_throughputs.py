import pandas as pd
import numpy as np
import os
import re
from matplotlib import pyplot as plt
import japanize_matplotlib

# expno
expno = "ppl_response_60M_300batches"

alphas = ["0", "0.4", "0.99"]
ops = ["get"]
build_modes = ["ppl","ppl-50trees"]

# ignore the first row (initialization batch)
first_batch = 1

result_dir = "./" + expno + "/result/"
graph_dir = "./" + expno + "/graphs/"
plus = lambda x, y : x + y

def makefigure_migration(first_batch):
    plt.rcParams["font.family"] = "IPAexMincho"
    plt.rcParams["savefig.dpi"] = 300
    plt.rcParams["font.size"] = 18
    fig, axs = plt.subplots(1, 3, figsize=(15, 5))
    title_headers = ["a) ", "b) ", "c) "]
    handles, labels = [], []
    for j, alpha in enumerate(alphas):
        ax = axs[j]
        ax.set_xlabel('バッチ')
        ax.text(0.5, -0.2, title_headers[j] + f'α = {alpha}', ha='center', va='top', transform=ax.transAxes)
        if (j == 0):
            ax.set_ylabel('スループット (MOP/s)')
        else:
            ax.set_yticklabels([])
        for i, build_mode in enumerate(build_modes):
            for op in ops:
                file_name = build_mode + "_" + alpha + "_" + op
                throughput = []
                df = pd.read_csv(result_dir + file_name + ".csv")
                x_axis = list(range(len(df)))
                throughput = [x / 1000000 for x in df[' throughput'].values.tolist()]
                throughput_bpforest = max(throughput)
                throughput_PIM_skip_list=0.2
                label = '粗い分割' if i == 0 else '細かい分割'
                linestyle = '-' if i == 0 else '--'
                line, = ax.plot(x_axis[first_batch:len(df)-1], throughput[first_batch:len(df)-1], label='細かい分割', linewidth=1, linestyle=linestyle)   
                if j == 0:
                    handles.append(line)
                    labels.append(label)
        loc = list(ax.get_xticks())
        loc.append(1)
        #loc.append(300)
        ax.tick_params(labelsize=14)
        ax.set_xticks(loc)
        ax.set_xlim(first_batch-0.5,len(df) - 1.5)
        ax.set_ylim(0,3.5)
        #plt.grid()
        #fig.subplots_adjust(left=0.15)
    os.makedirs(graph_dir, exist_ok=True)
    fig.legend(handles, labels, loc='upper right', ncol=1, edgecolor='black', fancybox=False, framealpha=1)
    plt.tight_layout(pad=0.2, h_pad=0.4, w_pad=0.4) 
    plt.savefig(graph_dir + "/bp_forest_throughput" + ".svg", transparent = True)
    plt.savefig(graph_dir + "/bp_forest_throughput" + ".png", transparent = False)


def makefigure_migration_slide(first_batch):
    plt.rcParams["savefig.dpi"] = 300
    plt.rcParams["font.size"] = 24
    fig, axs = plt.subplots(1, 2, figsize=(15, 5))
    #title_headers = ["a) ", "b) ", "c) "]
    handles, labels = [], []
    for j, build_mode in enumerate(build_modes):
        ax = axs[j]
        ax.set_xlabel('バッチ目')
        #ax.text(0.5, -0.2, title_headers[j] + f'α = {alpha}', ha='center', va='top', transform=ax.transAxes)
        if (j == 0):
            ax.set_ylabel('スループット (MOP/s)')
        else:
            ax.set_yticklabels([])
        for i, alpha in enumerate(alphas):
            for op in ops:
                file_name = build_mode + "_" + alpha + "_" + op
                throughput = []
                df = pd.read_csv(result_dir + file_name + ".csv")
                x_axis = list(range(len(df)))
                throughput = [x / 1000000 for x in df[' throughput'].values.tolist()]
                throughput_bpforest = max(throughput)
                throughput_PIM_skip_list=0.2
                if i == 0:
                    label = 'α=0'
                    linestyle = 'dotted' 
                elif i == 1:
                    label = 'α=0.4'
                    linestyle = '-' 
                else:
                    label = 'α=0.99'
                    linestyle = '--' 
                line, = ax.plot(x_axis[first_batch:len(df)-1], throughput[first_batch:len(df)-1], label='細かい分割', linewidth=1.5, linestyle=linestyle)   
                if j == 0:
                    handles.append(line)
                    labels.append(label)
        loc = list(ax.get_xticks())
        loc.append(1)
        #loc.append(300)
        ax.tick_params(labelsize=24)
        ax.set_xticks(loc)
        ax.set_xlim(first_batch-0.5,len(df) - 1.5)
        ax.set_ylim(0,4.5)
        #plt.grid()
        #fig.subplots_adjust(left=0.15)
    os.makedirs(graph_dir, exist_ok=True)
    fig.legend(handles, labels, loc='upper right', ncol=1, edgecolor='black', fancybox=False, framealpha=1)
    plt.tight_layout(pad=0.2, h_pad=0.4, w_pad=0.4) 
    plt.savefig(graph_dir + "/bp_forest_throughput_slide" + ".svg", transparent = True)
    plt.savefig(graph_dir + "/bp_forest_throughput_slide" + ".png", transparent = False)

makefigure_migration_slide(first_batch)
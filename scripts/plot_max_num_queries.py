import pandas as pd
import numpy as np
import os
import re
from matplotlib import pyplot as plt
import japanize_matplotlib

# expno
expno = "ppl_response_60M_rize"

alphas = ["0", "0.4", "0.99"]
ops = ["get"]
build_modes = ["print-distribution", "print-distribution-50trees"]

# ignore the first row (initialization batch)
first_batch = 1

result_dir = "./" + expno + "/result/"
graph_dir = "./" + expno + "/graphs/"
plus = lambda x, y : x + y

def max_query_nums_from_distribution(first_batch):
    plt.rcParams["font.family"] = "IPAexMincho"
    plt.rcParams["savefig.dpi"] = 300
    plt.rcParams["font.size"] = 18
    fig, axs = plt.subplots(1, 2, figsize=(12, 5))
    title_headers = ["a) ", "b) ", "c) "]
    handles, labels = [], []
    for j, alpha in enumerate(["0.4", "0.99"]):
        ax = axs[j]
        ax.text(0.5, -0.2, title_headers[j] + f'α = {alpha}', ha='center', va='top', transform=ax.transAxes)
        if (j == 0):
            ax.set_ylabel('最集中クエリ数')
        ax.set_xlabel('バッチ')
        for i, build_mode in enumerate(build_modes):
            for op in ops:
                file_name = build_mode + "_" + alpha + "_" + op
                send_size = []
                df = pd.read_csv(result_dir + file_name + ".csv")
                df_summary = df[df[' DPU'] == -1]
                #print(df_summary.head())
                x_axis = list(range(len(df_summary)))
                send_size += df_summary[' nqueries'].values.tolist()
                label = '粗い分割' if i == 0 else '細かい分割'
                linestyle = '-' if i == 0 else '--'
                line, = ax.plot(x_axis[first_batch:len(df_summary)-1], send_size[first_batch:len(df_summary)-1], label=label, linestyle=linestyle)                
                #plt.xticks(x_axis[first_batch:len(df)-1])
                if j == 0:
                    handles.append(line)
                    labels.append(label)
        ax.tick_params(labelsize=14)
        ax.set_xlim(1, 299)
        ax.set_ylim(0,)
        ax.set_xticks([1,100,200])
    #fig.subplots_adjust(left=0.2)
    fig.legend(handles, labels, loc='upper right', ncol=1, edgecolor='black', fancybox=False, framealpha=1)
    plt.tight_layout(pad=0.7, h_pad=0.4, w_pad=0.2) 
    plt.savefig(graph_dir + "/bp_forest_max_num_queries" + ".svg", transparent = True)
    plt.savefig(graph_dir + "/bp_forest_max_num_queries"  + ".png", transparent = False)

max_query_nums_from_distribution(first_batch)

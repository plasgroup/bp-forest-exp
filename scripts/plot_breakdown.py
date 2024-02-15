import pandas as pd
import numpy as np
import os
import re
from matplotlib import pyplot as plt
import japanize_matplotlib

# expno
expno = "ppl_response_60M_300batches"

alphas = ["0", "0.4", "0.99"]
build_modes = ["ppl", "ppl-50trees"]

# ignore the first row (initialization batch)
first_batch = 1

result_dir = "./" + expno + "/result/"
graph_dir = "./" + expno + "/graphs/"
plus = lambda x, y : x + y

title_headers = ["a) ", "b) ", "c) "]
def plot_time_bar_migration(ax, file_name, first_batch):
    colors = ["black", "maroon", "lightcoral", "mistyrose"]
    df = pd.read_csv(result_dir + file_name + ".csv")
    x_axis = list(range(len(df)))
    migration_time = df[' migration_time'].values.tolist()
    send_time = df[' send_time'].values.tolist()
    execution_time = df[' execution_time'].values.tolist()
    preprocess_time1 = df[' preprocess_time1'].values.tolist()
    preprocess_time2 = df[' preprocess_time2'].values.tolist()
    preprocess_time = list(map(plus, preprocess_time1, preprocess_time2))
    receive_result_time = df[' receive_result_time'].values.tolist()
    communication_time = list(map(plus, send_time, receive_result_time))
    ax.bar(x_axis[first_batch:len(df) - 1], migration_time[first_batch:len(df) - 1], bottom=[execution_time[i] + communication_time[i] + preprocess_time[i] for i in range(first_batch, len(df) - 1)], label="1. 部分木の移動", color=colors[1])
    ax.bar(x_axis[first_batch:len(df) - 1], preprocess_time[first_batch:len(df) - 1], bottom=[execution_time[i] + communication_time[i] for i in range(first_batch, len(df) - 1)], label="2. クエリの前処理", color=colors[0])
    ax.bar(x_axis[first_batch:len(df) - 1], communication_time[first_batch:len(df) - 1], bottom=execution_time[first_batch:len(df) - 1], label="3. クエリの送信 + 5. 結果の受信", color=colors[2])
    ax.bar(x_axis[first_batch:len(df) - 1], execution_time[first_batch:len(df) - 1], label="4. クエリの実行", color=colors[3])
    ax.set_xlim(first_batch-0.5,len(df) - 1.5)
    ax.set_ylim(0, 0.17)
    ax.tick_params(labelsize=18)
# 2x3のサブプロットを持つ1枚の図を生成
fig, axs = plt.subplots(2, 3, figsize=(18, 10), sharex=True, sharey=True)
fig.subplots_adjust(hspace=0.5, wspace=0.3)

for i, build_mode in enumerate(build_modes):
    for j, alpha in enumerate(alphas):
        ax = axs[i, j]
        plt.rcParams["font.family"] = "IPAexMincho"
        plt.rcParams["savefig.dpi"] = 300
        plt.rcParams["font.size"] = 18
        if i == 1:
            ax.set_xlabel('バッチ', fontsize=24)
            ax.text(0.5, -0.2, f'{title_headers[j]} α = {alpha}', ha='center', va='top', transform=ax.transAxes, fontsize=28)
            
        if j == 0:
            ax.set_ylabel('実行時間 (秒)', fontsize=24)
        fig.text(0.01, 0.77, '粗い分割', va='center', rotation='vertical', fontsize=28)
        fig.text(0.01, 0.34, '細かい分割', va='center', rotation='vertical', fontsize=28)
        file_name = build_mode + "_" + alpha + "_get"
        plot_time_bar_migration(ax, file_name, first_batch)
        loc = list(ax.get_xticks())
        ax.set_xticks(loc)
        #ax.set_title(f'Build Mode: {build_mode}, α = {alpha}')
        
handles, labels = ax.get_legend_handles_labels()
fig.legend(handles, labels, loc='upper right', borderaxespad=0, ncol=1, edgecolor='black', fancybox=False, framealpha=1)

os.makedirs(graph_dir, exist_ok=True)
plt.tight_layout(h_pad=0.2, w_pad=0.4) 
fig.subplots_adjust(left=0.12)
plt.savefig(graph_dir + "bp_forest_breakdown.png", transparent=False)
plt.savefig(graph_dir + "bp_forest_breakdown.svg", transparent=True)

import cairosvg
svg_file = graph_dir + "bp_forest_breakdown.svg"
pdf_file = graph_dir + "bp_forest_breakdown.pdf"
# SVGをPDFに変換
cairosvg.svg2pdf(url=svg_file, write_to=pdf_file)
            
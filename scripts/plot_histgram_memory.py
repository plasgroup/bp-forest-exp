import pandas as pd
import numpy as np
import os
import re
from matplotlib import pyplot as plt
import japanize_matplotlib

# expno
expno = "ppl_response_memory"
# base file names to convert from csv to graph
file_names = ["print-distribution_0.4_insert", "print-distribution_0.99_insert"]

result_dir = "./" + expno + "/result/"
graph_dir = "./" + expno + "/graphs/"

def makefigure_histgram_memory(file_name):
    plt.rcParams["savefig.dpi"] = 300
    plt.rcParams["font.size"] = 30
    #plt.rcParams["font.family"] = "Times New Roman"
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
    plt.savefig(graph_dir + file_name + "/distribution_kvpairs" + file_name + ".png", transparent = False)
    plt.savefig(graph_dir + file_name + "/distribution_kvpairs" + file_name + ".svg", transparent = True)

from mpl_toolkits.axes_grid1 import make_axes_locatable
def makefigure_histgrams_memory(file_names):
    #plt.rcParams["font.family"] = "IPAexMincho"
    plt.rcParams["savefig.dpi"] = 300
    plt.rcParams["font.size"] = 30
    batches = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 50, 100, 200, 499]
    fig, axs = plt.subplots(len(batches), 2, figsize=(15, 12), sharey=True) 
    for j, file_name in enumerate(file_names):
        df = pd.read_csv(result_dir + file_name + ".csv")

        df_summary = df[df[' DPU'] == -1]
        max_num_kvpairs = max(df_summary[' nkvpairs'].values.tolist())
        for i, batch in enumerate(batches):
            ax = axs[i][j] # 右または左のサブプロットを選択
            df_batch = df[df['batch'] == batch]
            num_kvpairs = df_batch[' nkvpairs'].values.tolist()
            ax.hist(num_kvpairs, bins=10, range=(0, max_num_kvpairs), log=True, color="black")
            ax.set_xlim(0, max_num_kvpairs+10000)
            ax.set_yticks([])
            if i < len(batches) - 1: # 最後のバッチ以外はx軸のラベルを非表示
                ax.tick_params(labelbottom=False)
                ax.set_xticks([])
                ax.set_xticklabels([])
            else:
                ax.set_xlabel("キーと値のペア数", fontsize=30)
            if j == 0: # 左側の図にbatchesの値を表示
                ax.text(-0.02, 0.5, str(batch), transform=ax.transAxes, va='center', ha='right', fontsize=30)
        fig.text(0.06, 0.97, "バッチ目", fontsize=30, ha='center', va='center')
        fig.text(0.28, 0.03, f"a) α = 0.4", ha='center', va='top', fontsize=30)
        fig.text(0.78, 0.03, f"b) α = 0.99", ha='center', va='top', fontsize=30)
    os.makedirs(graph_dir, exist_ok=True)
    plt.tight_layout(h_pad=0.2, w_pad=0.1, pad = 0.3) 
    fig.subplots_adjust(bottom = 0.1, top = 0.95)
    plt.savefig(graph_dir + "insert_hists_slide.png", transparent=False)
    plt.savefig(graph_dir + "insert_hists_slide.svg", transparent=True)



makefigure_histgrams_memory(file_names)

#import cairosvg
#svg_file = graph_dir + "insert_hists.svg"
#pdf_file = graph_dir + "insert_hists.pdf"
# SVGをPDFに変換
#cairosvg.svg2pdf(url=svg_file, write_to=pdf_file)
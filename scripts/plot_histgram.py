import pandas as pd
import numpy as np
import os
import re
from matplotlib import pyplot as plt

# expno
expno = "1"
# base file names to convert from csv to graph
file_names = ["print-distribution_0", "print-distribution_0.2", "print-distribution_0.4", "print-distribution_0.6", "print-distribution_0.8", "print-distribution_0.99", "print-distribution_1.2"]

result_dir = "./" + expno + "/result/"
graph_dir = "./" + expno + "/graphs/"

    
def makefigure_histgram(file_name):
    plt.rcParams["savefig.dpi"] = 300
    num_nodes = []
    num_queries = []
    df = pd.read_csv(result_dir + file_name + ".csv")
    batches = [0, 5, 10]
    fig = plt.figure()
    for i, batch in enumerate(batches):
        df_batch = df[df['batch'] == batch]
        num_nodes = df_batch[' nnodes'].values.tolist()
        num_queries = df_batch[' nqueries'].values.tolist()

        ax1 = fig.add_subplot(len(batches),2,2*i+1)
        ax2 = fig.add_subplot(len(batches),2,2*i+2)
        ax1.hist(num_nodes)
        ax2.hist(num_queries)
    #plt.grid()
    #fig.subplots_adjust(left=0.15)
    os.makedirs(graph_dir + file_name, exist_ok=True)
    plt.savefig(graph_dir + file_name + "/throughput_" + file_name + ".png", transparent = False)
    
# generate graphs
makefigure_histgram("print-distribution_0.99"):
for file_name in file_names:
    makefigure_histgram(file_name)
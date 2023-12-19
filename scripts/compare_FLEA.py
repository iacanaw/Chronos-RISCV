import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from get_data import get_data


def autolabel(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax.annotate('{}'.format(height),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom')

plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

labels   = ["A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "C1", "C2", "C3", "C4", "C5"]
heuristics = ['Grouped', 'Pattern', 'PID', 'PID (MIG)', 'FLEA+', 'FLEA+ (MIG)']
colors = ['#F51839', '#F59018', '#0CADF5', '#00F5BA',  '#0CF53B', '#E2F530']

x = np.arange(len(labels))  # the label locationss
width = 0.85/len(heuristics)  # the width of the bars


def generate_graph(TSV_label, Y_label, graph_label, limitINF, limitSUP, fig_name):
    fig, ax = plt.subplots(figsize =(8,3.2))
    ax.yaxis.grid(True, color='black', linestyle='-', linewidth=0.5)
    rects=[]
    data = []
    #get data
    for i in range(len(heuristics)):
        data.append(get_data(TSV_label, heuristics[i]))
        print(get_data(TSV_label, heuristics[i]))
    #print
    for i in range(len(heuristics)):
        rects.append(ax.bar((x - width*len(heuristics)/2) + i*width, data[i], width, label=heuristics[i], color=colors[i], edgecolor='black', linewidth=0.5))
    #add cosméticos
    ax.set_ylabel(Y_label)
    ax.set_title(graph_label)
    ax.set_xticks(x)
    ax.set_xticklabels(labels)
    ax.set_ylim(limitINF,limitSUP)
    ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.093), fancybox=False, shadow=False, ncol=6, fontsize='8')
    fig.tight_layout()
    plt.xticks(rotation=0)
    fig.savefig(fig_name+'.png', format='png', dpi=300, bbox_inches='tight')
    fig.savefig(fig_name+'.pdf', format='pdf', bbox_inches='tight')

if __name__ == '__main__': # chamada da funcao principal
    TSV_label = 'Avg(Temp)'
    Y_label = 'Temperature (°C)'
    graph_label = 'Average Temperature'
    limitINF = 60
    limitSUP = 100
    fig_name = 'avg_temp'
    generate_graph(TSV_label, Y_label, graph_label, limitINF, limitSUP, fig_name)

    TSV_label = 'AvgHop'
    Y_label = "Hop count"
    graph_label = 'Average Hop count'
    limitINF = 0
    limitSUP = 10
    fig_name = 'avg_hop'
    generate_graph(TSV_label, Y_label, graph_label, limitINF, limitSUP, fig_name)

    TSV_label = 'Migrations'
    Y_label = "Migrations"
    graph_label = 'Registred Migrations'
    limitINF = 0
    limitSUP = 100
    fig_name = 'avg_mig'
    generate_graph(TSV_label, Y_label, graph_label, limitINF, limitSUP, fig_name)

    TSV_label = 'Avg(PeakTemp)'
    Y_label = "Temperature (°C)"
    graph_label = 'Average Peak Temperature'
    limitINF = 70
    limitSUP = 110
    fig_name = 'avg_peak'
    generate_graph(TSV_label, Y_label, graph_label, limitINF, limitSUP, fig_name)

    TSV_label = 'MTTF monte'
    Y_label = "MTTF (years)"
    graph_label = 'Mean Time to Failure'
    limitINF = 0
    limitSUP = 10
    fig_name = 'avg_mttf'
    generate_graph(TSV_label, Y_label, graph_label, limitINF, limitSUP, fig_name)


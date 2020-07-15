import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Patch
import sys


inputFile=open(sys.argv[1]).readlines()
opType=sys.argv[2]
inputFile=[list(map(float,x.split("\t"))) for x in inputFile ]

def prepare(arr2):
    arr=arr2.copy()
    bottoms=[]
    for l in arr:
        bottoms.append([0]*len(l))

    for i in range(len(arr[0])):
        tmp=sorted(range(len(arr)),key=lambda x:arr[x][i])
        for k in range(1,4):
            for j in range(k):
                arr[tmp[k]][i]-=arr[tmp[j]][i]
                bottoms[tmp[k]][i]+=arr[tmp[j]][i]
    return (arr,bottoms)


mqf_res=prepare([inputFile[0],inputFile[1],inputFile[2],inputFile[3]])
# mqf_fpr_0_01 = np.array(inputFile[0])
# mqf_fpr_0_001 = np.array(inputFile[1])
# mqf_fpr_0_0001 = np.array(inputFile[2])
# mqf_fpr_0_001-=mqf_fpr_0_0001
# mqf_fpr_0_01-=mqf_fpr_0_001+mqf_fpr_0_0001


cqf_res=prepare([inputFile[4],inputFile[5],inputFile[6],inputFile[7]])
# cqf_fpr_0_01 = np.array(inputFile[3])
# cqf_fpr_0_001 = np.array(inputFile[4])
# cqf_fpr_0_0001 = np.array(inputFile[5])
# cqf_fpr_0_001-=cqf_fpr_0_0001
# cqf_fpr_0_01-=cqf_fpr_0_001+cqf_fpr_0_0001

bmqf_res=prepare([inputFile[8],inputFile[9],inputFile[10],inputFile[11]])
# bmqf_fpr_0_01 = np.array(inputFile[6])
# bmqf_fpr_0_001 = np.array(inputFile[7])
# bmqf_fpr_0_0001 = np.array(inputFile[8])
# bmqf_fpr_0_001-=bmqf_fpr_0_0001
# bmqf_fpr_0_01-=bmqf_fpr_0_001+bmqf_fpr_0_0001
# there is a problem here because bmqf_fpr_0_01 > bmqf_fpr_0_001

CountminKhmer_res=prepare([inputFile[12],inputFile[13],inputFile[14],inputFile[15]])
# CountminKhmer_fpr_0_01 = np.array(inputFile[9])
# CountminKhmer_fpr_0_001 = np.array(inputFile[10])
# CountminKhmer_fpr_0_0001 = np.array(inputFile[11])
# CountminKhmer_fpr_0_001-=CountminKhmer_fpr_0_0001
# CountminKhmer_fpr_0_01-=CountminKhmer_fpr_0_001+CountminKhmer_fpr_0_0001

Countmin_res=prepare([inputFile[16],inputFile[17],inputFile[18],inputFile[19]])
# Countmin_fpr_0_01 = np.array(inputFile[12])
# Countmin_fpr_0_001 = np.array(inputFile[13])
# Countmin_fpr_0_0001 = np.array(inputFile[14])
# Countmin_fpr_0_001-=Countmin_fpr_0_0001
# Countmin_fpr_0_01-=Countmin_fpr_0_001+Countmin_fpr_0_0001




distributions = ['Zipfian Z=2', 'Zipfian Z=3', 'Zipfian Z=5','Real Kmers']
fig, ax = plt.subplots()

bar_width = 0.35
epsilon = .035
line_width = 1
opacity = 1
mqf_bar_positions = np.arange(len(mqf_res[0][0]))*2.5
cqf_bar_positions = mqf_bar_positions + bar_width
bmqf_bar_positions = mqf_bar_positions + 2*bar_width
CountminKhmer_bar_positions = mqf_bar_positions + 3*bar_width
Countmin_bar_positions = mqf_bar_positions + 4*bar_width

mqfColor='#d73027'
cqfColor='#fc8d59'
bmqfColor='#fee090'
CountminKhmerColor='#91bfdb'
CountminColor='#4575b4'


# make bar plots
mqf_fpr_0_0001_bar = plt.bar(mqf_bar_positions, mqf_res[0][3], bar_width-epsilon,
                          color=mqfColor,
                             edgecolor=mqfColor,
                             linewidth=line_width,
                             bottom=mqf_res[1][3],
                          label='MQF FPR 0.0001')
mqf_fpr_0_001_bar = plt.bar(mqf_bar_positions, mqf_res[0][2], bar_width-epsilon,
                          bottom=mqf_res[1][2],
                          alpha=opacity,
                          color='white',
                          edgecolor=mqfColor,
                          linewidth=line_width,
                          hatch='//',
                          label='MQF FPR 0.001')
mqf_fpr_0_01_bar = plt.bar(mqf_bar_positions, mqf_res[0][1], bar_width-epsilon,
                           bottom=mqf_res[1][1],
                           alpha=opacity,
                           color='white',
                           edgecolor=mqfColor,
                           linewidth=line_width,
                           hatch='0',
                           label='MQF FPR 0.01')
mqf_fpr_0_1_bar = plt.bar(mqf_bar_positions, mqf_res[0][0], bar_width-epsilon,
                           bottom=mqf_res[1][0],
                           alpha=opacity,
                           color='white',
                           edgecolor=mqfColor,
                           linewidth=line_width,
                           hatch='.',
                           label='MQF FPR 0.1')

cqf_fpr_0_0001_bar = plt.bar(cqf_bar_positions, cqf_res[0][3], bar_width- epsilon,
                             color=cqfColor,
                             bottom=cqf_res[1][3],
                             linewidth=line_width,
                             edgecolor=cqfColor,
                             ecolor="#0000DD",
                          label='CQF FPR 0.0001')
cqf_fpr_0_001_bar = plt.bar(cqf_bar_positions, cqf_res[0][2], bar_width-epsilon,
                          bottom=cqf_res[1][2],
                          color="white",
                          hatch='//',
                          edgecolor=cqfColor,
                          ecolor="#0000DD",
                          linewidth=line_width,
                          label='CQF FPR 0.001')
cqf_fpr_0_01_bar = plt.bar(cqf_bar_positions, cqf_res[0][1], bar_width-epsilon,
                           bottom=cqf_res[1][1],
                           color="white",
                           hatch='0',
                           edgecolor=cqfColor,
                           linewidth=line_width,
                           label='CQF FPR 0.01')
cqf_fpr_0_1_bar = plt.bar(cqf_bar_positions, cqf_res[0][0], bar_width-epsilon,
                           bottom=cqf_res[1][0],
                           color="white",
                           hatch='.',
                           edgecolor=cqfColor,
                           linewidth=line_width,
                           label='CQF FPR 0.1')


CountminKhmer_fpr_0_0001_bar = plt.bar(CountminKhmer_bar_positions, CountminKhmer_res[0][3], bar_width- epsilon,
                          color=CountminKhmerColor,
                          bottom=CountminKhmer_res[1][3],
                          edgecolor=CountminKhmerColor,
                          linewidth=line_width,
                          label='CMS Khmer FPR 0.0001')
CountminKhmer_fpr_0_001_bar = plt.bar(CountminKhmer_bar_positions, CountminKhmer_res[0][2], bar_width-epsilon,
                          bottom=CountminKhmer_res[1][2],
                          alpha=opacity,
                          color='white',
                          edgecolor=CountminKhmerColor,
                          linewidth=line_width,
                          hatch='//',
                          label='CMS Khmer FPR 0.001')
CountminKhmer_fpr_0_01_bar = plt.bar(CountminKhmer_bar_positions, CountminKhmer_res[0][1], bar_width-epsilon,
                           bottom=CountminKhmer_res[1][1],
                           alpha=opacity,
                           color='white',
                           edgecolor=CountminKhmerColor,
                           linewidth=line_width,
                           hatch='0',
                           label='CMS Khmer FPR 0.01')

CountminKhmer_fpr_0_1_bar = plt.bar(CountminKhmer_bar_positions, CountminKhmer_res[0][0], bar_width-epsilon,
                           bottom=CountminKhmer_res[1][0],
                           alpha=opacity,
                           color='white',
                           edgecolor=CountminKhmerColor,
                           linewidth=line_width,
                           hatch='.',
                           label='CMS Khmer FPR 0.1')


bmqf_fpr_0_0001_bar = plt.bar(bmqf_bar_positions, bmqf_res[0][3], bar_width- epsilon,
                          bottom=bmqf_res[1][3],
                          color=bmqfColor,
                          edgecolor=bmqfColor,
                          linewidth=line_width,
                          label='Buffered MQF FPR 0.0001')
bmqf_fpr_0_001_bar = plt.bar(bmqf_bar_positions, bmqf_res[0][2], bar_width-epsilon,
                          bottom=bmqf_res[1][2],
                          alpha=opacity,
                          color='white',
                          edgecolor=bmqfColor,
                          linewidth=line_width,
                          hatch='//',
                          label='Buffered MQF FPR 0.001')
bmqf_fpr_0_01_bar = plt.bar(bmqf_bar_positions, bmqf_res[0][1], bar_width-epsilon,
                           bottom=bmqf_res[1][1],
                           alpha=opacity,
                           color='white',
                           edgecolor=bmqfColor,
                           linewidth=line_width,
                           hatch='0',
                           label='Buffered MQF FPR 0.01')
bmqf_fpr_0_1_bar = plt.bar(bmqf_bar_positions, bmqf_res[0][0], bar_width-epsilon,
                           bottom=bmqf_res[1][0],
                           alpha=opacity,
                           color='white',
                           edgecolor=bmqfColor,
                           linewidth=line_width,
                           hatch='.',
                           label='Buffered MQF FPR 0.1')

Countmin_fpr_0_0001_bar = plt.bar(Countmin_bar_positions, Countmin_res[0][3], bar_width- epsilon,
                          color=CountminColor,
                          bottom=Countmin_res[1][3],
                          edgecolor=CountminColor,
                          linewidth=line_width,
                          label='CMS FPR 0.0001')
Countmin_fpr_0_001_bar = plt.bar(Countmin_bar_positions, Countmin_res[0][2], bar_width-epsilon,
                          bottom=Countmin_res[1][2],
                          alpha=opacity,
                          color='white',
                          edgecolor=CountminColor,
                          linewidth=line_width,
                          hatch='//',
                          label='CMS FPR 0.001')
Countmin_fpr_0_01_bar = plt.bar(Countmin_bar_positions, Countmin_res[0][1], bar_width-epsilon,
                           bottom=Countmin_res[1][1],
                           alpha=opacity,
                           color='white',
                           edgecolor=CountminColor,
                           linewidth=line_width,
                           hatch='0',
                           label='CMS FPR 0.01')

Countmin_fpr_0_1_bar = plt.bar(Countmin_bar_positions, Countmin_res[0][0], bar_width-epsilon,
                           bottom=Countmin_res[1][0],
                           alpha=opacity,
                           color='white',
                           edgecolor=CountminColor,
                           linewidth=line_width,
                           hatch='.',
                           label='CMS FPR 0.1')



plt.xticks(bmqf_bar_positions, distributions, rotation=45)
plt.ylabel('Million of %s Per Second'%opType)

legend_elements = [
    Patch(facecolor=mqfColor,label='MQF',linewidth=0.5,edgecolor='black'),
    Patch(facecolor=cqfColor,label='CQF',linewidth=0.5,edgecolor='black'),
    Patch(facecolor=bmqfColor,label='Bufferd MQF',linewidth=0.5,edgecolor='black'),
    Patch(facecolor=CountminKhmerColor,label='CMS Khmer',linewidth=0.5,edgecolor='black'),
    Patch(facecolor=CountminColor,label='CMS',linewidth=0.5,edgecolor='black')
]
fpr_leged=[Patch(facecolor="black",label='0.0001',linewidth=0.5,edgecolor='black'),
    Patch(facecolor="white",label='0.001',hatch='//',linewidth=0.5,edgecolor='black'),
    Patch(facecolor="white",label='0.01',hatch='0',linewidth=0.5,edgecolor='black'),
    Patch(facecolor="white",label='0.1',hatch='.',linewidth=0.5,edgecolor='black')
    ]

#l1=plt.legend(handles=legend_elements, bbox_to_anchor=(1.19, 0.95),
#              fancybox=True,title='Data Structures')
#l2=plt.legend(handles=fpr_leged, bbox_to_anchor=(1.171, 0.650),
#              fancybox=True,title='False Positive Rates')
l1=plt.legend(handles=legend_elements, bbox_to_anchor=(1., 0.95),
              fancybox=True,title='Data Structures')
l2=plt.legend(handles=fpr_leged, bbox_to_anchor=(1., 0.450),
              fancybox=True,title='False Positive Rates')

ax.add_artist(l1)
ax.add_artist(l2)
#    plt.legend(loc='best')
#ax.legend()
 #    sns.despine()
#plt.show()
fig.set_size_inches(5.5, 3.5)
fig.savefig(opType+'.png',bbox_inches='tight', dpi=fig.dpi)

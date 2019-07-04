import sys

inputFolder=sys.argv[1]
outPrefix=sys.argv[2]

insertionOut=open(outPrefix+"."+"insertion",'w')
queryOut=open(outPrefix+"."+"query",'w')

distributions=['z2','z3','z5','kmers']
fprs=['f0.1','f0.01','f0.001','f0.0001']
trials=['1','2','3','4','5','6','7','8','9','10']
datastructures=['MQF','CQF','Buffered MQF','CountMinKhmer','CountMin']
insertion={}
query={}

def MInsertionsPerSecond(numElements,milliseconds):
    return (float(numElements)/1000000.0)/(float(milliseconds)/1000.0)

numInsertionsTag="Number of insertions"

for f in fprs:
    insertion[f]={}
    query[f]={}
    for d in distributions:
        insertion[f][d]=dict([(x,[]) for x in datastructures])
        query[f][d]=dict([(x,[]) for x in datastructures])
        numQueries=0
        for t in trials:
            filename=inputFolder+"speed.%s.s28.%s.%s"%(d,f,t)
            for l in open(filename):
                if "Number of insertions =" in l:
                    numInsertions=int(l.split("=")[1])
                if "Number of non succesfull lookups = " in l:
                    numQueries+=int(l.split("=")[1])
                if "Number of succesfull lookups = " in l:
                    numQueries+=int(l.split("=")[1])

                ll=l.split("\t")
                if ll[0] in datastructures:
                    insertion[f][d][ll[0]].append(MInsertionsPerSecond(numInsertions,int(ll[4])))
                    query[f][d][ll[0]].append(MInsertionsPerSecond(numQueries,int(ll[5])))

for s in datastructures:
    for f in fprs:
        tmpInsertion=[]
        tmpQuery=[]
        for d in distributions:
            avgInsertion=sum(insertion[f][d][s])/len(insertion[f][d][s])
            avgQuery=sum(query[f][d][s])/len(query[f][d][s])
            tmpInsertion.append(str((avgInsertion)))
            tmpQuery.append(str((avgQuery)))
        insertionOut.write("\t".join(tmpInsertion)+"\n")
        queryOut.write("\t".join(tmpQuery)+"\n")

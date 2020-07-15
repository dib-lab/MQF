#commit 103ba80e3ee589057cc644bea0f78d3bd850bf84 (HEAD -> benchmark, origin/benchmark)
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR105/005/ERR1050075/ERR1050075_1.fastq.gz 
wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR105/005/ERR1050075/ERR1050075_2.fastq.gz

gzip -dc ERR1050075_1.fastq.gz ERR1050075_2.fastq.gz > ERR1050075.fastq

mkdir res/ 

parallel --gnu -j1 './speedPerformance -s 28 -d kmers -k ERR1050075.fastq -f {1} > res/speed.kmers.s28.f{1}.{2} 2> speed.kmers.s28.f{1}.{2}.log ' ::: 0.0001  0.001 0.01 0.1 ::: 1 2 3 4 5 6 7 8 9 10
parallel --gnu -j1 './speedPerformance -z {2} -s 28  -f {1}  > res/speed.z{2}.s28.f{1}.{3} 2> speed.z{2}.s28.f{1}.{3}.log' :::  0.0001 0.001 0.01 0.1  ::: 2 3 5 ::: 1 2 3 4 5 6 7 8 9 10

python3 generateSpeedTable.py res/ restable
python3 plotSpeed.py restable.insertion Insertions 
python3 plotSpeed.py restable.query Queries 

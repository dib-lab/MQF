#commit f105b74a5577793b630c1ec575380b4741e102d6 (HEAD -> BufferedMQFIntegration, origin/BufferedMQFIntegration)

##### Prepare the environement
##############################
git clone https://github.com/dib-lab/khmer.git
git checkout f105b74a5577793b630c1ec575380b4741e102d6
virtualenv -p python3 envname
source env/bin/activate
make install-dep
make all install
##################################3


####Download Data
##########################
wget ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/data/NA12878/NIST_NA12878_HG001_HiSeq_300x/140407_D00360_0017_BH947YADXX/Project_RM8398/Sample_U0a/U0a_CGATGT_L001_R1_001.fastq.gz
wget ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/data/NA12878/NIST_NA12878_HG001_HiSeq_300x/140407_D00360_0017_BH947YADXX/Project_RM8398/Sample_U0a/U0a_CGATGT_L001_R1_002.fastq.gz
wget ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/data/NA12878/NIST_NA12878_HG001_HiSeq_300x/140407_D00360_0017_BH947YADXX/Project_RM8398/Sample_U0a/U0a_CGATGT_L001_R1_003.fastq.gz
wget ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/data/NA12878/NIST_NA12878_HG001_HiSeq_300x/140407_D00360_0017_BH947YADXX/Project_RM8398/Sample_U0a/U0a_CGATGT_L001_R1_004.fastq.gz
wget ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/data/NA12878/NIST_NA12878_HG001_HiSeq_300x/140407_D00360_0017_BH947YADXX/Project_RM8398/Sample_U0a/U0a_CGATGT_L002_R1_001.fastq.gz
wget ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/data/NA12878/NIST_NA12878_HG001_HiSeq_300x/140407_D00360_0017_BH947YADXX/Project_RM8398/Sample_U0a/U0a_CGATGT_L002_R1_002.fastq.gz
wget ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/data/NA12878/NIST_NA12878_HG001_HiSeq_300x/140407_D00360_0017_BH947YADXX/Project_RM8398/Sample_U0a/U0a_CGATGT_L002_R1_003.fastq.gz
wget ftp://ftp-trace.ncbi.nlm.nih.gov/giab/ftp/data/NA12878/NIST_NA12878_HG001_HiSeq_300x/140407_D00360_0017_BH947YADXX/Project_RM8398/Sample_U0a/U0a_CGATGT_L002_R1_004.fastq.gz
######################################################333



####Run
#######################

/usr/bin/time -v  -o mqf.exact.time python scripts/normalize-by-median.py -o - -k 23 -U 1044756168  --mqf  --fp-rate 0.00000001  ../data/WGS/U0a/U0a_CGATGT_L00* 2>  ../data/WGS/U0a/mqf.exact.log | gzip -c > ../data/WGS/U0a/mqf.keep.exact.fastq.gz


/usr/bin/time -v  -o mqf.f0.0001.time python scripts/normalize-by-median.py -o - -k 23 -U 1044756168  --mqf  --fp-rate 0.0001  U0a_CGATGT_L00* 2>  mqf.f0.0001.log | gzip -c > mqf.keep.f0.0001.fastq.gz

/usr/bin/time -v  -o countmin.f0.0001.time python scripts/normalize-by-median.py --force -o - -k 23 -U 1044756168    --fp-rate 0.0001 -M 4.5979G -N 10 U0a_CGATGT_L00* 2>  countmin.f0.0001.log | gzip -c > countmin.keep.f0.0001.fastq.gz


/usr/bin/time -v  -o mqf.f0.001.time python scripts/normalize-by-median.py -o - -k 23 -U 1044756168  --mqf  --fp-rate 0.001  U0a_CGATGT_L00* 2>  mqf.f0.001.log | gzip -c > mqf.keep.f0.001.fastq.gz


/usr/bin/time -v  -o countmin.f0.001.time python scripts/normalize-by-median.py --force -o - -k 23 -U 1044756168    --fp-rate 0.001 -M 3.523G -N 7 U0a_CGATGT_L00* 2>  countmin.f0.001.log | gzip -c > countmin.keep.f0.001.fastq.gz


/usr/bin/time -v  -o mqf.f0.01.time python scripts/normalize-by-median.py -o - -k 23 -U 1044756168  --mqf  --fp-rate 0.01  U0a_CGATGT_L00* 2>  mqf.f0.01.log | gzip -c > mqf.keep.f0.01.fastq.gz

/usr/bin/time -v  -o countmin.f0.01.time python scripts/normalize-by-median.py --force -o - -k 23 -U 1044756168    --fp-rate 0.01 -M 2.718G -N 5 U0a_CGATGT_L00* 2>  countmin.f0.01.log | gzip -c > countmin.keep.f0.01.fastq.gz


/usr/bin/time -v  -o mqf.f0.1.time python scripts/normalize-by-median.py -o - -k 23 -U 1044756168  --mqf  --fp-rate 0.1  U0a_CGATGT_L00* 2>  mqf.f0.1.log | gzip -c > mqf.keep.f0.1.fastq.gz

/usr/bin/time -v  -o countmin.f0.01.time python scripts/normalize-by-median.py --force -o - -k 23 -U 1044756168    --fp-rate 0.1 -M 1.91G -N 3 U0a_CGATGT_L00* 2>  countmin.f0.1.log | gzip -c > countmin.keep.f0.1.fastq.gz



/usr/bin/time -v  -o mqf.trim.f0.0001.time scripts/trim-low-abund.py -k 23 -Z 20 -C 3 --mqf -U 1044756168  --fp-rate 0.0001 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> mqf.trim.f0.0001.log  | gzip -c > mqf.trimmed.f0.0001.fastq.gz

/usr/bin/time -v  -o mqf.trim.f0.001.time scripts/trim-low-abund.py -k 23 -Z 20 -C 3 --mqf -U 1044756168  --fp-rate 0.001 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> mqf.trim.f0.001.log  | gzip -c > mqf.trimmed.f0.001.fastq.gz

/usr/bin/time -v  -o mqf.trim.f0.01.time scripts/trim-low-abund.py -k 23 -Z 20 -C 3 --mqf -U 1044756168  --fp-rate 0.01 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> mqf.trim.f0.01.log  | gzip -c > mqf.trimmed.f0.01.fastq.gz

/usr/bin/time -v  -o countmin.trim.f0.0001.time scripts/trim-low-abund.py -k 23 -Z 20 -C 3   -M 4.5979G -N 10 -U 1044756168  --fp-rate 0.0001 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> countmin.trim.f0.0001.log  | gzip -c > countmin.trimmed.f0.0001.fastq.gz

/usr/bin/time -v  -o countmin.trim.f0.001.time scripts/trim-low-abund.py -k 23 -Z 20 -C 3  -M 3.523G -N 7 -U 1044756168  --fp-rate 0.001 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> countmin.trim.f0.001.log  | gzip -c > countmin.trimmed.f0.001.fastq.gz

/usr/bin/time -v  -o countmin.trim.f0.01.time scripts/trim-low-abund.py -k 23 -Z 20 -C 3  -M 1.91G -N 3 -U 1044756168  --fp-rate 0.01 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> countmin.trim.f0.01.log  | gzip -c > countmin.trimmed.f0.01.fastq.gz


##### Gather Results
###########################3
parallel --gnu -k 'grep "kept" countmin.f{}.log |tail -n1' ::: 0.01 0.001 0.0001  | cut -f5 -d' '
parallel --gnu -k 'grep "kept" mqf.f{}.log |tail -n1' ::: 0.01 0.001 0.0001  | cut -f5 -d' '

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

/usr/bin/time -v  -o mqf.trim.exact.time scripts/trim-low-abund.py -k 23 -Z 20 -C 3 --mqf -U 1044756168  --fp-rate 0.00000001 -o -  ../data/WGS/U0a/U0a_CGATGT_L00*  --ignore-pairs  2> ../data/WGS/U0a/mqf.trim.exact.log   | gzip -c > ../data/WGS/U0a/mqf.trimme.exact.fastq.gz

/usr/bin/time -v  -o mqf.trim.f0.0001.time scripts/trim-low-abund.py -k 23 -Z 20 -C 3 --mqf -U 1044756168  --fp-rate 0.0001 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> mqf.trim.f0.0001.log  | gzip -c > mqf.trimmed.f0.0001..fastq.gz


/usr/bin/time -v  -o mqf.trim.f0.001.time scripts/trim-low-abund.py -k 23 -Z 20 -C 3 --mqf -U 1044756168  --fp-rate 0.001 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> mqf.trim.f0.001.log  | gzip -c > mqf.trimmed.f0.001..fastq.gz

/usr/bin/time -v  -o mqf.trim.f0.01.time scripts/trim-low-abund.py -k 23 -Z 20 -C 3 --mqf -U 1044756168  --fp-rate 0.01 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> mqf.trim.f0.01.log  | gzip -c > mqf.trimmed.f0.01.fastq.gz

/usr/bin/time -v  -o countmin.trim.f0.0001.time scripts/trim-low-abund.py --force -k 23 -Z 20 -C 3   -M 4.5979G -N 10 -U 1044756168  --fp-rate 0.0001 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> countmin.trim.f0.0001.log  | gzip -c > countmin.trimmed.f0.0001.fastq.gz


/usr/bin/time -v  -o countmin.trim.f0.001.time scripts/trim-low-abund.py --force -k 23 -Z 20 -C 3  -M 3.523G -N 7 -U 1044756168  --fp-rate 0.001 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> countmin.trim.f0.001.log  | gzip -c > countmin.trimmed.f0.001.fastq.gz

/usr/bin/time -v  -o countmin.trim.f0.01.time scripts/trim-low-abund.py --force  -k 23 -Z 20 -C 3  -M 2.6G -N 3 -U 1044756168  --fp-rate 0.01 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> countmin.trim.f0.01.log  | gzip -c > countmin.trimmed.f0.01.fastq.gz

/usr/bin/time -v  -o countmin.trim.7G.f0.0001.time scripts/trim-low-abund.py --force -k 23 -Z 20 -C 3   -M 7G -N 10 -U 1044756168  --fp-rate 0.0001 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> countmin.trim.7G.f0.0001.log  | gzip -c > countmin.trimmed.7G.f0.0001.fastq.gz

/usr/bin/time -v  -o countmin.trim.7G.f0.001.time scripts/trim-low-abund.py --force -k 23 -Z 20 -C 3  -M 7G -N 7 -U 1044756168  --fp-rate 0.001 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> countmin.trim.7G.f0.001.log  | gzip -c > countmin.trimmed.7G.f0.001.fastq.gz

/usr/bin/time -v  -o countmin.trim.7G.f0.01.time scripts/trim-low-abund.py --force  -k 23 -Z 20 -C 3  -M 7G -N 3 -U 1044756168  --fp-rate 0.01 -o -  U0a_CGATGT_L00*  --ignore-pairs  2> countmin.trim.7G.f0.01.log  | gzip -c > countmin.trimmed.7G.f0.01.fastq.gz
############################################33333


##### Gather Results
###########################3

gzip -dc countmin.trimmed.7G.f0.01.fastq.gz | sed -n '2~4p' |awk '{reads+=1;if(length($1)<148){events+=1;bases+=(148-length($1))}} END {print "Bases="bases+148*(53828588-reads)"\nEvents="events+(53828588-reads)}' > countmin.trimmed.7G.f0.01.res

gzip -dc countmin.trimmed.7G.f0.001.fastq.gz | sed -n '2~4p' |awk '{reads+=1;if(length($1)<148){events+=1;bases+=(148-length($1))}} END {print "Bases="bases+148*(53828588-reads)"\nEvents="events+(53828588-reads)}' > countmin.trimmed.7G.f0.001.res

gzip -dc countmin.trimmed.7G.f0.0001.fastq.gz | sed -n '2~4p' |awk '{reads+=1;if(length($1)<148){events+=1;bases+=(148-length($1))}} END {print "Bases="bases+148*(53828588-reads)"\nEvents="events+(53828588-reads)}' > countmin.trimmed.7G.f0.0001.res

###############################################

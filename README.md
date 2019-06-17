# Benchmarking MQF vs CQF vs Countmin
## Dastasets
Three datasets called z2,z3, and z5 where simulated to follow zipfian distribution(Powers 1998) using different coefficient: 2, 3, and 5. The bigger the coefficient the more singleton in the dataset. The Fourth was called kmers which represent real kmers generated from RNA seq experiment(ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR105/005/ERR1050075/ERR1050075_1.fastq.gz) .
## Compiling:
```
make all NH=1 test
```
## Experiment 1
Experiment 1 compares the load factor of CQF and MQF when the same datasets are inserted to them. First, CQF and MQF are created using the same number of slots(227). Then, chunks of items are inserted iteratively to both data-structures while recording the load factor after the insertion of each chunk. The experiments stops when CQF’s load factor reaches 90%.
Running Commands:
```
./compareLoadingFactor -s 27 -z2
./compareLoadingFactor -s 27 -z3
./compareLoadingFactor -s 27 -z5
./compareLoadingFactor -s 27 -d uniform -u 10 -f4
./compareLoadingFactor -s 27 -d kmers -k ERR1050075_1.fastq
```

## Experiment 2
Experiment 2 is measuring the insertion and query speed  MQF, CQF, buffered MQF,  khmer’s implementation countmin sketch , and original implementation of countmin sketch.
The commands used in the bench-marking is found in runSpeedtest.sh

## Experiment 3
Experiment 3 compares the optimal memory space required by CQF and MQF to count a items from different distributions.
The commands used in the bench-marking is found in runSizeTest.sh

## Experiments 4,5
Experiment 5 and 6 compare the MQF and countmin sketch performance in a real applications: Error Trimming and Digital Normalization. Both Experiments need Khmer software. Scripts runTrim.sh and runDiginorm.sh includes the installation of khmer and running the experiments. 

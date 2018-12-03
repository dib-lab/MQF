parallel --gnu -j1 './speedPerformance -s 27 -d kmers -k ERR1050075_1.fastq -f {1} > speed.kmers.s27.f{1}.{2} 2> speed.kmers.s27.f{1}.{2}.log ' ::: 0.01 0.001 0.0001 ::: 1 2 3 4 5 
parallel --gnu -j1 './speedPerformance -z {2} -s 27  -f {1}  > speed.z{2}.s27.f{1}.{3} 2> speed.z{2}.s27.f{1}.{3}.log' ::: 0.01  0.001 0.0001 ::: 2 3 5 ::: 1 2 3 4 5 

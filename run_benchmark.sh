seq 100000 100000 1000000 |parallel --gnu -j 2 'python Benchmark_Utils_Script/generateSeq.py {} 20 10000 datasets/dataset.s4.5.{}'
ls datasets/dataset.s3.*0.dat | parallel --gnu -j 1 ./bestParameters.sh {} {.}.gold 17 {.}.mqf.res
ls ../datasets/dataset.s1.5.*0.dat | parallel --gnu -j 1 ./bestParameters.sh {} {.}.gold 17 {.}.cqf.res


ls datasets/*cqf.res > tmp
cat datasets/*cqf.res > tmp2
sed  --in-place -e  's/datasets\/dataset.s3.\(.*\).cqf.res/\1/' tmp
paste tmp tmp2|sort -k1,1n >cqf.res

ls datasets/dataset.s3.*mqf.res > tmp
cat datasets/dataset.s3.*mqf.res >tmp2
sed  --in-place -e  's/datasets\/dataset.s3.\(.*\).mqf.res/\1/' tmp
paste tmp tmp2|sort -k1,1n >mqf.s3.res

ls datasets/dataset.s4.5.*mqf.res > tmp
cat datasets/dataset.s4.5.*mqf.res >tmp2
sed  --in-place -e  's/datasets\/dataset.s4.5.\(.*\).mqf.res/\1/' tmp
paste tmp tmp2|sort -k1,1n >mqf.s4.5.res

ls datasets/dataset.s4.5.*cqf.res > tmp
cat datasets/dataset.s4.5.*cqf.res >tmp2
sed  --in-place -e  's/datasets\/dataset.s4.5.\(.*\).cqf.res/\1/' tmp
paste tmp tmp2|sort -k1,1n >cqf.4.5.res

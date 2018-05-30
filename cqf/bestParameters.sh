dataset=$1
gold=$2
start=$3
outFile=$4

r=$RANDOM

timeFile="time.$r"
memory="memory.$r"

while true; do
  /usr/bin/time -v -o $timeFile ./main $start $dataset $gold > $memory && break
  let "start += 1"
done

time=$( grep "Elapsed" $timeFile|sed -e 's/(h:mm:ss or m:ss)//' |sed -e 's/.*: \(.*\)/\1/' )
memory=$( cat $memory)
paste <( echo $memory) <(echo  $time) <(echo $start)  > $outFile

rm -f *.$r

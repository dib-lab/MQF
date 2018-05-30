dataset=$1
gold=$2
start=$3
outFile=$4

r=$RANDOM

timeFile="time.$r"
memory="memory.$r"

fixed=1
while true; do
  /usr/bin/time -v -o $timeFile ./main $start $fixed $dataset $gold > $memory && break
  let "fixed +=1"
  if [ $fixed == 10 ]
  then
    let "start += 1"
    fixed=1
  fi
done

time=$( grep "Elapsed" $timeFile|sed -e 's/(h:mm:ss or m:ss)//' |sed -e 's/.*: \(.*\)/\1/' )
memory=$( cat $memory)

paste <( echo $memory) <(echo  $time) <(echo $start) <(echo $fixed) > $outFile

rm -f *.$r

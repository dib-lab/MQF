## To run the experiments 
parallel --gnu   './sizeTest -z {2}  -n {1}000000 > size.z{2}.n{1}M' ::: 35 40 45 50 55 60 65 70 75 80 85 90 95 100 105 110  ::: 2 3 5

## Parsing the results for  CQF and zipfian distribution of coeffcient =3
parallel --gnu -k 'cat size.z3.n{}M |grep "CQF"|awk "{if(\$5==1){res=\$0}} END {print res}" ' ::: 50 55 60 65 70 75 80 85 90 95 100 105 110 |cut -f3 |tr -d "MB"



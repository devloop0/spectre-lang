for f in `seq 1 26`; do fname="$f/test_outputs"; echo $fname; diff $fname "../ref_outputs/$fname"; done

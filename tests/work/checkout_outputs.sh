for f in `seq 1 24`; do fname="$f/test_outputs"; echo $fname; diff $fname "../ref_outputs/$fname"; done
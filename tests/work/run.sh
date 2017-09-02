for d in $(find . -mindepth 1 -type d); do
	cd $d
	for f in $(find . -name '*.sp'); do
		touch test_outputs
		base=$(basename $f)
		fname="${base%.*}"
		echo "------------Building ${f}-------------" >> test_outputs
		echo "------------Building ${f}-------------"
		sp $base
		echo "------------Running ${f}-------------" >> test_outputs
		echo "------------Running ${f}-------------"
		./$fname >> test_outputs
		echo "------------Return code: $?-------------" >> test_outputs
		find . -type f -executable -delete
	done
	cd ..
done

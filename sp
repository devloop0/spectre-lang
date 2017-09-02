CURRENT_DIR=$PWD

OUTPUT_EXEC="${1%.*}"
OBJ_FILES=()

for ARG in $@; do
	ARG="`realpath $ARG`"
	ARG_NAME="`basename $ARG`"
	ARG_DIR="`dirname $ARG`"
	TRUE_ARG_NAME="${ARG_NAME%.*}"
	
	cd $ARG_DIR
	ASM_FILE="${TRUE_ARG_NAME}.s"
	O_FILE="${TRUE_ARG_NAME}.o"
	spectre $ARG_NAME
	if [ -f $ASM_FILE ]; then
		mv $ASM_FILE $CURRENT_DIR 2>/dev/null
		cd $CURRENT_DIR
		as -mips32 $ASM_FILE -o $O_FILE
		OBJ_FILES=("${OBJ_FILES[@]}" $O_FILE)
		rm $ASM_FILE
	else
		cd $CURRENT_DIR
		rm ${OBJ_FILES[@]} 2>/dev/null
		exit
	fi
done

ld ${OBJ_FILES[@]} -o $OUTPUT_EXEC -L/usr/include/libspectre -l:libspectre.a
rm ${OBJ_FILES[@]}

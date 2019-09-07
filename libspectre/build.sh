CURRENT_DIR=$PWD
LIB_DIR=$(dirname $(realpath "$0"))

mkdir asm
mkdir o

STD_DIR="${LIB_DIR}/std"
STDX_DIR="${LIB_DIR}/stdx"

cd $STD_DIR

for f in $(find . -name '*.sp'); do
	fdir=$(dirname $f)
	fbase=$(basename $f)
	echo "Building std/$fbase..."
	fname="${fbase%.*}"
	fdirbase=$(basename $fdir)
	fasm="$fdir/$fname.s"
	fdestasm="$CURRENT_DIR/asm/std-$fname-$fdirbase.s"
	spectre $f
	mv $fasm $fdestasm
done

cd $CURRENT_DIR

cd $STDX_DIR

for f in $(find . -name '*.sp'); do
	fdir=$(dirname $f)
	fbase=$(basename $f)
	echo "Building stdx/$fbase..."
	fname="${fbase%.*}"
	fdirbase=$(basename $fdir)
	fasm="$fdir/$fname.s"
	fdestasm="$CURRENT_DIR/asm/stdx-$fname-$fdirbase.s"
	spectre $f
	mv $fasm $fdestasm
done

cd $CURRENT_DIR

cd asm

for f in $(find . -name '*.s'); do
	echo "Assembling $fbase..."
	fbase=$(basename $f)
	fname="${fbase%.*}"
	fo="$fname.o"
	as -mfloat-abi=hard -mfpu=vfp $fbase -o $fo
	mv $fo ../o
done

cd ../o

echo "Making the libspectre.a..."
ar rcs ../libspectre.a *.o

cd ..

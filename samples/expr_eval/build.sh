echo Building expr_eval
mkdir -p asm o

echo Building expr_eval.sp
spectre expr_eval.sp
mv expr_eval.s asm/

echo Building lex.sp
spectre lex.sp
mv lex.s asm/

echo Building util.sp
spectre util.sp
mv util.s asm/

cd asm
as expr_eval.s -o ../o/expr_eval.o
as lex.s -o ../o/lex.o
as util.s -o ../o/util.o

cd ../
ld o/*.o -o main -L/usr/include/libspectre -l:libspectre.a

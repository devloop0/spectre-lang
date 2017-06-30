CXX_FLAGS = --std=c++14
CXX_LINKER_FLAGS = -lstdc++fs
CC = g++-6

all: outputs/spectre

install: setup all

rinstall: setup release

clean:
	rm -f intermediates/* outputs/*

setup:
	mkdir intermediates outputs

uninstall:
	rm -rf intermediates outputs

release: CXX_FLAGS += -O3
release: outputs/spectre

outputs/spectre: intermediates/ast_printer.o intermediates/buffer.o intermediates/diagnostics.o \
		intermediates/error.o intermediates/expr.o intermediates/lir.o intermediates/main.o \
		intermediates/mips.o intermediates/parser.o intermediates/stmt.o intermediates/token.o \
		intermediates/type.o intermediates/file_io.o intermediates/mips2.o
	$(CC) $(CXX_FLAGS) -o outputs/spectre intermediates/ast_printer.o intermediates/buffer.o \
		intermediates/diagnostics.o intermediates/error.o intermediates/expr.o intermediates/lir.o \
		intermediates/main.o intermediates/mips.o intermediates/parser.o intermediates/stmt.o \
		intermediates/token.o intermediates/type.o intermediates/file_io.o \
		intermediates/mips2.o $(CXX_LINKER_FLAGS)

intermediates/main.o: main.cpp buffer.hpp token.hpp error.hpp diagnostics.hpp token_keywords.hpp parser.hpp stmt.hpp \
		expr.hpp ast_printer.hpp lir.hpp lir_instruction_set.hpp lir.hpp mips.hpp mips_instruction_set.hpp \
		file_io.hpp mips2.hpp
	$(CC) $(CXX_FLAGS) -c main.cpp -o intermediates/main.o $(CXX_LINKER_FLAGS)

intermediates/ast_printer.o: ast_printer.cpp ast_printer.hpp
	$(CC) $(CXX_FLAGS) -c ast_printer.cpp -o intermediates/ast_printer.o $(CXX_LINKER_FLAGS)

intermediates/buffer.o: buffer.cpp buffer.hpp error.hpp token_keywords.hpp
	$(CC) $(CXX_FLAGS) -c buffer.cpp -o intermediates/buffer.o $(CXX_LINKER_FLAGS)

intermediates/diagnostics.o: diagnostics.cpp diagnostics.hpp
	$(CC) $(CXX_FLAGS) -c diagnostics.cpp -o intermediates/diagnostics.o $(CXX_LINKER_FLAGS)

intermediates/error.o: error.cpp error.hpp
	$(CC) $(CXX_FLAGS) -c error.cpp -o intermediates/error.o $(CXX_LINKER_FLAGS)

intermediates/expr.o: expr.cpp expr.hpp
	$(CC) $(CXX_FLAGS) -c expr.cpp -o intermediates/expr.o $(CXX_LINKER_FLAGS)

intermediates/lir.o: lir.cpp lir.hpp
	$(CC) $(CXX_FLAGS) -c lir.cpp -o intermediates/lir.o $(CXX_LINKER_FLAGS)

intermediates/mips.o: mips.cpp mips.hpp
	$(CC) $(CXX_FLAGS) -c mips.cpp -o intermediates/mips.o $(CXX_LINKER_FLAGS)

intermediates/parser.o: parser.cpp parser.hpp file_io.hpp
	$(CC) $(CXX_FLAGS) -c parser.cpp -o intermediates/parser.o $(CXX_LINKER_FLAGS)

intermediates/stmt.o: stmt.cpp stmt.hpp
	$(CC) $(CXX_FLAGS) -c stmt.cpp -o intermediates/stmt.o $(CXX_LINKER_FLAGS)

intermediates/token.o: token.cpp token.hpp
	$(CC) $(CXX_FLAGS) -c token.cpp -o intermediates/token.o $(CXX_LINKER_FLAGS)

intermediates/type.o: type.cpp type.hpp
	$(CC) $(CXX_FLAGS) -c type.cpp -o intermediates/type.o $(CXX_LINKER_FLAGS)

intermediates/file_io.o: file_io.cpp file_io.hpp
	$(CC) $(CXX_FLAGS) -c file_io.cpp -o intermediates/file_io.o $(CXX_LINKER_FLAGS)

intermediates/mips2.o: mips2.cpp mips2.hpp
	$(CC) $(CXX_FLAGS) -c mips2.cpp -o intermediates/mips2.o $(CXX_LINKER_FLAGS)

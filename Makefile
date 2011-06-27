default: all

all:
	bison -o parser.cpp -y -d cezinho-parser.y
	flex -o lexer.cpp cezinho-scanner.l
	g++ *.cpp -O2 -o cezinho 
	
clean:
	rm *.cpp *.hpp cezinho
	
	
debug:
	bison -o parser.cpp -y -d cezinho-parser.y
	flex -o lexer.cpp cezinho-scanner.l
	g++ *.cpp -o cezinho -O2 -DDBG_PRINT_TREE

debug2:
	bison -o parser.cpp -y -d cezinho-parser.y
	flex -o lexer.cpp cezinho-scanner.l
	g++ *.cpp -o cezinho -O2 -Wall -DDBG_SYM_TAB -DDBG_PRINT_TREE

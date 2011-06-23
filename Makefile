default: all

all:
	bison -o parser.cpp -y -d cezinho-parser.y
	flex -o lexer.cpp cezinho-scanner.l
	g++ *.cpp -o cezinho 
	
clean:
	rm *.cpp *.hpp cezinho

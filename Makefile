all: calculator.cpp 
	g++ -o calculator calculator.cpp -lm

clean: 
	$(RM) calculator

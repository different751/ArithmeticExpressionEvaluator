/*
*Author: Tyler Coy
*
*Program Description:
*The program below is a calculator that takes text arithmetic expressions
*from the user, parses that input, then evaluates that input. The idea behind
*parsing and evaluating these expressions is inspired by the split and merge
*algorithm cited below. This calculator allows for decimal numbers, negative
*numbers, sin/cos/tan functions, addition, multiplication, subtraction,
*division, and exponents. Please view the included Readme for more information
*about running the program and about the design of the program.
*
*
*Split and Merge algorithm reference:
*Kaplan, Vassili. "A New Algorithm to Parse a Mathematical 
*Expression and its Application to Create a Customizable 
*Programming Language." ICSEA 2016 (2016): 285.
*
*
*/



#include<iostream>
#include<string>
#include<cmath>
#include<exception>
#include<vector>

using namespace std;


/*
*Custom Exception class which inherits from the standard exception class.
*This class is used to build the exception objects that contain the error 
*messages for when errors are thrown during calculations or when
*parsing user input.
*/

class Exception : public exception {
	
	private:
		string error_message; //holds the error message
	
	public:
		Exception(string msg){
			error_message = msg;
		}
	
	   const char * what () const throw () {
		  return error_message.c_str();
	   }
	   
};




/*
*This is the Action class which is used to define actions which are
*values that are paired with an operator. An example would be 1 + 1
*which is broken up into actions [1,+] [1,)]. Each action contains
the value, operation, and the operation rank which determines when 
the action will be executed.
*/

class Action{
	
	private:
		double val; //value to be used for operation
		char op; //operator to be used 
		int opRank; //rank of the operator assocated with pemdas
	
	
	public:
	
		//standard constructor
		Action(){
			val = 0;
			op = '!';
			opRank = -1;
		}

		//constructor to build action given a value and operation
		Action(double value, char operation){
			val = value;
			op = operation;
				
			if(op == '^'){ //exponent highest rank of zero
				opRank = 0;
			}
			else if(op == '*' || op == '/'){ //multiplication and division next in the rank
				opRank = 1;
			}
			else if(op == '+' || op == '-'){ //addition and subtraction next in the rank
				opRank = 2;
			}
			else{ //finally the last rank associated with the non operation symbol ')'
				opRank = 3;
			}
		}
		
		//do the action assigned by the operator of the calling action.
		Action doAction(Action actionTwo){
			double result;
				switch(op){
					case '^':
						result = pow(val, actionTwo.getValue());
						return Action(result,actionTwo.getOperation());
					case '+':
						result = val + actionTwo.getValue();
						return Action(result,actionTwo.getOperation());
					case '*':
						result = val * actionTwo.getValue();
						return Action(result,actionTwo.op);
					case '/':
						if(actionTwo.getValue() == 0){
							throw Exception("Cannot divide by zero.");
						}
						result = val / actionTwo.getValue();
						return Action(result,actionTwo.getOperation());
					case '-':
						result = val - actionTwo.getValue();
						return Action(result,actionTwo.getOperation());
					default:
						break;
					
				}
		}
		
		double getValue(){
			return val;
		}
		
		char getOperation(){
			return op;
		}
		
		char getOpRank(){
			return opRank;
		}
	
};





/*
*This is the parsing class. This does the heavy lifting of parsing input
*and building the action list used for calculations. Inspired by the
*split and merge algorithm. 
*/
class Parser{
	
	private:
		vector<Action> expList; //holds the actions used for calculations
		int index = 0; //location inside the expList
		
		//delete all items in expList
		void clearVector(){
			expList.clear();
		}
		
		
		//merge the two actions at mergeIndex and endIndex
		//after the merge all items on the right side of the
		//merged element are moved left in the expList
		void merge(int mergeIndex,int endIndex){
	
			if(expList[mergeIndex].getOpRank() <= expList[mergeIndex+1].getOpRank()){ //make sure operation is correct pemdas
				Action tempAction = expList[mergeIndex].doAction(expList[mergeIndex+1]); //Do the action of the calling action
				expList[mergeIndex] = tempAction;
				int moveIndex = mergeIndex+1;
			
				while(moveIndex < endIndex-1){ //move all items after the merge to the left once
					expList[moveIndex] = expList[moveIndex+1];
					moveIndex++;
				}
	
			}
	
		}
		
		
		//Walk through expList and find an action that needs to be calculated
		//by adhering to pemdas. Walks through the changing expList until no
		//more actions are present.
		double mergeActions(){
			
			int mergeIndex = 0; //location to walk expList to find merge target
			int opRank = 0; //counts which operation in pemdas we are on
			int endIndex = expList.size(); //The size of expList that will shrink after merges.
			bool foundOperator = false; //Notifies that we have found an operator
			
			
			while(mergeIndex < endIndex && endIndex != 1){ //walk expList until at the end or there is only 1 item
			
				if(expList[mergeIndex].getOpRank() == opRank ){ //found operation in pemdas order
					merge(mergeIndex,endIndex);
					mergeIndex = 0; //resets to the front of the list to walk through again
					endIndex--; //shrink expList
					continue;
				}
				
				mergeIndex++;
				
				if(mergeIndex >= endIndex){
					mergeIndex=0;
					opRank++; //At this point no operation with the rank were found so move to next rank.
					
				}
				
			}
			
			return expList[0].getValue();
			
		}
		
		
		//This method extracts the nested expressions inside parentheses
		string getNestedExpression(string exp){
			
			string nestedExpression = ""; //holds the extracted nestedExpression
			int parenCount = 0; //counts parentheses
			bool firstParen = true; //will notify if all parentheses are accounted for
			index++;
			
			while(index < exp.size()){ //walk the expression exp
			
				if(exp[index] != ')' && exp[index] != ' '){ //looking for numbers or symbols to build expression
					if(exp[index] == '('){
						parenCount++;
					}
					
					nestedExpression = nestedExpression + exp[index];
					
				}
				else if(exp[index] == ')'){ //if closing parentheses we need to check if we are at the end of the expression
					if(parenCount > 0){
						nestedExpression = nestedExpression + exp[index];
						parenCount--;
					}
					else{ //We are at the end of the expression
						index++;
						firstParen = false;
						break;
					}
				}
				
				index++;
				
			}
			
			if(firstParen){ //The parentheses do not match if this is true
				throw Exception("Mismatched parentheses");
			}
			
			return nestedExpression;
			
		}
		
		
		//Small method to print the expList
		//Mainly used for debugging
		void printVector(){
			for(int i = 0; i < expList.size(); i++){
				cout << expList[i].getValue() << " " << expList[i].getOperation() << endl;
			}
		}
	
	public:
	
		//This is the main driving method for parsing of the expression given by the user.
		//This method will call parseExpression again if nested Expressions are found.
		//This also handles input errors from the user.
		double parseExpression(string exp){
			
			if(exp.size() == 0){ //check to see if the given expression exist
				throw Exception("Expression given as argument is empty");
			}
			
			double number; //Used to store the value returned from a calculated Action or string input
			char operation; //Used to store the operation from the inputted experssion.
			string strNum = ""; //Holds the string representation of the numbers until converted.
			bool haveNumber = false; //Notifies that a number is ready to be placed into an action.
			bool haveOp = false; //Notifies that an operation is ready to be placed into an action
			bool haveNested = false; //Notifies that we have extracted a nested expression
			Parser tempParser; //Used for nested expressions.
		
			while(index < exp.size()){ //begin walking inputted expression
				if(exp[index] == ' '){ //Skip spaces
					index++;
	
					if(index == exp.size()){
						throw Exception("Cannot leave trailing spaces in expression.");
					}
					
					continue;	
				}
				else if(exp[index] == ')'){ //We should never see a closing parentheses
					throw Exception("Mismatched parentheses.");
				}
				else if(exp[index] == '('){ //We have found a nested expression
					if(haveNumber == true && haveOp == false){ //If true we have to add a multiplication to the list ex: 2(2)
						expList.push_back(Action(number,'*'));
						haveNumber = false;
					}
					
					number = tempParser.parseExpression(getNestedExpression(exp));
					tempParser.clearVector();
					tempParser.index = 0;
					haveNumber = true;
					haveNested = true;

				}
				else if(exp[index] == '+' || exp[index] == '-' || exp[index] == '*' || exp[index] == '/' || exp[index] == '^'){
					//we have found an operation
					operation = exp[index];
					haveOp = true;
					index++;
				}
				else if(exp[index] == 's' && exp[index+1] == 'i' && exp[index+2] == 'n'){ //We have found a sin function
					
					if(haveNumber == true && haveOp == false){ //If true we have to add a multiplication to the list ex: 2sin(2)
						expList.push_back(Action(number,'*'));
						haveNumber = false;
					}
					
					index += 3;
					
					if (exp[index] == '(') { //if parentheses not attached to sin function throw an error
						number = sin(tempParser.parseExpression(getNestedExpression(exp)));
						tempParser.clearVector();
						tempParser.index = 0;
						haveNumber = true;
						haveNested = true;
					}
					else{
						throw Exception("sin function must follow 'sin()'");
					}
					
				}
				else if(exp[index] == 'c' && exp[index+1] == 'o' && exp[index+2] == 's'){ //We have found a cos function
				
					if(haveNumber == true && haveOp == false){ //If true we have to add a multiplication to the list ex: 2cos(2)
						expList.push_back(Action(number,'*'));
						haveNumber = false;
					}
					
					index += 3;
					
					if (exp[index] == '(') { //if parentheses not attached to cos function throw an error
						number = cos(tempParser.parseExpression(getNestedExpression(exp)));
						tempParser.clearVector();
						tempParser.index = 0;
						haveNumber = true;
						haveNested = true;
					}
					else{
						throw Exception("cos function must follow 'cos()'");
					}
					
				}
				else if(exp[index] == 't' && exp[index+1] == 'a' && exp[index+2] == 'n'){ //We have found a tan function
				
					if(haveNumber == true && haveOp == false){ //If true we have to add a multiplication to the list ex: 2tan(2)
						expList.push_back(Action(number,'*'));
						haveNumber = false;
					}
					
					index += 3;
					
					if (exp[index] == '(') {//if parentheses not attached to tan function throw an error
						number = tan(tempParser.parseExpression(getNestedExpression(exp)));
						tempParser.clearVector();
						tempParser.index = 0;
						haveNumber = true;
						haveNested = true;
						
					}else{
						throw Exception("tan function must follow 'tan()'");
					}			
					
				}
				else if((exp[index] < '0' || exp[index] > 57) && exp[index] != '.'){ //Check for anything not allowed in input
					throw Exception("Invalid expression. Can only use real numbers or symbols ' + - / * ^ ( ) sin() cos() tan()'. Type 'EXIT' to quit.");
				}
				else{
					if(haveNumber == true && haveNested == true && haveOp == false){ //Catches multiplication ex (2)2
						expList.push_back(Action(number,'*'));
						haveNumber = false;
						haveNested = false;
					}
				
					
					while(isdigit(exp[index]) || exp[index] == '.'){ //We have found a number
						strNum = strNum + exp[index];
						index++;
					}
	
					if(strNum.compare(".") == 0){ //We found a stranded decimal
						throw Exception("Invalid expression. '.' must be leading, trailing, or encased in numbers. Cannot be standalone.");
					}
					
					number = stod(strNum); //convert number into double from string
					strNum = "";
					haveNumber = true;
					
				}
				
				
				
				if(haveNumber == true && index < exp.size() && haveOp == true){ //We have a number and operation
					expList.push_back(Action(number,operation)); //Insert new action
					haveOp = false;
					haveNumber = false;
				}
				else if(haveNumber == true && index >= exp.size()){ //We have the last item in the expression
					if(haveOp == true){
						throw Exception("Invalid use of operation. Must be infix notiation. Operations must have a left and right operand");
					}
					expList.push_back(Action(number,')')); //Insert last action
				}
				else if(haveNumber == false && haveOp == true && operation == '-'){ //Found a negative operation ex: (-(3+2))
					if(index >= exp.size()){
						throw Exception("Invalid use of operation. Must be infix notiation. Operations must have a left and right operand");
					}
					expList.push_back(Action(-1, '*'));
					haveOp = false;
				}
				else if(haveNumber == false && haveOp == true && operation != '-'){ //Must be a stranded operation
					throw Exception("Invalid use of operations. Must be infix notiation. Operations must have a left and right operand");
				}
				
			}
			
			//printVector();
			return mergeActions();
			
		}
	
};






/*
*Calculator class for interfacing with user
*This also has the ability to calculate single expressions
*without the need of the continuous calculator
*/
class Calculator{
	
	private:
	
		//Simple quick and easy test suite
		void testSuite(){
			double result = 0;
			
			//easy computation
			cout << "Testing expression: 1+1" << endl;
			result = calculateExpression("1+1");
			cout << "Result should be 2: Result = " << result << endl << endl;
			
			//A more diffcult computation
			cout << "Testing expression: (3 + 4) * 6" << endl;
			result = calculateExpression("(3 + 4) * 6");
			cout << "Result should be 42: Result = " << result << endl << endl;
			
			//Increasing the difficulty
			cout << "Testing expression: (1 * 4) + (5 * 2)" << endl;
			result = calculateExpression("(1 * 4) + (5 * 2)");
			cout << "Result should be 14: Result = " << result << endl << endl;
			
			//Neagtive Numbers!!!!
			cout << "Testing expression: -22 + -10 + 2" << endl;
			result = calculateExpression("-22 + -10 + 2");
			cout << "Result should be -30: Result = " << result << endl << endl;
			
			//Decimal numbers
			cout << "Testing expression: -22 + -10 + 2" << endl;
			result = calculateExpression("1.5 + 1.5 - 1.2 * 3.3");
			cout << "Result should be close to -0.96: Result = " << result << endl << endl;
			
			//Exponent
			cout << "Testing expression: 2^4 + 2^(3+1)" << endl;
			result = calculateExpression("2^4 + 2^(3+1)");
			cout << "Result should be 32: Result = " << result << endl << endl;
			
			//Crazy nested expression
			cout << "Testing expression: (1+(2+(3+(4))))" << endl;
			result = calculateExpression("(1+(2+(3+(4))))");
			cout << "Result should be 10: Result = " << result << endl << endl;
			
			//Crazy big expression
			cout << "Testing expression: ((((2*2)))) * ((2*2)^2) + 4 - 1 + 8 / 2 + 4 * 2^2 - sin(1) + tan(1) + 2" << endl;
			result = calculateExpression("((((2*2)))) * ((2*2)^2) + 4 - 1 + 8 / 2 + 4 * 2^2 - sin(1) + tan(1) + 2");
			cout << "Result should be close to 89.715: Result = " << result << endl << endl;
			
			//Super crazy big expression
			cout << "Testing expression : ((-((2*2)))) * (-(2*2)^2) - 1 + 8 / 2 + 4 * 2^2 - sin(1) + tan(1) + 2 + (4-(8*8)+tan(1)^2 + 15*3 + -(6/2))^3 + -2(3(sin(2))5)" << endl;
			result = calculateExpression("((-((2*2)))) * (-(2*2)^2) - 1 + 8 / 2 + 4 * 2^2 - sin(1) + tan(1) + 2 + (4-(8*8)+tan(1)^2 + 15*3 + -(6/2))^3 + -2(3(sin(2))5)");
			cout << "Result should be close to -3719.378: Result = " << result << endl << endl;
			
			//Test division by zero error
			try{
				cout << "Testing expression: 1/0" << endl;
				result = calculateExpression("1/0");
			}
			catch(const Exception& e){
				cout << "Error: " << e.what() << endl;
			}
			cout << "We should have recieved an error" << endl << endl;
			
			//Test mismatched parentheses
			try{
				cout << "Testing expression: (1+1))" << endl;
				result = calculateExpression("(1+1))");
			}
			catch(const Exception& e){
				cout << "Error: " << e.what() << endl;
			}
			cout << "We should have recieved an error" << endl << endl;
			
			//Test incorrect input
			try{
				cout << "Testing expression: A + B + C" << endl;
				result = calculateExpression("A + B + C");
			}
			catch(const Exception& e){
				cout << "Error: " << e.what() << endl;
			}
			cout << "We should have recieved an error" << endl << endl;
		}
	
	public:
	
		//Begin continuous calculator
		void startCalculator(){
			cout << "Starting Calculator" << endl;
			cout << "Calculator works with addition, multiplication, subtraction, division, and exponents" << endl;
			cout << "Calculator also has the ability to use the cos, sin, and tan functions" << endl;
			cout << "Use real numbers or symbols ' + - / * ^ ( ) sin() cos() tan()'" << endl;
			cout << "Type 'TEST' to run a test suite" << endl;
			cout << "Type 'EXIT' to quit the program\n" << endl;
			
			cout << "Example equations" << endl;
			cout << "1 + 1" << endl;
			cout << "(1 * 4) + (5 * 2)" << endl;
			cout << "(2*3) + 4 - 2 + (3^4) +(4+2) * (4/2) + sin(1+1)" << endl;
			cout << "2^-1" << endl;
			cout << "1.5 * -1" << endl;
			
			while(true){
				
				string input;
				
				cout << "\nPlease enter an expression: ";	
				getline(cin,input);
				if(input.compare("EXIT") == 0){
					cout << "\nExiting Calculator\n";
					return;
				}
				else if(input.compare("TEST") == 0){
					testSuite();
					cout << "End of test suite.\n"<< endl;
					continue;
				}
				
				if(input.size() == 0){
					continue;
				}
				
				try{
					double result = calculateExpression(input);
					cout << "Result: " << result << endl;
				}
				catch(const Exception& e){
					cout << "Error: " << e.what() << endl;
				}
				
			}
			
		}
		
		//Calculate single expression
		double calculateExpression(string exp){
			Parser myParser;
			return myParser.parseExpression(exp);
		}
			
	
};


/*
*Main method for the program
*/
int main(int argc, char **argv){
	
	Calculator myCalc;
	
	myCalc.startCalculator();
	
	return 0;
}


/*
	___Notes to myself____
	
	
	Cannot use shunting yard 

	Ideas:
	tree? Maybe tree like structure but not using a tree?
	brute force?
	
	
	
	Items that need to be completed:
	-split into seperate files??
	
	
	Completed items:
	-take input
	-parse input
	-merge expression
	-error checking
	-standard operations
	-sin,cos,tan
	-negative numbers
	-nested functions
	
	


*/
import util.control.Breaks._
import scala.collection.mutable.ArrayBuffer 


class Action() {
	
	private var value: Double = 0
	private var op: Char = '!'
	private var opRank: Int = -1
	
	def this(xValue: Double, xOperation: Char){
		this()
		this.value = xValue
		this.op = xOperation
		
		if(op == '^'){ //exponent highest rank of zero
			opRank = 0
		} else if(op == '*' || op == '/'){ //multiplication and division next in the rank
			opRank = 1
		} else if(op == '+' || op == '-'){ //addition and subtraction next in the rank
			opRank = 2
		} else{ //finally the last rank associated with the non operation symbol ')'
			opRank = 3
		}
	}
	
	def doAction(actionTwo: Action): Action = {
		var result: Double = 0
		
		this.op match {
			case '^' => result = scala.math.pow(this.value,actionTwo.getValue())
						return new Action(result,actionTwo.getOperation())
						
			case '+' => result = this.value + actionTwo.getValue()
						return new Action(result,actionTwo.getOperation())
						
			case '*' => result = this.value * actionTwo.getValue()
						return new Action(result,actionTwo.op)
						
			case '/' => result = this.value / actionTwo.getValue()
						return new Action(result,actionTwo.getOperation())
						
			case '-' => result = this.value - actionTwo.getValue()
						return new Action(result, actionTwo.getOperation())
		}
	}
	
	def getValue(): Double = {
		return this.value
	}
	
	def getOperation(): Char = {
		return op
	}
	
	def getOpRank(): Int = {
		return this.opRank
	}
	
	
	
}


class Parser(){
	
	private var expList = new ArrayBuffer[Action]()
	private var index : Int = 0
	
	private def clearArrayBuffer() {
		expList.clear()
	}
	
	private def merge(mergeIndex: Int, endIndex: Int) {
		if(expList(mergeIndex).getOpRank() <= expList(mergeIndex+1).getOpRank()){ //make sure operation is correct pemdas
			var tempAction = expList(mergeIndex).doAction(expList(mergeIndex+1)) //Do the action of the calling action
			expList(mergeIndex) = tempAction
			var moveIndex: Int = mergeIndex+1
		
			while(moveIndex < endIndex-1){ //move all items after the merge to the left once
				expList(moveIndex) = expList(moveIndex+1)
				moveIndex = moveIndex + 1
			}
		}
	}
	
	
	private def mergeActions(): Double = {
		var mergeIndex: Int = 0
		var opRank: Int = 0
		var endIndex: Int = expList.size
		var foundOperator: Boolean = false
		
		while(mergeIndex < endIndex && endIndex != 1){ //walk expList until at the end or there is only 1 item
			breakable {
				if(expList(mergeIndex).getOpRank() == opRank ){ //found operation in pemdas order
					merge(mergeIndex,endIndex)
					mergeIndex = 0 //resets to the front of the list to walk through again
					endIndex = endIndex - 1 //shrink expList
					break
				}
				
				mergeIndex = mergeIndex + 1
				
				if(mergeIndex >= endIndex){
					mergeIndex=0
					opRank = opRank + 1 //At this point no operation with the rank were found so move to next rank.
					
				}
			}
			
		}
		
		return expList(0).getValue()
	}
	
	
	private def getNestedExpression(exp: String): String = {
		var nestedExpression: String = "" //holds the extracted nestedExpression
		var parenCount: Int = 0 //counts parentheses
		var firstParen: Boolean = true //will notify if all parentheses are accounted for
		index = index + 1
		
		breakable {
			while(index < exp.length()){ //walk the expression exp
			
				if(exp.charAt(index) != ')' && exp.charAt(index) != ' '){ //looking for numbers or symbols to build expression
					if(exp.charAt(index) == '('){
						parenCount = parenCount + 1
					}
					
					nestedExpression = nestedExpression + exp.charAt(index)
					
				}
				else if(exp.charAt(index) == ')'){ //if closing parentheses we need to check if we are at the end of the expression
					if(parenCount > 0){
						nestedExpression = nestedExpression + exp.charAt(index)
						parenCount = parenCount - 1
					}
					else{ //We are at the end of the expression
						index = index + 1
						firstParen = false
						break
					}
				}
				
				index = index + 1
				
			}
		}
		
		return nestedExpression
		
	}
	
	def parseExpression(exp: String): Double = {
			
		var number: Double = 0 //Used to store the value returned from a calculated Action or string input
		var operation: Char = 'x' //Used to store the operation from the inputted experssion.
		var strNum: String = "" //Holds the string representation of the numbers until converted.
		var haveNumber: Boolean = false //Notifies that a number is ready to be placed into an action.
		var haveOp: Boolean = false //Notifies that an operation is ready to be placed into an action
		var haveNested: Boolean = false //Notifies that we have extracted a nested expression
		var tempParser = new Parser() //Used for nested expressions.

		while(index < exp.length()){ //begin walking inputted expression
			breakable {
				if(exp.charAt(index) == ' '){ //Skip spaces
					index = index + 1
					break	
				}
				else if(exp.charAt(index) == ')'){ //We should never see a closing parentheses
					//throw Exception("Mismatched parentheses.")
				}
				else if(exp.charAt(index) == '('){ //We have found a nested expression
					if(haveNumber == true && haveOp == false){ //If true we have to add a multiplication to the list ex: 2(2)
						expList.append(new Action(number,'*'))
						haveNumber = false
					}
					
					number = tempParser.parseExpression(getNestedExpression(exp))
					tempParser.clearArrayBuffer()
					tempParser.index = 0
					haveNumber = true
					haveNested = true

				}
				else if(exp.charAt(index) == '+' || exp.charAt(index) == '-' || exp.charAt(index) == '*' || exp.charAt(index) == '/' || exp.charAt(index) == '^'){
					//we have found an operation
					operation = exp.charAt(index)
					haveOp = true
					index = index + 1
				}
				else if(exp.charAt(index) == 's' && exp.charAt(index+1) == 'i' && exp.charAt(index+2) == 'n'){ //We have found a sin function
					
					if(haveNumber == true && haveOp == false){ //If true we have to add a multiplication to the list ex: 2sin(2)
						expList.append(new Action(number,'*'))
						haveNumber = false
					}
					
					index = index + 3
					
					if (exp.charAt(index) == '(') { //if parentheses not attached to sin function throw an error
						number = scala.math.sin(tempParser.parseExpression(getNestedExpression(exp)))
						tempParser.clearArrayBuffer()
						tempParser.index = 0
						haveNumber = true
						haveNested = true
					}
					else{
						//throw Exception("sin function must follow 'sin()'")
					}
					
				}
				else if(exp.charAt(index) == 'c' && exp.charAt(index+1) == 'o' && exp.charAt(index+2) == 's'){ //We have found a cos function
				
					if(haveNumber == true && haveOp == false){ //If true we have to add a multiplication to the list ex: 2cos(2)
						expList.append(new Action(number,'*'))
						haveNumber = false
					}
					
					index = index + 3
					
					if (exp.charAt(index) == '(') { //if parentheses not attached to cos function throw an error
						number = scala.math.cos(tempParser.parseExpression(getNestedExpression(exp)))
						tempParser.clearArrayBuffer()
						tempParser.index = 0
						haveNumber = true
						haveNested = true
					}
					else{
						//throw Exception("cos function must follow 'cos()'")
					}
					
				}
				else if(exp.charAt(index) == 't' && exp.charAt(index+1) == 'a' && exp.charAt(index+2) == 'n'){ //We have found a tan function
				
					if(haveNumber == true && haveOp == false){ //If true we have to add a multiplication to the list ex: 2tan(2)
						expList.append(new Action(number,'*'))
						haveNumber = false
					}
					
					index = index + 3
					
					if (exp.charAt(index) == '(') {//if parentheses not attached to tan function throw an error
						number = scala.math.tan(tempParser.parseExpression(getNestedExpression(exp)))
						tempParser.clearArrayBuffer()
						tempParser.index = 0
						haveNumber = true
						haveNested = true
						
					}else{
						//throw Exception("tan function must follow 'tan()'")
					}			
					
				}
				else if((exp.charAt(index) < '0' || exp.charAt(index) > 57) && exp.charAt(index) != '.'){ //Check for anything not allowed in input
					//throw Exception("Invalid expression. Can only use real numbers or symbols ' + - / * ^ ( ) sin() cos() tan()'. Type 'EXIT' to quit.")
				}
				else{
					if(haveNumber == true && haveNested == true && haveOp == false){ //Catches multiplication ex (2)2
						expList.append(new Action(number,'*'))
						haveNumber = false
						haveNested = false
					}
				
					
					while(index < exp.length() && (exp.charAt(index).isDigit || exp.charAt(index) == '.')){ //We have found a number
						strNum = strNum + exp.charAt(index)
						index = index + 1
					}

					if(strNum.equals(".") == true){ //We found a stranded decimal
						//throw Exception("Invalid expression. '.' must be leading, trailing, or encased in numbers. Cannot be standalone.")
					}
					
					number = strNum.toDouble //convert number into double from string
					strNum = ""
					haveNumber = true
					
				}
				
				
				
				if(haveNumber == true && index < exp.length() && haveOp == true){ //We have a number and operation
					expList.append(new Action(number,operation)) //Insert new action
					haveOp = false
					haveNumber = false
				}
				else if(haveNumber == true && index >= exp.length()){ //We have the last item in the expression
					if(haveOp == true){
						//throw Exception("Invalid use of operation. Must be infix notiation. Operations must have a left and right operand")
					}
					expList.append(new Action(number,')')) //Insert last action
				}
				else if(haveNumber == false && haveOp == true && operation == '-'){ //Found a negative operation ex: (-(3+2))
					if(index >= exp.length()){
						//throw Exception("Invalid use of operation. Must be infix notiation. Operations must have a left and right operand")
					}
					expList.append(new Action(-1, '*'))
					haveOp = false
				}
				else if(haveNumber == false && haveOp == true && operation != '-'){ //Must be a stranded operation
					//throw Exception("Invalid use of operations. Must be infix notiation. Operations must have a left and right operand")
				}
			}
		}
		
		//printVector()
		return mergeActions()
	
	}
	
}




class Calculator(){
	
	private def testSuite(){
		var result: Double = 0
			
		//easy computation
		println("Testing expression: 1+1")
		result = calculateExpression("1+1")
		println("Result should be 2: Result = " + result + "\n")
		
		//A more diffcult computation
		println("Testing expression: (3 + 4) * 6")
		result = calculateExpression("(3 + 4) * 6")
		println("Result should be 42: Result = " + result + "\n")
		
		//Increasing the difficulty
		println("Testing expression: (1 * 4) + (5 * 2)")
		result = calculateExpression("(1 * 4) + (5 * 2)")
		println("Result should be 14: Result = " + result + "\n")
		
		//Neagtive Numbers!!!!
		println("Testing expression: -22 + -10 + 2")
		result = calculateExpression("-22 + -10 + 2")
		println("Result should be -30: Result = " + result + "\n")
		
		//Decimal numbers
		println("Testing expression: -22 + -10 + 2")
		result = calculateExpression("1.5 + 1.5 - 1.2 * 3.3")
		println("Result should be close to -0.96: Result = " + result + "\n")
		
		//Exponent
		println("Testing expression: 2^4 + 2^(3+1)")
		result = calculateExpression("2^4 + 2^(3+1)")
		println("Result should be 32: Result = " + result + "\n")
		
		//Crazy nested expression
		println("Testing expression: (1+(2+(3+(4))))")
		result = calculateExpression("(1+(2+(3+(4))))")
		println("Result should be 10: Result = " + result + "\n")
		
		//Crazy big expression
		println("Testing expression: ((((2*2)))) * ((2*2)^2) + 4 - 1 + 8 / 2 + 4 * 2^2 - sin(1) + tan(1) + 2")
		result = calculateExpression("((((2*2)))) * ((2*2)^2) + 4 - 1 + 8 / 2 + 4 * 2^2 - sin(1) + tan(1) + 2")
		println("Result should be close to 89.715: Result = " + result + "\n")
		
		//Super crazy big expression
		println("Testing expression : ((-((2*2)))) * (-(2*2)^2) - 1 + 8 / 2 + 4 * 2^2 - sin(1) + tan(1) + 2 + (4-(8*8)+tan(1)^2 + 15*3 + -(6/2))^3 + -2(3(sin(2))5)")
		result = calculateExpression("((-((2*2)))) * (-(2*2)^2) - 1 + 8 / 2 + 4 * 2^2 - sin(1) + tan(1) + 2 + (4-(8*8)+tan(1)^2 + 15*3 + -(6/2))^3 + -2(3(sin(2))5)")
		println("Result should be close to -3719.378: Result = " + result + "\n")
		
		
	}
	
	def startCalculator(){
		println("Starting Calculator")
		println("Calculator works with addition, multiplication, subtraction, division, and exponents")
		println("Calculator also has the ability to use the cos, sin, and tan functions")
		println("Use real numbers or symbols ' + - / * ^ ( ) sin() cos() tan()'")
		println("Type 'TEST' to run a test suite")
		println("Type 'EXIT' to quit the program\n")
		
		println("Example equations")
		println("1 + 1")
		println("(1 * 4) + (5 * 2)")
		println("(2*3) + 4 - 2 + (3^4) +(4+2) * (4/2) + sin(1+1)")
		println("2^-1")
		println("1.5 * -1")
		
		while(true){
			var input:String = ""
			breakable {
				
				
				println("\nPlease enter an expression: ")	
				input = scala.io.StdIn.readLine() 
				if(input.equals("EXIT") == true){
					println("\nExiting Calculator\n")
					return
				}
				else if(input.equals("TEST") == true){
					testSuite()
					println("End of test suite.\n")
					break
				}
				
				if(input.length() == 0){
					break
				}
			
			
			var result: Double = calculateExpression(input);
			println("Result: " + result)
			}
			
		}
		
	}
	
	def calculateExpression(exp: String) : Double = {
		val myParser = new Parser()
		return myParser.parseExpression(exp)
	}
	
}



object Main{
	def main(args: Array[String]) = {
		val myClac = new Calculator()
		myClac.startCalculator()
	}
}
***********************
Author: Tyler Coy
Date: 5/17/2020
Technical Problem


If there are any issues or errors please feel free to contact me 
at tcdifferent751@gmail.com
***********************


****************** DIRECTORY DESCRIPTION ******************

calculator.cpp - c++ source file containing the program for the arithmetic expression parser and evaluator

Makefile - make file to compile the calculator.cpp program

README.txt - This document contains information about the program.

************************************************************



********* Program Description ***********
The program is a calculator that takes text arithmetic expressions
from the user, parses that input, then evaluates that input. The idea behind
parsing and evaluating these expressions is inspired by the split and merge
algorithm cited below. This calculator allows for decimal numbers, negative
numbers, sin/cos/tan functions, addition, multiplication, subtraction,
division, and exponents. 

Here is an example of how the program works. 
Let us use the expression (1 * 4) + (5 * 2) as an example
First the program will begin by parsing the inputted expression from
left to right. When a opening paranthese is encountered the nested
expression will be extracted and evaluated. So the expression inside
the first paranthese group is 1*4 this will be split up into actions
that are inserted into a vector 'expList'. The vector will contain the following

[1,*] [4,)]

The action objects above hold the values and the operations that need
to be calculated. These objects are then passed to a merging function
that merges actions based on rank they have corresponding to the operations
inside that action. PEMDAS is used to give a rank for each action. So the above
actions after merging will look like the following.

[4,)]

4 is then returned to the orignal parsing method which continues walking
through the expression. Now a + symbol is encoutered and the 4 and + are
combined into [4,+]. The parser then continues and encounters another
opening paranthese. Again the nested expression is extracted and evaluated
which returns the number 10. 10 is the final item in the expression and is combined
with the no operation symbol ')'. Now the expression list looks like the following.

[4,+] [10,)]

Now for the final merging sequence of the expression list. In the end the final
answer of 14 is returned.


Split and Merge algorithm reference:
Kaplan, Vassili. "A New Algorithm to Parse a Mathematical 
Expression and its Application to Create a Customizable 
Programming Language." ICSEA 2016 (2016): 285.


********* HOW TO RUN ***********
To run the program the user must first have a machine that
can execute c++ programs. If using command line there is a
provided makefile to compile for you. Otherwise visualStudio
or any other IDE should work.

Once the program is running you will be greeted with a message that
says the calculator is starting. There will also be some instructions
displayed. Then you are prompted to enter expressions. At this
point you can enter arithmetic expressions to be parsed and evaluated.

You can also type TEST for a test suite to run or you can type EXIT to end
the program.



********* EXAMPLE EXPRESSIONS ***********
Below are some big expressions I have used for testing.

(2*3) + 4 - 2 + (3^4) +(4+2) * (4/2) + sin(1+1)
((((2*2)))) * ((2*2)^2) + 4 - 1 + 8 / 2 + 4 * 2^2 - sin(1) + tan(1) + 2
((((2*2)))) * ((2*2)^2) + 4 - 1 + 8 / 2 + 4 * 2^2 - sin(1) + tan(1) + 2 + (4-(8*8)+tan(1)^2 + 15*3 + (6/2))^3



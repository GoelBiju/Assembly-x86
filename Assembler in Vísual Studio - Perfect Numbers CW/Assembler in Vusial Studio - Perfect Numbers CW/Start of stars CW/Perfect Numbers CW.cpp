//**************************************************************************//
// Assembler in Vusial Studio.cpp.  Usually a "C" application kicks off in	//
// main(), here we kick off in wmain(), not main() this is a unicode build  //
// (w for "wide").															//
//																			//
// This is a "C" program, and we do the input and output in "C", as doing	//
// so using assembler is a real pain.										//
//**************************************************************************//

//**************************************************************************//
// This code is copyright of Dr Nigel Barlow, lecturer in computing,		//
// University of Plymouth, UK.  email: nigel@soc.plymouth.ac.uk.			//
//																			//
// You may use, modify and distribute this (rather cack-handed in places)	//
// code subject to the following conditions:								//
//																			//
//	1:	You may not use it, or sell it, or use it in any adapted form for	//
//		financial gain, without my written premission.						//
//																			//
//	2:	You must not remove the copyright messages.							//
//																			//
//	3:	You should correct at least 10% of the typing and spelling errors.   //
//**************************************************************************//


#include "stdafx.h"
#include <stdlib.h>


void printChar(char c);
void printStr(char *strAddr);
void printInt(int someInt);


//**************************************************************************//
// Define some variables.  Not in assembler yet.							//
//**************************************************************************//

int numberFactor = 1;  // Store the number to find factors of (it will start from 2 when it is incremented 
					   // in the next_number_factor subroutine).

int divisionQuotient;  // Store the quotient from the division.
int divisionRemainder; // Store the remainder from the division to ensure 
					   // that the contents of the CPU registers are not lost
					   // when calling "C" subroutines.

int *factors;          // Pointer to factors.
int startAddress;      // Points to the first item's memory location in the factors array.
int lastIndexAddress;  // Points to the last item's memory location in the factors array. (RESET)
int factorCounter;     // A counter to allow each number to be tested when
					   // working out the factors.

int sumFactors;		   // Store the sum of the factors; make sure we reset this to 0 before (RESET)
					   // we start finding factors for each number.

// A set of strings to print to the console during the program.

char *programTitleMessage = "Perfect Numbers (brute-force)\n-----------------------------\n(After 8128 it will take a long time)";

char *nextNumberMessage = "The next to test is: ";

// char *foundFactorMessage = "A factor is: ";
// char *sumMessage = "Current sum: ";
// char *sumFinalMessage = "Final sum is: ";

char *perfectNumberMessage = "--> Found a PERFECT NUMBER: ";
char *workingMessage = "Sum = ";


/*
NOTES:
// TEST: Move a number into the array.
// mov factors[0], 45
// push factors[0]
// call printInt

// TEST: Dividing numbers.
// mov eax, 6  // dividend
// mov edx, 0  // clear to make way for remainder
// mov ebx, 3  // divisor

// div ebx     // begin division by taking whats in eax and dividing it by this register.

// mov divisionRemainder, edx  // move whats in edx (remainder) to a "C" variable to avoid losing it while calling a function.

// push eax
// call printInt
// call printNewLine

// push divisionRemainder
// call printInt
// call printNewLine
*/


//**************************************************************************//
// C programs usually kick off in main().  This is a unicode build, so it	//
// kicks off in wmain() (wide main).										//
//**************************************************************************//
int wmain(int argc, _TCHAR* argv[])
{
	// Allocate 1000 bytes in memory for the factors array:
	// A limitation of this program is that if it runs "forever" then it will 
	// eventually hit an issue with memory as with 1000 bytes of memory it can 
	// only store 250 factors of a number, the issue will only occur in very large
	// numbers but the memory allocated would have to be increased if it needs to 
	// handle factors of extremely large numbers.
	factors = (int *) malloc(1000); // I think that should cover it.


	//**********************************************************************//
	// Into assembler.														//
	// All the text in green after the "//" is a comment.  Comments are		//
	// purely for us humans to leave notes for ourselves; they are ignored	//
	// by the computer.														//
	//**********************************************************************//
	__asm
	{

		// Print to console the program title.
		push [programTitleMessage]
		call printStr
		call printNewLine
		call printNewLine

		//**************************************************************************
		// Move initial items into registers and the "C" variables for use. 
		// As we will be jumping from subroutines, we will not get a chance to 
		// otherwise initialise.
		//**************************************************************************
		
		// Store first memory location of array; each int is 4 bytes 
		// (we can use start address to help create a loop to place factors into the array).
		mov esi, [factors]
		mov startAddress, esi

		// TESTING: Print the start address.
		//mov al, 'S'
		//push al
		//call printChar
		//push startAddress
		//call printInt
		//call printNewLine

		// The initial last index address is the start address as this is the last index used.
		mov lastIndexAddress, esi


		//**********************************
		// Get the next number to calculate 
		// the factors of, the program always
		// returns back up to here.
		//**********************************
		next_number_factor: inc numberFactor

							// Print out the next number to test.
							// push [nextNumberMessage]
							// call printStr
							// push numberFactor
							// call printInt
							// call printNewLine

							// Move the contents of the number we want to find factors
							// of into the factor counter, as we do this, we also put 
							// number we want to divide and find factors of into eax.
							mov eax, numberFactor
							mov factorCounter, eax

							cmp numberFactor, 1
							je find_factors
							// Subtract to avoid having the 1 or the original number as a factor twice in the array.
							sub factorCounter, 1


		//*****************************************************
		// Subroutine to find factors of the specified number.
		//*****************************************************
		find_factors: 
			    // Move the factor and counter into their rightful 
				// registers to prepare for the division.
				mov eax, numberFactor
				mov ebx, factorCounter

				// Clear the edx register to allow for the remainder to be stored.
				mov edx, 0

				// Divide the contents of eax by ebx.
				div ebx

				// Store the values of eax and edx safely in "C" variables.
				mov divisionQuotient, eax
				mov divisionRemainder, edx


				// TESTING: Print the divison information: factor counter, quotient, remainder.
				// Place any printing related calls here, 
				// otherwise we may disturb the contents of the CPU registers.
				// push factorCounter
				// call printInt
				// mov al, ','
				// push al
				// call printChar
				
				// push divisionQuotient
				// call printInt
				// mov al, ','
				// push al
				// call printChar

				// push divisionRemainder
				// call printInt
				// call printNewLine


				// Save the factor counter from this operation to the factors array
				// if the division remainder was equal to 0, indicating a factor.
				cmp divisionRemainder, 0
				je save_factor

				// A blank subroutine to allow us to carry on after jumping before.
				backToFactors:

				// Decrement the factor counter.
				dec factorCounter

				// Make a comparision to see if the factor counter has reached 0,
				// if it has exit the loop as we would have found the possible factors.
				cmp factorCounter, 0
				jnz find_factors

				// Once we have found and saved the factors, we need to 
				// jump to finding the sum of the factors in the array.
				mov esi, lastIndexAddress  // Before we jump point the stack pointer to the last index address to help the loop.
				jmp sum_all_factors


		//**********************************************
		// Subroutine to save each factor to the array.
		//**********************************************
		save_factor: add lastIndexAddress, 4

					 // TESTING: State the memory locations where we will save the factors in the array.
					 //mov al, 'M'
					 //push al
					 //call printChar
					 //push lastIndexAddress
					 //call printInt
					 //call printNewLine

					 // As last index address is a "C" variable, it referring to what's in it will
					 // not get the equivalent value in stack, instead use ESI! This can change the last index value 
					 // to the factor itself, which will cause issues when trying to sum up the factors in the array.
					 // This was a major problem, only use ESI in brackets to get a value from the stack.
					 mov ecx, factorCounter
					 mov esi, lastIndexAddress
					 mov [esi], ecx
			
					 // TESTING: State that we have a found a factor along with 
					 //			 the actual factor we have found.
				     //push [foundFactorMessage]
					 //call printStr
					 //push [esi]
					 //call printInt
					 //call printNewLine

					 // Allow us to jump back into the find_factors subroutine.
					 jmp backToFactors


		//*****************************************
		// A subroutine to calculate the sum of the 
		// factors stored into the array.
		//*****************************************
		sum_all_factors: mov ecx, [esi]
						 add sumFactors, ecx

						 // TESTING: Print the current value of the sum of the factors added.
						 //push sumMessage
					     //call printStr
						 //push sumFactors
						 //call printInt
						 //call printNewLine

						 // Get to the next index by adding 4 to the memory location.
		 				 sub esi, 4

						 // Compare the what's in the current memory location to 0; a 0 means it is the 
						 // last index as nothing else has been stored in the array.
		 			     cmp esi, startAddress
	                     jne sum_all_factors


		//*******************************************
		// This subroutine tests to see if the value
		// in the counter is a perfect number or not.
		//*******************************************
		test_perfect_number: //call printNewLine 
							 //push [sumFinalMessage]
		 			         //call printStr
		 				     //push sumFactors
							 //call printInt
							 //call printNewLine

							 // Compare the number we were factorising to the sum to
						     // to determine if it is a perfect number or not.
					         mov eax, numberFactor
					         cmp eax, sumFactors

							 // If it is a perfect number, we can proceed with it's working.
							 je perfect_number_factors

                             // Make sure we reset the memory addresses and sum variables to return 
							 // to the beginning for the next number.
							 reset_and_return:

							 // Reset the last index address variable back to the start address for the next set of factors.
							 mov esi, startAddress
							 mov lastIndexAddress, esi
							 // Reset the sumFactors variable, for the next set of factors.
							 mov sumFactors, 0

							 // Jump to the next number to test.
							 jmp next_number_factor


        //****************************************************
		// This subroutine is for if the number we tested 
		// was a perfect number, this allows us to print 
		// the perfect number message and the working out
		// in order to make up the sum of the perfect number.
		//****************************************************
		perfect_number_factors: push[perfectNumberMessage]
								call printStr
								push numberFactor
								call printInt
								call printNewLine

								push [workingMessage]
								call printStr

								// In order to print the factors that make up the 
								// sum, we need to move the last index address into memory to 
								// reverse in the memory and print the factors.
								mov esi, lastIndexAddress
								jmp printSumFactors

								sum_printed: call printNewLine
											 call printNewLine	

								// Jump back into the test_perfect_number subroutine.
                                jmp reset_and_return


		//***************************************************
		// Print the specific factors that make up the sum.
		//***************************************************
		printSumFactors: push [esi]
						 call printInt

						 // In order to reverse, jump back 4 bytes to get 
						 // every integer value.
						 sub esi, 4

						 cmp esi, startAddress
						 je skip_symbols
						  
						 // Print the addition symbol in between the numbers for working out.
						 mov al, ' '
						 push al
						 call printChar

						 mov al, '+'
						 push al
						 call printChar
						 
						 mov al, ' '
						 push al
						 call printChar

                         // Jump back into this subroutine in the event there 
						 // are still more factors to print out.
						 jmp printSumFactors

					     // This will allow us to skip printing any unnecessary symbols 
						 // and head back up into the perfect_number_factors subroutine,
						 /// to reset and return back to the top to start the process again.
						 skip_symbols:

						 jmp sum_printed
						 
						 
		//**********************************************************************//
		// This subroutine just makes one new line by printing the carriage		//
		// return and line feed characters.  Do we need both for a new line?  I //
		// think so...  Nigel.													//
		//																		//
		// No parameters go in.  Nothing comes back.							//
		//**********************************************************************//
		printNewLine: push '\r'			// Two lines to print a char.
					  call printChar

					  push '\n'		    // Two lines to print another char.
					  call printChar

			          ret				// And back to <wherever we came from>



		//**********************************************************************//
		// Label to mark the end; do nothing, just jump here to finish.			//
		//**********************************************************************//
		finish:						// Do nothing			
	}


	//**********************************************************************//
	// Out of assembler.													//
	//**********************************************************************//
	printf("\npress enter to quit\n");
	char dummy[10];	     //Just in case several keys in buffer
	scanf("%c", dummy);  //pause.
	scanf("%c", dummy);  //pause.  And once more.  Something weird going on.
}



//**********************************************************************//
// Prints a single character.											//
// Push the char to be printed onto the stack; a First In Last Out		//
// data structure.  Remember, unlike C#, a char here is 1 byte in size. //
//																		//
// Parameters in: Push a single char onto the stack, as above.			//
// Returns:		  Nothing.												//
// Other issues:  Does it preserve CPU registers eax, ebx, ecx, edx,	//
//				  esi, edi etc.?  No idea (it takes us into "C", so		//
//				  assume not.											//
//**********************************************************************//
void printChar(char c)
{
	printf("%c", c);  //%c means as a char

}	// we don't seee the "ret" instruction unless you view the ".cod" listing 
	// in the "debug" folder.



//**********************************************************************//
// Print a whole string, which must end with a zero byte.				//
// it takes one parameter, which is the start address of the string.	//
//**********************************************************************//
void printStr(char *strAddr)
{
	printf("%s", strAddr);
}	// we don't seee the "ret" instruction unless you view the ".cod" listing 
	// in the "debug" folder.



//**********************************************************************//
// Prints a single integer	.											//
// Push the integer to be printed onto the stack; a First In Last Out	//
// data structure.  Remember, an integer here is 4 bytes in size.		//
//																		//
// Parameters in: push a single integer onto the stack, as above.		//
// Returns:		  Nothing.												//
// Other issues:  Does it preserve CPU registers eax, ebx, ecx, edx,	//
//				  esi, edi etc.?  No idea (it takes us into "C", so		//
//				  assume not.											//
//**********************************************************************//
void printInt(int someInt)
{
	printf("%d", someInt);
}	// we don't seee the "ret" instruction unless you view the ".cod" listing 
	// in the "debug" folder.



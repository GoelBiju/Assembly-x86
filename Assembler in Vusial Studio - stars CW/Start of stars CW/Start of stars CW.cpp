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
int numItems;     // Number of the item the user enters in.
int *items;		  // Pointer to the items.
int anItem;       // Stores a specific item from the array.

// Custom "C" variables used for the program.
int startAddress; // Store the first pointer address.
int endAddress;   // Store the end address of the last array item.
				  // The end address will be defined as: start address + (numItems x 4 bytes)
				  // This will give the address of the final item, which is 0. So we could store 
				  // instead end address - 4 bytes which gives us the final number we will print.

int itemCounter;  // A counter to help display the data item number based on the number of items in the stack.
int starCounter;  // A counter which will hold the number of stars (asterisks) to print.


//**************************************************************************//
// C programs usually kick off in main().  This is a unicode build, so it	//
// kicks off in wmain() (wide main).										//
//**************************************************************************//
int wmain(int argc, _TCHAR* argv[])
{
	// Allocate 1000 bytes in memory for the items array.
	items = (int *) malloc(1000); // I think that should cover it.


	//**********************************************************************//
	// As usual, we use "C" to do the heavy lifting and enter the numbers.
	// The do while loop halts when the user enters a 0, indicating the end 
	// of data entry.
	//**********************************************************************//
	numItems = 0;
	do
	{
		printf("Enter item %d (0 means end of data): ", numItems + 1);
		scanf("%d", &anItem);
		items[numItems] = anItem;
		numItems++;

	} while (anItem != 0);
		


	//**********************************************************************//
	// Into assembler.														//
	// All the text in green after the "//" is a comment.  Comments are		//
	// purely for us humans to leave notes for ourselves; they are ignored	//
	// by the computer.														//
	//**********************************************************************//
	__asm
	{

		//*************************************************************************
		// MULTIPLICATION EXAMPLE:
		// 
	    // NOTE: Multiplication answer and value the other register is multiplied by
		//		 is stored in eax.
		//   
		//		e.g. mov eax, 2
		//			 mov ebx, 2
		//			 
		//			 mul ebx
		//           push ebx
		//           call printInt
		//           jmp finish 
		//
		//  OUTPUT: 4
		//
		// We need to jump past the subroutine(s) that follow
		// else the CPU will just carry on going.
		//**************************************************************************


		//**************************************************************************
		// Move initial items into registers and the "C" variables for use. 
		// As we will be jumping from subroutines, we will not get a chance to 
		// otherwise initialise.
		//**************************************************************************

		// Catch when there are no valid entry items (an item number of 1 will mean 
		// an entry of 0 was entered or an invalid entry was made). An entry of 0
		// can potentially create an infinite loop and is only for termination.
		cmp numItems, 1
		jz finish

		// Print some new lines separating the input and output of the program.
		call printNewLine
		call printNewLine

		// Place the number of items we have into edx and from there move it into 
		// our item counter. We cannot move straight from a C variable to another C 
		// variable within the assembler without a register.
		mov edx, numItems
		mov itemCounter, edx

		// Pointer to the number in items, this will initially be the START ADDRESS.
		mov esi, [items]

		// Store the first pointer address of the first item in the array (this can be used later if we need to).
		mov startAddress, esi

		// Calculate the end address of the last item in the array.
		mov eax, numItems

		// 4 here represents the 4 bytes reserved for each integer.
		mov ebx, 4

		// eax will now store the additional address from the start address to the end address.
		mul ebx     // gets whats in ebx and multiplies it with eax, and stores in eax.
		
		// Add the start address to the multiplication result in eax, which means the value
		// currently stored in eax is the final address of the last number item.
		add eax, startAddress

		// Store what is in eax into the custom end address variable.
		mov endAddress, eax
		
		// TESTING ONLY - To view the start address and the end address.
		//push startAddress
		//call printInt
		//call printNewLine
		//push endAddress
		//call printInt
		//call printNewLine

		// Move the end address into esi before beginning.
		mov esi, endAddress

		//************************************************************************ 
		// Avoid the last 2 items (4 x 2 bytes) as they contain the end address 
		// zero which we do not need.
		sub esi, 8
		//************************************************************************

		//***********************************************************************
		// Adjust the numItems and item counter to discount the 0 at the end, 
		// as we do not print this to the console. Using dec will work, but to show 
		// that this is an intentional change, I will keep it using the sub opcode.
		//***********************************************************************
		sub numItems, 1
		sub itemCounter, 1

		//**********************************************************************//
		// Subroutimes start here, bits of code we want to execute more than	//
		// once, or just because we want to split a complicated task into several//
		// simpler ones.														//
		//**********************************************************************//

		// What am I intending to do?
		// 
		// We will have to figure out the final address of the last item in the items array, 
		// then we can loop and subtract 4 for each item whilst maintaining a counter for the 
		// number of items originally entered (which will be decremented at the same time), 
		// we keep on subtracting 4 until we decrement all the way to 0.

		reverse_items_stack: mov ecx, [esi]  // Move the next number item into the ecx register.

							 // Move the number of stars to output into the "C" star counter variable
							 // before we push the number in ecx into the stack.
							 mov starCounter, ecx

							 // REMEMBER: If you push an item onto the stack, make sure you print it out or pop it out 
							 //			 of the stack, otherwise you will encounter an ESP error.
							 // TESTING ONLY.
							 //push ecx
							 //call printInt
							 //call printNewLine

							 // Print details regarding the graph; the data item number and padding.
							 jmp printGraphPadding	

							 // A subroutine which acts as a way back due to the fact that we cannot 
							 // execute a conditional call, so we use a jmp at the end of the printGraphDetails.
							 back:
			
							 // Make a call to the subroutine allowing the stars to print.
							 call printStars

							 // Compare to esi (current address) with the start address.
							 cmp esi, startAddress
							 je printFinishingLine

							 // As integers are allocated 4 bytes in memory, subtract 4 from esi to get to previous 
							 // item in the data structure.
							 sub esi, 4

							 // Jump unconditionally to items_into_stack to ensure we can print the next number.
							 jmp reverse_items_stack


		//***********************************************
		// Work out the data item number and the padding 
		// it needs and then print the separator symbol.
		//***********************************************
		printGraphPadding: 
						   // Make sure padding is only needed when the number of items
						   // in total is greater than 10 (so we jump to printing details
						   // if it is anything below).
						   cmp numItems, 10
						   jb printGraphDetails

                           // If the current item is greater or equal to 10,
						   // then we don't need to apply any padding.
						   cmp itemCounter, 10
						   jae printGraphDetails
		
						   // Apply padding to the data item number; this will
						   // only apply in the case the conditions above won't be met,
						   // so the number of total items is greater than 9 and the current
						   // item number is less than 10.
						   mov al, ' '
						   push al
						   call printChar
	

		//**************************************
		// This subroutine allows the basic details 
		// of the each graph to be printed before
		// the actual stars are printed.
		//**************************************
		printGraphDetails: push itemCounter
						   call printInt

						   // Subtract 1 from edx so that next time it will 
						   // display the appropriate data item number for the next number.
			               dec itemCounter
						   
						   // The separator symbol is moved into al 
					       // register and pushed to the stack to be 
						   // printed from the stack.
						   mov al, '|'
						   push al
						   call printChar

						   // This jump is a fix to allow the program to
						   // work in the expected order, the program will head back
						   // to the empty subroutine allowing for it loop through again.
						   jmp back



		//******************************************
		// The subroutine prints stars based on the
		// the value set in the star counter.
		//******************************************
		printStars : mov al, '*'		
					 push al
					 call printChar

					 // Make sure we decrement the star counter and 
					 // check to make sure if it has reached 0 to 
					 // ret back to "reverse_items_stack" subroutine.
			         dec starCounter
			         cmp starCounter, 0	
			         jne printStars

			         call printNewLine
			         
					 ret


		//************************************
		// The subroutine allows for the final
		// line to print via the printStr 
		// function to finish the program.
		//************************************
		printFinishingLine:
							// Print the final line as a string by pushing the pointer to 
							// memory location of the string as defined as a "C" variable.
							// push [finishingLine]
						    mov esi, 0

							repeat_underscore:
								mov al, '_'
								push al
								call printChar
							
								add esi, 1
								cmp esi, 80  // This will be roughly a console width, so this is possibly the maximum 
											 // stars we fit in on one line.
								jne repeat_underscore

							// Print out the final greater than character.
							mov al, '>'
							push al
							call printChar

							// Finish the program with a new line and jumping to the finish subroutine.
							call printNewLine
							jmp finish

		
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



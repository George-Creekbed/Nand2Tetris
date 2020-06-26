// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.

@i
M = 1 //initialise loop index

@R2
M = 0 //initialise product cell to 0

(LOOP)
    @R1  
    D = M  //set max limit of loop to value stored in R1
    @i
    D = M - D // if this is > 0 loop ends   
    @END
    D; JGT  
      
    @R0          //if end condition not met,
    D = M
    @R2
    M = M + D    // R2 = R2 + R0
    
    @i           // increase loop index
    M = M + 1
    @LOOP        // stay in the loop
    0; JMP

(END)            //endless loop terminates the program
    @END
    0; JMP
    
    

//PSEUDO CODE
//do i=1,R1
//    R2 = R2 + R0
//end do

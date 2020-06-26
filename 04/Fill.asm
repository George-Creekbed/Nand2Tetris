// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

(LOOP)            //endless loop that checks keyboard input
    @counter      //initialise auxiliary counters
    M = 0
    @i
    M = 0
	@KBD          //read input from keyboard
	D = M
	@BLACKEN      //if a key is pressed go to BLACKEN
	D; JGT
	@WHITEN       //if instead the keyboard gets no input, go to WHITEN
	D; JMP
	@LOOP         //repeat this keyboard loop named LOOP endlessly
	0; JMP

(BLACKEN)         
    @8192         //8191 words is the total span of the screen memory map, from 16834 to 24575
    D = A
    @counter      //check if "counter" has passed the screen memory map span,
    D = M - D     //if yes, exit this loop and return to the main one
    @LOOP
    D; JGE

    @SCREEN       //set D to 16834
    D = A
    @counter      //set current counter value, 16834 <= counter < 24576
    D = D + M
    @i            //pass "counter" value to registry "i"
    M = D
    @i            
    A = M         //position the address value to "counter" value
    M = -1        //set that address value to 1111111111111111 (=-1 in 2-power language)
    @counter      
    M = M + 1     //increase "counter" by one for next iteration of the loop
    @BLACKEN
    0; JMP
    
(WHITEN)          //same as "BLACKEN" loop, just sets address value back to 0            
    @8192         // (whitewashing the screen)
    D = A
    @counter
    D = M - D
    @LOOP
    D; JGE
    
    @SCREEN
    D = A
    @counter
    D = D + M
    @i
    M = D
    @i
    A = M
    M = 0
    @counter
    M = M + 1
    @WHITEN
    0; JMP

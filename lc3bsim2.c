/*
    Remove all unnecessary lines (including this one) 
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Full name of the first partner 
    Name 2: Full name of the second partner
    UTEID 1: UT EID of the first partner
    UTEID 2: UT EID of the second partner
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/


void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */
  int currentPC = CURRENT_LATCHES.PC;
  int mSigInsByte = MEMORY[currentPC >> 1][0];
  int lSigInsByte = MEMORY[currentPC >> 1][1];
  int currentInstruction = (lSigInsByte << 8)|mSigInsByte;
  printf("currentPC: %#06x\n", currentPC);
  printf("currentInstruction: %#06x\n", currentInstruction);
  int opCode = (lSigInsByte >> 4) & 0x0F;
  
  if(opCode == 0x01){
    printf("ADD instruction \n");
    int steeringBit = (currentInstruction & 0x0020) >> 5;
    int destinationRegister = (currentInstruction & 0x0E00) >> 9;
    int sourceRegister1 = (currentInstruction & 0x01C0) >> 6;
    int dRegValue = 0;
    if(!steeringBit){
        int sourceRegister2 = (currentInstruction & 0x0007);
        dRegValue = CURRENT_LATCHES.REGS[sourceRegister1] + CURRENT_LATCHES.REGS[sourceRegister2];
    }else{
        int16_t immediateValue = (currentInstruction & 0x001F);
        if(immediateValue & 0x0010){
            immediateValue |= 0xFFE0;   // sign extend
        }
        dRegValue = CURRENT_LATCHES.REGS[sourceRegister1] + immediateValue;
    }
    if(dRegValue > 0){
        NEXT_LATCHES.P = 1;
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
    }else if(dRegValue == 0){
        NEXT_LATCHES.P = 0;
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
    }else {
        NEXT_LATCHES.P = 0;
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
    }
    for(int i = 0; i < 8; i++){
        NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
    }
    //printf("ADD dReg: %d\n", destinationRegister);
    //printf("ADD dRegVal: %d\n", dRegValue);
    NEXT_LATCHES.REGS[destinationRegister] = dRegValue;
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
  }else if(opCode == 0x05){
      printf("AND instruction \n");
    int steeringBit = (currentInstruction & 0x0020) >> 5;
    int destinationRegister = (currentInstruction & 0x0E00) >> 9;
    int sourceRegister1 = (currentInstruction & 0x01C0) >> 6;
    int dRegValue = 0;
    if(!steeringBit){
        int sourceRegister2 = (currentInstruction & 0x0007);
        dRegValue = CURRENT_LATCHES.REGS[sourceRegister1] & CURRENT_LATCHES.REGS[sourceRegister2];
    }else{
        int16_t immediateValue = (currentInstruction & 0x001F);
        if(immediateValue & 0x0010){
            immediateValue |= 0xFFE0;   // sign extend
        }
        dRegValue = CURRENT_LATCHES.REGS[sourceRegister1] & immediateValue;
    }
    if(dRegValue > 0){
        NEXT_LATCHES.P = 1;
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
    }else if(dRegValue == 0){
        NEXT_LATCHES.P = 0;
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
    }else {
        NEXT_LATCHES.P = 0;
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
    }
    for(int i = 0; i < 8; i++){
        NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
    }
    NEXT_LATCHES.REGS[destinationRegister] = dRegValue;
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
  }else if(opCode == 0x00){
    printf("BR instruction \n");
    int currentPC = CURRENT_LATCHES.PC + 2;
    int16_t PCoffset9 = (currentInstruction & 0x01FF);
    if(PCoffset9 & 0x0100){
        PCoffset9 |= 0xFE00;
    }
    int nBit = (currentInstruction & 0x0800) >> 11;
    int zBit = (currentInstruction & 0x0400) >> 10;
    int pBit = (currentInstruction & 0x0200) >> 9;
    /*
    printf("BR nBit: %d\n", nBit);
    printf("BR zBit: %d\n", zBit);
    printf("BR pBit: %d\n", pBit);
    printf("curN: %d\n", CURRENT_LATCHES.N);
    printf("curZ: %d\n", CURRENT_LATCHES.Z);
    printf("curP: %d\n", CURRENT_LATCHES.P);
    */
    NEXT_LATCHES.N = CURRENT_LATCHES.N;
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P;

    for(int i = 0; i < 8; i++){
        NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
    }

    if(nBit && CURRENT_LATCHES.N){
        NEXT_LATCHES.PC = currentPC + (PCoffset9 << 1);
    }else if(zBit && CURRENT_LATCHES.Z){
        NEXT_LATCHES.PC = currentPC + (PCoffset9 << 1);
        //printf("getting past brz done \n");
    }else if(pBit && CURRENT_LATCHES.P){
        NEXT_LATCHES.PC = currentPC + (PCoffset9 << 1);
    }else{
        NEXT_LATCHES.PC = currentPC;
    }
  }else if(opCode == 0x0C){
    printf("JMP instruction \n");
    int baseRegister = (currentInstruction & 0x01E0) >> 6;
    NEXT_LATCHES.N = CURRENT_LATCHES.N;
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P;
    for(int i = 0; i < 8; i++){
        NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
    }
    NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[baseRegister];
  }else if(opCode == 0x04){
    printf("JSR instruction \n");
  }else if(opCode == 0x02){
     printf("LDB instruction \n");
     int destinationRegister = (currentInstruction & 0x0E00) >> 9;
     int baseRegister = (currentInstruction & 0x01C0) >> 6;
     int16_t boffset6 = (currentInstruction & 0x003F);
     if(boffset6 & 0x0020){
         boffset6 |= 0xFFC0;
     }
     int memAddress = CURRENT_LATCHES.REGS[baseRegister] + boffset6;
     int16_t destinationValue = 0;
     if(memAddress % 2 == 0){
         destinationValue = MEMORY[memAddress >> 1][0];
     }else{
         destinationValue = MEMORY[(memAddress-1) >> 1][1];
     }
     if(destinationValue & 0x0080){
         destinationValue |= 0xFF00;
     }
     if(destinationValue > 0){
         NEXT_LATCHES.P = 1;
     }else if(destinationValue == 0){
         NEXT_LATCHES.Z = 1;
     }else{
         NEXT_LATCHES.N = 1;
     }

     for(int i = 0; i < 8; i++){
         NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
     }
     NEXT_LATCHES.REGS[destinationRegister] = destinationValue;
     NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
  }else if(opCode == 0x06){
    printf("LDW instruction \n");
    int destinationRegister = (currentInstruction & 0x0E00) >> 9;
    int baseRegister = (currentInstruction & 0x01C0) >> 6;
    int16_t boffset6 = (currentInstruction & 0x003F);
    if(boffset6 & 0x0020){
        boffset6 |= 0xFFC0;
    }
    //printf("base register: %d \n", baseRegister);
    //printf("base register val: %#06x \n", CURRENT_LATCHES.REGS[baseRegister]);
    int memAddress = CURRENT_LATCHES.REGS[baseRegister] + (boffset6 << 1);
    //printf("LDW memAddress : %#06x \n", memAddress);
    int16_t destinationValue = (MEMORY[memAddress >> 1][1] << 8) | MEMORY[memAddress >> 1][0];
    //printf("LDW destVal : %#06x \n", destinationValue);
    if(destinationValue & 0x0080){
        destinationValue |= 0xFF00;
    }
    if(destinationValue > 0){
        NEXT_LATCHES.P = 1;
    }else if(destinationValue == 0){
        NEXT_LATCHES.Z = 1;
    }else{
        NEXT_LATCHES.N = 1;
    }

    for(int i = 0; i < 8; i++){
        NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
    }
    NEXT_LATCHES.REGS[destinationRegister] = destinationValue;
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
  }else if(opCode == 0x0E){
    printf("LEA instruction \n");
    int destinationRegister = (currentInstruction & 0x0E00) >> 9;
    int16_t PCOffset9 = currentInstruction & 0x01FF;
    if(PCOffset9 & 0x0100){
        PCOffset9 |= 0xFE00;
    }
    NEXT_LATCHES.N = CURRENT_LATCHES.N;
    NEXT_LATCHES.N = CURRENT_LATCHES.Z;
    NEXT_LATCHES.N = CURRENT_LATCHES.P;
    for(int i = 0; i < 8; i++){
        NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
    }
    NEXT_LATCHES.REGS[destinationRegister] = currentPC + 2 + (PCOffset9 << 1);
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
  }else if(opCode == 0x0D){
    printf("SHF instruction \n");
    int destinationRegister = (currentInstruction & 0x0E00) >> 9;
    int sourceRegister = (currentInstruction & 0x01C0) >> 6;
    int shiftAmount = (currentInstruction & 0x000F);
    int shiftDecider = (currentInstruction & 0x0030) >> 4;

    for(int i = 0; i < 8; i++){
        NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
    }
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
    if(shiftDecider == 0){

    }else if(shiftDecider == 1){

    }else{
        
    }
  }else if(opCode == 0x03){
    printf("STB instruction \n");
  }else if(opCode == 0x07){
    printf("STW instruction \n");
  }else if(opCode == 0x0F){
    printf("TRAP instruction \n");
  }else if(opCode == 0x09){
     printf("XOR/NOT instruction \n");
  }    
}
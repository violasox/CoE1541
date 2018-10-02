/**************************************************************/
/* CS/COE 1541				 			
   compile with gcc -o pipeline five_stage.c			
   and execute using							
   ./pipeline  /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr	0  
***************************************************************/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "CPU.h" 
#include "superscalar.h"

int main(int argc, char **argv)
{
  struct instruction* tr_entry1;
  struct instruction* tr_entry2;
  struct instruction IF1, ID1, EX1, MEM1, WB1;
  struct instruction IF2, ID2, EX2, MEM2, WB2;
  struct instruction packing[2];
  struct instruction nop = {ti_NOP, 0, 0, 0, 0, 0};
  size_t size;
  char *trace_file_name;
  int trace_view_on = 0;
  int flush_counter = 4; //5 stage pipeline, so we have to move 4 instructions once trace is done
  int leftover = 0;
  int lastInstruction = 0;
  unsigned int cycle_number = 0;

  if (argc == 1) {
    fprintf(stdout, "\nUSAGE: tv <trace_file> <switch - any character>\n");
    fprintf(stdout, "\n(switch) to turn on or off individual item view.\n\n");
    exit(0);
  }
    
  trace_file_name = argv[1];
  if (argc == 3) trace_view_on = atoi(argv[2]) ;

  fprintf(stdout, "\n ** opening file %s\n", trace_file_name);

  trace_fd = fopen(trace_file_name, "rb");

  if (!trace_fd) {
    fprintf(stdout, "\ntrace file %s not opened.\n\n", trace_file_name);
    exit(0);
  }

  trace_init();

  while(1) {
    if (leftover) 
      tr_entry1 = tr_entry2;
    else {
      size = trace_get_item(&tr_entry1); /* put the instruction into a buffer */
    }
    size = trace_get_item(&tr_entry2);
    // Need another cycle to flush if the last
    if (!size && !lastInstruction) {
      flush_counter++;
    }
   
    int type1 = checkInstructionType(tr_entry1->type);
    int type2 = checkInstructionType(tr_entry2->type);
    // If first instruction is a branch, can't put both instructions in
    if (type1 == 3) {
      type1 = 1;
      type2 = 1;
    }
    // If the second is a branch, it acts like a regular instruction
    if (type2 == 3) 
        type2 = 1;

    // Can fully pack the superinstruction
    if (type1 != type2) {
      leftover = 0;
      if (type1 == 1) {
        memcpy(&packing[0], tr_entry1, sizeof(IF1));
        memcpy(&packing[1], tr_entry2, sizeof(IF1));
      }
      else {
        memcpy(&packing[0], tr_entry2, sizeof(IF1));
        memcpy(&packing[1], tr_entry1, sizeof(IF1));
      }
    }
    // Superinstruction gets one real instruction and one no-op
    else {
      leftover = 1;
      if (type1 == 1) {
        memcpy(&packing[0], tr_entry1, sizeof(IF1));
        memcpy(&packing[1], &nop, sizeof(IF1));
      }
      else {
        memcpy(&packing[0], &nop, sizeof(IF1));
        memcpy(&packing[1], tr_entry1, sizeof(IF1));
      }
    }

    if (!size && flush_counter==0) {       /* no more instructions (instructions) to simulate */
      printf("+ Simulation terminates at cycle : %u\n", cycle_number);
      break;
    }

    else{              /* move the pipeline forward */
      cycle_number++;

      /* move instructions one stage ahead */
      WB1 = MEM1;
      MEM1 = EX1;
      EX1 = ID1;
      ID1 = IF1;

      WB2 = MEM2;
      MEM2 = EX2;
      EX2 = ID2;
      ID2 = IF2;

      if(!size) {    /* if no more instructions in trace, reduce flush_counter */
        flush_counter--;   
        lastInstruction = 1;
      }
      else {
        memcpy(&IF1, &packing[0], sizeof(IF1));
        memcpy(&IF2, &packing[1], sizeof(IF2));
      }

      //printf("==============================================================================\n");
    }  


    if (trace_view_on && cycle_number>=5) {/* print the executed instruction if trace_view_on=1 */
      printInstruction(WB1, cycle_number, 1);
      printInstruction(WB2, cycle_number, 2);
    }
  }

  trace_uninit();

  exit(0);
}

void printInstruction(struct instruction WB, unsigned int cycle_number, int pipeline) {
  switch(WB.type) {
    case ti_NOP:
      printf("[cycle %d] NOP:\n",cycle_number) ;
      break;
    case ti_RTYPE: /* registers are translated for printing by subtracting offset  */
      printf("[cycle %d] RTYPE:",cycle_number) ;
      printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(dReg: %d) \n", WB.PC, WB.sReg_a, WB.sReg_b, WB.dReg);
      break;
    case ti_ITYPE:
      printf("[cycle %d] ITYPE:",cycle_number) ;
      printf(" (PC: %d)(sReg_a: %d)(dReg: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.dReg, WB.Addr);
      break;
    case ti_LOAD:
      printf("[cycle %d] LOAD:",cycle_number) ;      
      printf(" (PC: %d)(sReg_a: %d)(dReg: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.dReg, WB.Addr);
      break;
    case ti_STORE:
      printf("[cycle %d] STORE:",cycle_number) ;      
      printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.sReg_b, WB.Addr);
      break;
    case ti_BRANCH:
      printf("[cycle %d] BRANCH:",cycle_number) ;
      printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.sReg_b, WB.Addr);
      break;
    case ti_JTYPE:
      printf("[cycle %d] JTYPE:",cycle_number) ;
      printf(" (PC: %d)(addr: %d)\n", WB.PC,WB.Addr);
      break;
    case ti_SPECIAL:
      printf("[cycle %d] SPECIAL:\n",cycle_number) ;      	
      break;
    case ti_JRTYPE:
      printf("[cycle %d] JRTYPE:",cycle_number) ;
      printf(" (PC: %d) (sReg_a: %d)(addr: %d)\n", WB.PC, WB.dReg, WB.Addr);
      break;
  }
}

int checkInstructionType(enum opcode type) {
  switch(type) {
    case ti_LOAD:
      return 2;
      break;
    case ti_STORE:
      return 2;
      break;
    case ti_BRANCH:
      return 3;
      break;
    default:
      return 1; 
      break;
  }

  return 1;
}



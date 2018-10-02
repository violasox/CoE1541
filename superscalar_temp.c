/**************************************************************/
/* CS/COE 1541
   compile with gcc -o pipeline five_stage.c
   and execute using
   ./pipeline  /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr 1
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h"
#include "superscalar.h"

int main(int argc, char **argv)
{
  struct instruction *tr_entry;
  struct instruction IF1, ID1, EX1, MEM1, WB1;
  struct instruction IF2, ID2, EX2, MEM2, WB2;
  // Create a no-op instruction to be inserted when necessary
  struct instruction nop = {ti_NOP, 0, 0, 0, 0, 0};
  struct instruction* nopPointer = &nop;
  size_t size;
  char *trace_file_name;
  int trace_view_on = 0;
  int flush_counter = 6; //5 stage pipeline, so we have to move 4 instructions once trace is done

  struct instruction packing[2];

  // array of 2 instructions
  struct instruction *prefetch[2];

  unsigned int cycle_number = 0;
  unsigned int numNop = 0;
  int prefetch_ready = 0;

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

  // initial instruction fetch for prefetch queue
  size = trace_get_item(&tr_entry); /* put the instruction into a buffer */

  while(1) {

    // Only load a new instruction from the trace if we haven't inserted a no-op
    if (numNop == 0)
    {
    	prefetch[0] = tr_entry;
      size = trace_get_item(&tr_entry); /* put the instruction into a buffer */
    }
    else
    {
    	prefetch[0] = nopPointer;
      numNop--;
    }

    // Check for a data hazard
    if (prefetch[0]->type == ti_LOAD)
    {
      // get the register that the data will be loaded into
      uint8_t loadIntoReg = prefetch[0]->dReg;
      switch (tr_entry->type)
      {
      	case ti_RTYPE :
      	case ti_STORE :
      	case ti_BRANCH :
          // if the next instruction is RTYPE/STORE/BRANCH check sReg a and b for load use
		      if (tr_entry->sReg_a == loadIntoReg || tr_entry->sReg_b == loadIntoReg) {
		        	numNop = 1;
		      }
		      break;
		    case ti_ITYPE :
		    case ti_LOAD :
		    case ti_JRTYPE :
        // if the next instruction is ITYPE/LOAD/JRTYPE check ONLY sReg a for load use
		    	if (tr_entry->sReg_a == loadIntoReg) {
		        	numNop = 1;
		      }
		      break;
	    }
    }
    // check for control hazard
    else if (prefetch[0]->type == ti_BRANCH || prefetch[0]->type == ti_JTYPE || prefetch[0]->type == ti_JRTYPE)
    {
      // add a no op if the brach was taken
      if (tr_entry->PC == prefetch[0]->Addr)
        numNop = 1;
    }

    if (!size && flush_counter==0) {       /* no more instructions (instructions) to simulate */
      printf("+ Simulation terminates at cycle : %u\n", cycle_number);
      break;
    }
    else if (!prefetch_ready)
    	prefetch_ready = 1;
    else {            /* move the pipeline forward */
      cycle_number++;

      /* move instructions one stage ahead */
      WB1 = MEM1;
      MEM1 = EX1;
      EX1 = ID1;
      ID1 = IF1;

      if(!size) {    /* if no more instructions in trace, reduce flush_counter */
        flush_counter--;
      }
      // else {   /* copy the later queue entry into IF1 stage */
      	memcpy(&IF1, prefetch[1], sizeof(IF1));

      //printf("==============================================================================\n");
    	// }
    }

    // always shift the queue by 1
   	prefetch[1] = prefetch[0];

    if (trace_view_on && cycle_number>=5) {/* print the executed instructions if trace_view_on=1 */
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

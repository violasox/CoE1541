/**************************************************************/
/* CS/COE 1541				 			
   compile with gcc -o pipeline five_stage.c
   and execute using							
   ./pipeline  /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr	0 1
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h" 
#include "hash_table.h"

int main(int argc, char **argv)
{
  struct instruction *tr_entry;
  struct instruction IF, ID, EX, MEM, WB;
  // Create a no-op instruction to be inserted when necessary
  struct instruction nop = {ti_NOP, 0, 0, 0, 0, 0};
  struct instruction* nopPointer = &nop;
  size_t size;
  char *trace_file_name;
  int trace_view_on = 0;
  int prediction_method = 0;
  int flush_counter = 4; //5 stage pipeline, so we have to move 4 instructions once trace is done

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
  if (argc >= 3) prediction_method = atoi(argv[2]) ;
  if (argc == 4) trace_view_on = atoi(argv[3]) ;

  fprintf(stdout, "\n ** opening file %s\n", trace_file_name);

  trace_fd = fopen(trace_file_name, "rb");

  if (!trace_fd) {
    fprintf(stdout, "\ntrace file %s not opened.\n\n", trace_file_name);
    exit(0);
  }

  trace_init();

  // initial instruction fetch for prefetch queue
  trace_get_item(&tr_entry); /* put the instruction into a buffer */
  
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

    // there can only be hazards if there are instructions left
    if (size)
    {
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
        // if prediction_method == 0, add a no op if the brach was taken
        if (prediction_method == 0 && tr_entry->PC == prefetch[0]->Addr)
          numNop = 1;
        else if (prediction_method == 1)
        {
          int taken = 0;
          int prediction = 0;

          // determine if branch was taken
          if (tr_entry->PC == prefetch[0]->Addr)
            taken = 1;

          // determine hash key for target address
          int hashKey = prefetch[0]->Addr & 1008;
          hashKey = hashKey / 16;

          // check if target address is in hash table
          item = search(hashKey);

          // if target in hash table, prediction = value in table, else = not taken
          if (item != NULL)
            prediction = item->data;

          // if prediction isn't what actually happened, add a no op
          if (prediction != taken)
            numNop = 1;

          // add latest decision (taken/not) to hash table
          insert(hashKey, taken);
        }
      }
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
      WB = MEM;
      MEM = EX;
      EX = ID;
      ID = IF;

      if(!size) {    /* if no more instructions in trace, reduce flush_counter */
        flush_counter--;   
      }
      else {   /* copy the later queue entry into IF stage */
      	memcpy(&IF, prefetch[1], sizeof(IF));
      
      //printf("==============================================================================\n");
    	}  
    }

    // always shift the queue by 1
   	prefetch[1] = prefetch[0];

    if (trace_view_on && cycle_number>=5) {/* print the executed instruction if trace_view_on=1 */
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
  }

  trace_uninit();

  exit(0);
}



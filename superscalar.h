void printInstruction(struct instruction WB, unsigned int cycle_number, int pipeline);
int checkInstructionType(enum opcode type);
int checkDataDependency(uint8_t loadIntoReg, struct instruction* instruct);
int checkForHazards(struct instruction** packing, struct instruction* instruct, int checkControl);
void safe_tr_get_item(void);

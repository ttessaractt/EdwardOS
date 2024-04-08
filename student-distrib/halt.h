/* halt_asm
 * Description: assembly code used in halt
 * Inputs: pcb_parent->ebp - the ebp that was saved during execute
 *         stat - the 32-bit return value to the parent program's execute syscall 
 * Return Value: stat in EAX
 */
extern void halt_asm(uint32_t, uint32_t);

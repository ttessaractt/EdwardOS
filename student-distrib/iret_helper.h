/* jump_to_user
 * Description: push the IRET context needed to switch to the user program specified by the input 
 * Inputs: entry_addr - EIP/entrypoint into user program 
 * Return Value: none
 */
extern void jump_to_user(int32_t);


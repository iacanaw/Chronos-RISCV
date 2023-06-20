/*********************************************************************
* Filename:   aes_sl(n).c
* Author:     Leonardo Rezende Juracy and Luciano L. Caimi
* Copyleft:    
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:   
*********************************************************************/

/*************************** HEADER FILES ***************************/
#include "aes.h"
/**************************** VARIABLES *****************************/

static char start_print[]="task AES SLAVE 4\n";
static char config_print[]="Slave configuration\n";

static char myID_print[]=" >> My ID: ";
static char operation_print[]="\nOperation:";
static char blocks_print[]="Blocks:";
static char receiv_print[]="received msg: ";
static char encript_print[]="encript\n";
static char dencript_print[]="dencript\n";
static char finish_print[]="task AES SLAVE 4 finished\n";

volatile static Message theMessage;

/*************************** MAIN PROGRAM ***************************/

int main(){
	static unsigned int key_schedule[60];
	static int qtd_messages, op_mode, x, flag = 1, id, i;
	static unsigned int enc_buf[128];
	static unsigned int input_text[16]; 
	static unsigned int key[1][32] = { {0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4} };

    sys_Prints(&start_print); 
	
	if ( !isMigration() ){
		aes_key_setup(&key[0][0], key_schedule, 256);
		id = -1;
	}
    
    while(flag){

		checkMigration();

		sys_Receive(&theMessage, aes_master);
		aes_memcpy(input_text, theMessage.msg, 16);
			
#ifdef debug_comunication_on
        sys_Prints(&config_print);
        for(i=0; i<3;i++){
            sys_Printi(input_text[i]);
        }
        sys_Prints( &barra_ene ); 
#endif 
				
		op_mode = input_text[0];
		qtd_messages = input_text[1];
		x = input_text[2];	
		
		if(id == -1){
            id = x;
            sys_Prints( &myID_print );
            sys_Printi(id);
		}	
		sys_Prints(&operation_print); 
		sys_Printi(op_mode);
        sys_Prints( &barra_ene ); 
		sys_Prints(&blocks_print); 		
		sys_Printi(qtd_messages);
        sys_Prints( &barra_ene ); 

		if (op_mode == END_TASK){
			flag = 0;
			qtd_messages = 0;
		}
		for(x = 0; x < qtd_messages; x++){
			sys_Receive(&theMessage, aes_master);		
			aes_memcpy(input_text, theMessage.msg, 4*AES_BLOCK_SIZE);
			
#ifdef debug_comunication_on
            sys_Prints(&receiv_print);
            for(i=0; i<16;i++){
                sys_Printi(input_text[i]);	
            }
            sys_Prints( &barra_ene );
#endif 
			
			if(op_mode == CIPHER_MODE){
				sys_Prints(&encript_print);				
				aes_encrypt(input_text, enc_buf, key_schedule, KEY_SIZE);	
			}
			else{
				sys_Prints(&dencript_print);					
				aes_decrypt(input_text, enc_buf, key_schedule, KEY_SIZE);
			}			
			theMessage.length = 4*AES_BLOCK_SIZE;
			aes_memcpy( theMessage.msg, enc_buf, 4*AES_BLOCK_SIZE);
			sys_Send(&theMessage, aes_master);	
		}
	}
    sys_Prints(&finish_print);
	sys_Finish();
}

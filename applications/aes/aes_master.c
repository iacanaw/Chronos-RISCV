/*********************************************************************
* Filename:   aes_main.c
* Author:     Leonardo Rezende Juracy and Luciano L. Caimi
* Copyleft:    
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:   
*********************************************************************/

/*************************** HEADER FILES ***************************/
#include "aes.h"
/***************************** DEFINES ******************************/
// total message length
#define MSG_LENGHT 2048			
// number of efectived used slaves
#define NUMBER_OF_SLAVES 4	
// number of total slaves allocated
#define MAX_SLAVES 4		 	

/**************************** VARIABLES *****************************/

volatile static Message theMessage;

//index of slaves (slave names)
static int Slave[MAX_SLAVES] = {aes_slave1, aes_slave2, aes_slave3, aes_slave4};

static char start_print[]="AES master started...\n";
static char blocks_print[]="Blocks: ";
static char plain_print[]="plain msg: ";
static char space_print[]=" - ";
static char cipher_print[]="cipher msg: ";
static char decipher_print[]="decipher msg: ";
static char final_print[]="Final result: ";
static char finish_print[]="AES Master finished!\n";

/*************************** MAIN PROGRAM ***************************/

int main(){
	static int x, y, i, j;
	static int plain_msg[MSG_LENGHT];
	static int cipher_msg[MSG_LENGHT], decipher_msg[MSG_LENGHT];
	static int msg_length, blocks, qtd_messages[MAX_SLAVES];
	static int pad_value, aux_msg[3];
	static int aux1_blocks_PE;
	static int aux2_blocks_PE;	
	int a;

	if ( !isMigration() ){
		asm("nop");
	}
	
	// fill each block with values 'A', 'B', ...
	for(x = 0; x < MSG_LENGHT; x++){
		plain_msg[x] = ((x/16)%26)+0x41;
	}
	
    sys_Prints( &start_print );

	// calculate number of block and pad value (PCKS5) of last block
	msg_length = MSG_LENGHT;	
	blocks = (MSG_LENGHT%AES_BLOCK_SIZE)==0 ? (MSG_LENGHT/AES_BLOCK_SIZE) : (MSG_LENGHT/AES_BLOCK_SIZE)+1;
	pad_value = (AES_BLOCK_SIZE - (msg_length%AES_BLOCK_SIZE))%AES_BLOCK_SIZE;	
	
	sys_Prints( &blocks_print );	
	sys_Printi(blocks);
    sys_Prints( &barra_ene );

#ifdef debug_comunication_on	
    sys_Prints( &plain_print );
    for(x=0; x<MSG_LENGHT-1;x++){
		sys_Printi(plain_msg[x]);		
        sys_Prints(&space_print);
	}
    sys_Prints( &barra_ene );
#endif

	//	Calculate number of blocks/messages to sent
	//   to each Slave_PE
	aux1_blocks_PE = blocks / NUMBER_OF_SLAVES;
	aux2_blocks_PE = blocks % NUMBER_OF_SLAVES;
	
	////////////////////////////////////////////////
	//				Start Encrypt				  //
	////////////////////////////////////////////////	
	for(x = 0; x < MAX_SLAVES; x++){
		qtd_messages[x] = aux1_blocks_PE;
		if(x < aux2_blocks_PE)
			qtd_messages[x] += 1;
	}
	
	// Send number of block and operation mode and ID
	// to each Slave_PE
	for(x=0; x < MAX_SLAVES; x++){
		theMessage.length = 3;
		aux_msg[0] = CIPHER_MODE;
		aux_msg[1] = qtd_messages[x];
		aux_msg[2] = x+1;
		if(x >= NUMBER_OF_SLAVES) // zero messages to Slave not used
			aux_msg[0] = END_TASK;
		
		//aes_memcpy(theMessage.msg, aux_msg, sizeof(aux_msg));
		for( a = 0; a < theMessage.length; a++){
			theMessage.msg[a] = aux_msg[a];
		}
		sys_Send(&theMessage, Slave[x]);  
	}

	// Send blocks to Cipher and 
	// Receive the correspondent block Encrypted
	for(x = 0; x < blocks+1; x += NUMBER_OF_SLAVES){
		// send a block to Slave_PE encrypt
		for(y = 0; y < NUMBER_OF_SLAVES; y++){
			if(qtd_messages[(x+y) % NUMBER_OF_SLAVES] != 0){
				theMessage.length = 4*AES_BLOCK_SIZE;
				//aes_memcpy(theMessage.msg, &plain_msg[(x+y)*AES_BLOCK_SIZE], 4*AES_BLOCK_SIZE);
				for(a = 0; a < theMessage.length; a++){
					theMessage.msg[a] = plain_msg[((x+y)*AES_BLOCK_SIZE)+a];
				}
				sys_Send(&theMessage, Slave[(x+y) % NUMBER_OF_SLAVES]);
			}
		}
	
		// Receive Encrypted block from Slave_PE
		for(y = 0; y < NUMBER_OF_SLAVES; y++){
			if(qtd_messages[(x+y) % NUMBER_OF_SLAVES] != 0){
				sys_Receive(&theMessage, Slave[(x+y) % NUMBER_OF_SLAVES]);
				j = 0;
				for (i=(x+y)*AES_BLOCK_SIZE;i < ((x+y)*AES_BLOCK_SIZE) + AES_BLOCK_SIZE; i++)
				{
					cipher_msg[i] = theMessage.msg[j];
					j++;
				}
				j = 0;
				qtd_messages[(x+y) % NUMBER_OF_SLAVES]--;
			}
		}
	}
#ifdef debug_comunication_on
	sys_Prints( &barra_ene );
	sys_Prints( &cipher_print );
	for(i=0; i<MSG_LENGHT;i++){
		sys_Printi(cipher_msg[i]);		
	}
	sys_Prints( &barra_ene ); 
#endif 
	
	////////////////////////////////////////////////
	//				Start Decrypt				  //
	////////////////////////////////////////////////
	for(x = 0; x < NUMBER_OF_SLAVES; x++){
		qtd_messages[x] = aux1_blocks_PE;
		if(x <= aux2_blocks_PE)
			qtd_messages[x] += 1;
	}
	
	// Send number of block and operation mode
	// to each Slave_PE
	for(x=0; x < NUMBER_OF_SLAVES; x++){
		theMessage.length = 3;
		aux_msg[0] = DECIPHER_MODE;
		aux_msg[1] = qtd_messages[x];
		//aes_memcpy(theMessage.msg, aux_msg, sizeof(aux_msg));
		for(a = 0; a < theMessage.length; a++){
			theMessage.msg[a] = aux_msg[a];
		}
		sys_Send(&theMessage, Slave[x]);  
	}

	// Send blocks to Cipher and 
	// Receive the correspondent block Encrypted
	for(x = 0; x < blocks+1; x += NUMBER_OF_SLAVES){
		// send each block to a Slave_PE
		for(y = 0; y < NUMBER_OF_SLAVES; y++){
			if(qtd_messages[(x+y) % NUMBER_OF_SLAVES] != 0){
				theMessage.length = 4*AES_BLOCK_SIZE;
				//aes_memcpy(theMessage.msg, &cipher_msg[(x+y)*AES_BLOCK_SIZE], 4*AES_BLOCK_SIZE);
				for(a = 0; a < theMessage.length; a++){
					theMessage.msg[a] = cipher_msg[((x+y)*AES_BLOCK_SIZE)+a];
				}
				sys_Send(&theMessage, Slave[(x+y) % NUMBER_OF_SLAVES]);   
			} 
		}
		// Receive Encrypted block from Slave_PE
		for(y = 0; y < NUMBER_OF_SLAVES; y++){
			if(qtd_messages[(x+y) % NUMBER_OF_SLAVES] != 0){
				sys_Receive(&theMessage, Slave[(x+y) % NUMBER_OF_SLAVES]);
				j = 0;
				for (i=(x+y)*AES_BLOCK_SIZE;i < ((x+y)*AES_BLOCK_SIZE) + AES_BLOCK_SIZE; i++)
				{
					decipher_msg[i] = theMessage.msg[j];
					j++;
				}
				j = 0;
				qtd_messages[(x+y) % NUMBER_OF_SLAVES]--;
			}
		}
	}
#ifdef debug_comunication_on	
	sys_Prints( &decipher_print );
    for(x=0; x<MSG_LENGHT-1;x++){
		sys_Printi(decipher_msg[x]);		
	}
    sys_Prints( &barra_ene ); 
#endif
	//  End tasks still running
	//  End Applicattion
	for(x=0; x < NUMBER_OF_SLAVES; x++){
		theMessage.length = 3;
		aux_msg[0] = END_TASK;
		aux_msg[1] = 0;
		//aes_memcpy(&theMessage.msg, aux_msg, sizeof(aux_msg));
		for(a = 0; a < theMessage.length; a++){
			theMessage.msg[a] = aux_msg[a];
		}
		sys_Send(&theMessage, Slave[x]);  
	}

//#ifdef debug_comunication_on	
	sys_Prints( &final_print );
	static unsigned int int_aux2 = 0;
    for(x=0; x<MSG_LENGHT;x+=4){
		int_aux2 = decipher_msg[0+x] << 24;
		int_aux2 = int_aux2 | decipher_msg[1+x] << 16;
		int_aux2 = int_aux2 | decipher_msg[2+x] << 8;
		int_aux2 = int_aux2 | decipher_msg[3+x];
		sys_Prints( &int_aux2 );
		int_aux2 = 0;
	}
//#endif 
    sys_Prints( &finish_print );
	sys_Finish();		
}

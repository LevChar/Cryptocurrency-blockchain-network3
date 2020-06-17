/* ======================================================== */
/*   bitcoin.c		        								*/
/* ======================================================== */

//---------------------------------------------------------------------------
//----------------------------- Includes ------------------------------------
//---------------------------------------------------------------------------
#include "../include/bitcoin.h"

//---------------------------------------------------------------------------
//---------------------- Public Methods Implementations----------------------
//---------------------------------------------------------------------------
PUBLIC
void
print_bitcoin_block_data(bitcoin_block_data* block_to_print)
{
	printf("height(%u), ", block_to_print->height);
	printf("timestamp(%u), ", block_to_print->time_stamp);
	printf("hash(0x%x), ", block_to_print->hash);
	printf("prev_hash(0x%x), ", block_to_print->prev_hash);
	printf("difficulty(%u), ", block_to_print->difficulty);
	printf("nonce(%u)", block_to_print->nonce);
	printf("\n----------------------");
}

PUBLIC
void 
release_bitcoin_block_data(pVoid block_to_release)
{
    free(block_to_release);
}
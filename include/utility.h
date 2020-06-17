/* ======================================================== */
/*   utility.h				       						    */
/* ======================================================== */
#ifndef UTILITY_H
#define UTILITY_H

//---------------------------------------------------------------------------
//----------------------------- Includes ------------------------------------
//---------------------------------------------------------------------------
#include "bitcoin.h"

//---------------------------------------------------------------------------
//--------------------------- Utility Defines -------------------------------
//---------------------------------------------------------------------------
#define PLACE_HOLDER_TILL_MINER_WILL_MINE   0
#define RAND_LO                             1
#define RAND_HI                             2000000
#define MAX_PART_SIZE                       30
#define MAX_STR_SIZE                        60

//---------------------------------------------------------------------------
//---------------------- Public Methods Prototypes---------------------------
//---------------------------------------------------------------------------
EBoolType 
check_difficulty(Uint i_hash, Uint i_difficulty);

Uint 
create_hash_from_block(bitcoin_block_data* i_Block);

void 
append_to_string(char* i_OrigString, char* i_PartToAppend);

void 
append_int_to_string(char* i_OrigString, Uint i_Num);

char* 
concat_block(bitcoin_block_data* i_Block);

Uint 
get_current_time_stamp();

Uint 
create_hash(char* data);

Uint 
get_random();

void 
usage_err(Uint count, char* err_msg);

#endif // !UTILITY_H
/* ======================================================== */
/*   utility.c				       						    */
/* ======================================================== */

//---------------------------------------------------------------------------
//----------------------------- Includes ------------------------------------
//---------------------------------------------------------------------------
#include "../include/utility.h"

//---------------------------------------------------------------------------
//----------------------- Public Methods Implementations --------------------
//---------------------------------------------------------------------------
PRIVATE
void 
append_to_string(char* i_origin_string, char* i_part_to_append)
{
    int part_len = strlen(i_part_to_append);
    int origin_string_len = strlen(i_origin_string);
	
    for (int i = 0; i < part_len; ++i)
	{
        i_origin_string[origin_string_len + i] = i_part_to_append[i];
    }

	i_origin_string[origin_string_len + part_len] = '\0';
}

PRIVATE
void 
append_int_to_string(char* i_origin_string, unsigned int i_num)
{
    char* int_string = malloc(MAX_PART_SIZE * sizeof(char));
    sprintf((char*)int_string, "%x", i_num);
    append_to_string(i_origin_string, int_string);
    free(int_string);
}

PRIVATE
char*
concat_block(bitcoin_block_data* i_block)
{
    char* concated_data = malloc(MAX_STR_SIZE * sizeof(char));

    append_int_to_string(concated_data, i_block->height);
    append_int_to_string(concated_data, i_block->time_stamp);
    append_int_to_string(concated_data, i_block->prev_hash);
    append_int_to_string(concated_data, i_block->nonce);
    append_int_to_string(concated_data, i_block->relayed_by);

    return concated_data;
}

PUBLIC
Uint
get_current_time_stamp()
{
	return (Uint)time(NULL);
}

PUBLIC
Uint
get_random()
{
    return(rand() % (RAND_HI - RAND_LO + 1)) + RAND_LO;
}

PUBLIC
Uint 
create_hash(char* data)
{
    return(crc32(0L, (unsigned char*)data, strlen((const char*)data)));
}

PUBLIC
EBoolType
check_difficulty(Uint i_hash, Uint i_difficulty)
{
    Uint difficulty_max_hash_val = 1;
    
	difficulty_max_hash_val = difficulty_max_hash_val << (sizeof(Uint) * 8 - i_difficulty);
	difficulty_max_hash_val--;

	return i_hash <= difficulty_max_hash_val ? TRUE : FALSE;
}

PUBLIC
Uint
create_hash_from_block(bitcoin_block_data* i_Block)
{
    char* concatedBlock = concat_block(i_Block);
    Uint hashValue  = create_hash(concatedBlock);

    free(concatedBlock);

    return hashValue;
}

PUBLIC
void 
usage_err(Uint count, char* err_msg)
{
  fprintf(stderr, "Expecting one argument and got %d arguments\n", --count);
  fprintf(stderr, "The argument should be: %s .\n", err_msg);
  exit(EXIT_FAILURE);
}

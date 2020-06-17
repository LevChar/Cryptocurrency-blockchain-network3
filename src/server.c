/* ======================================================== */
/*   server.c		        								*/
/* ======================================================== */

//---------------------------------------------------------------------------
//----------------------------- Includes ------------------------------------
//---------------------------------------------------------------------------
#include "../include/server.h"
#include "../include/bitcoin_ipc_mq.h"

//---------------------------------------------------------------------------
//--------------------------- Global Variables  -----------------------------
//---------------------------------------------------------------------------
Singly_Linked_List* g_blockchain;
bitcoin_block_data curr_head;
Uint last_calculated_hash;
mqd_t g_bitcoin_mq[MAX_NUM_OF_MINERS] = {0};
Uint g_total_miners_joined = 0;

//---------------------------------------------------------------------------
//---------------------- Private Methods Prototypes -------------------------
//---------------------------------------------------------------------------
void initialize_list_with_genesis();
void print_block_acceptance();
void print_block_rejection(int i_difference, bitcoin_block_data* i_block_candidate);
int verify_block(bitcoin_block_data* i_Block);
bitcoin_block_data* createGenesis();
void bitcoin_msg_rcv_and_handle(void);
void treat_suggested_block(bitcoin_block_data* i_curr_candidate);
void anounce_new_head();
void handle_mine(MSG_PACK_T* rcvd_msg);
void handle_init(MSG_PACK_T* rcvd_msg);

//---------------------------------------------------------------------------
//-----------------------Private Methods Implementations---------------------
//---------------------------------------------------------------------------
PRIVATE
bitcoin_block_data* 
createGenesis()
{
		bitcoin_block_data* new_block = malloc(sizeof(bitcoin_block_data));
		new_block->height = GENESIS_VAL;
		new_block->time_stamp = get_current_time_stamp();
		new_block->prev_hash = GENESIS_VAL;
		new_block->difficulty = DIFFICULTY;
		new_block->nonce = GENESIS_VAL;
		new_block->relayed_by = GENESIS_VAL;
		new_block->hash = create_hash_from_block(new_block);
		return new_block;
}

PRIVATE
void
initialize_list_with_genesis()
{
	bitcoin_block_data* genesis_block = createGenesis();	
	append_To_List(g_blockchain, genesis_block);
}

PRIVATE
void
print_block_acceptance(bitcoin_block_data* i_block_candidate)
{
	printf("Server: New block added by %d, attributes: ", i_block_candidate->relayed_by);
	print_bitcoin_block_data(i_block_candidate);
	printf("\n");
}

PRIVATE
void
print_block_rejection(int i_difference, bitcoin_block_data* i_block_candidate)
{
    if (i_difference == WRONG_HASH)
    {
        printf("Wrong hash for block #%d by miner %d, received %x but calculated %x\n",
               i_block_candidate->height,
               i_block_candidate->relayed_by,
               i_block_candidate->hash,
               last_calculated_hash);
    }
    else
    {
        printf("Wrong height for block #%d by miner %d, received %d but height is %d\n",
               i_block_candidate->height,
               i_block_candidate->relayed_by,
               i_block_candidate->height,
               curr_head.height + 1);
    }
}

PRIVATE
int
verify_block(bitcoin_block_data* i_block)
{
    Uint next_block_height = curr_head.height + 1;
    last_calculated_hash = create_hash_from_block(i_block);
    Uint result = (i_block->height != next_block_height)  ?
		WRONG_HEIGHT : (i_block->hash != last_calculated_hash) ?
		WRONG_HASH : EQUAL_BLOCKS;

	return result;
}

PRIVATE
void
bitcoin_msg_rcv_and_handle(void)
{
	MSG_PACK_T* rcvd_msg = msg_rcv(g_bitcoin_mq[0]);

	if (rcvd_msg->type == INIT)
	{
		handle_init(rcvd_msg);
	}

	else if (rcvd_msg->type == MINE)
	{
        handle_mine(rcvd_msg);
	}
    free(rcvd_msg);
}

PRIVATE
void
handle_init(MSG_PACK_T* rcvd_msg)
{
    MSG_PACK_T *response;
    INIT_MSG_DATA_T *new_miner_conn = malloc(sizeof(INIT_MSG_DATA_T));
    new_miner_conn = (INIT_MSG_DATA_T*)rcvd_msg->data;

    g_bitcoin_mq[++g_total_miners_joined] = join_miners_q(new_miner_conn->miners_id);
    response = malloc(sizeof(MSG_PACK_T) + sizeof(MINE_MSG_DATA_T));
    response->type = MINE;
    ((MINE_MSG_DATA_T*)response->data)->block_detailes = curr_head;
    msg_send(g_bitcoin_mq[g_total_miners_joined], (char*)response);
    free(response);
}

PRIVATE
void
handle_mine(MSG_PACK_T* rcvd_msg)
{
    bitcoin_block_data *newly_mined_block = malloc(sizeof(bitcoin_block_data));
    *newly_mined_block = ((MINE_MSG_DATA_T*)rcvd_msg->data)->block_detailes;
    treat_suggested_block(newly_mined_block);
}

PRIVATE
void 
treat_suggested_block(bitcoin_block_data* i_curr_candidate)
{
	int comp_result = verify_block(i_curr_candidate);

	if (!comp_result)
	{
		append_To_List(g_blockchain, i_curr_candidate);
	   	curr_head = *((bitcoin_block_data*)get_last_block(g_blockchain));
		print_block_acceptance(&curr_head);
		anounce_new_head();
	}
	else
	{
		print_block_rejection(comp_result, i_curr_candidate);
        free(i_curr_candidate);
	}
}

PRIVATE
void anounce_new_head()
{
    MSG_PACK_T *response;
    response = malloc(sizeof(MSG_PACK_T) + sizeof(MINE_MSG_DATA_T));
    response->type = MINE;
    ((MINE_MSG_DATA_T*)response->data)->block_detailes = curr_head;

	for (size_t i = 1 ; i <= g_total_miners_joined ; i++)
	{
		msg_send(g_bitcoin_mq[i], (char*)response);
	}
}

//---------------------------------------------------------------------------
//----------------------- Public Methods Implementations---------------------
//---------------------------------------------------------------------------
PUBLIC
int
main(void)
{
    setpriority(PRIO_PROCESS, 0, -20);
	Q_PARAMS_T params_to_q;
		g_blockchain = initialize_Empty_List(
	        sizeof(bitcoin_block_data),
            release_bitcoin_block_data);

    initialize_list_with_genesis(g_blockchain);
	curr_head = *((bitcoin_block_data*)get_last_block(g_blockchain));

	printf("Listening on %s\n", MQ_SERVER_NAME);
	
	params_to_q.conn_type = CREAT;
	
	/*The main queue of the server is always @ position 0 (SERVER_Q)*/
	g_bitcoin_mq[SERVER_Q] = open_queue(MQ_SERVER_NAME, params_to_q);

	while(g_blockchain->length < 100)
    {
        bitcoin_msg_rcv_and_handle();
	}

    destroy_List(g_blockchain);
	mq_unlink(MQ_SERVER_NAME);

    return 0;
}

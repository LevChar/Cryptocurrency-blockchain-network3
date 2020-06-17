/* ======================================================== */
/*   miner.c		        								*/
/* ======================================================== */

//---------------------------------------------------------------------------
//----------------------------- Includes ------------------------------------
//---------------------------------------------------------------------------
#include "../include/miner.h"
#include "../include/server.h"
#include "../include/bitcoin_ipc_mq.h"
#include "../include/bitcoin.h"

//---------------------------------------------------------------------------
//---------------------- Private Methods Prototypes -------------------------
//---------------------------------------------------------------------------
bitcoin_block_data* get_current_block_from_server();
bitcoin_block_data* initialize_new_block(bitcoin_block_data* i_head_block);
EBoolType verify_block_is_relevant(bitcoin_block_data* i_block);
EBoolType mine_block(bitcoin_block_data* i_block, Uint i_miner_id, mqd_t mq_listen);
void send_block_to_server(bitcoin_block_data* i_block);
void print_mine_msg(bitcoin_block_data* i_block);
mqd_t set_miners_q_and_connect_srv(Uint i_miners_id, mqd_t *io_servers_mq);
mqd_t connect_server();
mqd_t init_miner_queue(Uint i_miners_id, mqd_t i_server_mq);

//---------------------------------------------------------------------------
//-----------------------Private Methods Implementations---------------------
//---------------------------------------------------------------------------
PUBLIC
bitcoin_block_data*
initialize_new_block(bitcoin_block_data* i_head_block)
{
	bitcoin_block_data* new_block = malloc(sizeof(bitcoin_block_data));
	new_block->height = i_head_block->height + 1;
	new_block->time_stamp = get_current_time_stamp();
	new_block->prev_hash = i_head_block->hash;
	new_block->difficulty = DIFFICULTY;
	new_block->nonce = PLACE_HOLDER_TILL_MINER_WILL_MINE;		// MINER responsibility
	new_block->relayed_by = PLACE_HOLDER_TILL_MINER_WILL_MINE;  // MINER responsibility
    new_block->hash = PLACE_HOLDER_TILL_MINER_WILL_MINE;		// MINER responsibility
    return new_block;
}

PRIVATE
EBoolType
mine_block(bitcoin_block_data* i_block, Uint i_miner_id, mqd_t mq_listen)
{
    i_block->relayed_by = i_miner_id;
    EBoolType block_relevant = TRUE;

    do
    {
        i_block->nonce = get_random();
        i_block->time_stamp = get_current_time_stamp();
        i_block->hash = create_hash_from_block(i_block);
        block_relevant = !check_for_new_msgs(mq_listen);
    }
    while (!check_difficulty(i_block->hash, DIFFICULTY) && block_relevant);

    if (block_relevant==TRUE)
    {
        print_mine_msg(i_block);
    }
    return block_relevant;  // Mine will succeed only if the block is relevant
}

PRIVATE
void
print_mine_msg(bitcoin_block_data* i_block)
{
    printf("Miner #%d: Mined a new block #%d, with the hash 0x%x\n", 
           i_block->relayed_by,
           i_block->height, 
           i_block->hash);
}

PRIVATE
mqd_t
set_miners_q_and_connect_srv(Uint i_miners_id, mqd_t *io_servers_mq)
{
    *io_servers_mq = connect_server();
    mqd_t miners_mq = init_miner_queue(i_miners_id, *io_servers_mq);

	return miners_mq;
}

PRIVATE
mqd_t
connect_server()
{
    mqd_t server_q_d;
    Q_PARAMS_T params_to_servers_q;
    Uint tries = 0; Uint max_tries = 3;
    Uint sleep_factor = 3;

    params_to_servers_q.conn_type = CONNECT;
    server_q_d = open_queue(MQ_SERVER_NAME, params_to_servers_q);

    while(server_q_d == -1 && tries < max_tries)
    {
        tries++;
        printf("Connection to server failed, retry in %d seconds (try %d/%d)\n", sleep_factor, tries, max_tries);
        sleep(sleep_factor);
        server_q_d = open_queue(MQ_SERVER_NAME, params_to_servers_q);
    }

    if(server_q_d == -1)
    {
        fprintf(stderr, "Server Unavailable, exiting...\n");
        perror("The error is:");
        exit(EXIT_FAILURE);
    }

    return server_q_d;
}

PRIVATE
mqd_t
init_miner_queue(Uint i_miners_id, mqd_t i_server_mq)
{
    char* miners_q_name;
    Q_PARAMS_T params_to_miners_q;
    INIT_MSG_DATA_T miners_details;
    mqd_t miners_mq;
    MSG_PACK_T *msg;
    /* Initialize miner's new Q attributes */
    miners_details.miners_id = i_miners_id;
    miners_q_name = set_miners_q_name(miners_details.miners_id);
    params_to_miners_q.conn_type = CREAT;
    params_to_miners_q.msg_pass_type = NON_BLOCK;
    printf("Miner ID = %d, queue name = %s\n",miners_details.miners_id, miners_q_name);

    /* Estabilish new Q for miner and sent MSG with connection details to Server, for it to connect*/
    msg = malloc(sizeof(MSG_PACK_T) + sizeof(INIT_MSG_DATA_T));
    msg->type = INIT;
    ((INIT_MSG_DATA_T*)msg->data)->miners_id = miners_details.miners_id;

    miners_mq = open_queue(miners_q_name, params_to_miners_q);

    if(miners_mq == -1)
    {
        fprintf(stderr, "miners_q Unavailable, exiting...\n");
        perror("The error is:");
        exit(EXIT_FAILURE);
    }

    msg_send(i_server_mq, (char*)msg);
    printf("Miner %d sent connection request on %s\n",miners_details.miners_id, miners_q_name);

    return miners_mq;
}

//---------------------------------------------------------------------------
//----------------------- Public Methods Implementations --------------------
//---------------------------------------------------------------------------
PUBLIC
int
main(int argc, char *argv[])
{		
    if (argc != 2)  { usage_err((Uint)argc, "name of miner's Q"); }
	
	bitcoin_block_data* curr_head_block = NULL;
    bitcoin_block_data* new_mined_block = NULL;
	MSG_PACK_T* rcvd_msg = NULL;
    MSG_PACK_T *response;
	Uint miner_id = (Uint)atoi(argv[1]);
	mqd_t servers_mq;
	EBoolType mine_succeed;
	
	mqd_t miners_bitcoin_mq = set_miners_q_and_connect_srv(miner_id, &servers_mq);
	
    while(1)
    {
		if((rcvd_msg = msg_rcv(miners_bitcoin_mq)) != NULL)
		{
			curr_head_block = malloc(sizeof(bitcoin_block_data));
			*curr_head_block = ((MINE_MSG_DATA_T*)rcvd_msg->data)->block_detailes;
            printf("Miner #%d received block:",miner_id);
            print_bitcoin_block_data(curr_head_block);
            printf("\n");
		}
		else
		{
			if(curr_head_block == NULL)
			{
			    printf("Wait for server...\n");
				sleep(1);	//To "slow down" busy waiting little bit.
				continue;
			}	
		}
		
		new_mined_block = initialize_new_block(curr_head_block);
        mine_succeed = mine_block(new_mined_block, miner_id, miners_bitcoin_mq);
        if (mine_succeed) {
            response = malloc(sizeof(MSG_PACK_T) + sizeof(MINE_MSG_DATA_T));
            response->type = MINE;
            ((MINE_MSG_DATA_T *) response->data)->block_detailes = *new_mined_block;
            msg_send(servers_mq, (char *) response);
            sleep(2);   //To "slow down" the mining litle bit.
        }
    }
    
	return 0;
}

/* ======================================================== */
/*   bitcoin_ipc_mq.h		      						    */
/* ======================================================== */
#ifndef BITCOIN_IPC_MQ_H
#define BITCOIN_IPC_MQ_H

//---------------------------------------------------------------------------
//----------------------------- Includes ------------------------------------
//---------------------------------------------------------------------------
#include "utility.h"
#include <errno.h>

//---------------------------------------------------------------------------
//------------------------ IPC MQ Definitions -------------------------------
//---------------------------------------------------------------------------
#define QUEUE_ATTR_INITIALIZER ((struct mq_attr){0, MQ_MAX_SIZE, MQ_MAX_MSG_SIZE, 0}) //{0}
#define MQ_SERVER_NAME             				"/server_mq"
#define MQ_MAX_SIZE       					10
#define MQ_MAX_MSG_SIZE     				200
#define MAX_NUM_OF_MINERS                   255
#define EMPTY_QUEUE                         0
#define CONCAT_POS					    	7
#define MAX_LEN_OF_MINER_ID				    3			// 0 - 999
#define MINER_Q_NAME_PATTERN			    "/miner__q"
#define MAX_LEN_OF_Q_NAME                   15

//---------------------------------------------------------------------------
//-------------------- IPC MQ Structures Definitions ------------------------
//---------------------------------------------------------------------------
typedef enum
{
	CREAT,
	CONNECT
} Q_CONNECTION_T;

typedef enum
{
	BLOCK,
	NON_BLOCK
} Q_MSG_PASS_T;

typedef struct q_params
{
	Q_CONNECTION_T conn_type;
	Q_MSG_PASS_T msg_pass_type;
} Q_PARAMS_T;

typedef enum
{
	INIT,
	MINE
} MSG_TYPE_T;

typedef struct msg_pack
{
	MSG_TYPE_T type;
	char data[];
} MSG_PACK_T;

typedef struct init_msg_data
{
	Uint miners_id;
} INIT_MSG_DATA_T;

typedef struct mine_msg_data
{
	bitcoin_block_data block_detailes;
} MINE_MSG_DATA_T;

//---------------------------------------------------------------------------
//---------------------- Public Method Prototypes ---------------------------
//---------------------------------------------------------------------------
mqd_t 
open_queue(const char* q_name, Q_PARAMS_T i_params);

EBoolType 
check_for_new_msgs(mqd_t queue_descriptor);

mqd_t 
join_miners_q(Uint i_miner_id_to_connect);

MSG_PACK_T* 
msg_rcv(mqd_t q_descriptor);

void 
msg_send(mqd_t q_descriptor, char* msg_to_send);

char* 
set_miners_q_name(Uint i_miners_id);

#endif // !BITCOIN_IPC_MQ_H
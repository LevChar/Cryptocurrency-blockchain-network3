/* ======================================================== */
/*   bitcoin_ipc_mq.c		      						    */
/* ======================================================== */

//---------------------------------------------------------------------------
//----------------------------- Includes ------------------------------------
//---------------------------------------------------------------------------
#include "../include/bitcoin_ipc_mq.h"

//---------------------------------------------------------------------------
//---------------------- Public Methods Implementations----------------------
//---------------------------------------------------------------------------
mqd_t open_queue(const char* q_name, Q_PARAMS_T i_params)
{
	mqd_t tmp_q;
	int flags = O_RDWR;
    
	/* initialize the queue attributes */
	struct mq_attr attr = QUEUE_ATTR_INITIALIZER;

    if(i_params.msg_pass_type == NON_BLOCK)
    {
        flags |= O_NONBLOCK;
    }

	if(i_params.conn_type == CREAT)
	{
		flags |= O_CREAT;
		if(mq_unlink(q_name) == -1)
		{

            if(errno != ENOENT)
            {
                fprintf(stderr, "mq_unlink failure in %s, exiting...\n", q_name);
                perror("The error is:");
    			exit(EXIT_FAILURE);
            }
		}

		tmp_q = mq_open(q_name, flags, S_IRWXU | S_IRWXG, &attr);
	}
	
	else if (i_params.conn_type == CONNECT)
	{
        tmp_q = mq_open(q_name, flags);
	}

	return tmp_q;
}

MSG_PACK_T* msg_rcv(mqd_t q_descriptor)
{
	ssize_t num_bytes_rcvd = 0;
	MSG_PACK_T* rcvd_msg = malloc(MQ_MAX_MSG_SIZE);
	num_bytes_rcvd = mq_receive(q_descriptor, (char*)rcvd_msg, MQ_MAX_MSG_SIZE, NULL);
	
	if (num_bytes_rcvd == -1)
	{
		if(errno == EAGAIN)
		{
			return NULL;
		}

		fprintf(stderr, "mq_receive failure in Q: %d, exiting...\n", q_descriptor);
		perror("The error is:");
		exit(EXIT_FAILURE);
	}
	
	return rcvd_msg;
}

void msg_send(mqd_t q_descriptor, char* msg_to_send)
{
	ssize_t num_bytes_sent = 0;	
	num_bytes_sent = mq_send(q_descriptor, msg_to_send, MQ_MAX_MSG_SIZE, 0);

	if (num_bytes_sent == -1)
	{
		fprintf(stderr, "mq_send failure on %d, errno: %d\n", q_descriptor, errno);
	}
}

EBoolType check_for_new_msgs(mqd_t queue_descriptor)
{
	struct mq_attr attr_of_queue = {0};		//used for check if there are msgs on the server
	mq_getattr(queue_descriptor, &attr_of_queue);

	return attr_of_queue.mq_curmsgs != EMPTY_QUEUE ? TRUE : FALSE;
}

mqd_t join_miners_q(Uint i_miner_id_to_connect)
{
	Q_PARAMS_T params_to_q;
	char* miners_q_name = malloc(MAX_LEN_OF_Q_NAME*sizeof(char));
    miners_q_name = set_miners_q_name(i_miner_id_to_connect);

	printf("Received connection request from miner id %d, queue name %s\n",
	i_miner_id_to_connect,
	miners_q_name);
	
	params_to_q.conn_type = CONNECT;
	params_to_q.msg_pass_type = NON_BLOCK;
	mqd_t tmp_q = open_queue(miners_q_name, params_to_q);
	
	if(tmp_q == -1)
	{
		fprintf(stderr, "mq_open failure of queue: %s, exiting...\n", miners_q_name);
        perror("The error is:");
		exit(EXIT_FAILURE);
	}
	
	return tmp_q;
}

PUBLIC
char*
set_miners_q_name(Uint i_miners_id)
{
	int pos = CONCAT_POS;
	char *int_string = malloc(MAX_LEN_OF_MINER_ID * sizeof(char));
	char *miner_q_name_prefix = MINER_Q_NAME_PATTERN;
	char *full_miner_q_name = malloc((strlen(int_string) + strlen(miner_q_name_prefix)) * sizeof(char));
	
	sprintf((char*)int_string, "%d", i_miners_id);
	strncpy(full_miner_q_name, miner_q_name_prefix, pos);
	full_miner_q_name[pos] = '\0';
	strcat(full_miner_q_name, int_string);
	strcat(full_miner_q_name, miner_q_name_prefix + pos);
	printf("%s\n", full_miner_q_name);
	free(int_string);
	
	return full_miner_q_name;
}

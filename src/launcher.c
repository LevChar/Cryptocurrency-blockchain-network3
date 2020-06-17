/* ======================================================== */
/*   launcher.c		        								*/
/* ======================================================== */

//---------------------------------------------------------------------------
//----------------------------- Includes ------------------------------------
//---------------------------------------------------------------------------
#include "../include/launcher.h"

//---------------------------------------------------------------------------
//---------------------- Private Methods Prototypes -------------------------
//---------------------------------------------------------------------------
void
launch(int i_amnt_of_miners);

void 
kill_miner(int i_child_pid);

//---------------------------------------------------------------------------
//-----------------------Private Methods Implementations---------------------
//---------------------------------------------------------------------------
PRIVATE
void 
kill_miner(int i_child_pid)
{
    kill(i_child_pid,SIGKILL);
}

PRIVATE
void
launch(int i_amnt_of_miners)
{
    pid_t s_pid, m_pid[i_amnt_of_miners];
    int returnStatus;    
    char *argv[3];

    /* Launch server process */
	s_pid = fork();

	if (s_pid == 0) //Server
	{
		char *argv[] = {"./server.out", 0};
		execv("./server.out", argv);
	}

    if (s_pid > 0) //Launcher
    {
	    /* Launch Miner processes */
        for(int i = 0; i < i_amnt_of_miners; i++)
        {
		    m_pid[i] = fork();
		    if(m_pid[i] == 0) //Miner
		    {
                argv[0] = "./miner.out";
                argv[1] = malloc(10);
                sprintf(argv[1], "%d", (i+1));
                argv[2] = 0;

			    execv("./miner.out", argv);
		    }
        }

        waitpid(s_pid, &returnStatus, 0); //wait for server to terminate
        for(int i = 0; i < i_amnt_of_miners; i++)
        {
            kill_miner(m_pid[i]);
        }
   } 
}

//---------------------------------------------------------------------------
//---------------------- Public Methods Implementations----------------------
//---------------------------------------------------------------------------
int 
main(int argc, char **argv)
{
    if(argc != 2)
    {
        usage_err((Uint)argc, "Amount of miners to launch");
    }

    launch((Uint)atoi(argv[1]));

    return 0;
}
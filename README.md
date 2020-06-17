# Cryptocurrency blockchain network
An application that illustrates a cryptocurrency blockchain network.
Based on client/server architecture with the help of POSIX message queues (MQ), 
in which the miners(clients) are mining blocks(calculate hashes) and send those 
blocks to a server that verifies and adds them to its blockchain.

## **Purpose:**
This software was written as part of assignment in the LINUX course. It demonstrates,
in a basic way, the blockchain mining process. It consists of variable number of mining 
processes and one server process.

Each one of the mining processes is responsible for mining new blocks
to the Blockchain, the server is responsible for validation	of miner's work.

In the process of creating this piece of software, there was a lot of practice
in writing multithreaded code, thread synchronization, etc.

## **Compilation & Execution:**
Download \ clone the whole repo & run make command from the 
folder where you downloaded. After the compilaton is done, there are several 
options to run the program:

1) Run the Server from one terminal using the command ./server.out and run variable number of miners
from different terminals using the command ./miner.out server_mq.

2)Run the server and N miners in the same terminal using the command ./launcher <N>

## **Compatibility:**
Program was compiled and tested in Linux (Mint) environment.

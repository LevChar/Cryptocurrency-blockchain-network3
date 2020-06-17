.PHONY: all clean

CC:= gcc 
CFLAGS:= -std=c99 -g -D_POSIX_C_SOURCE -Wall
LDFLAGS=-lm -lpthread -lz -lrt
SRCDIR:= src
SO_LINK_PARAMETERS:= -shared -fPIC

all: server.out miner.out launcher.out

server.out : lib_mq_ipc_manager.so

lib_mq_ipc_manager.so: $(SRCDIR)/linked_list.o $(SRCDIR)/bitcoin_ipc_mq.o $(SRCDIR)/utility.o $(SRCDIR)/bitcoin.o $(SRCDIR)/server.o
	 $(CC) $(SO_LINK_PARAMETERS) $^ $(LDFLAGS) -o $@ -lrt
	
%.out: $(SRCDIR)/%.o lib_mq_ipc_manager.so
	 $(CC) -Wl,-rpath,./ -o $@ $< lib_mq_ipc_manager.so -lrt
	
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	 $(CC) $(CFLAGS) -c -fPIC $< -o $@ -lrt

clean:
	 rm -f $(SRCDIR)/*.o *.so *.out

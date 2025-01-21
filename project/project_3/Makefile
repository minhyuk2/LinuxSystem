CC = gcc

HEADER = ssu_header
SYNC = ssu_sync
HELP = ssu_help
INIT = ssu_init
STRUCT = ssu_struct

$(SYNC) : $(SYNC).o $(HELP).o $(HEADER).o $(INIT).o $(STRUCT).o
	$(CC) -g -o $@ $^ -lcrypto

$(SYNC).o : $(SYNC).c
	$(CC) -c -o $@ $^ -lcrypto

$(HELP).o : $(HELP).c
	$(CC) -c -o $@ $^ -lcrypto

$(HEADER).o : $(HEADER).c
	$(CC) -c -o $@ $^ -lcrypto

$(INIT).o : $(INIT).c
	$(CC) -c -o $@ $^ -lcrypto

$(STRUCT).o : $(STRUCT).c
	$(CC) -c -o $@ $^ -lcrypto

clean :
	rm -rf $(SYNC)
	rm -rf *.o

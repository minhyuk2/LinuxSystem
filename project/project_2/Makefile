CC = gcc
HEADER = ssu_header
REPO = ssu_repo
HELP = help

$(REPO) : $(REPO).o $(HELP).o 
	$(CC) -o $(REPO) $(REPO).o $(HELP).o  -lcrypto

$(REPO).o : $(REPO).c 
	$(CC) -c -o $@ $(REPO).c -lcrypto

$(HELP).o : $(HELP).c
	$(CC) -c -o $@ $(HELP).c -lcrypto

clean :
	rm -rf $(REPO)
	rm -rf *.o
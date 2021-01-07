# CLIENT - gcc -o app2 main.c -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
# SERVER - gcc -o server server.c -lpthread `mysql_config --cflags --libs`
# change application name here (executable output name)
TARGETCLIENT=client
TARGETSERVER=server
PORT=9999
HOST=127.0.0.1


# compiler
CC=gcc

GTK=`pkg-config --cflags --libs gtk+-3.0` -export-dynamic
MYSQL=`mysql_config --cflags --libs`
JSON=-l json-c

all: clean server client runserver runclient

client: src/client/client.c
	gcc -o $(TARGETCLIENT) src/client/client.c -Wall $(GTK) ${JSON}
server: src/server/server.c
	gcc -o $(TARGETSERVER) src/server/server.c -lpthread $(MYSQL) ${JSON}
runclient: client
	./client $(HOST) $(PORT)
runserver: server
	./server $(PORT) &
clean:
	rm -f *.o $(TARGETCLIENT) $(TARGETSERVER)

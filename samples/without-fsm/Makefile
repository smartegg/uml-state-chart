all: client server
client:
	g++ echo_client.cpp -lboost_system  -o $@ -g
server:
	g++ echo_server.cpp  CircleBuf.cpp -lboost_system  -o $@ -g

.PHONY clean:
	rm client server

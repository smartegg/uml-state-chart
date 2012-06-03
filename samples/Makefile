all:
	make clean
	make client
	make server

client:
	g++ echo_client.cpp -lboost_system  -o $@ -g
server:
	g++ echo_server.cpp  CircleBuf.cpp fsm.a -lboost_system  -o $@ -g 
.PHONY clean:
	rm server client

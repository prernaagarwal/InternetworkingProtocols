clean:
	g++ server.cpp packets.cpp unrel_sendto.cpp -pthread -std=gnu++11 -lrt -Wl,-wrap=sendto -Wl,-wrap=recvfrom -o cs494rcp_server
	g++ client.cpp packets.cpp -o cs494rcp_client

server: server.cpp packets.cpp packets.h
	g++ server.cpp packets.cpp unrel_sendto.cpp -pthread -std=gnu++11 -lrt -Wl,-wrap=sendto -Wl,-wrap=recvfrom -o cs494rcp_server

client: client.cpp packets.cpp packets.h
	g++ client.cpp packets.cpp -o cs494rcp_client

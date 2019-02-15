clean:
	g++ server.cpp packets.cpp -o server
	g++ client.cpp packets.cpp -o client

server: server.cpp packets.cpp packets.h
	g++ server.cpp packets.cpp unrel_sendto.cpp -std=gnu++11 -lrt -Wl,-wrap=sendto -Wl,-wrap=recvfrom -o server

client: client.cpp packets.cpp packets.h
	g++ client.cpp packets.cpp -o client

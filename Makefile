clean:
	rm *.o 
	g++ server.cpp packets.cpp -o server
	g++ client.cpp packets.cpp -o client

server: server.cpp packets.cpp packets.h
	g++ server.cpp packets.cpp -o server

client: client.cpp packets.cpp packets.h 
	g++ client.cpp packets.cpp -o client

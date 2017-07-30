all:client_bin server_bin

serverobj=server.o my_socket.o
clientobj=client.o

client_bin:$(clientobj)
	g++ -o client_bin $(clientobj)
server_bin:$(serverobj)
	g++ -o server_bin $(serverobj) -lpthread

client.o:client.cpp
	g++ -c client.cpp
server.o:server.cpp my_socket.h
	g++ -c server.cpp
my_socket.o:my_socket.cpp my_socket.h
	g++ -c my_socket.cpp

clean:
	rm server.o server_bin
	rm client.o client_bin
	rm my_socket.o

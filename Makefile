all:client_bin server_bin

serverobj=server.o engine.o actor.o
clientobj=client.o

client_bin:$(clientobj)
	g++ -o client_bin $(clientobj)
server_bin:$(serverobj)
	g++ -o server_bin $(serverobj) -lpthread

client.o:client.cpp
	g++ -c client.cpp
server.o:server.cpp engine.h
	g++ -c server.cpp
engine.o:engine.cpp engine.h
	g++ -c engine.cpp
actor.o:actor.cpp actor.h
	g++ -c actor.cpp

clean:
	rm server.o server_bin
	rm client.o client_bin
	rm engine.o actor.o

all:client_bin server_bin

serverobj=server.o data_packet.o\
		  engine.o actor.o db.o

clientobj=client.o data_packet.o

client_bin:$(clientobj)
	g++ -o client_bin $(clientobj)

server_bin:$(serverobj)
	g++ -o server_bin $(serverobj) \
		-lpthread -lmysqlclient \
		-L/usr/lib/mysql

client.o:client.cpp
	g++ -c client.cpp

server.o:server.cpp engine.h
	g++ -c server.cpp
data_packet.o:data_packet.cpp data_packet.h
	g++ -c data_packet.cpp
engine.o:engine.cpp engine.h data_packet.h actor.h
	g++ -c engine.cpp
actor.o:actor.cpp actor.h data_packet.h
	g++ -c actor.cpp
db.o:db.cpp db.h
	g++ -c db.cpp -I/usr/include/mysql

clean:
	rm server.o server_bin \
		client.o client_bin \
		engine.o actor.o \
		data_packet.o db.o

CC = g++
CFLAGXX = -Wall -g3 -std=c++11
COMMON = common.cpp common.h
SERVER = server.cpp server.h
CLIENT = client.cpp client.h
P2PSERVER = p2pserver.cpp server.h common.h
P2PCLIENT = p2pclient.cpp client.h common.h
LIBS = -l pthread

all: p2pserver p2pclient 

common.o: ${COMMON}
	${CC} -o $@ ${CFLAGXX} -c $<
server.o: ${SERVER}
	${CC} -o $@ ${CFLAGXX} -c $<
client.o: ${CLIENT}
	${CC} -o $@ ${CFLAGXX} -c $<
p2pserver.o: ${P2PSERVER}
	${CC} -o $@ ${CFLAGXX} -c $<
p2pclient.o: ${P2PCLIENT}
	${CC} -o $@ ${CFLAGXX} -c $< 

p2pserver: p2pserver.o server.o common.o
	${CC} -o $@ p2pserver.o server.o common.o ${LIBS}
p2pclient: p2pclient.o client.o common.o
	${CC} -o $@ p2pclient.o client.o common.o ${LIBS}

.PYONY: clean
clean: 
	@echo "Removing binary and objects..."
	-rm -rf *.o p2pserver p2pclient core *.log


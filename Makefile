CC = g++
CFLAGXX = -Wall -g3 -std=c++11
INC = ./Inc
OBJDIR = ./Objs
COMMON = common.cpp ${INC}/common.h
SERVER = server.cpp ${INC}/server.h
CLIENT = client.cpp ${INC}/client.h
P2PSERVER = p2pserver.cpp ${INC}/server.h ${INC}/common.h
P2PCLIENT = p2pclient.cpp ${INC}/client.h ${INC}/common.h
LIBS = -l pthread

all: p2pserver p2pclient 

common.o: ${COMMON}
	${CC} -o ${OBJDIR}/$@ ${CFLAGXX} -c $< -I ${INC}
server.o: ${SERVER}
	${CC} -o ${OBJDIR}/$@ ${CFLAGXX} -c $< -I ${INC} 
client.o: ${CLIENT}
	${CC} -o ${OBJDIR}/$@ ${CFLAGXX} -c $< -I ${INC} 
p2pserver.o: ${P2PSERVER}
	${CC} -o ${OBJDIR}/$@ ${CFLAGXX} -c $< -I ${INC}
p2pclient.o: ${P2PCLIENT}
	${CC} -o ${OBJDIR}/$@ ${CFLAGXX} -c $< -I ${INC}

P2PSERVEROBJ = ${OBJDIR}/p2pserver.o ${OBJDIR}/server.o \
			   						 ${OBJDIR}/common.o
P2PCLIENTOBJ = ${OBJDIR}/p2pclient.o ${OBJDIR}/client.o \
			   						 ${OBJDIR}/common.o
p2pserver: ${P2PSERVEROBJ}
	${CC} -o $@ ${P2PSERVEROBJ} ${LIBS}
p2pclient: ${P2PCLIENTOBJ}
	${CC} -o $@ ${P2PCLIENTOBJ} ${LIBS}

.PYONY: clean
clean: 
	@echo "Removing binary and objects..."
	-rm -rf *.o p2pserver p2pclient core *.log


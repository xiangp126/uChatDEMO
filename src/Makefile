CC = g++
CFLAGXX = -Wall -g3 -std=c++11
SHELL := /bin/bash
INCDIR = ../inc
CONFIGDIR = ../config
OBJDIR = ../objs
BINDIR = ../bin
COMMON = common.cpp ${INCDIR}/common.h ${CONFIGDIR}/config.h
SERVER = server.cpp ${INCDIR}/server.h ${CONFIGDIR}/config.h
CLIENT = client.cpp ${INCDIR}/client.h ${CONFIGDIR}/config.h
P2PSERVER = p2pserver.cpp \
			${INCDIR}/server.h \
			${INCDIR}/common.h
P2PCLIENT = p2pclient.cpp \
			${INCDIR}/client.h \
			${INCDIR}/common.h

all: p2pserver p2pclient

common.o: ${COMMON}
	@mkdir -p ${OBJDIR}
	${CC} -o ${OBJDIR}/$@ ${CFLAGXX} -c $< -I ${INCDIR} -I ${CONFIGDIR}
server.o: ${SERVER}
	@mkdir -p ${OBJDIR}
	${CC} -o ${OBJDIR}/$@ ${CFLAGXX} -c $< -I ${INCDIR} -I ${CONFIGDIR}
client.o: ${CLIENT}
	@mkdir -p ${OBJDIR}
	${CC} -o ${OBJDIR}/$@ ${CFLAGXX} -c $< -I ${INCDIR} -I ${CONFIGDIR}
p2pserver.o: ${P2PSERVER}
	@mkdir -p ${OBJDIR}
	${CC} -o ${OBJDIR}/$@ ${CFLAGXX} -c $< -I ${INCDIR} -I ${CONFIGDIR}
p2pclient.o: ${P2PCLIENT}
	@mkdir -p ${OBJDIR}
	${CC} -o ${OBJDIR}/$@ ${CFLAGXX} -c $< -I ${INCDIR} -I ${CONFIGDIR}

P2PSERVEROBJ = ${OBJDIR}/p2pserver.o \
			   ${OBJDIR}/server.o \
			   ${OBJDIR}/common.o
P2PCLIENTOBJ = ${OBJDIR}/p2pclient.o \
			   ${OBJDIR}/client.o \
			   ${OBJDIR}/common.o

p2pserver: common.o server.o p2pserver.o
	@mkdir -p ${BINDIR}
	${CC} -o ${PWD}/${BINDIR}/$@ ${P2PSERVEROBJ} -l pthread
p2pclient: common.o client.o p2pclient.o
	@mkdir -p ${BINDIR}
	${CC} -o ${PWD}/${BINDIR}/$@ ${P2PCLIENTOBJ} -l pthread

.PYONY: clean
clean:
	@echo "Removing binary and objects..."
	-rm -f ${BINDIR}/* ${OBJDIR}/*

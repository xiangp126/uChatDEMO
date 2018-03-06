CC = g++
CFLAGXX = -Wall -g3 -std=c++11
SHELL := /bin/bash

SRCDIR = src
all:
	@echo "Entering into ${SRCDIR}"
	@cd ${SRCDIR} && ${MAKE}
	@echo "Leaving from ${SRCDIR}"

.PYONY: clean
clean:
	@echo -e "Entering into ${SRCDIR}"
	@cd ${SRCDIR} && ${MAKE} clean
	@echo "Leaving from ${SRCDIR}"
	-rm -f p2p_server.log

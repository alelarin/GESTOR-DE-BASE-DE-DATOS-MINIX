PROG=	minixDB
DIR =	src
SRCS=	$(DIR)/minixDB.cpp 
CC	=	g++
DEPS=	$(DIR)/fminixDB.h $(DIR)/headers.h

minixDB: $(SRCS) $(DEPS)
	@echo 'Compilando: $@'
	@echo 'Invocando: compilador de C++'
	$(CC) $(SRCS) -o $(PROG)
	@echo 'Finalizando la construccion del objetivo: $@'
	@echo ' '
	
.PHONY: clean

clean:
	rm minixDB
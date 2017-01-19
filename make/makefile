		##################################################
		#  __________________________________________    #
		# /\                                         \   #
		# \_| Make file for LSE				         |   #
		#   |                                        |   #
		#   |   _____________________________________|_  #
		#    \_/_______________________________________/ #
		##################################################
#########  MACROS  ##########

CC = gcc
RM = rm -f
BIN = ../bin
INC = ../include
MAKE = ../make
OBJ = ../obj
SRC = ../src
COV = -ftest-coverage -fprofile-arcs
TRACE = -DTRACE
######### COMPILING ##########
DEFAULT: client server

client: $(OBJ)/client.o $(OBJ)/c_funcs.o $(OBJ)/err_func.o
	$(CC) -o $(BIN)/client $(OBJ)/client.o $(OBJ)/c_funcs.o $(OBJ)/err_func.o -g -Wall $(COV)

$(OBJ)/client.o: $(SRC)/client.c
	$(CC) -c -o $(OBJ)/client.o $(SRC)/client.c -g -I$(INC) -Wall  $(COV) $(TRACE)
$(OBJ)/c_funcs.o: $(SRC)/c_funcs.c
	$(CC) -c -o $(OBJ)/c_funcs.o $(SRC)/c_funcs.c -g -I$(INC) -Wall $(COV) $(TRACE)


server: $(OBJ)/server.o $(OBJ)/s_funcs.o $(OBJ)/err_func.o
	$(CC) -o $(BIN)/server $(OBJ)/server.o $(OBJ)/s_funcs.o -lpthread $(OBJ)/err_func.o -g -Wall $(COV)

$(OBJ)/server.o: $(SRC)/server.c
	$(CC) -c -o $(OBJ)/server.o $(SRC)/server.c -g -I$(INC) -Wall  $(COV) $(TRACE)
$(OBJ)/s_funcs.o: $(SRC)/s_funcs.c
	$(CC) -c -o $(OBJ)/s_funcs.o $(SRC)/s_funcs.c -g -I$(INC) -Wall $(COV) $(TRACE)
$(OBJ)/err_func.o: $(SRC)/err_func.c
	$(CC) -c -o $(OBJ)/err_func.o $(SRC)/err_func.c -g -I$(INC) -Wall  $(COV) $(TRACE)


clean:
	$(RM) ../obj/*.o
	$(RM) ../bin/client
	$(RM) ../bin/server
	$(RM) ../obj/*.gcda
	$(RM) ../obj/*.gcno

	
server_run:
	../bin/server
client_run:
	../bin/client
	
	
splint:	 
	splint -maintype -warnposix ../src/client.c -I../include/
	splint -warnposix -nullpass -noret -mustfreefresh ../src/server.c -I ../include/
	splint -warnposix ../src/c_funcs.c -I ../include/  
	splint -warnposix -nestcomment -compdef -usedef ../src/s_funcs.c -I ../include/ 


server_val:
	valgrind ../bin/server
client_val:
	valgrind ../bin/client

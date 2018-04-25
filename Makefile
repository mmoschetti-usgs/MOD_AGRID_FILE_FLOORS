INST_DIR = $(HOME)/bin
BIN  = mod_agrid_file_floors

# need to include the math library when the math.h header file is used
#cflags = -O -lm
cflags = -O 
CFLAGS = $(DBG) $(cflags)  -Wall
FFLAGS = -O2 -ffixed-line-length-none 

FC = gfortran
CC = gcc

DBG =  -g
#FOBJS = mod_agrid_file_floors.o read_header.o Parse_Text.o
FOBJS = mod_agrid_file_floors.o Parse_Text.o

$(BIN) : $(FOBJS)
	$(CC) $(CFLAGS) $(FOBJS) -o $(BIN)

install :: $(BIN)
	install -s $(BIN) $(INST_DIR)

clean ::
	rm -f $(BIN) $(FOBJS)

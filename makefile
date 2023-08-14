# Project name, aloso name of executable file
PROJECT_NAME = 3_TCP_Socket_Exercise

# Variables
type = 1	# 1 is server, 0 is client
port_number = 8080
server_address = 127.0.0.1

# Folders's name
SRC_DIR = source
INC_DIR = include
OBJ_DIR = object
BIN_DIR = bin
OUT_DIR = output

# Get all folder in include folder
INC = $(wildcard $(INC_DIR)/*)
# Get all .c files in source folder
SRC = $(wildcard $(SRC_DIR)/*/*.c)
# Create list of object files from list of source files
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
# Location of executable file
EXE = $(BIN_DIR)/main
# List of folders holding object file
OBJ_FOLDER = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(wildcard $(SRC_DIR)/*))

# Compler
CC			=	gcc
# Remove command
RM			=	rm -r
# Flag for C compiler
CFLAGS		=	-Wall
# Flag to preprocessor
# "-I" + "[directory]"	=> Find header file in [directory]
#	Ex: -Iinclude		=> Find header file in folder name "include"
# CPPFLAGS	=	-Iinclude
CPPFLAGS	=	$(patsubst $(INC_DIR)/%, -I$(INC_DIR)/%, $(INC))
# Flag for linker
# "-L" + "directory"	=> Finc libraries in [directory]
#	Ex: -Llib			=> Find libraries in folder name "lib"
#LDFLAGS		:=	-Llib
#LDLIBS		:=	-lm

# Help rule
help:
	@echo ""
	@echo "========================= Server Command ========================="
	@echo
	@echo "Make for server using port 8080:"
	@echo "  $$ make server"
	@echo ""
	@echo "Make for server with port <port_number>:"
	@echo "  $$ make server port_number=<port_number>"
	@echo ""
	@echo "========================= Client Command ========================="
	@echo ""
	@echo "Make for client using port 8080 and server address 127.0.0.1"
	@echo "  $$ make client"
	@echo ""
	@echo "Make for client using port <port_number> and server address <server_address>"
	@echo "  $$ make server_address=<server_address> port_number=port_number"
	@echo "=================================================================="

# Rule to make for server
server: type = 1
server: clean $(EXE)
	@./$(EXE) $(port_number)

# Rule to make for client
client: type = 0
client: clean $(EXE)
	@./$(EXE) $(server_address) $(port_number)

# Rule create executable file and folder holding executable file
$(EXE): $(OBJ)
	@mkdir -p $(BIN_DIR)
	@gcc $^ -o $@

# Rule create object files and folder holding object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_FOLDER)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -DSERVER_OR_CLIENT=$(type) -c $< -o $@

# Clean object filess, executable files, and output files of program
clean:
	@$(RM) -f $(BIN_DIR)/* $(OBJ_DIR)/* $(OUT_DIR)/*

# Rule printing variable
print_var-%:
	@echo $($(subst print_var-,,$@))

.PHONY: help run build clean print server client

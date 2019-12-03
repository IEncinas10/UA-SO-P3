APP     := demo
CCFLAGS := -Wall -pedantic 
CC 		:= g++
MKDIR   := mkdir -p
SRC 	:= src
LIBS    := -lsfml-graphics -lsfml-window -lsfml-system -ltgui

demo: $(SRC)
	$(CC) $(SRC)/main.cpp -o $(APP) $(CCFLAGS) $(LIBS)


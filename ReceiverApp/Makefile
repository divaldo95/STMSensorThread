#makefile for serial project
#DE TTK - Villamosmérnök BSC - Baranyai Dávid

#Compiler:
CC = gcc

#Compiler flags:
CFLAGS = -lcurl
TARGET = serial_thread
SERIALHANDLER = serialhandler

all: $(TARGET) $(SERIALHANDLER)

$(SERIALHANDLER): $(SERIALHANDLER).c $(SERIALHANDLER).h
	$(CC) -c $(SERIALHANDLER).c -o $(SERIALHANDLER).o

$(TARGET): $(TARGET).c $(SERIALHANDLER)
	$(CC) -o $(TARGET) $(TARGET).c $(CFLAGS) $(SERIALHANDLER).o

clean:
	$(RM) $(TARGET)
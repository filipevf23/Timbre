TARGET = Timbre

SRCS = main.c audio.c observe.c timbre.c

CC = gcc

CFLAGS = -Wall

LIBS = -I"D:/SDL3/include" -L"D:/SDL3/lib" -lSDL3 -lm  -lws2_32 -latomic

# A regra principal: Como construir o programa
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)
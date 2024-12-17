# Derleyici ve bayrakları
CC = gcc
CFLAGS = -std=c11 -Wall -g

# Program adı
TARGET = adventure_game

# Kaynak dosyaları
SRCS = main.c

# Derleme hedefi
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Temizlik
clean:
	rm -f $(TARGET) *.o

# Programı çalıştır
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

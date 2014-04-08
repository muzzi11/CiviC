CC=g++ -std=c++11
AS=g++ -std=c++11
HEADERS=*.h
SOURCES=*.cpp
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=civcc

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AS) -o $(TARGET) $(OBJECTS)

$(OBJECTS): $(HEADERS) $(SOURCES)
	$(CC) -c $(SOURCES)

clean:
	rm -rf *.o $(TARGET)


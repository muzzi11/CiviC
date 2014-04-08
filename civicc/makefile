CC=g++ -std=c++11
AS=g++ -std=c++11
HEADERS=civicc/*.h
SOURCES=civicc/*.cpp
OBJECTS=*.o
TARGET=bin/civicc

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AS) -o $(TARGET) $(OBJECTS)

$(OBJECTS): $(HEADERS) $(SOURCES)
	$(CC) -c -I/civicc/ $(SOURCES)

clean:
	rm -rf *.o $(TARGET)


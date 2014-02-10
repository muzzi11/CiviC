CC=../civcc
AS=../civas
SOURCES=assignment1.cvc
OBJECTS=$(SOURCES:.cvc=.s)
TARGET=assignment1

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AS) -o $(TARGET) $(OBJECTS)

$(OBJECTS): $(SOURCES)
	$(CC) $(SOURCES)

run: $(TARGET)
	../civvm $(TARGET)

clean:
	rm -rf *.s $(TARGET)


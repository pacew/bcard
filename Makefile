CFLAGS = -g -Wall `pkg-config --cflags cairo freetype2 pangocairo`
LIBS = `pkg-config --libs cairo freetype2 pangocairo`

bcard: bcard.o
	$(CC) $(CFLAGS) -o bcard bcard.o $(LIBS)


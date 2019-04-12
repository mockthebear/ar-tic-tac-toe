IDIR =include/
SDIR =src/
CC=g++
CFLAGS=-I$(IDIR)

ODIR=obj

_DEPS = gamecontext.hpp imageprocessor.hpp lineprocessor.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o gamecontext.o lineprocessor.o imageprocessor.0
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -std=c++0x -c `pkg-config --cflags opencv` -o $@ $< $(CFLAGS) `pkg-config --libs opencv`

t1ipi: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) `pkg-config --libs opencv`

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 


CC=		g++ -std=c++11 -g

OBJS=		strtokenizer.o dataset.o utils.o model.o diskarray.o
MAIN=		lda
 
all:	$(OBJS) $(MAIN).cpp
	$(CC) -o $(MAIN) $(MAIN).cpp $(OBJS) -g 
#	strip $(MAIN)

strtokenizer.o:	strtokenizer.h strtokenizer.cpp
	$(CC) -c -o strtokenizer.o strtokenizer.cpp -g

dataset.o:	dataset.h dataset.cpp
	$(CC) -c -o dataset.o dataset.cpp -g

utils.o:	utils.h utils.cpp
	$(CC) -c -o utils.o utils.cpp -g

model.o:	model.h model.cpp
	$(CC) -c -o model.o model.cpp -g
diskArray.o:    diskarray.h diskarray.cpp
	$(CC) -c -o diskarray.o diskarray.cpp -g

test:
	

clean:
	rm $(OBJS) 
	rm $(MAIN)


include MyOS.Sockets
#include Posix.Sockets
#    include Windows.Sockets



all: sample camtest 


sample.o: sample.cpp surveyor.h  
	g++ -ggdb -Wall -c sample.cpp

camtest.o: camtest.cpp surveyor.h 
	g++ -ggdb -Wall -c camtest.cpp

surveyor.o: surveyor.cpp surveyor.h
	g++ -ggdb -Wall -c surveyor.cpp

sample: sample.o surveyor.o $(OS_SPEC)
	g++ -ggdb -Wall -o sample sample.o surveyor.o $(OS_SPEC) $(LIBS)

camtest: camtest.o surveyor.o $(OS_SPEC)
	g++ -ggdb -Wall -o camtest camtest.o surveyor.o $(OS_SPEC) $(LIBS)

posixSoc.o: posixSoc.cpp posixSoc.h
	g++ -ggdb -Wall -c posixSoc.cpp

winSoc.o: winSoc.cpp winSoc.h
	g++ -ggdb -Wall -c winSoc.cpp

clean:
	$(REMOVE) *.o
	$(REMOVE) sample$(BIN) camtest$(BIN)








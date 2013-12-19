dectvideo: control.o main.o amat.o kalman.o
	g++ -o dectvideo control.o main.o amat.o kalman.o `pkg-config opencv --cflags --libs`
main.o: main.cpp
	g++ -Wall -g -c main.cpp -o main.o `pkg-config opencv --cflags --libs`
control.o: control.cpp control.h amat.h
	g++ -Wall -g -c control.cpp -o control.o `pkg-config opencv --cflags --libs`
amat.o: amat.cpp amat.h
	g++ -Wall -g -c amat.cpp -o amat.o `pkg-config opencv --cflags --libs`
kalman.o: kalman.cpp kalman.h
	g++ -Wall -g -c kalman.cpp -o kalman.o `pkg-config opencv --cflags --libs`
clean:
	rm -f *.o *~ dectvideo

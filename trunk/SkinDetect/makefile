skindetect: control.o mask.o main.o
	g++ -o skindetect control.o mask.o main.o `pkg-config opencv --cflags --libs`
control.o: control.cpp
	g++ -Wall -g -c control.cpp -o control.o `pkg-config opencv --cflags --libs`
mask.o: mask.cpp
	g++ -Wall -g -c mask.cpp -o mask.o `pkg-config opencv --cflags --libs`
main.o: main.cpp
	g++ -Wall -g -c main.cpp -o main.o `pkg-config opencv --cflags --libs`
clean:
	rm -f *.o skindetect

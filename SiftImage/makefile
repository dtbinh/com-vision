siftimage: keypoint.o kpimage.o control.o main.o pairpoint.o
	g++ -o siftimage control.o main.o kpimage.o keypoint.o pairpoint.o `pkg-config opencv --cflags --libs`
main.o: main.cpp
	g++ -Wall -g -c main.cpp -o main.o `pkg-config opencv --cflags --libs`
control.o: control.cpp control.h kpimage.h
	g++ -Wall -g -c control.cpp -o control.o `pkg-config opencv --cflags --libs`
kpimage.o: kpimage.cpp kpimage.h keypoint.h
	g++ -Wall -g -c kpimage.cpp -o kpimage.o `pkg-config opencv --cflags --libs`
keypoint.o: keypoint.cpp keypoint.h
	g++ -Wall -g -c keypoint.cpp -o keypoint.o `pkg-config opencv --cflags --libs`
pairpoint.o: pairpoint.cpp pairpoint.h
	g++ -Wall -g -c pairpoint.cpp -o pairpoint.o `pkg-config opencv --cflags --libs`
clean:
	rm -f *.o *~ siftimage

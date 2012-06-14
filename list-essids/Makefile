
build:
	gcc -o list-essids -ldebconfclient list-essids.c -liw -Wall

clean:
	sudo rm list-essids
	sudo rm *\.dat

run: build
	sudo /usr/lib/cdebconf/debconf -fgtk ./list-essids


build:
	gcc -o list-essids -ldebconfclient list-essids.c -liw -Wall

clean:
	sudo rm -f list-essids
	sudo rm -f *\.dat

run: build
	sudo /usr/lib/cdebconf/debconf -fgtk ./list-essids


build:
	gcc -o set-essid-test -ldebconfclient set-essid.c -liw -Wall

clean:
	sudo rm -f set-essid-test
	sudo rm -f *\.dat

run: build
	sudo /usr/lib/cdebconf/debconf -fgtk ./set-essid-test


build:
	gcc -o set-essid -ldebconfclient set-essid.c -liw -Wall

clean:
	sudo rm -f set-essid
	sudo rm -f *\.dat

run: build
	sudo /usr/lib/cdebconf/debconf -fgtk ./set-essid

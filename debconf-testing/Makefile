
build:
	gcc -o cdebconf-test -ldebconfclient cdebconf-test.c -Wall

clean:
	rm cdebconf-test
	rm *\.dat

run: build
#	 DEBCONF_DEBUG=developer debconf ./cdebconf-test
	/usr/lib/cdebconf/debconf -fgtk ./cdebconf-test

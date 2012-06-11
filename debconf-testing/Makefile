
build:
	gcc -o cdebconf-test -ldebconfclient cdebconf-test.c -Wall

clean:
	rm cdebconf-test

run: build
	 DEBCONF_DEBUG=developer debconf ./cdebconf-test

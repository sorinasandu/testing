
build:
	gcc -o netcfg-test -ldebconfclient netcfg_templates_test.c -Wall

clean:
	rm -f netcfg-test
	rm -f *\.dat

run: build
	DEBCONF_DEBUG=developer /usr/lib/cdebconf/debconf -fgtk ./netcfg-test

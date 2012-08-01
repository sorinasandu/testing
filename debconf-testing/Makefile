
build:
	gcc -o netcfg_test -ldebconfclient netcfg_templates_test.c -Wall

clean:
	rm -f netcfg_test
	rm -f *\.dat

run: build
	DEBCONF_DEBUG=developer /usr/lib/cdebconf/debconf -fgtk ./netcfg_test

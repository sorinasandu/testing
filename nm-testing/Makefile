
build:
	gcc -o nm-test main.c nm-conf.c uuid4.c -liw -Wall -luuid

run: build
	./nm-test

test_uuid:
	gcc -o uuid4 uuid4.c
	./uuid4

clean:
	rm -f nm-test uuid4

build:
	gcc src/*.c -Iinclude -o easymake -s -Os

install:
	sudo cp easymake /usr/local/bin/easymake
	ln /usr/local/bin/easymake /usr/local/bin/ezmake

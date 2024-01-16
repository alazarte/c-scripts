programs=session open hello
cflags=-Wall -Wextra

.PHONY: run
build: $(programs)

$(programs):
	$(CC) $(cflags) -o $@ $@.c

.PHONY: clear
clear:
	rm $(programs)

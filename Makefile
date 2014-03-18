links: links.c
	clang $(CFLAGS) -o links -g links.c `pkg-config --libs --cflags gtk+-3.0` -lm

clean:
	rm links

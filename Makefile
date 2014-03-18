pi: pi.c
	clang $(CFLAGS) -o pi -g pi.c `pkg-config --libs --cflags gtk+-3.0` -lm

clean:
	rm pi

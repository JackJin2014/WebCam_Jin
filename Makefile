OBJECT = obj/main.o obj/webcam.o obj/query.o obj/yuv2jpeg.o obj/sock.o obj/th.o
CC	   = gcc
CFLAGS = -c
INC	   = -Iinclude

all:bin/main

bin/main: $(OBJECT)
	$(CC) $(OBJECT) -o $@ -ljpeg -lpthread
obj/main.o:src/main.c include/webcam.h 
	$(CC) $(CFLAGS) $(INC) $< -o $@
obj/webcam.o:src/webcam.c include/webcam.h
	$(CC) $(CFLAGS) $(INC) $< -o $@
obj/query.o:src/query.c include/query.h
	$(CC) $(CFLAGS) $(INC) $< -o $@
obj/yuv2jpeg.o:src/yuv2jpeg.c include/yuv2jpeg.h
	$(CC) $(CFLAGS) $(INC) $< -o $@
obj/sock.o:src/sock.c include/sock.h
	$(CC) $(CFLAGS) $(INC) $< -o $@
obj/th.o:src/th.c include/th.h
	$(CC) $(CFLAGS) $(INC) $< -o $@

clean:
	rm -rf $(OBJECT)
disclean: clean
	rm -rf bin/*


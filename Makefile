CFLAGS = -Wall `pkg-config fuse3 --cflags --libs`

SOURCES = roomfs.c ./src/engine/engine.c ./src/engine/event.c ./src/engine/item.c ./src/engine/room.c ./src/fs/dir.c ./src/fs/file.c

build: $(SOURCES)
	gcc $(SOURCES) $(CFLAGS) -o roomfs

run: build
	mkdir -p ./mountdir
	./roomfs ./mountdir

forerun: build
	mkdir -p ./mountdir
	./roomfs -f ./mountdir

clean:
	umount ./mountdir
	rm ./roomfs



















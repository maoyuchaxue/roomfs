CFLAGS = -Wall `pkg-config fuse3 --cflags --libs`

SOURCES = roomfs.c ./src/engine/engine.c ./src/engine/event.c ./src/engine/inventory.c ./src/engine/eval.c ./src/engine/description.c ./src/engine/reaction.c ./src/engine/global_state.c ./src/engine/item.c ./src/engine/room.c ./src/fs/dir.c ./src/fs/file.c ./src/fs/inventory_dir.c

ifndef RES_FILE
  RES_FILE=./res/demo.conf 
endif

build: $(SOURCES)
	gcc $(SOURCES) $(CFLAGS) -o roomfs

run: build
	mkdir -p ./mountdir
	./roomfs -o --path=$(RES_FILE) ./mountdir

forerun: build
	mkdir -p ./mountdir
	./roomfs -f --path=$(RES_FILE) ./mountdir

clean:
	umount ./mountdir
	rm ./roomfs



















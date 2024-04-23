CFLAGS = -Wall \
		-g \
		-std=c++11 \
         -I/usr/include/acl-lib/acl_cpp \
		 `mysql_config --cflags` \
		 -I./inc
LIBS   = -lpthread -lacl_all `mysql_config --libs`

RM     = rm -rf

OBJECTS_TRACKER = build/TNV/obj/common/util.cc.o \
	build/TNV/obj/tracker/cache.cc.o \
	build/TNV/obj/tracker/db.cc.o \
	build/TNV/obj/tracker/globals.cc.o \
	build/TNV/obj/tracker/server.cc.o \
	build/TNV/obj/tracker/service.cc.o \
	build/TNV/obj/tracker/status.cc.o

OBJECTS_STORAGE = build/TNV/obj/common/util.cc.o \
	build/TNV/obj/storage/cache.cc.o \
	build/TNV/obj/storage/db.cc.o \
	build/TNV/obj/storage/file.cc.o \
	build/TNV/obj/storage/globals.cc.o \
	build/TNV/obj/storage/id.cc.o \
	build/TNV/obj/storage/server.cc.o \
	build/TNV/obj/storage/service.cc.o  \
	build/TNV/obj/storage/tracker.cc.o  

TEST_TRACKER = build/TNV/tests/test_tracker.cc.o

TEST_STORAGE = build/TNV/tests/test_storage.cc.o
# 先生成.o目录

build/TNV/tests/%.cc.o: tests/%.cc
	mkdir -p ${@D}
	g++ ${CFLAGS} -o $@ -c $<

build/TNV/obj/%.cc.o: src/%.cc
	mkdir -p ${@D}
	g++ ${CFLAGS} -o $@ -c $<

# 再生成bin
bin/test_tracker.bin: ${TEST_TRACKER} ${OBJECTS_TRACKER}
	mkdir -p ${@D}
	g++ ${CFLAGS} -o $@ $^ ${LIBS}

bin/test_storage.bin: ${TEST_STORAGE} ${OBJECTS_STORAGE}
	mkdir -p ${@D}
	g++ ${CFLAGS} -o $@ $^ ${LIBS}

all: bin/test_storage.bin bin/test_tracker.bin

.PHONY: clean
clean:
	${RM} build/TNV bin/
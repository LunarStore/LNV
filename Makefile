CFLAGS = -Wall \
		-std=c++11 \
         -I/usr/include/acl-lib/acl_cpp \
		 `mysql_config --cflags` \
		 -I./inc
LIBS   = -lpthread -lacl_all `mysql_config --libs`

RM     = rm -rf

OBJECTS = build/TNV/obj/common/util.cc.o \
	build/TNV/obj/tracker/cache.cc.o \
	build/TNV/obj/tracker/db.cc.o \
	build/TNV/obj/tracker/globals.cc.o \
	build/TNV/obj/tracker/server.cc.o \
	build/TNV/obj/tracker/service.cc.o \
	build/TNV/obj/tracker/status.cc.o \

TESTS = build/TNV/tests/test_main.cc.o
# 先生成.o目录

build/TNV/tests/%.cc.o: tests/%.cc
	mkdir -p ${@D}
	g++ ${CFLAGS} -o $@ -c $<

build/TNV/obj/%.cc.o: src/%.cc
	mkdir -p ${@D}
	g++ ${CFLAGS} -o $@ -c $<

# 再生成bin
bin/test_main.bin: ${OBJECTS} ${TESTS}
	g++ ${CFLAGS} -o $@ $^ ${LIBS}


.PHONY: clean
clean:
	${RM} build/TNV bin/
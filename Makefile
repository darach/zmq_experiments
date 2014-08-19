CC	= gcc
LD	=
AR	= ar
OBJS	= zpush.o zpull.o zsub.o zpub.o
LIBS	= -lzmq -lczmq -lwjelement -lwjreader #-lpthreads
CFLAGS	= -Ideps/libzmq/include -Ideps/czmq/include -Ideps/wjelement/include -O2 -funroll-loops -g -std=c99 -pedantic -Wall -fPIC
LDFLAGS = -Ldeps/libzmq/lib -Ldeps/czmq/src/.libs -Ldeps/wjelement/src/wjelement -Ldeps/wjelement/src/wjreader/
ARFLAGS =

all: zpush zpull zpub zsub

zpush.o: zpush.c
	$(CC) $(CFLAGS) -c zpush.c

zpull.o: zpull.c
	$(CC) $(CFLAGS) -c zpull.c

zpub.o: zpub.c
	$(CC) $(CFLAGS) -c zpub.c

zsub.o: zsub.c
	$(CC) $(CFLAGS) -c zsub.c

clean		:
		cd deps/wjelement \
		make clean \
		cd ../..
		/bin/rm -f *.o
		/bin/rm -f zpush
		/bin/rm -f zpull
		/bin/rm -f zpub
		/bin/rm -f zsub

purge:  clean
		/bin/rm -rf deps

get-deps:
	@if [ ! -d deps ]; then \
        mkdir deps ; \
        echo "Dependencies missing. Charging." ; \
    fi ;
	@if [ -e deps/wjelement ]; then \
        echo "Dependency 'deps/wjelement' already bound." ; \
    else \
		echo Git cloning deps/wjelement ; \
		git clone https://github.com/netmail-open/wjelement deps/wjelement ; \
		echo Building deps/wjelement ; \
		cd deps/wjelement ; \
        cmake -Wno-dev . ; \
        make ; \
    fi ;
	@if [ -e deps/libzmq ]; then \
        echo "Dependency 'deps/libzmq' already bound." ; \
    else \
		echo Git cloning deps/libzmq; \
		git clone git://github.com/zeromq/libzmq deps/libzmq ; \
		echo Building deps/libzmq; \
		cd deps/libzmq; \
        ./autogen.sh ; \
        ./configure --prefix=`pwd`; \
        make ; \
        make install ; \
    fi
	@if [ -e deps/czmq ]; then \
        echo "Dependency 'deps/czmq' already bound." ; \
    else \
		echo Git cloning deps/czmq; \
		git clone git://github.com/zeromq/czmq deps/czmq ; \
		echo Building deps/czmq; \
		cd deps/czmq; \
        ./autogen.sh ; \
        ./configure --with-libzmq=`pwd`/../libzmq --with-libzmq-include-dir=`pwd`/../libzmq/include/ --with-libzmq-lib-dir=`pwd`/../libzmq/lib/ ; \
        make ; \
    fi

zsub: get-deps zsub.o
	@gcc $(LDFLAGS) $(LIBS) -o zsub zsub.o

zpub: get-deps zpub.o
	@gcc $(LDFLAGS) $(LIBS) -o zpub zpub.o

zpush: get-deps zpush.o
	@gcc $(LDFLAGS) $(LIBS) -o zpush zpush.o

zpull: get-deps zpull.o
	@gcc $(LDFLAGS) $(LIBS) -o zpull zpull.o

.PHONY: get-deps zpush zpull zpub zsub all

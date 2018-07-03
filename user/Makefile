INCLUDES = -I../include/

#CROSS_COMPILE := /usr/eldk/usr/bin/mips_4KC-
#CC                       := $(CROSS_COMPILE)gcc
#CFLAGS           := -O -G 0 -mno-abicalls -fno-builtin -Wa,-xgot -Wall -fPIC
#LD                       := $(CROSS_COMPILE)ld
# For embedding one program in another

USERLIB := printf.o \
                print.o \
                libos.o \
                fork.o \
                pgfault.o \
                syscall_lib.o \
                ipc.o \
                string.o \
                fd.o \
                pageref.o \
                file.o \
                pipe.o \
                fsipc.o \
                wait.o \
                spawn.o \
                console.o \
                fprintf.o

CFLAGS += -nostdlib -static

all: fktest.x fktest.b pingpong.x pingpong.b idle.x testarg.b testpipe.x testpiperace.x \
        icode.x init.b sh.b cat.b ls.b fstest.x fstest.b \
        $(USERLIB) entry.o syscall_wrap.o tree.x tree.b cd.x cd.b quit.x quit.b \
        rm.x rm.b mkdir.x mkdir.b mkfile.x mkfile.b ps.x ps.b checkdev.x checkdev.b \
        cp.x cp.b bash.x bash.b nano.x nano.b login.x login.b su.x su.b

%.x: %.b.c
        echo cc1 $<
        $(CC) $(CFLAGS) -c -o $@ $<

%.b.c: %.b
        echo create $@
        echo bintoc $* $< > $@~
        chmod +x ./bintoc
        ./bintoc $* $< > $@~ && mv -f $@~ $@
#       grep \. $@

%.b: entry.o syscall_wrap.o %.o $(USERLIB)
        echo ld $@
        $(LD) -o $@ $(LDFLAGS) -G 0 -static -n -nostdlib -T ./user.lds $^

%.o: %.c
        echo user1 cc $<
        $(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.o: %.S
        echo as $<
        $(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.o: lib.h

.PHONY: clean

clean:
        rm -rf *~ *.o *.b.c *.x *.b

include ../include.mk
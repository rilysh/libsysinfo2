PROG     = sysinfo_test
SRCS     = test.c
LDADD    = -L/usr/local/lib -I/usr/local/include -lkvm -lsysinfo
CFLAGS  += -ggdb

all:
	${CC} ${SRCS} ${CFLAGS} ${LDADD} -o ${PROG}

clean:
	rm ${PROG}

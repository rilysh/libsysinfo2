LIB		  = sysinfo
PREFIX           ?= /usr/local
LIBDIR		  = ${PREFIX}/lib
INCLUDEDIR 	  = ${PREFIX}/include/sys
MANDIR		  = ${PREFIX}/man/man3
SRCS              = sysinfo.c
CFLAGS	          = -Wall -Wextra -fPIC -I.

all:
	${CC} -c ${SRCS} ${CFLAGS} -o ${LIB}.o
	${CC} --shared ${CFLAGS} ${SRCS} -o lib${LIB}.so
	ar -rc lib${LIB}.a ${LIB}.o

install:
	@if [ -d ${INCLUDEDIR} ]; then else mkdir ${INCLUDEDIR}; fi
	@if [ -d ${MANDIR} ]; then else mkdir ${MANDIR}; fi
	install -c -m 0444 ${.CURDIR}/libsysinfo.a ${LIBDIR}
	install -c -m 0444 ${.CURDIR}/libsysinfo.so ${LIBDIR}
	install -c -m 0444 ${.CURDIR}/sysinfo.h ${INCLUDEDIR}
	install -c -m 0444 ${.CURDIR}/sysinfo.3 ${MANDIR}

uninstall:
	rm -f ${INCLUDEDIR}/sysinfo.h ${MANDIR}/sysinfo.3 \
	${LIBDIR}/libsysinfo.a ${LIBDIR}/libsysinfo.so

clean:
	rm -f libsysinfo.a libsysinfo.so sysinfo.o

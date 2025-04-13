# $APPASERVER_HOME/src_hydrology/makefile
# ---------------------------------------

CC=gcc
CFLAGS=-O3 -s -Wall -Wunused-parameter

LIBRARY=${APPASERVER_HOME}/library

APPASERVER_LIBRARY=							\
		${LIBRARY}/application.o				\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/application_log.o				\
		${LIBRARY}/application_constant.o			\
		${LIBRARY}/appaserver_parameter.o			\
		${LIBRARY}/document.o					\
		${LIBRARY}/javascript.o					\
		${LIBRARY}/String.o 					\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/security.o 					\
		${LIBRARY}/spool.o 					\
		${LIBRARY}/appaserver.o 				\
		${LIBRARY}/piece.o					\
		${LIBRARY}/date.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/column.o					\
		${LIBRARY}/float.o					\
		${LIBRARY}/timlib.o					\
		${LIBRARY}/julian.o					\
		${LIBRARY}/dictionary.o					\
		${LIBRARY}/hash_table.o					\
		${LIBRARY}/list.o

EXECUTABLES=	station_datatype_list

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f *.o ${EXECUTABLES}

station_datatype_list: station_datatype_list.c				\
	${APPASERVER_LIBRARY}						\
	station_datatype.o						\
	station.o							\
	datatype.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		station_datatype_list.c					\
		${APPASERVER_LIBRARY}					\
		station_datatype.o					\
		station.o						\
		datatype.o						\
		-o $@

station.o: station.c station.h
	${CC} ${CFLAGS} -I${LIBRARY} -c $<

datatype.o: datatype.c datatype.h
	${CC} ${CFLAGS} -I${LIBRARY} -c $<

station_datatype.o: station_datatype.c station_datatype.h
	${CC} ${CFLAGS} -I${LIBRARY} -c $<


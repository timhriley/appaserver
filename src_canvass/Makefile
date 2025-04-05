# -------------------------------------
# $APPASERVER_HOME/src_canvass/makefile
# -------------------------------------

CC=gcc
#WARN=-Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
#WARN=-Wall -Wextra -Wtype-limits
WARN=-Wall -Wextra -Wunused-parameter
#CFLAGS=-g
CFLAGS=-O3 -s ${WARN}

LIBRARY=${APPASERVER_HOME}/library

CORE_LIBRARY=								\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/application.o				\
		${LIBRARY}/String.o					\
		${LIBRARY}/piece.o					\
		${LIBRARY}/float.o					\
		${LIBRARY}/dictionary.o					\
		${LIBRARY}/column.o					\
		${LIBRARY}/hash_table.o					\
		${LIBRARY}/security.o					\
		${LIBRARY}/spool.o					\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/application_log.o				\
		${LIBRARY}/application_constant.o			\
		${LIBRARY}/appaserver_parameter.o			\
		${LIBRARY}/environ.o					\
		${LIBRARY}/date.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/julian.o					\
		${LIBRARY}/javascript.o					\
		${LIBRARY}/list.o

STANDARD_LIBRARY=							\
		${CORE_LIBRARY}						\
		${LIBRARY}/basename.o					\
		${LIBRARY}/aggregate_level.o				\
		${LIBRARY}/aggregate_statistic.o

APPASERVER_LIBRARY=							\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/date_convert.o				\
		${LIBRARY}/folder_attribute.o				\
		${LIBRARY}/insert.o					\
		${LIBRARY}/widget.o					\
		${LIBRARY}/sql.o					\
		${LIBRARY}/button.o					\
		${LIBRARY}/query.o					\
		${LIBRARY}/security_entity.o				\
		${LIBRARY}/entity.o					\
		${LIBRARY}/query_isa.o					\
		${LIBRARY}/appaserver_link.o				\
		${LIBRARY}/folder.o					\
		${LIBRARY}/relation.o					\
		${LIBRARY}/relation_mto1.o				\
		${LIBRARY}/relation_one2m.o				\
		${LIBRARY}/relation_join.o				\
		${LIBRARY}/process.o					\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/role_folder.o				\
		${LIBRARY}/folder_operation.o				\
		${LIBRARY}/operation.o

EXECUTABLES=								\
		canvass_street

all: 	${EXECUTABLES}

clean:
	/bin/rm -f ${EXECUTABLES}

touch:
	/usr/bin/touch *.c

canvass_street: canvass_street.c					\
	${LIBRARY}/waypoint.o						\
	${LIBRARY}/canvass_waypoint.o					\
	${LIBRARY}/distance.o
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		canvass_street.c 					\
		${LIBRARY}/waypoint.o					\
		${LIBRARY}/canvass_waypoint.o				\
		${LIBRARY}/distance.o					\
		-o canvass_street -lm


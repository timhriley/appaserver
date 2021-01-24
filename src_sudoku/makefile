# src_sudoku/makefile
# ---------------------------

CC=gcc
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

SOURCE_METRICS=.

LIBRARY=${APPASERVER_HOME}/library

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/String.o 					\
		${LIBRARY}/float.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hashtbl.o

EXECUTABLES=	sudoku_dvr						\
		post_sudoku

EXTERNAL_LIBRARYS=							\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/decode_html_post.o				\
		${LIBRARY}/html_table.o					\
		${LIBRARY}/fopen_path.o					\
		${LIBRARY}/stack.o					\
		${LIBRARY}/name_arg.o					\
		${LIBRARY}/basename.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/application_constants.o			\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/appaserver_library.o				\
		${LIBRARY}/prompt_recursive.o				\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/process_generic_output.o			\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/application.o				\
		${LIBRARY}/aggregate_statistic.o			\
		${LIBRARY}/dictionary_appaserver.o			\
		${LIBRARY}/aggregate_level.o				\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/element.o					\
		${LIBRARY}/form.o					\
		${LIBRARY}/query.o					\
		${LIBRARY}/session.o					\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/related_folder.o				\
		${LIBRARY}/operation_list.o				\
		${LIBRARY}/operation.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/insert_database.o				\
		${LIBRARY}/post2dictionary.o				\
		${LIBRARY}/update_database.o				\
		${LIBRARY}/process_parameter_list.o			\
		${LIBRARY}/appaserver_parameter_file.o			\
		${LIBRARY}/process.o					\
		${LIBRARY}/dictionary2file.o

SOURCE_LIBRARYS= 							\
		sudoku.o

all: 	${SOURCE_LIBRARYS} ${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f ${SOURCE_LIBRARYS} ${EXECUTABLES}
#	/bin/rm -f ${CGI_HOME}/post_sudoku

post_sudoku: post_sudoku.c ${EXTERNAL_LIBRARYS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBRARYS}					\
		post_sudoku.c						\
		-o post_sudoku
#	touch ${CGI_HOME}/post_sudoku
#	/bin/rm -f ${CGI_HOME}/post_sudoku
#	ln ${APPASERVER_LINK_FLAGS}					\
#	${APPASERVER_HOME}/src_sudoku/post_sudoku ${CGI_HOME}

sudoku_dvr: sudoku_dvr.c						\
	${SOURCE_LIBRARYS} ${EXTERNAL_LIBRARYS}
	${CC}    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBRARYS}					\
		${SOURCE_LIBRARYS}					\
		sudoku_dvr.c						\
		-o sudoku_dvr

sudoku.o: sudoku.c sudoku.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c sudoku.c


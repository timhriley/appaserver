# src_panther/makefile
# --------------------

CC=gcc
#CFLAGS=-g 
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

SOURCE_EVERGLADES=.

LIBRARY=${APPASERVER_HOME}/library

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/String.o 					\
		${LIBRARY}/float.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hash_table.o

EXECUTABLES=	load_observation_file					\
		extract_compare_date_columns				\
		panther_google_map_operation

EXTERNAL_LIBS=								\
		${LIBRARY}/aggregate_level.o 				\
		${LIBRARY}/aggregate_statistic.o			\
		${LIBRARY}/process_generic_output.o			\
		${LIBRARY}/decode_html_post.o				\
		${LIBRARY}/update_statement.o				\
		${LIBRARY}/fopen_path.o					\
		${LIBRARY}/name_arg.o					\
		${LIBRARY}/html_table.o					\
		${LIBRARY}/basename.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/application_constants.o			\
		${LIBRARY}/make_date_time_between_compatable.o		\
		${LIBRARY}/appaserver_library.o				\
		${LIBRARY}/prompt_recursive.o				\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/application.o				\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/security.o					\
		${LIBRARY}/element.o					\
		${LIBRARY}/form.o					\
		${LIBRARY}/query.o 					\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/validation_level.o				\
		${LIBRARY}/dictionary_appaserver.o			\
		${LIBRARY}/related_folder.o				\
		${LIBRARY}/vertical_new_button.o			\
		${LIBRARY}/relation.o					\
		${LIBRARY}/operation_list.o				\
		${LIBRARY}/operation.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/insert_database.o				\
		${LIBRARY}/update_database.o				\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_parameter_file.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/process.o					\
		${LIBRARY}/validation_form.o				\
		${LIBRARY}/expected_count.o				\
		${LIBRARY}/post_dictionary.o				\
		${LIBRARY}/dictionary2file.o				\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/session.o

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	rm -f *.o ${EXECUTABLES}

load_observation_file: load_observation_file.c				\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		load_observation_file.c					\
		-o load_observation_file

extract_compare_date_columns: extract_compare_date_columns.c		\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		extract_compare_date_columns.c				\
		-o extract_compare_date_columns

panther_google_map_operation: panther_google_map_operation.c		\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/google_map.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/google_map.o					\
		panther_google_map_operation.c				\
		-o panther_google_map_operation


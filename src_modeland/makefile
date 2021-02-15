# src_modeland/makefile
# -----------------------

CC=gcc
#CFLAGS=-g 
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

SOURCE_EVERGLADES=$APPASERVER_HOME/src_hydrology

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

EXECUTABLES=	get_aggregation_sum_yn

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
		${LIBRARY}/dictionary_appaserver.o			\
		${LIBRARY}/element.o					\
		${LIBRARY}/form.o					\
		${LIBRARY}/query.o 					\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/validation_level.o				\
		${LIBRARY}/related_folder.o				\
		${LIBRARY}/vertical_new_button.o			\
		${LIBRARY}/relation.o					\
		${LIBRARY}/operation_list.o				\
		${LIBRARY}/operation.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/insert_database.o				\
		${LIBRARY}/update_database.o				\
		${LIBRARY}/process_parameter_list.o			\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/appaserver_parameter_file.o			\
		${LIBRARY}/process.o					\
		${LIBRARY}/validation_form.o				\
		${LIBRARY}/expected_count.o				\
		${LIBRARY}/post2dictionary.o				\
		${LIBRARY}/dictionary2file.o				\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/session.o

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	rm -f *.o ${EXECUTABLES}

model_results_hydroperiod_annually:					\
	model_results_hydroperiod_annually.c				\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		model_results_hydroperiod_annually.c			\
		-o model_results_hydroperiod_annually

get_aggregation_sum_yn:							\
	get_aggregation_sum_yn.c					\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		get_aggregation_sum_yn.c				\
		-o get_aggregation_sum_yn


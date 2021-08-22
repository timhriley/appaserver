# $APPASERVER_HOME/src_coronavirus/makefile
# -----------------------------------------

CC=gcc
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

LIBRARY=${APPASERVER_HOME}/library

STANDARD_LIBRARY_OBJECT=						\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/String.o 					\
		${LIBRARY}/float.o 					\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hash_table.o

EXECUTABLES=	post_change_hospital					\
		post_change_patient_count

EXTERNAL_LIBRARY_OBJECT=						\
		${STANDARD_LIBRARY_OBJECT}				\
		${LIBRARY}/date_aggregate.o 				\
		${LIBRARY}/aggregate_level.o 				\
		${LIBRARY}/aggregate_statistic.o 			\
		${LIBRARY}/html_table.o 				\
		${LIBRARY}/prompt_recursive.o 				\
		${LIBRARY}/decode_html_post.o				\
		${LIBRARY}/appaserver_parameter_file.o			\
		${LIBRARY}/appaserver_library.o				\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/application_constants.o			\
		${LIBRARY}/process_generic_output.o			\
		${LIBRARY}/name_arg.o					\
		${LIBRARY}/basename.o					\
		${LIBRARY}/fopen_path.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/semaphore.o					\
		${LIBRARY}/application.o				\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/security.o					\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/related_folder.o				\
		${LIBRARY}/vertical_new_button.o			\
		${LIBRARY}/relation.o					\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/dictionary_appaserver.o			\
		${LIBRARY}/query.o					\
		${LIBRARY}/latex.o					\
		${LIBRARY}/process.o					\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/form.o					\
		${LIBRARY}/element.o					\
		${LIBRARY}/operation.o					\
		${LIBRARY}/role_folder.o				\
		${LIBRARY}/session.o					\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/insert_database.o				\
		${LIBRARY}/update_database.o				\
		${LIBRARY}/operation_list.o				\
		${LIBRARY}/validation_form.o				\
		${LIBRARY}/post2dictionary.o				\
		${LIBRARY}/dictionary2file.o

all: 	${EXTENRAL_LIBRARY_OBJECT}					\
	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	rm ${EXECUTABLES} *.o

post_change_hospital:							\
		post_change_hospital.c					\
		${EXTERNAL_LIBRARY_OBJECT} 				\
		hospital.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY_OBJECT}				\
		-I{$LIBRARY}						\
		post_change_hospital.c					\
		hospital.o						\
		-o post_change_hospital

post_change_patient_count:						\
		post_change_patient_count.c				\
		${EXTERNAL_LIBRARY_OBJECT} 				\
		hospital.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY_OBJECT}				\
		-I{$LIBRARY}						\
		post_change_patient_count.c				\
		hospital.o						\
		-o post_change_patient_count

hospital.o: hospital.c hospital.h
	${CC} -c -I${LIBRARY}						\
		hospital.c


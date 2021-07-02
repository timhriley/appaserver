# src_hydrology/reg_sched/makefile
# ---------------------------------

CC=gcc
#CFLAGS=-g 
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

LIBRARY=${APPASERVER_HOME}/library

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/list.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/String.o 					\
		${LIBRARY}/float.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hash_table.o

EXECUTABLES=	regulation_schedule_dvr					\
		wca2a_reg_sched						\
		WCA3A_reg_sched20020225					\
		WCA3A_reg_sched						\
		zone_array_print_dvr

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
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
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
	rm ${EXECUTABLES} *.o

regulation_schedule_dvr: regulation_schedule_dvr.c			\
	${EXTERNAL_LIBS} regulation_schedule.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		regulation_schedule.o					\
		regulation_schedule_dvr.c				\
		-o regulation_schedule_dvr

wca2a_reg_sched: wca2a_reg_sched.c					\
	${EXTERNAL_LIBS} regulation_schedule.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		regulation_schedule.o					\
		wca2a_reg_sched.c					\
		-o wca2a_reg_sched

WCA3A_reg_sched20020225: WCA3A_reg_sched20020225.c			\
	${EXTERNAL_LIBS} regulation_schedule.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		regulation_schedule.o					\
		WCA3A_reg_sched20020225.c				\
		-o WCA3A_reg_sched20020225

WCA3A_reg_sched: WCA3A_reg_sched.c					\
	${EXTERNAL_LIBS} regulation_schedule.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		regulation_schedule.o					\
		WCA3A_reg_sched.c					\
		-o WCA3A_reg_sched

zone_array_print_dvr: zone_array_print_dvr.c				\
	${EXTERNAL_LIBS} regulation_schedule.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		regulation_schedule.o					\
		zone_array_print_dvr.c					\
		-o zone_array_print_dvr

regulation_schedule.o: regulation_schedule.c regulation_schedule.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c regulation_schedule.c


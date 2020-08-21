# src_education/makefile
# ----------------------

CC=gcc
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

LIBRARY=${APPASERVER_HOME}/library
PREDICTIVEBOOKS_LIBRARY=${APPASERVER_HOME}/src_predictive

EDUCATION_CORE=								\
		offering.o						\
		registration.o						\
		enrollment.o						\
		course.o						\
		education.o						\
		deposit.o						\
		semester.o						\
		payment.o

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/String.o	 				\
		${LIBRARY}/hashtbl.o

EXTERNAL_LIBRARY=							\
		${STANDARD_LIBRARY}					\
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
		${LIBRARY}/semaphore.o					\
		${LIBRARY}/fopen_path.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/application.o				\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/process_parameter_list.o			\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/related_folder.o				\
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
		${LIBRARY}/frameset.o					\
		${LIBRARY}/insert_database.o				\
		${LIBRARY}/update_database.o				\
		${LIBRARY}/operation_list.o				\
		${LIBRARY}/validation_form.o				\
		${LIBRARY}/post2dictionary.o				\
		${LIBRARY}/dictionary2file.o

PREDICTIVEBOOKS_CORE= 							\
		${PREDICTIVEBOOKS_LIBRARY}/transaction.o		\
		${PREDICTIVEBOOKS_LIBRARY}/journal.o			\
		${PREDICTIVEBOOKS_LIBRARY}/element.o			\
		${PREDICTIVEBOOKS_LIBRARY}/entity.o			\
		${PREDICTIVEBOOKS_LIBRARY}/subclassification.o		\
		${PREDICTIVEBOOKS_LIBRARY}/account.o

EXECUTABLES=	post_change_offering

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f *.o ${EXECUTABLES}

post_change_offering: post_change_offering.c				\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		post_change_offering.c					\
		-o post_change_offering

# Education core
# -------------- 
offering.o: offering.c offering.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c offering.c

registration.o: registration.c registration.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c registration.c

enrollment.o: enrollment.c enrollment.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c enrollment.c

course.o: course.c course.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c course.c

payment.o: payment.c payment.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c payment.c

semester.o: semester.c semester.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c semester.c

education.o: education.c education.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c education.c

deposit.o: deposit.c deposit.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c deposit.c


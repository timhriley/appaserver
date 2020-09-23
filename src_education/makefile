# src_education/makefile
# ----------------------

CC=gcc
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall -Wunused-parameter

LIBRARY=${APPASERVER_HOME}/library
PREDICTIVEBOOKS_LIBRARY=${APPASERVER_HOME}/src_predictive

EDUCATION_CORE=								\
		offering.o						\
		registration.o						\
		enrollment.o						\
		course.o						\
		program.o						\
		program_payment.o					\
		education.o						\
		deposit.o						\
		semester.o						\
		payment.o						\
		student.o						\
		payment_item_title.o

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/float.o 					\
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
		${PREDICTIVEBOOKS_LIBRARY}/predictive.o			\
		${PREDICTIVEBOOKS_LIBRARY}/account.o			\
		${PREDICTIVEBOOKS_LIBRARY}/spreadsheet_upload_event.o	\
		${PREDICTIVEBOOKS_LIBRARY}/spreadsheet_column.o		\
		${PREDICTIVEBOOKS_LIBRARY}/spreadsheet.o		\
		${PREDICTIVEBOOKS_LIBRARY}/paypal.o			\
		${PREDICTIVEBOOKS_LIBRARY}/paypal_dataset.o

EXECUTABLES=	offering_trigger					\
		deposit_trigger						\
		registration_trigger					\
		enrollment_trigger					\
		payment_trigger						\
		program_payment_trigger					\
		paypal_upload

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f *.o ${EXECUTABLES}

offering_trigger: offering_trigger.c					\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		offering_trigger.c					\
		-o offering_trigger

enrollment_trigger: enrollment_trigger.c				\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		enrollment_trigger.c					\
		-o enrollment_trigger

payment_trigger: payment_trigger.c					\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		payment_trigger.c					\
		-o payment_trigger

program_payment_trigger: program_payment_trigger.c			\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		program_payment_trigger.c				\
		-o program_payment_trigger

deposit_trigger: deposit_trigger.c					\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		deposit_trigger.c					\
		-o deposit_trigger

registration_trigger: registration_trigger.c				\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		registration_trigger.c					\
		-o registration_trigger

paypal_upload: paypal_upload.c						\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		paypal_upload.c						\
		-o paypal_upload

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

program.o: program.c program.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c program.c

payment.o: payment.c payment.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c payment.c

program_payment.o: program_payment.c program_payment.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c program_payment.c

student.o: student.c student.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c student.c

payment_item_title.o: payment_item_title.c payment_item_title.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c payment_item_title.c

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


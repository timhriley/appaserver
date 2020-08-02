# src_education/makefile
# ----------------------

CC=gcc
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

LIBRARY=${APPASERVER_HOME}/library
PREDICTIVEBOOKS_LIBRARY=${APPASERVER_HOME}/src_predictive

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

SOURCE_LIBRARY_OBJECT= 							\
		${PREDICTIVEBOOKS_LIBRARY}/ledger.o			\
		${PREDICTIVEBOOKS_LIBRARY}/accrual.o			\
		${PREDICTIVEBOOKS_LIBRARY}/purchase.o			\
		${PREDICTIVEBOOKS_LIBRARY}/reoccurring.o		\
		${PREDICTIVEBOOKS_LIBRARY}/customer.o			\
		${PREDICTIVEBOOKS_LIBRARY}/inventory.o			\
		${PREDICTIVEBOOKS_LIBRARY}/inventory_purchase_return.o	\
		${PREDICTIVEBOOKS_LIBRARY}/inventory_sale_return.o	\
		${PREDICTIVEBOOKS_LIBRARY}/depreciation.o		\
		${PREDICTIVEBOOKS_LIBRARY}/fixed_asset.o		\
		${PREDICTIVEBOOKS_LIBRARY}/entity.o

EXECUTABLES=	post_change_offering

all: 	${SOURCE_LIBRARY_OBJECT} ${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f ${SOURCE_LIBRARY_OBJECT} ${EXECUTABLES}

post_change_offering: post_change_offering.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY_OBJECT}			\
		offering.o						\
		registration.o						\
		enrollment.o						\
		course.o						\
		payment.o
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY_OBJECT}				\
		post_change_offering.c					\
		offering.o						\
		registration.o						\
		enrollment.o						\
		course.o						\
		payment.o						\
		-o post_change_offering

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


# $APPASERVER_HOME/src_camp/makefile
# ----------------------------------

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
		${LIBRARY}/float.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hashtbl.o

EXECUTABLES=	camp_enrollment_update_transaction			\
		enrollment_payment_update_transaction			\
		generate_invoice

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
		${LIBRARY}/dictionary2file.o				\
		${PREDICTIVEBOOKS_LIBRARY}/ledger.o			\
		${PREDICTIVEBOOKS_LIBRARY}/accrual.o			\
		${PREDICTIVEBOOKS_LIBRARY}/purchase.o			\
		${PREDICTIVEBOOKS_LIBRARY}/depreciation.o		\
		${PREDICTIVEBOOKS_LIBRARY}/customer.o			\
		${PREDICTIVEBOOKS_LIBRARY}/inventory.o			\
		${PREDICTIVEBOOKS_LIBRARY}/inventory_purchase_return.o	\
		${PREDICTIVEBOOKS_LIBRARY}/inventory_sale_return.o	\
		${PREDICTIVEBOOKS_LIBRARY}/fixed_asset.o		\
		${PREDICTIVEBOOKS_LIBRARY}/entity.o

SOURCE_LIBRARY= 							\
		camp.o

all: 	${SOURCE_LIBRARY} ${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f ${SOURCE_LIBRARY} ${EXECUTABLES}

generate_invoice: generate_invoice.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		${LIBRARY}/latex_invoice.o				\
		${LIBRARY}/email.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		${LIBRARY}/latex_invoice.o				\
		${LIBRARY}/email.o					\
		generate_invoice.c					\
		-o generate_invoice

camp_enrollment_update_transaction:					\
	camp_enrollment_update_transaction.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		camp_enrollment_update_transaction.c			\
		-o camp_enrollment_update_transaction

enrollment_payment_update_transaction:					\
	enrollment_payment_update_transaction.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		enrollment_payment_update_transaction.c			\
		-o enrollment_payment_update_transaction

camp.o: camp.c camp.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		-c camp.c


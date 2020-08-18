# src_autorepair/makefile
# ---------------------------

CC=gcc
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

LIBRARY=${APPASERVER_HOME}/library
LEDGER_LIBRARY=${APPASERVER_HOME}/src_predictive

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hashtbl.o

EXTERNAL_LIBRARY=							\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/date_aggregate.o 				\
		${LIBRARY}/aggregate_level.o 				\
		${LIBRARY}/aggregate_statistic.o 			\
		${LIBRARY}/html_table.o 				\
		${LIBRARY}/decode_html_post.o				\
		${LIBRARY}/appaserver_parameter_file.o			\
		${LIBRARY}/appaserver_library.o				\
		${LIBRARY}/prompt_recursive.o				\
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
		${LIBRARY}/process_parameter_list.o			\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/related_folder.o				\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/dictionary_appaserver.o			\
		${LIBRARY}/query.o					\
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
		${LIBRARY}/latex.o					\
		${LEDGER_LIBRARY}/ledger.o				\
		${LEDGER_LIBRARY}/accrual.o				\
		${LEDGER_LIBRARY}/purchase.o				\
		${LEDGER_LIBRARY}/depreciation.o			\
		${LEDGER_LIBRARY}/customer.o				\
		${LEDGER_LIBRARY}/inventory.o				\
		${LEDGER_LIBRARY}/inventory_purchase_return.o		\
		${LEDGER_LIBRARY}/inventory_sale_return.o		\
		${LEDGER_LIBRARY}/fixed_asset.o				\
		${LEDGER_LIBRARY}/entity.o

EXECUTABLES=	generate_invoice

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	rm ${EXECUTABLES} *.o generate_workorder

generate_invoice: generate_invoice.c					\
	${EXTERNAL_LIBRARY} autorepair.o				\
	${LIBRARY}/latex_invoice.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${LEDGER_LIBRARY}		\
		${EXTERNAL_LIBRARY}					\
		${LIBRARY}/latex_invoice.o				\
		autorepair.o						\
		generate_invoice.c					\
		-o generate_invoice
	touch generate_workorder
	rm generate_workorder
	ln generate_invoice generate_workorder

autorepair.o: autorepair.h autorepair.c
	${CC}	${CFLAGS} -I${LIBRARY} -I${LEDGER_LIBRARY}		\
		-c autorepair.c

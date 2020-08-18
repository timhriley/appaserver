# src_communityband/makefile
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
		${LIBRARY}/sed.o 					\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hashtbl.o

EXECUTABLES=	post_capitolpops					\
		parse_contact_list					\
		build_capitolpops					\
		post_change_donation_account				\
		post_change_donation					\
		donor_receipt						\
		composition_venue_count_output				\
		composition_venue_count

EXTERNAL_LIBRARY=							\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/decode_html_post.o				\
		${LIBRARY}/html_table.o					\
		${LIBRARY}/fopen_path.o					\
		${LIBRARY}/name_arg.o					\
		${LIBRARY}/basename.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/appaserver_library.o				\
		${LIBRARY}/prompt_recursive.o				\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/process_generic_output.o			\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/semaphore.o					\
		${LIBRARY}/aggregate_statistic.o			\
		${LIBRARY}/aggregate_level.o				\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/application_constants.o			\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/dictionary_appaserver.o			\
		${LIBRARY}/latex.o					\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/application.o				\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/element.o					\
		${LIBRARY}/form.o					\
		${LIBRARY}/query.o					\
		${LIBRARY}/session.o					\
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
		${LIBRARY}/dictionary2file.o				\
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

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	rm -f ${EXECUTABLES} *.o
#	rm -f ${CGI_HOME}/post_capitolpops

post_capitolpops: post_capitolpops.c					\
	${EXTERNAL_LIBRARY} musician.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBRARY}					\
		post_capitolpops.c					\
		musician.o						\
		-o post_capitolpops
#	touch ${CGI_HOME}/post_capitolpops
#	rm -f ${CGI_HOME}/post_capitolpops
#	ln ${APPASERVER_LINK_FLAGS}					\
#	${APPASERVER_HOME}/src_capitolpops/post_capitolpops ${CGI_HOME}

build_capitolpops: build_capitolpops.c					\
	${EXTERNAL_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBRARY}					\
		build_capitolpops.c					\
		-o build_capitolpops

composition_venue_count_output: composition_venue_count_output.c	\
	${EXTERNAL_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBRARY}					\
		composition_venue_count_output.c			\
		-o composition_venue_count_output

parse_contact_list: parse_contact_list.c				\
	${EXTERNAL_LIBRARY} musician.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBRARY}					\
		musician.o						\
		parse_contact_list.c					\
		-o parse_contact_list

post_change_donation: post_change_donation.c				\
	${STANDARD_LIBRARY} ${LEDGER_LIBRARY} donation.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-I${LEDGER_LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBRARY}					\
		post_change_donation.c					\
		donation.o						\
		-o post_change_donation

post_change_donation_account: post_change_donation_account.c		\
	${STANDARD_LIBRARY} ${LEDGER_LIBRARY} donation.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-I${LEDGER_LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBRARY}					\
		post_change_donation_account.c				\
		donation.o						\
		-o post_change_donation_account

composition_venue_count: composition_venue_count.c			\
	${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		composition_venue_count.c				\
		-o composition_venue_count

donor_receipt: donor_receipt.c						\
	${EXTERNAL_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-I${LEDGER_LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBRARY}					\
		donor_receipt.c						\
		-o donor_receipt

musician.o: musician.h musician.c
	${CC} -c ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		musician.c

donation.o: donation.h donation.c
	${CC} -c ${CFLAGS} -I${LIBRARY}					\
		-I${LEDGER_LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		donation.c

dues_payment.o: dues_payment.h dues_payment.c
	${CC} -c ${CFLAGS} -I${LIBRARY}					\
		-I${LEDGER_LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		dues_payment.c


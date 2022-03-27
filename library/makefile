# $APPASERVER_HOME/library/makefile
# ---------------------------------

CC=gcc
#CFLAGS=-g
#WARN = -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall -Wunused-parameter

LIBRARY=${APPASERVER_HOME}/library
UTILITY=${APPASERVER_HOME}/utility

STANDARD_LIBRARY=						\
		date.o 						\
		julian.o 					\
		dictionary.o 					\
		list.o 						\
		list_usage.o 					\
		timlib.o 					\
		float.o 					\
		piece.o 					\
		column.o 					\
		date_convert.o 					\
		String.o 					\
		hash_table.o					\
		javascript.o					\
		security.o

APPASERVER_LIBRARY=						\
		aggregate_level.o 				\
		aggregate_statistic.o				\
		update_statement.o				\
		fopen_path.o					\
		name_arg.o					\
		html_table.o					\
		google_earth.o					\
		google_chart.o					\
		environ.o 					\
		html_invoice.o					\
		latex.o						\
		latex_invoice.o					\
		basename.o					\
		role.o						\
		role_folder.o					\
		role_operation.o				\
		token.o						\
		make_date_time_between_compatable.o		\
		populate_helper_process.o			\
		drillthru.o					\
		appaserver_library.o				\
		appaserver_error.o				\
		folder.o					\
		attribute.o					\
		date_aggregate.o				\
		process_generic.o				\
		folder_menu.o					\
		application.o					\
		appaserver_user.o				\
		security.o					\
		element.o					\
		sed.o						\
		form.o						\
		form_prompt_edit.o				\
		form_edit_table.o				\
		form_choose_isa.o				\
		form_choose_role.o				\
		radio.o						\
		button.o					\
		email.o						\
		query.o 					\
		stack.o 					\
		grace.o 					\
		frameset.o					\
		validation_level.o				\
		relation.o					\
		vertical_new.o					\
		operation.o					\
		document.o					\
		insert.o					\
		expected_count.o				\
		update.o					\
		delete.o					\
		start_stop_time.o				\
		appaserver_parameter.o				\
		appaserver_link.o				\
		process.o					\
		process_parameter.o				\
		queue.o						\
		post_dictionary.o				\
		period_wo_date.o				\
		dictionary2file.o				\
		application_constants.o				\
		query_statistic.o				\
		table.o						\
		row_security.o					\
		dictionary_separate.o				\
		scatterplot.o					\
		regression.o					\
		pivot_table.o					\
		generic_load.o					\
		statistics_weighted.o				\
		semaphore.o					\
		dollar.o					\
		array.o						\
		session.o					\
		foreign_attribute.o				\
		moving_average.o				\
		pair_one2m.o					\
		mysqldump.o					\
		insert_table.o					\
		prompt_edit.o					\
		prompt_insert.o					\
		edit_table.o					\
		menu.o						\
		choose_isa.o					\
		choose_role.o					\
		post_login.o					\
		post_choose_folder.o				\
		post_choose_process.o				\
		post_choose_role.o				\
		post_edit_table.o

all: 	${APPASERVER_LIBRARY} ${STANDARD_LIBRARY}

touch:
	/usr/bin/touch *.c

clean:
	rm -f *.[oe]

fork_control_dvr.e: fork_control_dvr.c fork_control.o
	${CC} ${CFLAGS} -I${LIBRARY} fork_control_dvr.c			\
		${STANDARD_LIBRARY}					\
		fork_control.o						\
	   -o fork_control_dvr.e

semaphore_dvr.e: semaphore_dvr.c semaphore.o
	${CC} ${CFLAGS} -I${LIBRARY} semaphore_dvr.c			\
		${STANDARD_LIBRARY}					\
		semaphore.o						\
	   -o semaphore_dvr.e

dollar_dvr.e: dollar.c dollar.o dollar_dvr.c
	${CC} ${CFLAGS} -I${LIBRARY} dollar_dvr.c			\
		${STANDARD_LIBRARY}					\
		dollar.o						\
	   -o dollar_dvr.e

covariance_dvr.e: covariance_dvr.c covariance.o
	${CC} ${CFLAGS} -I${LIBRARY} covariance_dvr.c			\
		${STANDARD_LIBRARY}					\
		covariance.o						\
	   -o covariance_dvr.e

fifo_dvr.e: fifo_dvr.c fifo.o
	${CC} ${CFLAGS} -I${LIBRARY} fifo_dvr.c				\
		${STANDARD_LIBRARY}					\
		fifo.o							\
	   -o fifo_dvr.e

pipe_dvr.e: pipe_dvr.c pipe.o
	${CC} ${CFLAGS} -I${LIBRARY} pipe_dvr.c				\
		${STANDARD_LIBRARY}					\
		pipe.o							\
	   -o pipe_dvr.e

google_map_dvr.e: google_map_dvr.c google_map.o environ.h application.h	\
		${STANDARD_LIBRARY}					\
		appaserver_error.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		google_map_dvr.c					\
		${STANDARD_LIBRARY}					\
		appaserver_error.o					\
		environ.o						\
		application.o						\
		appaserver_parameter.o					\
		fopen_path.o						\
		application_constants.o					\
		google_map.o						\
	   -o google_map_dvr.e

google_earth_dvr.e: google_earth_dvr.c					\
		google_earth.o 		 				\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}
	${CC} ${CFLAGS} -I${LIBRARY}					\
		google_earth_dvr.c					\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		google_earth.o						\
	   -o google_earth_dvr.e

histogram_dvr.e: histogram_dvr.c histogram.o
	${CC} ${CFLAGS} -I${LIBRARY} histogram_dvr.c			\
		timlib.o 						\
		float.o 						\
		date.o 							\
		julian.o 						\
		piece.o 						\
		list.o 							\
		histogram.o 						\
		dictionary.o 						\
		list_usage.o 						\
		date_convert.o 						\
		column.o 						\
		hash_table.o						\
	   -o histogram_dvr.e

time2day_percentage_dvr.e: time2day_percentage_dvr.c hash_table.o
	${CC} ${CFLAGS} -I${LIBRARY} time2day_percentage_dvr.c		\
		timlib.o 						\
		float.o 						\
		date.o 							\
		julian.o 						\
		piece.o 						\
		list.o 							\
		list_usage.o 						\
		date_convert.o 						\
		column.o 						\
		dictionary.o 						\
		hash_table.o 						\
	   -o time2day_percentage_dvr.e

julian_dvr.e: julian_dvr.c julian.o hash_table.o
	${CC} ${CFLAGS} -I${LIBRARY} julian_dvr.c			\
		column.o 						\
		date.o 							\
		timlib.o 						\
		float.o 						\
		piece.o 						\
		list.o 							\
		list_usage.o 						\
		date_convert.o 						\
		dictionary.o 						\
		hash_table.o 						\
		julian.o						\
	   -o julian_dvr.e

sed_dvr.e: sed_dvr.c sed.o
	${CC} ${CFLAGS} -I${LIBRARY} sed_dvr.c				\
		column.o 						\
		date.o 							\
		timlib.o 						\
		float.o 						\
		piece.o 						\
		list.o 							\
		list_usage.o 						\
		date_convert.o 						\
		dictionary.o 						\
		hash_table.o 						\
		julian.o						\
		sed.o							\
	   -o sed_dvr.e

linear_interpolate_dvr.e:
		linear_interpolate_dvr.c				\
		timlib.o						\
		hash_table.o						\
		float.o
	${CC} ${CFLAGS} linear_interpolate_dvr.c 			\
		column.o						\
		date.o							\
		julian.o						\
		timlib.o						\
		float.o							\
		list.o							\
		list_usage.o						\
		date_convert.o 						\
		dictionary.o						\
		hash_table.o						\
		piece.o							\
	   -o linear_interpolate_dvr.e

regression_dvr.e: regression_dvr.c					\
		${STANDARD_LIBRARY}
	${CC} ${CFLAGS} -I${LIBRARY} regression_dvr.c			\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
	   -o regression_dvr.e

google_chart_dvr.e: google_chart_dvr.c					\
	google_chart.o
	${CC} ${CFLAGS} -I${LIBRARY} google_chart_dvr.c			\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		google_chart.o						\
	   -o google_chart_dvr.e

html_table_dvr.e: html_table_dvr.c
	${CC} ${CFLAGS} -I${LIBRARY} html_table_dvr.c			\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
	   -o html_table_dvr.e

date_convert_dvr.e: date_convert_dvr.c date_convert.o
	${CC} ${CFLAGS} -I${LIBRARY} date_convert_dvr.c			\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
	   -o date_convert_dvr.e

parse_report_dvr.e: parse_report_dvr.c parse_report.o hash_table.o
	${CC} ${CFLAGS} -I${LIBRARY} parse_report_dvr.c			\
		list.o 							\
		date.o 							\
		julian.o 						\
		document.o 						\
		application.o 						\
		list_usage.o						\
		date_convert.o 						\
		timlib.o						\
		float.o							\
		environ.o						\
		dictionary.o						\
		fopen_path.o						\
		column.o						\
		hash_table.o						\
		piece.o							\
		appaserver_parameter.o					\
		parse_report.o						\
	   -o parse_report_dvr.e
	touch ${UTILITY}/parse_report
	rm ${UTILITY}/parse_report
	ln parse_report_dvr.e ${UTILITY}/parse_report

html_table.o: html_table.c html_table.h
	${CC} ${CFLAGS} -c html_table.c

html_invoice.o: html_invoice.c html_invoice.h
	${CC} ${CFLAGS} -c html_invoice.c

latex.o: latex.c latex.h
	${CC} ${CFLAGS} -c latex.c

latex_invoice.o: latex_invoice.c latex_invoice.h
	${CC} ${CFLAGS} -c latex_invoice.c

grace.o: grace.c grace.h
	${CC} ${CFLAGS} -I${LIBRARY} -c grace.c

pair_one2m.o: pair_one2m.c pair_one2m.h
	${CC} ${CFLAGS} -c pair_one2m.c

expected_count.o: expected_count.c expected_count.h julian.o
	${CC} ${CFLAGS} -c expected_count.c

julian.o: julian.c julian.h
	${CC} ${CFLAGS} -c julian.c

parse_insert_statement.o: parse_insert_statement.c parse_insert_statement.h
	${CC} ${CFLAGS} -c parse_insert_statement.c

start_stop_time.o: start_stop_time.c start_stop_time.h
	${CC} ${CFLAGS} -c start_stop_time.c

token_dvr.e: token_dvr.c token.o
	${CC} ${CFLAGS} token_dvr.c token.o				\
	   -o token_dvr.e

token.o: token.c token.h
	${CC} ${CFLAGS} -c token.c

environ_dvr.e:	environ_dvr.c						\
		environ.o						\
		piece.o							\
		fopen_path.o						\
		hash_table.o						\
		environ.o
	${CC} ${CFLAGS} -I${LIBRARY} environ_dvr.c 			\
		environ.o timlib.o 					\
		float.o							\
		piece.o							\
		list.o							\
		fopen_path.o						\
		dictionary.o						\
		list_usage.o						\
		column.o						\
		hash_table.o						\
		date_convert.o 						\
		date.o							\
		julian.o						\
	   -o environ_dvr.e

environ.o: environ.c environ.h timlib.o
	${CC} ${CFLAGS} -I${LIBRARY} -c environ.c

maskdvr.e: maskdvr.c mask.o list.o piece.o basename.o
	${CC} ${CFLAGS} maskdvr.c mask.o -o maskdvr.e

mask.o: mask.c mask.h
	${CC} ${CFLAGS} -c mask.c

reportdvr.e: reportdvr.c report.o hash_table.o
	${CC} ${CFLAGS} reportdvr.c 		\
			piece.o 		\
			date.o 			\
			julian.o 		\
			list.o 			\
			report.o 		\
			using.o 		\
			dictionary.o 		\
			hash_table.o 		\
			list_usage.o 		\
			date_convert.o 		\
			column.o 		\
			timlib.o 		\
			float.o 		\
			-o reportdvr.e


name_dvr.e: name_dvr.c name_arg.o list.o piece.o basename.o hash_table.o
	${CC} ${CFLAGS} name_dvr.c 		\
			piece.o 		\
			date.o 			\
			julian.o 		\
			basename.o 		\
			name_arg.o 		\
			dictionary.o 		\
			hash_table.o 		\
			list_usage.o 		\
			date_convert.o 		\
			column.o 		\
			list.o 			\
			timlib.o 		\
			float.o 		\
			-o name_dvr.e

name_arg.o: name_arg.c name_arg.h
	${CC} ${CFLAGS} -c name_arg.c


date_dvr.e: date_dvr.c					\
		${STANDARD_LIBRARY}			\
		${APPASERVER_LIBRARY}
	${CC} ${CFLAGS} date_dvr.c 			\
			${STANDARD_LIBRARY}		\
			${APPASERVER_LIBRARY}		\
			-o date_dvr.e

date.o: date.c date.h
	${CC} ${CFLAGS} -c date.c

basename.o: basename.c
	${CC} ${CFLAGS} -c basename.c

piece_dvr.e: piece_dvr.c				\
		piece.o					\
		${STANDARD_LIBRARY}			\
		${APPASERVER_LIBRARY}
	${CC} ${CFLAGS} piece_dvr.c			\
		${STANDARD_LIBRARY}			\
		${APPASERVER_LIBRARY}			\
	-o piece_dvr.e

piece.o: piece.c piece.h
	${CC} ${CFLAGS} -c piece.c

stack.o: stack.c stack.h
	${CC} ${CFLAGS} -c stack.c

fifo.o: fifo.c fifo.h
	${CC} ${CFLAGS} -c fifo.c

column.o: column.c column.h
	${CC} ${CFLAGS} -c column.c

zip_code_dist_dvr.e: zip_code_dist_dvr.c		\
	zip_code_distance.o bsrch.o timlib.o rnd_io.o	\
	piece.o hash_table.o
	${CC} ${CFLAGS} dist_dvr.c 			\
		piece.o 				\
		zip_code_distance.o			\
		list.o 					\
		date.o 					\
		julian.o 				\
		bsrch.o 				\
		list_usage.o 				\
		date_convert.o				\
		dictionary.o 				\
		hash_table.o 				\
		column.o 				\
		timlib.o 				\
		float.o 				\
		rnd_io.o 				\
		-o zip_code_dist_dvr.e -lm

distance.o: distance.c distance.h
	${CC} ${CFLAGS} -c distance.c

zip_code_distance.o: zip_code_distance.c zip_code_distance.h
	${CC} ${CFLAGS} -c zip_code_distance.c

moving_average_dvr.e: moving_average_dvr.c moving_average.o
	${CC} ${CFLAGS} -I${LIBRARY} moving_average_dvr.c		\
		timlib.o 						\
		float.o 						\
		date.o 							\
		julian.o 						\
		piece.o 						\
		list.o 							\
		moving_average.o 					\
		dictionary.o 						\
		list_usage.o 						\
		date_convert.o 						\
		column.o 						\
		hash_table.o						\
	   -o moving_average_dvr.e

expected_count_dvr.e: expected_count_dvr.c expected_count.o
	${CC} ${CFLAGS} -I${LIBRARY} expected_count_dvr.c		\
		${STANDARD_LIBRARY}					\
		timlib.o 						\
		float.o 						\
		date.o 							\
		julian.o 						\
		piece.o 						\
		list.o 							\
		expected_count.o 					\
		dictionary.o 						\
		list_usage.o 						\
		date_convert.o 						\
		column.o 						\
		hash_table.o						\
	   -o expected_count_dvr.e

moving_average.o: moving_average.c moving_average.h
	${CC} ${CFLAGS} ${CFLAGS} -c moving_average.c

queue_dvr.e: queue_dvr.c queue.o
	${CC} ${CFLAGS} ${CFLAGS} queue_dvr.c queue.o			\
		-o queue_dvr.e

queue.o: queue.c queue.h
	${CC} ${CFLAGS} ${CFLAGS} -c queue.c

list_dvr.e: list_dvr.c list.o dictionary.o list_usage.o	\
		column.o hash_table.o
	${CC} ${CFLAGS} list_dvr.c 			\
			piece.o 			\
			timlib.o 			\
			float.o				\
			date.o 				\
			julian.o 			\
			dictionary.o 			\
			list_usage.o 			\
			date_convert.o 			\
			column.o 			\
			hash_table.o 			\
			list.o 				\
			-o list_dvr.e

list.o: list.c list.h
	${CC} ${CFLAGS} -c list.c

list_usage.o: list_usage.c list.h
	${CC} ${CFLAGS} -c list_usage.c

dictionary_dvr.e: dictionary_dvr.c dictionary.o hash_table.o
	${CC} ${CFLAGS} dictionary_dvr.c 		\
			dictionary.o 			\
			list_usage.o 			\
			date_convert.o 			\
			column.o 			\
			date.o	 			\
			julian.o 			\
			hash_table.o 			\
			piece.o 			\
			timlib.o 			\
			float.o 			\
			list.o 				\
			-o dictionary_dvr.e

hashdvr.e: hashdvr.c hash_table.o
	${CC} ${CFLAGS} hashdvr.c 			\
			piece.o 			\
			date.o 				\
			julian.o 			\
			timlib.o 			\
			hash_table.o 			\
			dictionary.o 			\
			list_usage.o 			\
			date_convert.o 			\
			column.o 			\
			list.o 				\
			-o hashdvr.e

dictionary.o: dictionary.c dictionary.h hash_table.o
	${CC} ${CFLAGS} -c dictionary.c

hash_table.o: hash_table.c hash_table.h
	${CC} ${CFLAGS} -c hash_table.c

cc_dvr.e: cc_dvr.c cc_chk.o
	${CC} ${CFLAGS} cc_dvr.c cc_chk.o -o cc_dvr.e

cc_chk.o: cc_chk.c
	${CC} ${CFLAGS} -c cc_chk.c

timlib.o: timlib.c timlib.h
	${CC} -c $(CFLAGS) timlib.c

compbear.e: compbear.c
	${CC} ${CFLAGS} -lm compbear.c -o compbear.e

std_dvr.e: std_dvr.c std_addr.o 
	${CC} ${CFLAGS} std_dvr.c std_addr.o  -o std_dvr.e

std_addr.o: std_addr.c std_addr.h
	${CC} ${CFLAGS} -c std_addr.c

using.o: using.c
	${CC} ${CFLAGS} -c using.c

bsrchdvr.e: bsrchdvr.c bsrch.o rnd_io.o hash_table.o
	${CC} ${CFLAGS} bsrchdvr.c 					\
		bsrch.o rnd_io.o timlib.o piece.o list.o		\
		list_usage.o dictionary.o hash_table.o column.o		\
		date_convert.o 						\
		date.o julian.o						\
	-o bsrchdvr.e

bsrch.o: bsrch.c bsrch.h
	${CC} ${CFLAGS} -c bsrch.c

date_aggregate.o: date_aggregate.c date_aggregate.h
	${CC} ${CFLAGS} -c date_aggregate.c

rnd_io.o: rnd_io.c rnd_io.h
	${CC} ${CFLAGS} -c rnd_io.c

julian_time_double_list.o: julian_time_double_list.c julian_time_double_list.h
	${CC} ${CFLAGS} -c julian_time_double_list.c

timlib_dvr.e: timlib_dvr.c timlib.o list.o piece.o hash_table.o	\
		date.o julian.o dictionary.o list_usage.o	\
		column.o
	${CC} ${CFLAGS} timlib_dvr.c 				\
			piece.o 				\
			date.o 					\
			julian.o 				\
			list.o 					\
			dictionary.o 				\
			list_usage.o 				\
			column.o 				\
			hash_table.o 				\
			timlib.o 				\
			date_convert.o 				\
			-o timlib_dvr.e

fopen_path_dvr.e: fopen_path_dvr.c fopen_path.o hash_table.o	\
	environ.o
	${CC} ${CFLAGS} fopen_path_dvr.c 			\
			environ.o 				\
			piece.o 				\
			date.o 					\
			julian.o 				\
			timlib.o 				\
			list.o 					\
			list_usage.o 				\
			date_convert.o 				\
			dictionary.o 				\
			hash_table.o 				\
			column.o 				\
			fopen_path.o 				\
			-o fopen_path_dvr.e

fopen_path.o: fopen_path.c fopen_path.h
	${CC} ${CFLAGS} -c fopen_path.c

update_statement.o: update_statement.c update_statement.h
	${CC} ${CFLAGS} -c update_statement.c

appaserver_parameter.o: appaserver_parameter.c			\
	appaserver_parameter.h
	${CC} ${CFLAGS} -c appaserver_parameter.c

appaserver_link.o: appaserver_link.c appaserver_link.h
	${CC} ${CFLAGS} -c appaserver_link.c

two_way_pipe_dvr.e: two_way_pipe_dvr.c two_way_pipe.o
	${CC} ${CFLAGS} two_way_pipe_dvr.c 			\
			piece.o 				\
			date.o 					\
			julian.o 				\
			timlib.o 				\
			list_usage.o 				\
			date_convert.o 				\
			dictionary.o 				\
			hash_table.o 				\
			column.o 				\
			list.o 					\
			two_way_pipe.o 				\
			-o two_way_pipe_dvr.e

permutation_dvr.e: permutation_dvr.c permutation.o
	${CC} ${CFLAGS} permutation_dvr.c 			\
			permutation.o 				\
			-o permutation_dvr.e

permutation.o: permutation.c permutation.h
	${CC} ${CFLAGS} -c permutation.c

two_way_pipe.o: two_way_pipe.c two_way_pipe.h
	${CC} ${CFLAGS} -c two_way_pipe.c

table.o: table.c table.h
	${CC} ${CFLAGS} -c table.c

regression.o: regression.c regression.h
	${CC} ${CFLAGS} -c regression.c

document.o: document.c document.h
	${CC} ${CFLAGS} -c document.c

application.o: application.c application.h
	${CC} ${CFLAGS} -c application.c

appaserver_user.o: appaserver_user.c appaserver_user.h
	${CC} ${CFLAGS} -c appaserver_user.c

folder.o: folder.c folder.h
	${CC} ${CFLAGS} -c folder.c

process.o: process.c process.h
	${CC} ${CFLAGS} -c process.c

process_parameter.o: process_parameter.c process_parameter.h
	${CC} ${CFLAGS} -c process_parameter.c

attribute.o: attribute.c attribute.h
	${CC} ${CFLAGS} -c attribute.c

element.o: element.c element.h
	${CC} ${CFLAGS} -c element.c

pipe.o: pipe.c pipe.h
	${CC} ${CFLAGS} -c pipe.c

operation.o: operation.c operation.h process.h
	${CC} ${CFLAGS} -c operation.c

form.o: form.c form.h
	${CC} ${CFLAGS} -c form.c

form_prompt_edit.o: form_prompt_edit.c form_prompt_edit.h element.o
	${CC} ${CFLAGS} -c form_prompt_edit.c

form_edit_table.o: form_edit_table.c form_edit_table.h element.o
	${CC} ${CFLAGS} -c form_edit_table.c

form_choose_isa.o: form_choose_isa.c form_choose_isa.h element.o
	${CC} ${CFLAGS} -c form_choose_isa.c

form_choose_role.o: form_choose_role.c form_choose_role.h element.o
	${CC} ${CFLAGS} -c form_choose_role.c

radio.o: radio.c radio.h
	${CC} ${CFLAGS} -c radio.c

button.o: button.c button.h
	${CC} ${CFLAGS} -c button.c

query.o: query.c query.h relation_join.o
	${CC} ${CFLAGS} -c query.c

vertical_new.o: vertical_new.c vertical_new.h
	${CC} ${CFLAGS} -c vertical_new.c

role_folder.o: role_folder.c role_folder.h
	${CC} ${CFLAGS} -c role_folder.c

role_operation.o: role_operation.c role_operation.h
	${CC} ${CFLAGS} -c role_operation.c

post_dictionary.o: post_dictionary.c post_dictionary.h
	${CC} ${CFLAGS} -c post_dictionary.c

period_wo_date.o: period_wo_date.c period_wo_date.h timlib.h
	${CC} ${CFLAGS} -c period_wo_date.c

dictionary2file.o: dictionary2file.c dictionary2file.h
	${CC} ${CFLAGS} -c dictionary2file.c

application_constants.o: application_constants.c application_constants.h
	${CC} ${CFLAGS} -c application_constants.c

session.o: session.c session.h
	${CC} ${CFLAGS} -c session.c

waypoint.o: waypoint.c waypoint.h
	${CC} ${CFLAGS} -c waypoint.c

pivot_table.o: pivot_table.c pivot_table.h
	${CC} ${CFLAGS} -c pivot_table.c

generic_load.o: generic_load.c generic_load.h element.o
	${CC} ${CFLAGS} -c generic_load.c

scatterplot.o: scatterplot.c scatterplot.h
	${CC} ${CFLAGS} -c scatterplot.c

role.o: role.c role.h
	${CC} ${CFLAGS} -c role.c

sed.o: sed.c sed.h
	${CC} ${CFLAGS} -c sed.c

frameset.o: frameset.c frameset.h
	${CC} ${CFLAGS} -c frameset.c

insert.o: insert.c insert.h
	${CC} ${CFLAGS} -c insert.c

update.o: update.c update.h
	${CC} ${CFLAGS} -c update.c

delete.o: delete.c delete.h
	${CC} ${CFLAGS} -c delete.c

query_statistic.o: query_statistic.c query_statistic.h
	${CC} ${CFLAGS} -I${LIBRARY} 					\
		-c query_statistic.c

aggregate_level.o: aggregate_level.c aggregate_level.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c aggregate_level.c

validation_level.o: validation_level.c validation_level.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c validation_level.c

appaserver_library.o: appaserver_library.c appaserver_library.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c appaserver_library.c

appaserver_error.o: appaserver_error.c appaserver_error.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c appaserver_error.c

aggregate_statistic.o: aggregate_statistic.c aggregate_statistic.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c aggregate_statistic.c

make_date_time_between_compatable.o: make_date_time_between_compatable.c \
	make_date_time_between_compatable.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c make_date_time_between_compatable.c

populate_helper_process.o: populate_helper_process.c 			\
	populate_helper_process.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c populate_helper_process.c

drillthru.o: drillthru.c
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c drillthru.c

process_generic.o: process_generic.c
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c process_generic.c

folder_menu.o: folder_menu.c						\
	folder_menu.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c folder_menu.c

google_map.o: google_map.c google_map.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c google_map.c

google_earth.o: google_earth.c google_earth.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c google_earth.c

google_chart.o: google_chart.c google_chart.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c google_chart.c

covariance.o: covariance.c covariance.h list.h hash_table.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c covariance.c

semaphore.o: semaphore.c semaphore.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c semaphore.c

dollar.o: dollar.c dollar.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c dollar.c

fork_control.o: fork_control.c fork_control.h list.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c fork_control.c

histogram.o: histogram.c histogram.h list.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c histogram.c

parse_report.o: parse_report.c parse_report.h list.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c parse_report.c

statistics_weighted.o: statistics_weighted.c statistics_weighted.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c statistics_weighted.c

date_convert.o: date_convert.c date_convert.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c date_convert.c

row_security.o: row_security.c row_security.h element.o
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c row_security.c

dictionary_separate.o: dictionary_separate.c dictionary_separate.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c dictionary_separate.c

post_login.o: post_login.c post_login.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c post_login.c

array.o: array.c array.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c array.c

mysqldump.o: mysqldump.c mysqldump.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		-c mysqldump.c

foreign_attribute.o: foreign_attribute.c foreign_attribute.h
	${CC} ${CFLAGS} -c foreign_attribute.c

keys_match_sum.o: keys_match_sum.c keys_match_sum.h
	${CC} ${CFLAGS} -c keys_match_sum.c

email.o: email.c email.h
	${CC} ${CFLAGS} -c email.c

float.o: float.c float.h
	${CC} ${CFLAGS} -c float.c

String.o: String.c String.h
	${CC} ${CFLAGS} -c String.c

security.o: security.c security.h
	${CC} ${CFLAGS} -c security.c

relation.o: relation.c relation.h
	${CC} ${CFLAGS} -c relation.c

javascript.o: javascript.c javascript.h
	${CC} ${CFLAGS} -c javascript.c

insert_table.o: insert_table.c insert_table.h
	${CC} ${CFLAGS} -c insert_table.c

prompt_edit.o: prompt_edit.c prompt_edit.h
	${CC} ${CFLAGS} -c prompt_edit.c

prompt_insert.o: prompt_insert.c prompt_insert.h
	${CC} ${CFLAGS} -c prompt_insert.c

edit_table.o: edit_table.c edit_table.h
	${CC} ${CFLAGS} -c edit_table.c

frameset_link_file.o: frameset_link_file.c frameset_link_file.h
	${CC} ${CFLAGS} -c frameset_link_file.c

menu.o: menu.c menu.h
	${CC} ${CFLAGS} -c menu.c

choose_isa.o: choose_isa.c choose_isa.h
	${CC} ${CFLAGS} -c choose_isa.c

choose_role.o: choose_role.c choose_role.h
	${CC} ${CFLAGS} -c choose_role.c

post_choose_folder.o: post_choose_folder.c post_choose_folder.h
	${CC} ${CFLAGS} -c post_choose_folder.c

post_choose_process.o: post_choose_process.c post_choose_process.h
	${CC} ${CFLAGS} -c post_choose_process.c

post_choose_role.o: post_choose_role.c post_choose_role.h
	${CC} ${CFLAGS} -c post_choose_role.c

post_edit_table.o: post_edit_table.c post_edit_table.h
	${CC} ${CFLAGS} -c post_edit_table.c

relation_join.o: relation_join.c relation_join.h
	${CC} ${CFLAGS} -c relation_join.c


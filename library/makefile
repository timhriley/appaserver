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
		form_prompt_attribute_relational.o		\
		form_prompt_lookup.o				\
		form_prompt_insert.o				\
		form_prompt_process.o				\
		form_table_edit.o				\
		form_choose_isa.o				\
		form_choose_role.o				\
		form_drillthru.o				\
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
		prompt_lookup.o					\
		prompt_insert.o					\
		prompt_process.o				\
		table_edit.o					\
		table_insert.o					\
		menu.o						\
		recall.o					\
		choose_isa.o					\
		choose_role.o					\
		post_login.o					\
		post_choose_folder.o				\
		post_choose_process.o				\
		post_choose_role.o				\
		post_table_edit.o				\
		post_table_insert.o				\
		post_prompt_insert.o				\
		post_prompt_process.o				\
		post_prompt_lookup.o

all: 	${APPASERVER_LIBRARY} ${STANDARD_LIBRARY}

touch:
	/usr/bin/touch *.c

clean:
	rm -f *.[oe]

html_table.o: html_table.c html_table.h
	${CC} ${CFLAGS} -c html_table.c

html_invoice.o: html_invoice.c html_invoice.h
	${CC} ${CFLAGS} -c html_invoice.c

latex.o: latex.c latex.h
	${CC} ${CFLAGS} -c latex.c

latex_invoice.o: latex_invoice.c latex_invoice.h
	${CC} ${CFLAGS} -c latex_invoice.c

grace.o: grace.c grace.h
	${CC} ${CFLAGS} -c grace.c

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

token.o: token.c token.h
	${CC} ${CFLAGS} -c token.c

environ.o: environ.c environ.h
	${CC} ${CFLAGS} -c environ.c

mask.o: mask.c mask.h
	${CC} ${CFLAGS} -c mask.c

name_arg.o: name_arg.c name_arg.h
	${CC} ${CFLAGS} -c name_arg.c


date.o: date.c date.h
	${CC} ${CFLAGS} -c date.c

basename.o: basename.c
	${CC} ${CFLAGS} -c basename.c

piece.o: piece.c piece.h
	${CC} ${CFLAGS} -c piece.c

stack.o: stack.c stack.h
	${CC} ${CFLAGS} -c stack.c

fifo.o: fifo.c fifo.h
	${CC} ${CFLAGS} -c fifo.c

column.o: column.c column.h
	${CC} ${CFLAGS} -c column.c

distance.o: distance.c distance.h
	${CC} ${CFLAGS} -c distance.c

zip_code_distance.o: zip_code_distance.c zip_code_distance.h
	${CC} ${CFLAGS} -c zip_code_distance.c

moving_average.o: moving_average.c moving_average.h
	${CC} ${CFLAGS} -c moving_average.c

queue.o: queue.c queue.h
	${CC} ${CFLAGS} -c queue.c

list.o: list.c list.h
	${CC} ${CFLAGS} -c list.c

list_usage.o: list_usage.c list.h
	${CC} ${CFLAGS} -c list_usage.c

dictionary.o: dictionary.c dictionary.h hash_table.o
	${CC} ${CFLAGS} -c dictionary.c

hash_table.o: hash_table.c hash_table.h
	${CC} ${CFLAGS} -c hash_table.c

cc_chk.o: cc_chk.c
	${CC} ${CFLAGS} -c cc_chk.c

timlib.o: timlib.c timlib.h
	${CC} $(CFLAGS) -c timlib.c

std_addr.o: std_addr.c std_addr.h
	${CC} ${CFLAGS} -c std_addr.c

using.o: using.c
	${CC} ${CFLAGS} -c using.c

bsrch.o: bsrch.c bsrch.h
	${CC} ${CFLAGS} -c bsrch.c

date_aggregate.o: date_aggregate.c date_aggregate.h
	${CC} ${CFLAGS} -c date_aggregate.c

rnd_io.o: rnd_io.c rnd_io.h
	${CC} ${CFLAGS} -c rnd_io.c

julian_time_double_list.o: julian_time_double_list.c julian_time_double_list.h
	${CC} ${CFLAGS} -c julian_time_double_list.c

fopen_path.o: fopen_path.c fopen_path.h
	${CC} ${CFLAGS} -c fopen_path.c

update_statement.o: update_statement.c update_statement.h
	${CC} ${CFLAGS} -c update_statement.c

appaserver_parameter.o: appaserver_parameter.c			\
	appaserver_parameter.h
	${CC} ${CFLAGS} -c appaserver_parameter.c

appaserver_link.o: appaserver_link.c appaserver_link.h
	${CC} ${CFLAGS} -c appaserver_link.c

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

form_drillthru.o: form_drillthru.c form_drillthru.h element.o
	${CC} ${CFLAGS} -c form_drillthru.c

form_prompt_attribute_relational.o:				\
	form_prompt_attribute_relational.c			\
	form_prompt_attribute_relational.h			\
	element.o
	${CC} ${CFLAGS} -c form_prompt_attribute_relational.c

form_prompt_lookup.o: form_prompt_lookup.c form_prompt_lookup.h	\
	element.o
	${CC} ${CFLAGS} -c form_prompt_lookup.c

form_prompt_insert.o: form_prompt_insert.c form_prompt_insert.h	\
	element.o
	${CC} ${CFLAGS} -c form_prompt_insert.c

form_prompt_process.o:						\
	form_prompt_process.c					\
	form_prompt_process.h
	${CC} ${CFLAGS} -c form_prompt_process.c

form_table_edit.o: form_table_edit.c form_table_edit.h element.o
	${CC} ${CFLAGS} -c form_table_edit.c

form_choose_isa.o: form_choose_isa.c form_choose_isa.h element.o
	${CC} ${CFLAGS} -c form_choose_isa.c

form_choose_role.o:						\
	form_choose_role.c					\
	form_choose_role.h					\
	element.o
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
	${CC} ${CFLAGS} -c query_statistic.c

aggregate_level.o: aggregate_level.c aggregate_level.h
	${CC} ${CFLAGS} -c aggregate_level.c

validation_level.o: validation_level.c validation_level.h
	${CC} ${CFLAGS} -c validation_level.c

appaserver_library.o: appaserver_library.c appaserver_library.h
	${CC} ${CFLAGS} -c appaserver_library.c

appaserver_error.o: appaserver_error.c appaserver_error.h
	${CC} ${CFLAGS} -c appaserver_error.c

aggregate_statistic.o: aggregate_statistic.c aggregate_statistic.h
	${CC} ${CFLAGS} -c aggregate_statistic.c

make_date_time_between_compatable.o: make_date_time_between_compatable.c \
	make_date_time_between_compatable.h
	${CC} ${CFLAGS} -c make_date_time_between_compatable.c

populate_helper_process.o: populate_helper_process.c 			\
	populate_helper_process.h
	${CC} ${CFLAGS} -c populate_helper_process.c

drillthru.o: drillthru.c
	${CC} ${CFLAGS} -c drillthru.c

process_generic.o: process_generic.c
	${CC} ${CFLAGS} -c process_generic.c

folder_menu.o: folder_menu.c						\
	folder_menu.h
	${CC} ${CFLAGS} -c folder_menu.c

google_map.o: google_map.c google_map.h
	${CC} ${CFLAGS} 
		-c google_map.c

google_earth.o: google_earth.c google_earth.h
	${CC} ${CFLAGS} -c google_earth.c

google_chart.o: google_chart.c google_chart.h
	${CC} ${CFLAGS} -c google_chart.c

covariance.o: covariance.c covariance.h list.h hash_table.h
	${CC} ${CFLAGS} -c covariance.c

semaphore.o: semaphore.c semaphore.h
	${CC} ${CFLAGS} -c semaphore.c

dollar.o: dollar.c dollar.h
	${CC} ${CFLAGS} -c dollar.c

fork_control.o: fork_control.c fork_control.h list.h
	${CC} ${CFLAGS} -c fork_control.c

histogram.o: histogram.c histogram.h list.h
	${CC} ${CFLAGS} -c histogram.c

parse_report.o: parse_report.c parse_report.h list.h
	${CC} ${CFLAGS} -c parse_report.c

statistics_weighted.o: statistics_weighted.c statistics_weighted.h
	${CC} ${CFLAGS} -c statistics_weighted.c

date_convert.o: date_convert.c date_convert.h
	${CC} ${CFLAGS} -c date_convert.c

row_security.o: row_security.c row_security.h element.o
	${CC} ${CFLAGS} -c row_security.c

dictionary_separate.o: dictionary_separate.c dictionary_separate.h
	${CC} ${CFLAGS} -c dictionary_separate.c

post_login.o: post_login.c post_login.h
	${CC} ${CFLAGS} -c post_login.c

array.o: array.c array.h
	${CC} ${CFLAGS} -c array.c

mysqldump.o: mysqldump.c mysqldump.h
	${CC} ${CFLAGS} -c mysqldump.c

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

prompt_lookup.o: prompt_lookup.c prompt_lookup.h
	${CC} ${CFLAGS} -c prompt_lookup.c

prompt_insert.o: prompt_insert.c prompt_insert.h
	${CC} ${CFLAGS} -c prompt_insert.c

prompt_process.o: prompt_process.c prompt_process.h
	${CC} ${CFLAGS} -c prompt_process.c

table_edit.o: table_edit.c table_edit.h
	${CC} ${CFLAGS} -c table_edit.c

table_insert.o: table_insert.c table_insert.h
	${CC} ${CFLAGS} -c table_insert.c

frameset_link_file.o: frameset_link_file.c frameset_link_file.h
	${CC} ${CFLAGS} -c frameset_link_file.c

menu.o: menu.c menu.h
	${CC} ${CFLAGS} -c menu.c

recall.o: recall.c recall.h
	${CC} ${CFLAGS} -c recall.c

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

post_table_edit.o: post_table_edit.c post_table_edit.h
	${CC} ${CFLAGS} -c post_table_edit.c

post_table_insert.o: post_table_insert.c post_table_insert.h
	${CC} ${CFLAGS} -c post_table_insert.c

post_prompt_insert.o: post_prompt_insert.c post_prompt_insert.h
	${CC} ${CFLAGS} -c post_prompt_insert.c

post_prompt_lookup.o: post_prompt_lookup.c post_prompt_lookup.h
	${CC} ${CFLAGS} -c post_prompt_lookup.c

post_prompt_process.o: post_prompt_process.c post_prompt_process.h
	${CC} ${CFLAGS} -c post_prompt_process.c

relation_join.o: relation_join.c relation_join.h
	${CC} ${CFLAGS} -c relation_join.c


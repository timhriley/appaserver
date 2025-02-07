# $APPASERVER_HOME/library/makefile
# ---------------------------------

CC=gcc
#WARN=-Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
#WARN=-Wall -Wextra -Wtype-limits
WARN=-Wall -Wextra -Wunused-parameter
#CFLAGS=-g
CFLAGS=-O3 -s ${WARN}

APPASERVER_LIBRARY=						\
		timlib.o 					\
		String.o 					\
		date.o 						\
		julian.o 					\
		dictionary.o 					\
		list.o 						\
		float.o 					\
		piece.o 					\
		column.o 					\
		date_convert.o 					\
		hash_table.o					\
		javascript.o					\
		security.o					\
		security_entity.o				\
		aggregate_level.o 				\
		aggregate_statistic.o				\
		update_statement.o				\
		fopen_path.o					\
		name_arg.o					\
		google_earth.o					\
		environ.o 					\
		latex.o						\
		basename.o					\
		role.o						\
		role_folder.o					\
		folder_operation.o				\
		token.o						\
		make_date_time_between_compatable.o		\
		populate_helper_process.o			\
		drillthru.o					\
		drilldown.o					\
		appaserver_error.o				\
		folder.o					\
		folder_row_level_restriction.o			\
		attribute.o					\
		folder_attribute.o				\
		date_aggregate.o				\
		html.o						\
		spreadsheet.o					\
		folder_menu.o					\
		appaserver_user.o				\
		widget.o					\
		sed.o						\
		form.o						\
		form_prompt_attribute.o				\
		form_prompt_lookup.o				\
		form_prompt_insert.o				\
		form_prompt_process.o				\
		form_table_edit.o				\
		form_table_insert.o				\
		form_choose_isa.o				\
		form_choose_role.o				\
		form_drillthru.o				\
		form_generic_load.o				\
		form_merge_purge.o				\
		form_lookup_delete.o				\
		form_lookup_sort.o				\
		radio.o						\
		button.o					\
		email.o						\
		keys_match_sum.o				\
		array.o						\
		query.o 					\
		query_statistic.o				\
		query_isa.o 					\
		stack.o 					\
		grace.o 					\
		frameset.o					\
		validation_level.o				\
		relation.o					\
		relation_mto1.o					\
		relation_one2m.o				\
		relation_join.o					\
		operation.o					\
		document.o					\
		insert.o					\
		expected_count.o				\
		update.o					\
		delete.o					\
		delete_primary.o				\
		start_stop_time.o				\
		appaserver_parameter.o				\
		appaserver_link.o				\
		process.o					\
		process_parameter.o				\
		queue.o						\
		post_dictionary.o				\
		period_wo_date.o				\
		dictionary2file.o				\
		application_constant.o				\
		row_security.o					\
		row_security_role_update.o			\
		dictionary_separate.o				\
		regression.o					\
		pivot_table.o					\
		statistic.o					\
		covariance.o					\
		semaphore.o					\
		dollar.o					\
		histogram.o					\
		session.o					\
		foreign_attribute.o				\
		moving_statistic.o				\
		pair_one2m.o					\
		mysqldump.o					\
		fork_control.o					\
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
		post_choose_isa.o				\
		post_table_edit.o				\
		post_table_insert.o				\
		post_prompt_insert.o				\
		post_prompt_process.o				\
		post_drillthru.o				\
		appaserver.o					\
		generic_load.o					\
		merge_purge.o					\
		change_password.o				\
		orphan.o					\
		rectification.o					\
		drop_table.o					\
		drop_column.o					\
		google.o					\
		google_graph.o					\
		google_map.o					\
		google_map_operation.o				\
		group_count.o					\
		lookup_statistic.o				\
		lookup_histogram.o				\
		shell_script.o					\
		add_column.o					\
		alter_datatype.o				\
		rename_table.o					\
		rename_column.o					\
		create_table.o					\
		export_subschema.o				\
		export_process.o				\
		export_process_set.o				\
		clone_folder.o					\
		select_statement.o				\
		execute_select.o				\
		export_table.o					\
		pkzip.o						\
		ajax.o						\
		sql.o						\
		export_application.o				\
		view_source.o					\
		post_prompt_lookup.o				\
		lookup_spreadsheet.o				\
		lookup_delete.o					\
		lookup_sort.o					\
		chart.o						\
		lookup_grace.o					\
		lookup_google.o					\
		units.o						\
		output_medium.o					\
		accumulate_boolean.o				\
		execute_system_string.o				\
		appaserver_upgrade.o				\
		grant_select.o					\
		upload_source.o					\
		application.o					\
		application_data.o				\
		application_upload.o				\
		application_log.o				\
		application_backup.o				\
		application_create.o				\
		application_delete.o				\
		application_clone.o				\
		spool.o						\
		process_generic.o				\
		vertical_new_checkbox.o				\
		vertical_new_prompt.o				\
		vertical_new_filename.o				\
		vertical_new_table.o				\
		vertical_new_post.o				\
		entity.o					\
		entity_self.o					\
		upload_purge.o					\
		preupdate_change.o				\
		bot_generated.o					\
		canvass_waypoint.o				\
		waypoint.o					\
		file.o

all: 	${APPASERVER_LIBRARY}					\
	credit_provider_dvr

touch:
	/usr/bin/touch *.c

clean:
	rm -f *.[oe] credit_provider_dvr

credit_provider_dvr: credit_provider_dvr.c				\
	credit_provider.o
	$(CC)   ${CFLAGS}						\
		credit_provider_dvr.c					\
		credit_provider.o					\
		-o credit_provider_dvr

html.o: html.c html.h
	${CC} ${CFLAGS} -c html.c

spreadsheet.o: spreadsheet.c spreadsheet.h
	${CC} ${CFLAGS} -c spreadsheet.c

latex.o: latex.c latex.h
	${CC} ${CFLAGS} -c latex.c

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

moving_statistic.o: moving_statistic.c moving_statistic.h
	${CC} ${CFLAGS} -c moving_statistic.c

queue.o: queue.c queue.h
	${CC} ${CFLAGS} -c queue.c

list.o: list.c list.h
	${CC} ${CFLAGS} -c list.c

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

table.o: table.c table.h
	${CC} ${CFLAGS} -c table.c

regression.o: regression.c regression.h
	${CC} ${CFLAGS} -c regression.c

document.o: document.c document.h
	${CC} ${CFLAGS} -c document.c

application.o: application.c application.h
	${CC} ${CFLAGS} -c application.c

application_login.o: application_login.c application_login.h
	${CC} ${CFLAGS} -c application_login.c

application_data.o: application_data.c application_data.h
	${CC} ${CFLAGS} -c application_data.c

application_upload.o: application_upload.c application_upload.h
	${CC} ${CFLAGS} -c application_upload.c

application_log.o: application_log.c application_log.h
	${CC} ${CFLAGS} -c application_log.c

application_backup.o: application_backup.c application_backup.h
	${CC} ${CFLAGS} -c application_backup.c

application_clone.o: application_clone.c application_clone.h
	${CC} ${CFLAGS} -c application_clone.c

application_create.o: application_create.c application_create.h
	${CC} ${CFLAGS} -c application_create.c

application_delete.o: application_delete.c application_delete.h
	${CC} ${CFLAGS} -c application_delete.c

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

folder_attribute.o: folder_attribute.c folder_attribute.h
	${CC} ${CFLAGS} -c folder_attribute.c

widget.o: widget.c widget.h
	${CC} ${CFLAGS} -c widget.c

pipe.o: pipe.c pipe.h
	${CC} ${CFLAGS} -c pipe.c

operation.o: operation.c operation.h process.h
	${CC} ${CFLAGS} -c operation.c

form.o: form.c form.h
	${CC} ${CFLAGS} -c form.c

form_drillthru.o: form_drillthru.c form_drillthru.h
	${CC} ${CFLAGS} -c form_drillthru.c

form_prompt_attribute.o:					\
	form_prompt_attribute.c					\
	form_prompt_attribute.h
	${CC} ${CFLAGS} -c form_prompt_attribute.c

form_prompt_lookup.o: form_prompt_lookup.c form_prompt_lookup.h
	${CC} ${CFLAGS} -c form_prompt_lookup.c

form_prompt_insert.o: form_prompt_insert.c form_prompt_insert.h
	${CC} ${CFLAGS} -c form_prompt_insert.c

form_prompt_process.o:						\
	form_prompt_process.c					\
	form_prompt_process.h					\
	recall.o						\
	form_prompt_attribute.o
	${CC} ${CFLAGS} -c form_prompt_process.c

form_table_edit.o: form_table_edit.c form_table_edit.h
	${CC} ${CFLAGS} -c form_table_edit.c

form_table_insert.o: form_table_insert.c form_table_insert.h
	${CC} ${CFLAGS} -c form_table_insert.c

form_choose_isa.o: form_choose_isa.c form_choose_isa.h
	${CC} ${CFLAGS} -c form_choose_isa.c

form_choose_role.o: form_choose_role.c form_choose_role.h
	${CC} ${CFLAGS} -c form_choose_role.c

form_generic_load.o: form_generic_load.c form_generic_load.h
	${CC} ${CFLAGS} -c form_generic_load.c

form_merge_purge.o:				\
		form_merge_purge.c		\
		form_merge_purge.h
	${CC} ${CFLAGS} -c form_merge_purge.c

form_lookup_delete.o: form_lookup_delete.c form_lookup_delete.h
	${CC} ${CFLAGS} -c form_lookup_delete.c

form_lookup_sort.o: form_lookup_sort.c form_lookup_sort.h
	${CC} ${CFLAGS} -c form_lookup_sort.c

radio.o: radio.c radio.h
	${CC} ${CFLAGS} -c radio.c

button.o: button.c button.h
	${CC} ${CFLAGS} -c button.c

query.o: query.c query.h
	${CC} ${CFLAGS} -c query.c

query_statistic.o: query_statistic.c query_statistic.h
	${CC} ${CFLAGS} -c query_statistic.c

query_isa.o: query_isa.c query_isa.h
	${CC} ${CFLAGS} -c query_isa.c

vertical_new.o: vertical_new.c vertical_new.h
	${CC} ${CFLAGS} -c vertical_new.c

vertical_new_checkbox.o: vertical_new_checkbox.c vertical_new_checkbox.h
	${CC} ${CFLAGS} -c vertical_new_checkbox.c

vertical_new_prompt.o: vertical_new_prompt.c vertical_new_prompt.h
	${CC} ${CFLAGS} -c vertical_new_prompt.c

vertical_new_table.o: vertical_new_table.c vertical_new_table.h
	${CC} ${CFLAGS} -c vertical_new_table.c

vertical_new_filename.o: vertical_new_filename.c vertical_new_filename.h
	${CC} ${CFLAGS} -c vertical_new_filename.c

vertical_new_post.o: vertical_new_post.c vertical_new_post.h
	${CC} ${CFLAGS} -c vertical_new_post.c

role_folder.o: role_folder.c role_folder.h
	${CC} ${CFLAGS} -c role_folder.c

folder_operation.o: folder_operation.c folder_operation.h
	${CC} ${CFLAGS} -c folder_operation.c

post_dictionary.o: post_dictionary.c post_dictionary.h
	${CC} ${CFLAGS} -c post_dictionary.c

period_wo_date.o: period_wo_date.c period_wo_date.h timlib.h
	${CC} ${CFLAGS} -c period_wo_date.c

dictionary2file.o: dictionary2file.c dictionary2file.h
	${CC} ${CFLAGS} -c dictionary2file.c

application_constant.o: application_constant.c application_constant.h
	${CC} ${CFLAGS} -c application_constant.c

session.o: session.c session.h
	${CC} ${CFLAGS} -c session.c

canvass_waypoint.o: canvass_waypoint.c canvass_waypoint.h
	${CC} ${CFLAGS} -c canvass_waypoint.c

waypoint.o: waypoint.c waypoint.h
	${CC} ${CFLAGS} -c waypoint.c

pivot_table.o: pivot_table.c pivot_table.h
	${CC} ${CFLAGS} -c pivot_table.c

generic_load.o: generic_load.c generic_load.h
	${CC} ${CFLAGS} -c generic_load.c

merge_purge.o:				\
	merge_purge.c			\
	merge_purge.h			\
	form_merge_purge.o		\
	update.o			\
	delete.o
	${CC} ${CFLAGS} -c merge_purge.c

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

delete_primary.o: delete_primary.c delete_primary.h
	${CC} ${CFLAGS} -c delete_primary.c

lookup_statistic.o: lookup_statistic.c lookup_statistic.h
	${CC} ${CFLAGS} -c lookup_statistic.c

aggregate_level.o: aggregate_level.c aggregate_level.h
	${CC} ${CFLAGS} -c aggregate_level.c

validation_level.o: validation_level.c validation_level.h
	${CC} ${CFLAGS} -c validation_level.c

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

drillthru.o: drillthru.c drillthru.h
	${CC} ${CFLAGS} -c drillthru.c

drilldown.o: drilldown.c drilldown.h
	${CC} ${CFLAGS} -c drilldown.c

process_generic.o: process_generic.c process_generic.h
	${CC} ${CFLAGS} -c process_generic.c

process_generic_statistic.o: process_generic_statistic.c		\
	process_generic_statistic.h
	${CC} ${CFLAGS} -c process_generic_statistic.c

process_generic_annual.o: process_generic_annual.c			\
	process_generic_annual.h
	${CC} ${CFLAGS} -c process_generic_annual.c

process_generic_grace.o: process_generic_grace.c			\
	process_generic_grace.h						\
	process_generic.o
	${CC} ${CFLAGS} -c process_generic_grace.c

process_generic_whisker.o: process_generic_whisker.c			\
	process_generic_whisker.h					\
	process_generic.o
	${CC} ${CFLAGS} -c process_generic_whisker.c

process_generic_histogram.o: process_generic_histogram.c		\
	process_generic_histogram.h					\
	process_generic.o
	${CC} ${CFLAGS} -c process_generic_histogram.c

process_generic_moving.o: process_generic_moving.c			\
	process_generic_moving.h					\
	process_generic.o
	${CC} ${CFLAGS} -c process_generic_moving.c

process_generic_google.o: process_generic_google.c			\
	process_generic_google.h					\
	process_generic.o
	${CC} ${CFLAGS} -c process_generic_google.c

folder_menu.o: folder_menu.c folder_menu.h
	${CC} ${CFLAGS} -c folder_menu.c

google_map.o: google_map.c google_map.h
	${CC} ${CFLAGS} -c google_map.c

google_map_operation.o: google_map_operation.c				\
	google_map_operation.h						\
	google_map.o
	${CC} ${CFLAGS} -c google_map_operation.c

google_earth.o: google_earth.c google_earth.h
	${CC} ${CFLAGS} -c google_earth.c

google.o: google.c google.h
	${CC} ${CFLAGS} -c google.c

google_graph.o: google_graph.c google_graph.h
	${CC} ${CFLAGS} -c google_graph.c

covariance.o: covariance.c covariance.h
	${CC} ${CFLAGS} -c covariance.c

semaphore.o: semaphore.c semaphore.h
	${CC} ${CFLAGS} -c semaphore.c

dollar.o: dollar.c dollar.h
	${CC} ${CFLAGS} -c dollar.c

fork_control.o: fork_control.c fork_control.h
	${CC} ${CFLAGS} -c fork_control.c

histogram.o: histogram.c histogram.h
	${CC} ${CFLAGS} -c histogram.c

parse_report.o: parse_report.c parse_report.h
	${CC} ${CFLAGS} -c parse_report.c

statistic.o: statistic.c statistic.h
	${CC} ${CFLAGS} -c statistic.c

date_convert.o: date_convert.c date_convert.h
	${CC} ${CFLAGS} -c date_convert.c

row_security.o: row_security.c row_security.h
	${CC} ${CFLAGS} -c row_security.c

row_security_role_update.o:					\
	row_security_role_update.c				\
	row_security_role_update.h
	${CC} ${CFLAGS} -c row_security_role_update.c

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

security_entity.o: security_entity.c security_entity.h
	${CC} ${CFLAGS} -c security_entity.c

relation.o: relation.c relation.h
	${CC} ${CFLAGS} -c relation.c

relation_mto1.o: relation_mto1.c relation_mto1.h
	${CC} ${CFLAGS} -c relation_mto1.c

relation_one2m.o: relation_one2m.c relation_one2m.h
	${CC} ${CFLAGS} -c relation_one2m.c

relation_join.o: relation_join.c relation_join.h
	${CC} ${CFLAGS} -c relation_join.c

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

post_choose_isa.o: post_choose_isa.c post_choose_isa.h
	${CC} ${CFLAGS} -c post_choose_isa.c

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

post_drillthru.o: post_drillthru.c post_drillthru.h
	${CC} ${CFLAGS} -c post_drillthru.c

appaserver.o: appaserver.c appaserver.h
	${CC} ${CFLAGS} -c appaserver.c

change_password.o: change_password.c change_password.h
	${CC} ${CFLAGS} -c change_password.c

orphan.o: orphan.c orphan.h
	${CC} ${CFLAGS} -c orphan.c

rectification.o: rectification.c rectification.h
	${CC} ${CFLAGS} -c rectification.c

drop_table.o: drop_table.c drop_table.h
	${CC} ${CFLAGS} -c drop_table.c

drop_column.o: drop_column.c drop_column.h
	${CC} ${CFLAGS} -c drop_column.c

group_count.o: group_count.c group_count.h
	${CC} ${CFLAGS} -c group_count.c

shell_script.o: shell_script.c shell_script.h
	${CC} ${CFLAGS} -c shell_script.c

add_column.o: add_column.c add_column.h
	${CC} ${CFLAGS} -c add_column.c

alter_datatype.o: alter_datatype.c alter_datatype.h
	${CC} ${CFLAGS} -c alter_datatype.c

rename_table.o: rename_table.c rename_table.h
	${CC} ${CFLAGS} -c rename_table.c

rename_column.o: rename_column.c rename_column.h
	${CC} ${CFLAGS} -c rename_column.c

create_table.o: create_table.c create_table.h
	${CC} ${CFLAGS} -c create_table.c

export_subschema.o: export_subschema.c export_subschema.h
	${CC} ${CFLAGS} -c export_subschema.c

export_process.o: export_process.c export_process.h
	${CC} ${CFLAGS} -c export_process.c

export_process_set.o: export_process_set.c export_process_set.h
	${CC} ${CFLAGS} -c export_process_set.c

clone_folder.o: clone_folder.c clone_folder.h
	${CC} ${CFLAGS} -c clone_folder.c

select_statement.o: select_statement.c select_statement.h
	${CC} ${CFLAGS} -c select_statement.c

execute_select.o: execute_select.c execute_select.h
	${CC} ${CFLAGS} -c execute_select.c

export_table.o: export_table.c export_table.h
	${CC} ${CFLAGS} -c export_table.c

pkzip.o: pkzip.c pkzip.h
	${CC} ${CFLAGS} -c pkzip.c

ajax.o: ajax.c ajax.h
	${CC} ${CFLAGS} -c ajax.c

sql.o: sql.c sql.h
	${CC} ${CFLAGS} -c sql.c

export_application.o: 				\
	export_application.c 			\
	export_application.h			\
	export_table.o
	${CC} ${CFLAGS} -c export_application.c

view_source.o: view_source.c view_source.h
	${CC} ${CFLAGS} -c view_source.c

lookup_spreadsheet.o: lookup_spreadsheet.c lookup_spreadsheet.h
	${CC} ${CFLAGS} -c lookup_spreadsheet.c

lookup_delete.o: lookup_delete.c lookup_delete.h
	${CC} ${CFLAGS} -c lookup_delete.c

lookup_sort.o: lookup_sort.c lookup_sort.h
	${CC} ${CFLAGS} -c lookup_sort.c

lookup_grace.o: lookup_grace.c lookup_grace.h
	${CC} ${CFLAGS} -c lookup_grace.c

chart.o: chart.c chart.h
	${CC} ${CFLAGS} -c chart.c

lookup_histogram.o: lookup_histogram.c lookup_histogram.h
	${CC} ${CFLAGS} -c lookup_histogram.c

lookup_google.o: lookup_google.c lookup_google.h
	${CC} ${CFLAGS} -c lookup_google.c

units.o: units.c units.h
	${CC} ${CFLAGS} -c units.c

output_medium.o: output_medium.c output_medium.h
	${CC} ${CFLAGS} -c output_medium.c

accumulate_boolean.o: accumulate_boolean.c accumulate_boolean.h
	${CC} ${CFLAGS} -c accumulate_boolean.c

execute_system_string.o: execute_system_string.c execute_system_string.h
	${CC} ${CFLAGS} -c execute_system_string.c

appaserver_upgrade.o: appaserver_upgrade.c appaserver_upgrade.h
	${CC} ${CFLAGS} -c appaserver_upgrade.c

grant_select.o: grant_select.c grant_select.h
	${CC} ${CFLAGS} -c grant_select.c

upload_source.o: upload_source.c upload_source.h
	${CC} ${CFLAGS} -c upload_source.c

folder_row_level_restriction.o:					\
	folder_row_level_restriction.c				\
	folder_row_level_restriction.h
	${CC} ${CFLAGS} -c folder_row_level_restriction.c

spool.o: spool.c spool.h
	${CC} ${CFLAGS} -c spool.c

entity.o: entity.c entity.h
	${CC} ${CFLAGS} -c entity.c

entity_self.o: entity_self.c entity_self.h
	${CC} ${CFLAGS} -c entity_self.c

upload_purge.o: upload_purge.c upload_purge.h
	${CC} ${CFLAGS} -c upload_purge.c

preupdate_change.o: preupdate_change.c preupdate_change.h
	${CC} ${CFLAGS} -c preupdate_change.c

bot_generated.o: bot_generated.c bot_generated.h
	${CC} ${CFLAGS} -c bot_generated.c

file.o: file.c file.h
	${CC} ${CFLAGS} -c file.c

credit_provider.o: credit_provider.c credit_provider.h
	${CC} ${CFLAGS} -c credit_provider.c


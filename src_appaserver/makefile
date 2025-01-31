# $APPASERVER_HOME/src_appaserver/makefile
# ------------------------------------------------------------
# These executables are not allowed to be executed by PROCESS.
# ------------------------------------------------------------

CC=gcc
#WARN=-Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
#WARN=-Wall -Wextra -Wtype-limits
WARN=-Wall -Wextra -Wunused-parameter
#CFLAGS=-g
CFLAGS=-O3 -s ${WARN}

LIBRARY=${APPASERVER_HOME}/library

EXECUTABLES=								\
		database_table						\
		appaserver_error_filename				\
		sql.e							\
		post_choose_folder					\
		random_session						\
		output_choose_isa					\
		output_choose_role					\
		output_table_edit					\
		post_login						\
		post_signup						\
		post_contact						\
		select							\
		post_prompt_insert					\
		post_prompt_lookup					\
		output_prompt_process					\
		post_choose_process					\
		post_prompt_process					\
		post_table_edit						\
		post_table_insert					\
		post_choose_isa						\
		output_table_insert					\
		output_spreadsheet					\
		output_grace_chart					\
		output_histogram					\
		output_google_chart					\
		output_group						\
		lookup_statistic					\
		generic_load						\
		post_generic_load_choose				\
		post_generic_load_folder				\
		drop_application					\
		table_list						\
		appaserver_mount_point					\
		post_delete_folder_block				\
		post_ajax_window					\
		post_change_sort_order					\
		post_choose_role			 		\
		post_drillthru				 		\
		output_prompt_lookup					\
		appaserver_upgrade					\
		upload_source_file					\
		output_prompt_insert					\
		output_frameset						\
		output_menu						\
		upload_purge

CORE_LIBRARY=								\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/application.o				\
		${LIBRARY}/String.o					\
		${LIBRARY}/piece.o					\
		${LIBRARY}/float.o					\
		${LIBRARY}/dictionary.o					\
		${LIBRARY}/column.o					\
		${LIBRARY}/hash_table.o					\
		${LIBRARY}/security.o					\
		${LIBRARY}/spool.o					\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/application_log.o				\
		${LIBRARY}/application_constant.o			\
		${LIBRARY}/appaserver_parameter.o			\
		${LIBRARY}/environ.o					\
		${LIBRARY}/date.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/julian.o					\
		${LIBRARY}/javascript.o					\
		${LIBRARY}/list.o

APPASERVER_LIBRARY=							\
		${CORE_LIBRARY}						\
		${LIBRARY}/sed.o					\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/security_entity.o				\
		${LIBRARY}/entity.o					\
		${LIBRARY}/process_generic.o				\
		${LIBRARY}/output_medium.o				\
		${LIBRARY}/units.o					\
		${LIBRARY}/accumulate_boolean.o				\
		${LIBRARY}/basename.o					\
		${LIBRARY}/session.o					\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/folder_attribute.o				\
		${LIBRARY}/process_parameter.o				\
		${LIBRARY}/appaserver_link.o				\
		${LIBRARY}/widget.o					\
		${LIBRARY}/button.o					\
		${LIBRARY}/process.o					\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/operation.o					\
		${LIBRARY}/role_folder.o				\
		${LIBRARY}/folder_operation.o				\
		${LIBRARY}/form.o					\
		${LIBRARY}/dictionary_separate.o			\
		${LIBRARY}/query.o					\
		${LIBRARY}/query_isa.o					\
		${LIBRARY}/insert.o					\
		${LIBRARY}/update.o					\
		${LIBRARY}/folder_row_level_restriction.o		\
		${LIBRARY}/relation.o					\
		${LIBRARY}/relation_mto1.o				\
		${LIBRARY}/relation_one2m.o				\
		${LIBRARY}/relation_join.o				\
		${LIBRARY}/aggregate_level.o				\
		${LIBRARY}/aggregate_statistic.o			\
		${LIBRARY}/choose_role.o				\
		${LIBRARY}/prompt_lookup.o				\
		${LIBRARY}/form_prompt_lookup.o				\
		${LIBRARY}/prompt_process.o				\
		${LIBRARY}/form_prompt_process.o			\
		${LIBRARY}/post_prompt_process.o			\
		${LIBRARY}/post_choose_process.o			\
		${LIBRARY}/form_choose_role.o				\
		${LIBRARY}/form_prompt_attribute.o			\
		${LIBRARY}/post_choose_role.o				\
		${LIBRARY}/recall.o					\
		${LIBRARY}/radio.o					\
		${LIBRARY}/pair_one2m.o					\
		${LIBRARY}/row_security.o				\
		${LIBRARY}/row_security_role_update.o			\
		${LIBRARY}/post_dictionary.o				\
		${LIBRARY}/folder_menu.o				\
		${LIBRARY}/menu.o					\
		${LIBRARY}/ajax.o					\
		${LIBRARY}/sql.o					\
		${LIBRARY}/execute_system_string.o			\
		${LIBRARY}/post_choose_folder.o				\
		${LIBRARY}/drillthru.o					\
		${LIBRARY}/form_drillthru.o

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f ${EXECUTABLES}					\
	*.[oe]								\
	${CGI_HOME}/post_delete_folder_block				\
	${CGI_HOME}/post_ajax_window					\
	${CGI_HOME}/post_change_sort_order				\
	${CGI_HOME}/post_login						\
	${CGI_HOME}/post_signup						\
	${CGI_HOME}/post_choose_role	 				\
	${CGI_HOME}/post_drillthru	 				\
	${CGI_HOME}/post_prompt_insert					\
	${CGI_HOME}/post_prompt_lookup					\
	${CGI_HOME}/post_choose_isa					\
	${CGI_HOME}/output_prompt_insert				\
	${CGI_HOME}/post_choose_folder					\
	${CGI_HOME}/post_choose_process					\
	${CGI_HOME}/post_table_edit					\
	${CGI_HOME}/post_table_insert					\
	${CGI_HOME}/post_generic_load_choose				\
	${CGI_HOME}/post_generic_load_folder				\
	${CGI_HOME}/post_prompt_process					\
	${CGI_HOME}/post_contact					\
	${CGI_HOME}/output_menu						\
	appaserver_mount_point						\
	appaserver_data_directory					\
	appaserver_error_directory					\
	appaserver_log_directory					\
	appaserver_upload_directory					\
	appaserver_backup_directory					\
	appaserver_log_filename						\
	document_root							\
	appaserver_home							\
	appaserver-upgrade						\
	sql								\
	sql_quick							\
	get_folder_data							\
	select.e							\
	view_pdf_files							\
	get_table_name							\
	table_name							\
	upgrade-appaserver-database

appaserver_mount_point: appaserver_mount_point.c 			\
	${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		appaserver_mount_point.c				\
		-o appaserver_mount_point
	touch appaserver_home
	/bin/rm -f appaserver_home
	ln appaserver_mount_point appaserver_home
	touch appaserver_error_directory
	/bin/rm -f appaserver_error_directory
	ln appaserver_mount_point appaserver_error_directory
	touch document_root
	/bin/rm -f document_root
	ln appaserver_mount_point document_root
	touch appaserver_data_directory
	/bin/rm -f appaserver_data_directory
	ln appaserver_mount_point appaserver_data_directory
	touch appaserver_log_directory
	/bin/rm -f appaserver_log_directory
	ln appaserver_mount_point appaserver_log_directory
	touch appaserver_upload_directory
	/bin/rm -f appaserver_upload_directory
	ln appaserver_mount_point appaserver_upload_directory
	touch appaserver_backup_directory
	/bin/rm -f appaserver_backup_directory
	ln appaserver_mount_point appaserver_backup_directory

lookup_statistic: lookup_statistic.c ${APPASERVER_LIBRARY}		\
	${LIBRARY}/lookup_statistic.o					\
	${LIBRARY}/query_statistic.o					\
	${LIBRARY}/statistic.o						\
	${LIBRARY}/html.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/lookup_statistic.o 				\
		${LIBRARY}/query_statistic.o 				\
		${LIBRARY}/statistic.o 					\
		${LIBRARY}/html.o					\
		lookup_statistic.c					\
		-o lookup_statistic -lm

post_delete_folder_block: post_delete_folder_block.c			\
	${APPASERVER_LIBRARY}						\
	${LIBRARY}/lookup_delete.o					\
	${LIBRARY}/form_lookup_delete.o					\
	${LIBRARY}/delete.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_delete_folder_block.c				\
		${LIBRARY}/lookup_delete.o				\
		${LIBRARY}/form_lookup_delete.o				\
		${LIBRARY}/delete.o					\
		-o post_delete_folder_block
	touch ${CGI_HOME}/post_delete_folder_block
	/bin/rm -f ${CGI_HOME}/post_delete_folder_block
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_delete_folder_block 	\
	${CGI_HOME}

post_ajax_window: post_ajax_window.c					\
	${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_ajax_window.c					\
		-o post_ajax_window
	touch ${CGI_HOME}/post_ajax_window
	/bin/rm -f ${CGI_HOME}/post_ajax_window
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_ajax_window	 	\
	${CGI_HOME}

post_change_sort_order: post_change_sort_order.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/lookup_sort.o				\
		${LIBRARY}/form_lookup_sort.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_change_sort_order.c				\
		${LIBRARY}/lookup_sort.o				\
		${LIBRARY}/form_lookup_sort.o				\
		-o post_change_sort_order
	touch ${CGI_HOME}/post_change_sort_order
	/bin/rm -f ${CGI_HOME}/post_change_sort_order
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_change_sort_order 	\
	${CGI_HOME}

post_login: post_login.c ${APPASERVER_LIBRARY}				\
		${LIBRARY}/post_login.o					\
		${LIBRARY}/bot_generated.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_login.c						\
		${LIBRARY}/post_login.o					\
		${LIBRARY}/bot_generated.o				\
		-o post_login
	touch ${CGI_HOME}/post_login
	/bin/rm -f ${CGI_HOME}/post_login
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_login			\
	${CGI_HOME}

post_signup: post_signup.c ${APPASERVER_LIBRARY}			\
		${LIBRARY}/post_login.o					\
		${LIBRARY}/post_signup.o				\
		${LIBRARY}/bot_generated.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_signup.c						\
		${LIBRARY}/post_login.o					\
		${LIBRARY}/post_signup.o				\
		${LIBRARY}/bot_generated.o				\
		-o post_signup
	touch ${CGI_HOME}/post_signup
	/bin/rm -f ${CGI_HOME}/post_signup
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_signup			\
	${CGI_HOME}

post_contact: post_contact.c						\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/post_contact.o				\
		${LIBRARY}/bot_generated.o				\
		${LIBRARY}/upload_source.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_contact.c						\
		${LIBRARY}/post_contact.o				\
		${LIBRARY}/bot_generated.o				\
		${LIBRARY}/upload_source.o				\
		-o post_contact
	touch ${CGI_HOME}/post_contact
	/bin/rm -f ${CGI_HOME}/post_contact
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_contact			\
	${CGI_HOME}

table_list: table_list.c 						\
		${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		table_list.c						\
		-o table_list

sql.e: sql.c ${APPASERVER_LIBRARY}
	 ${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		sql.c							\
		-o sql.e
	touch sql
	touch sql_quick.e
	touch sql_quick
	/bin/rm -f sql
	/bin/rm -f sql_quick.e
	/bin/rm -f sql_quick
	ln sql.e sql
	ln sql.e sql_quick.e
	ln sql.e sql_quick

select: select.c ${APPASERVER_LIBRARY}					\
	${LIBRARY}/name_arg.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		select.c						\
		${LIBRARY}/name_arg.o					\
		-o select
	touch get_folder_data
	/bin/rm -f get_folder_data
	ln select get_folder_data
	/bin/rm -f select.e
	ln select select.e

synchronize_mysql_produce: synchronize_mysql_produce.c			\
			${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		synchronize_mysql_produce.c				\
		-o synchronize_mysql_produce

synchronize_mysql_consume: synchronize_mysql_consume.c			\
		${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		synchronize_mysql_consume.c				\
		-o synchronize_mysql_consume

post_choose_role: post_choose_role.c ${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_choose_role.c					\
		-o post_choose_role	
	touch ${CGI_HOME}/post_choose_role
	/bin/rm -f ${CGI_HOME}/post_choose_role
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_choose_role	 	\
	${CGI_HOME}

post_drillthru: post_drillthru.c ${APPASERVER_LIBRARY}			\
		${LIBRARY}/post_drillthru.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_drillthru.c					\
		${LIBRARY}/post_drillthru.o				\
		-o post_drillthru	
	touch ${CGI_HOME}/post_drillthru
	/bin/rm -f ${CGI_HOME}/post_drillthru
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_drillthru	 	\
	${CGI_HOME}

output_table_insert: output_table_insert.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/form_table_insert.o				\
		${LIBRARY}/table_insert.o				\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/vertical_new_table.o				\
		${LIBRARY}/vertical_new_filename.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_table_insert.c					\
		${LIBRARY}/form_table_insert.o				\
		${LIBRARY}/table_insert.o				\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/vertical_new_table.o				\
		${LIBRARY}/vertical_new_filename.o			\
		-o output_table_insert

output_table_edit: output_table_edit.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/table_edit.o					\
		${LIBRARY}/form_table_edit.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_table_edit.c					\
		${LIBRARY}/table_edit.o					\
		${LIBRARY}/form_table_edit.o				\
		-o output_table_edit

generic_load: generic_load.c ${APPASERVER_LIBRARY}			\
	${LIBRARY}/generic_load.o					\
	${LIBRARY}/form_generic_load.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/generic_load.o				\
		${LIBRARY}/form_generic_load.o				\
		generic_load.c						\
		-o generic_load

output_email: output_email.c ${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_email.c						\
		-o output_email

output_spreadsheet: output_spreadsheet.c ${APPASERVER_LIBRARY}		\
		${LIBRARY}/lookup_spreadsheet.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_spreadsheet.c					\
		${LIBRARY}/lookup_spreadsheet.o				\
		-o output_spreadsheet

output_grace_chart: output_grace_chart.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/chart.o					\
		${LIBRARY}/lookup_grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_grace_chart.c					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/chart.o					\
		${LIBRARY}/lookup_grace.o				\
		-o output_grace_chart

output_google_chart: output_google_chart.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/google.o					\
		${LIBRARY}/google_graph.o				\
		${LIBRARY}/lookup_google.o				\
		${LIBRARY}/chart.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_google_chart.c					\
		${LIBRARY}/google.o					\
		${LIBRARY}/google_graph.o				\
		${LIBRARY}/lookup_google.o				\
		${LIBRARY}/chart.o					\
		-o output_google_chart

generic_gracechart: generic_gracechart.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/process_generic_grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_gracechart.c					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/process_generic_grace.o			\
		-o generic_gracechart

generic_high_average_low: generic_high_average_low.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/process_generic_whisker.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_high_average_low.c				\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/process_generic_whisker.o			\
		-o generic_high_average_low

generic_overlay_output: generic_overlay_output.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/scatterplot.o				\
		${LIBRARY}/regression.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_overlay_output.c				\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/scatterplot.o				\
		${LIBRARY}/regression.o					\
		-o generic_overlay_output

generic_googlechart: generic_googlechart.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/google.o					\
		${LIBRARY}/google_graph.o				\
		${LIBRARY}/process_generic_google.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/google.o					\
		${LIBRARY}/google_graph.o				\
		${LIBRARY}/process_generic_google.o			\
		generic_googlechart.c					\
		-o generic_googlechart

output_histogram: output_histogram.c					\
		${APPASERVER_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/lookup_histogram.o				\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/chart.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_histogram.c					\
		${LIBRARY}/lookup_histogram.o				\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/chart.o					\
		-o output_histogram

output_group: output_group.c						\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/group_count.o				\
		${LIBRARY}/query_statistic.o				\
		${LIBRARY}/statistic.o					\
		${LIBRARY}/html.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/group_count.o				\
		${LIBRARY}/query_statistic.o				\
		${LIBRARY}/statistic.o					\
		${LIBRARY}/html.o					\
		output_group.c						\
		-o output_group -lm

generic_histogram: generic_histogram.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/process_generic_histogram.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_histogram.c					\
		${LIBRARY}/process_generic_histogram.o			\
		-o generic_histogram

drop_application: drop_application.c					\
	${APPASERVER_LIBRARY}						\
	${LIBRARY}/application_delete.o					\
	${LIBRARY}/application_backup.o					\
	${LIBRARY}/application_data.o					\
	${LIBRARY}/application_upload.o					\
	${LIBRARY}/application_login.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		drop_application.c					\
		${LIBRARY}/application_delete.o				\
		${LIBRARY}/application_backup.o				\
		${LIBRARY}/application_data.o				\
		${LIBRARY}/application_upload.o				\
		${LIBRARY}/application_login.o				\
		-o drop_application

appaserver_error_filename: appaserver_error_filename.c 			\
		${CORE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		appaserver_error_filename.c				\
		${CORE_LIBRARY}						\
		-o appaserver_error_filename
	touch appaserver_log_filename
	/bin/rm -f appaserver_log_filename
	ln appaserver_error_filename appaserver_log_filename

upload_purge: upload_purge.c 						\
		${CORE_LIBRARY}						\
		${LIBRARY}/upload_purge.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		upload_purge.c						\
		${CORE_LIBRARY}						\
		${LIBRARY}/upload_purge.o				\
		-o upload_purge

database_table: database_table.c					\
		${CORE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${CORE_LIBRARY}						\
		database_table.c					\
		-o database_table
	touch table_name
	/bin/rm -f table_name
	ln database_table table_name
	touch get_table_name
	/bin/rm -f get_table_name
	ln database_table get_table_name

dot_process2option_value: dot_process2option_value.c			\
			${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		dot_process2option_value.c				\
		-o dot_process2option_value

post_prompt_insert: post_prompt_insert.c ${APPASERVER_LIBRARY}		\
	${LIBRARY}/post_prompt_insert.o					\
	${LIBRARY}/vertical_new_prompt.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_prompt_insert.c					\
		${LIBRARY}/post_prompt_insert.o				\
		${LIBRARY}/vertical_new_prompt.o			\
		-o post_prompt_insert
	touch ${CGI_HOME}/post_prompt_insert
	/bin/rm -f ${CGI_HOME}/post_prompt_insert
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_prompt_insert		\
	${CGI_HOME}

post_prompt_lookup: post_prompt_lookup.c ${APPASERVER_LIBRARY}		\
	${LIBRARY}/post_prompt_lookup.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_prompt_lookup.c					\
		${LIBRARY}/post_prompt_lookup.o				\
		-o post_prompt_lookup
	touch ${CGI_HOME}/post_prompt_lookup
	/bin/rm -f ${CGI_HOME}/post_prompt_lookup
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_prompt_lookup		\
	${CGI_HOME}

generic_output_text_file: generic_output_text_file.c			\
			${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_output_text_file.c				\
		-o generic_output_text_file

generic_statistics_report: generic_statistics_report.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/process_generic_statistic.o			\
		${LIBRARY}/html.o					\
		${LIBRARY}/statistic.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_statistics_report.c				\
		${LIBRARY}/process_generic_statistic.o			\
		${LIBRARY}/html.o					\
		${LIBRARY}/statistic.o					\
		-o generic_statistics_report -lm

generic_output_annual_periods: generic_output_annual_periods.c		\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_output_annual_periods.c				\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		-o generic_output_annual_periods

generic_output_annual_comparison: generic_output_annual_comparison.c	\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_output_annual_comparison.c			\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		-o generic_output_annual_comparison

generic_output_exceedance: generic_output_exceedance.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_output_exceedance.c				\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		-o generic_output_exceedance

generic_output_multi_datatype_exceedance:				\
	generic_output_multi_datatype_exceedance.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_output_multi_datatype_exceedance.c		\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		-o generic_output_multi_datatype_exceedance

generic_daily_moving_average: generic_daily_moving_average.c		\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/process_generic_moving.o			\
		${LIBRARY}/process_generic_grace.o			\
		${LIBRARY}/google.o					\
		${LIBRARY}/google_graph.o				\
		${LIBRARY}/html.o					\
		${LIBRARY}/spreadsheet.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_daily_moving_average.c				\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		${LIBRARY}/process_generic_moving.o			\
		${LIBRARY}/process_generic_grace.o			\
		${LIBRARY}/google.o					\
		${LIBRARY}/google_graph.o				\
		${LIBRARY}/html.o					\
		${LIBRARY}/spreadsheet.o				\
		-o generic_daily_moving_average

generic_output_merged_datasets: generic_output_merged_datasets.c	\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/google_graph.o				\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		generic_output_merged_datasets.c			\
		${LIBRARY}/google_graph.o				\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		-o generic_output_merged_datasets

post_choose_isa: post_choose_isa.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/choose_isa.o					\
		${LIBRARY}/form_choose_isa.o				\
		${LIBRARY}/post_choose_isa.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		post_choose_isa.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/choose_isa.o					\
		${LIBRARY}/form_choose_isa.o				\
		${LIBRARY}/post_choose_isa.o				\
		-o post_choose_isa
	touch ${CGI_HOME}/post_choose_isa
	/bin/rm -f ${CGI_HOME}/post_choose_isa
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_choose_isa		\
	${CGI_HOME}

output_prompt_lookup: output_prompt_lookup.c ${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_prompt_lookup.c					\
		-o output_prompt_lookup

output_prompt_insert: output_prompt_insert.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/prompt_insert.o				\
		${LIBRARY}/form_prompt_insert.o				\
		${LIBRARY}/vertical_new_checkbox.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_prompt_insert.c					\
		${LIBRARY}/prompt_insert.o				\
		${LIBRARY}/form_prompt_insert.o				\
		${LIBRARY}/vertical_new_checkbox.o			\
		-o output_prompt_insert
	# Called from a window.
	touch ${CGI_HOME}/output_prompt_insert
	/bin/rm -f ${CGI_HOME}/output_prompt_insert
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/output_prompt_insert		\
	${CGI_HOME}

output_choose_isa: output_choose_isa.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/choose_isa.o					\
		${LIBRARY}/form_choose_isa.o				\
		${LIBRARY}/post_choose_isa.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		output_choose_isa.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/choose_isa.o					\
		${LIBRARY}/form_choose_isa.o				\
		${LIBRARY}/post_choose_isa.o				\
		-o output_choose_isa

view_csv_backup: view_csv_backup.c ${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		view_csv_backup.c					\
		-o view_csv_backup

get_data: get_data.c dynamic_sql.o ${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		dynamic_sql.o						\
		get_data.c						\
		-o get_data

documentation_list: documentation_list.c ${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		documentation_list.c					\
		-o documentation_list

output_choose_role: output_choose_role.c				\
		${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_choose_role.c					\
		-o output_choose_role

post_table_edit: post_table_edit.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/post_table_edit.o				\
		${LIBRARY}/table_edit.o					\
		${LIBRARY}/form_table_edit.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_table_edit.c					\
		${LIBRARY}/post_table_edit.o				\
		${LIBRARY}/table_edit.o					\
		${LIBRARY}/form_table_edit.o				\
		-o post_table_edit
	touch ${CGI_HOME}/post_table_edit
	/bin/rm -f ${CGI_HOME}/post_table_edit
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_table_edit		\
	${CGI_HOME}

post_table_insert: post_table_insert.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/post_table_insert.o				\
		${LIBRARY}/post_prompt_insert.o				\
		${LIBRARY}/vertical_new_post.o				\
		${LIBRARY}/vertical_new_filename.o			\
		${LIBRARY}/vertical_new_prompt.o			\
		${LIBRARY}/vertical_new_checkbox.o			\
		${LIBRARY}/prompt_insert.o				\
		${LIBRARY}/form_prompt_insert.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_table_insert.c					\
		${LIBRARY}/post_table_insert.o				\
		${LIBRARY}/post_prompt_insert.o				\
		${LIBRARY}/vertical_new_post.o				\
		${LIBRARY}/vertical_new_filename.o			\
		${LIBRARY}/vertical_new_prompt.o			\
		${LIBRARY}/vertical_new_checkbox.o			\
		${LIBRARY}/prompt_insert.o				\
		${LIBRARY}/form_prompt_insert.o				\
		-o post_table_insert
	touch ${CGI_HOME}/post_table_insert
	/bin/rm -f ${CGI_HOME}/post_table_insert
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_table_insert		\
	${CGI_HOME}

random_session: random_session.c ${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		random_session.c					\
		-o random_session

appaserver_upgrade: appaserver_upgrade.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/appaserver_upgrade.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		appaserver_upgrade.c					\
		${LIBRARY}/appaserver_upgrade.o				\
		-o appaserver_upgrade
		touch appaserver-upgrade
		/bin/rm -f appaserver-upgrade
		ln appaserver_upgrade appaserver-upgrade

upload_source_file: upload_source_file.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/upload_source.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		upload_source_file.c					\
		${LIBRARY}/upload_source.o				\
		-o upload_source_file

post_choose_folder: post_choose_folder.c				\
		${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_choose_folder.c					\
		-o post_choose_folder
	touch ${CGI_HOME}/post_choose_folder
	/bin/rm -f ${CGI_HOME}/post_choose_folder
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_choose_folder		\
	${CGI_HOME}

post_generic_load_choose: post_generic_load_choose.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/generic_load.o				\
		${LIBRARY}/form_generic_load.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/generic_load.o				\
		${LIBRARY}/form_generic_load.o				\
		post_generic_load_choose.c				\
		-o post_generic_load_choose
	touch ${CGI_HOME}/post_generic_load_choose
	/bin/rm -f ${CGI_HOME}/post_generic_load_choose
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_generic_load_choose	\
	${CGI_HOME}

post_generic_load_folder: post_generic_load_folder.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/generic_load.o				\
		${LIBRARY}/form_generic_load.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/generic_load.o				\
		${LIBRARY}/form_generic_load.o				\
		post_generic_load_folder.c				\
		-o post_generic_load_folder
	touch ${CGI_HOME}/post_generic_load_folder
	/bin/rm -f ${CGI_HOME}/post_generic_load_folder
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_generic_load_folder	\
	${CGI_HOME}

post_choose_process: post_choose_process.c ${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_choose_process.c					\
		-o post_choose_process
	touch ${CGI_HOME}/post_choose_process
	/bin/rm -f ${CGI_HOME}/post_choose_process
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_choose_process		\
	${CGI_HOME}

output_frameset: output_frameset.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/frameset.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_frameset.c					\
		${LIBRARY}/frameset.o					\
		-o output_frameset

output_menu: output_menu.c						\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/frameset.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_menu.c						\
		${LIBRARY}/frameset.o					\
		-o output_menu
	touch ${CGI_HOME}/output_menu
	/bin/rm -f ${CGI_HOME}/output_menu
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/output_menu			\
	${CGI_HOME}

output_prompt_process: output_prompt_process.c ${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		output_prompt_process.c					\
		-o output_prompt_process

post_prompt_process: post_prompt_process.c ${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		post_prompt_process.c					\
		-o post_prompt_process
	touch ${CGI_HOME}/post_prompt_process
	/bin/rm -f ${CGI_HOME}/post_prompt_process
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_prompt_process		\
	${CGI_HOME}

dynamic_sql.o: dynamic_sql.c dynamic_sql.h
	${CC}	-c ${CFLAGS} -I${LIBRARY}				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		dynamic_sql.c

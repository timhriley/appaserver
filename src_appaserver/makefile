# src_appaserver/makefile
# -----------------------

CC=gcc
#WARN=-Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
#WARN=-Wall -Wextra -Wtype-limits ${APPASERVER_UNUSED_PARAMETER_FLAG}
WARN=-Wall -Wextra -Wunused-parameter
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s ${WARN}

LIB=${APPASERVER_HOME}/library

EXECUTABLES=								\
		post_prompt_insert_form					\
		append_reference_number					\
		insert_mysql						\
		post_choose_folder					\
		get_random_session					\
		post_choose_isa_drop_down				\
		output_choose_isa_drop_down				\
		output_edit_table_form					\
		output_insert_table_form				\
		post_edit_table_form					\
		post_maintain_user_account				\
		output_results 						\
		post_login						\
		post_login_cloudacus					\
		fix_orphans						\
		get_folder_data						\
		get_table_name						\
		dot_process2option_value				\
		table_rectification					\
		output_prompt_process_form				\
		post_prompt_process_form				\
		post_choose_process					\
		output_email						\
		output_transmit						\
		output_grace_chart					\
		output_google_chart					\
		output_histogram					\
		output_group						\
		add_column						\
		generic_load						\
		post_generic_load					\
		post_report_writer					\
		merge_purge						\
		alter_column_datatype					\
		rename_table						\
		rename_column						\
		drop_column						\
		drop_table						\
		drop_application					\
		create_application					\
		create_empty_application				\
		create_table						\
		export_subschema					\
		export_process						\
		export_process_set					\
		clone_folder						\
		execute_select_statement				\
		export_application					\
		export_application_spreadsheet				\
		appaserver_folder_list					\
		attribute_list						\
		sql.e							\
		get_appaserver_mount_point				\
		detail							\
		view_source						\
		view_diagrams						\
		view_csv_backup						\
		documentation_list					\
		delete_folder_row					\
		post_delete_folder_block				\
		post_ajax_fill_drop_down				\
		post_change_sort_order					\
		statistics_folder					\
		output_open_html_document				\
		output_close_html_document				\
		output_choose_role_drop_down 				\
		post_choose_role_drop_down		 		\
		output_choose_role_folder_process_form			\
		output_choose_folder_process_menu			\
		maintain_user_account					\
		output_frameset						\
		synchronize_mysql_produce				\
		synchronize_mysql_consume				\
		generic_output_text_file				\
		generic_output_annual_comparison			\
		generic_annual_comparison				\
		generic_statistics_report				\
		generic_gracechart					\
		generic_high_average_low				\
		generic_easychart					\
		generic_googlechart					\
		generic_daily_moving_average				\
		generic_output_annual_periods				\
		generic_output_exceedance				\
		generic_output_multi_datatype_exceedance		\
		generic_histogram					\
		generic_overlay_output					\
		generic_output_merged_datasets				\
		post_merge_purge					\
		post_prompt_edit_form					\
		output_prompt_edit_form					\
		upgrade_appaserver_database				\
		grant_select_to_user					\
		google_map_operation					\
		upload_source_file					\
		output_prompt_insert_form

EXTERNAL_LIBS=	${LIB}/piece.o 						\
		${LIB}/list.o 						\
		${LIB}/list_usage.o 					\
		${LIB}/timlib.o 					\
		${LIB}/String.o 					\
		${LIB}/float.o 						\
		${LIB}/column.o 					\
		${LIB}/date_aggregate.o 				\
		${LIB}/aggregate_level.o 				\
		${LIB}/aggregate_statistic.o 				\
		${LIB}/date.o 						\
		${LIB}/html_table.o 					\
		${LIB}/prompt_recursive.o				\
		${LIB}/dictionary.o 					\
		${LIB}/sed.o 						\
		${LIB}/decode_html_post.o				\
		${LIB}/appaserver_parameter_file.o			\
		${LIB}/appaserver_library.o				\
		${LIB}/appaserver_error.o				\
		${LIB}/application_constants.o				\
		${LIB}/process_generic_output.o				\
		${LIB}/name_arg.o					\
		${LIB}/basename.o					\
		${LIB}/julian.o						\
		${LIB}/fopen_path.o					\
		${LIB}/environ.o					\
		${LIB}/document.o					\
		${LIB}/application.o					\
		${LIB}/appaserver_user.o				\
		${LIB}/process_parameter_list.o				\
		${LIB}/populate_helper_process.o			\
		${LIB}/appaserver_link_file.o				\
		${LIB}/related_folder.o					\
		${LIB}/folder.o						\
		${LIB}/role.o						\
		${LIB}/appaserver.o					\
		${LIB}/dictionary_appaserver.o				\
		${LIB}/query.o						\
		${LIB}/process.o					\
		${LIB}/attribute.o					\
		${LIB}/form.o						\
		${LIB}/element.o					\
		${LIB}/operation.o					\
		${LIB}/role_folder.o					\
		${LIB}/session.o					\
		${LIB}/foreign_attribute.o				\
		${LIB}/frameset.o					\
		${LIB}/insert_database.o				\
		${LIB}/update_database.o				\
		${LIB}/operation_list.o					\
		${LIB}/validation_form.o				\
		${LIB}/post2dictionary.o				\
		${LIB}/dictionary2file.o				\
		${LIB}/date_convert.o					\
		${LIB}/hashtbl.o

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f ${EXECUTABLES}					\
	*.[oe]								\
	${CGI_HOME}/post_delete_folder_block				\
	${CGI_HOME}/post_ajax_fill_drop_down				\
	${CGI_HOME}/post_change_sort_order				\
	${CGI_HOME}/post_login						\
	${CGI_HOME}/post_login_cloudacus				\
	${CGI_HOME}/post_choose_role_drop_down 				\
	${CGI_HOME}/post_prompt_edit_form				\
	${CGI_HOME}/post_prompt_insert_form				\
	${CGI_HOME}/post_choose_isa_drop_down				\
	${CGI_HOME}/output_prompt_insert_form				\
	${CGI_HOME}/post_choose_folder					\
	${CGI_HOME}/post_choose_process					\
	${CGI_HOME}/post_edit_table_form				\
	${CGI_HOME}/post_generic_load					\
	${CGI_HOME}/post_report_writer					\
	${CGI_HOME}/post_merge_purge					\
	${CGI_HOME}/post_maintain_user_account				\
	${CGI_HOME}/post_prompt_process_form				\
	${CGI_HOME}/drop_column						\
	${CGI_HOME}/drop_table						\
	get_appaserver_error_directory					\
	get_appaserver_data_directory					\
	get_document_root						\
	sql								\
	sql_quick							\
	select								\
	view_pdf_files							\
	upgrade-appaserver-database					\
	clone_application

get_appaserver_mount_point: get_appaserver_mount_point.c 		\
	${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		get_appaserver_mount_point.c				\
		-o get_appaserver_mount_point
	touch get_appaserver_home.e
	/bin/rm -f get_appaserver_home.e
	ln get_appaserver_mount_point get_appaserver_home.e
	touch get_appaserver_error_directory
	/bin/rm -f get_appaserver_error_directory
	ln get_appaserver_mount_point get_appaserver_error_directory
	touch get_appaserver_error_directory.e
	/bin/rm -f get_appaserver_error_directory.e
	ln get_appaserver_mount_point get_appaserver_error_directory.e
	touch get_document_root
	/bin/rm -f get_document_root
	ln get_appaserver_mount_point get_document_root
	touch get_document_root.e
	/bin/rm -f get_document_root.e
	ln get_appaserver_mount_point get_document_root.e
	touch get_appaserver_data_directory
	/bin/rm -f get_appaserver_data_directory
	ln get_appaserver_mount_point get_appaserver_data_directory
	touch get_appaserver_data_directory.e
	/bin/rm -f get_appaserver_data_directory.e
	ln get_appaserver_mount_point get_appaserver_data_directory.e

delete_folder_row: delete_folder_row.c					\
	${EXTERNAL_LIBS}						\
	${LIB}/delete_database.o					\
	${LIB}/folder_menu.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		delete_folder_row.c					\
		${LIB}/delete_database.o				\
		${LIB}/folder_menu.o					\
		-o delete_folder_row

statistics_folder: statistics_folder.c					\
	${EXTERNAL_LIBS}						\
	${LIB}/lookup_before_drop_down.o				\
	${LIB}/query_attribute_statistics_list.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/query_attribute_statistics_list.o 		\
		${LIB}/lookup_before_drop_down.o			\
		statistics_folder.c					\
		-o statistics_folder

post_delete_folder_block: post_delete_folder_block.c			\
	${EXTERNAL_LIBS}						\
	${LIB}/query_attribute_statistics_list.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/query_attribute_statistics_list.o 		\
		post_delete_folder_block.c				\
		-o post_delete_folder_block
	touch ${CGI_HOME}/post_delete_folder_block
	/bin/rm -f ${CGI_HOME}/post_delete_folder_block
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_delete_folder_block 	\
	${CGI_HOME}

post_ajax_fill_drop_down: post_ajax_fill_drop_down.c			\
	${EXTERNAL_LIBS}						\
	${LIB}/query_attribute_statistics_list.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/query_attribute_statistics_list.o 		\
		post_ajax_fill_drop_down.c				\
		-o post_ajax_fill_drop_down
	touch ${CGI_HOME}/post_ajax_fill_drop_down
	/bin/rm -f ${CGI_HOME}/post_ajax_fill_drop_down
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_ajax_fill_drop_down 	\
	${CGI_HOME}

post_change_sort_order: post_change_sort_order.c			\
	${EXTERNAL_LIBS}						\
	${LIB}/row_security.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/row_security.o			 		\
		post_change_sort_order.c				\
		-o post_change_sort_order
	touch ${CGI_HOME}/post_change_sort_order
	/bin/rm -f ${CGI_HOME}/post_change_sort_order
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_change_sort_order 	\
	${CGI_HOME}

post_login: post_login.c ${EXTERNAL_LIBS}				\
		${LIB}/post_login_library.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/post_login_library.o				\
		post_login.c						\
		-o post_login
	touch ${CGI_HOME}/post_login
	/bin/rm -f ${CGI_HOME}/post_login
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_login			\
	${CGI_HOME}

post_login_cloudacus: post_login_cloudacus.c				\
		${EXTERNAL_LIBS}					\
		${LIB}/post_login_library.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/post_login_library.o				\
		post_login_cloudacus.c					\
		-o post_login_cloudacus
	touch ${CGI_HOME}/post_login_cloudacus
	/bin/rm -f ${CGI_HOME}/post_login_cloudacus
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_login_cloudacus		\
	${CGI_HOME}

post_email_timriley: post_email_timriley.c				\
		${EXTERNAL_LIBS}					\
		${LIB}/sed.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/sed.o				 		\
		post_email_timriley.c					\
		-o post_email_timriley
	touch ${CGI_HOME}/post_email_timriley
	/bin/rm -f ${CGI_HOME}/post_email_timriley
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_email_timriley		\
	${CGI_HOME}

appaserver_folder_list: appaserver_folder_list.c 			\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		appaserver_folder_list.c				\
		-o appaserver_folder_list

attribute_list: attribute_list.c 					\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		attribute_list.c					\
		-o attribute_list

sql.e: sql.c ${EXTERNAL_LIBS}
	 ${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
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

export_subschema: export_subschema.c ${EXTERNAL_LIBS}			\
	create_clone_filename.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		create_clone_filename.o					\
		export_subschema.c					\
		-o export_subschema

export_process: export_process.c ${EXTERNAL_LIBS}			\
	create_clone_filename.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		create_clone_filename.o					\
		export_process.c					\
		-o export_process

export_process_set: export_process_set.c ${EXTERNAL_LIBS}		\
	create_clone_filename.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		create_clone_filename.o					\
		export_process_set.c					\
		-o export_process_set

clone_folder: clone_folder.c ${EXTERNAL_LIBS}				\
	create_clone_filename.o ${LIB}/appaserver_library.h
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		create_clone_filename.o					\
		clone_folder.c						\
		-o clone_folder

execute_select_statement: execute_select_statement.c ${EXTERNAL_LIBS}	\
		${LIB}/token.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		execute_select_statement.c				\
		${LIB}/token.o						\
		-o execute_select_statement

export_application: export_application.c ${EXTERNAL_LIBS}		\
	create_clone_filename.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		create_clone_filename.o					\
		export_application.c					\
		-o export_application
	touch clone_application
	/bin/rm -f clone_application
	ln export_application clone_application

export_application_spreadsheet: export_application_spreadsheet.c	\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		export_application_spreadsheet.c			\
		-o export_application_spreadsheet

get_folder_data: get_folder_data.c ${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		get_folder_data.c					\
		-o get_folder_data
	touch select
	/bin/rm -f select
	ln get_folder_data select

output_frameset: output_frameset.c					\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_frameset.c					\
		-o output_frameset

synchronize_mysql_produce: synchronize_mysql_produce.c			\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		synchronize_mysql_produce.c				\
		-o synchronize_mysql_produce
#	touch synchronize_mysql_produce_everglades
#	/bin/rm -f synchronize_mysql_produce_everglades
#	ln synchronize_mysql_produce synchronize_mysql_produce_everglades

synchronize_mysql_consume: synchronize_mysql_consume.c			\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		synchronize_mysql_consume.c				\
		-o synchronize_mysql_consume

maintain_user_account: maintain_user_account.c				\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		maintain_user_account.c					\
		-o maintain_user_account

output_prompt_insert_frameset: output_prompt_insert_frameset.c		\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_prompt_insert_frameset.c				\
		-o output_prompt_insert_frameset

output_prompt_edit_frameset: output_prompt_edit_frameset.c		\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_prompt_edit_frameset.c				\
		-o output_prompt_edit_frameset

post_choose_role_drop_down:						\
	post_choose_role_drop_down.c					\
			${LIB}/folder_menu.o				\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/folder_menu.o					\
		post_choose_role_drop_down.c				\
		-o post_choose_role_drop_down
	touch ${CGI_HOME}/post_choose_role_drop_down
	/bin/rm -f ${CGI_HOME}/post_choose_role_drop_down
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_choose_role_drop_down 	\
	${CGI_HOME}

output_choose_role_folder_process_form:					\
	output_choose_role_folder_process_form.c			\
			${EXTERNAL_LIBS}				\
			${LIB}/folder_menu.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/folder_menu.o					\
		output_choose_role_folder_process_form.c		\
		-o output_choose_role_folder_process_form

output_choose_role_folder_process_frameset:				\
	output_choose_role_folder_process_frameset.c			\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_choose_role_folder_process_frameset.c		\
		-o output_choose_role_folder_process_frameset

output_edit_table_frameset: output_edit_table_frameset.c		\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_edit_table_frameset.c				\
		-o output_edit_table_frameset

output_results: output_results.c					\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_results.c					\
		-o output_results

output_insert_table_form: output_insert_table_form.c			\
			${EXTERNAL_LIBS}				\
			${LIB}/pair_one2m.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_insert_table_form.c				\
		${LIB}/pair_one2m.o					\
		-o output_insert_table_form

output_edit_table_form: output_edit_table_form.c			\
			${EXTERNAL_LIBS}				\
			${LIB}/lookup_before_drop_down.o		\
			${LIB}/row_security.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/lookup_before_drop_down.o			\
		${LIB}/row_security.o					\
		output_edit_table_form.c				\
		-o output_edit_table_form

add_column: add_column.c						\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		add_column.c						\
		-o add_column

generic_load: generic_load.c						\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_load.c						\
		-o generic_load

merge_purge: merge_purge.c						\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		merge_purge.c						\
		-o merge_purge

rename_column: rename_column.c						\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		rename_column.c						\
		-o rename_column

output_email: output_email.c						\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_email.c						\
		-o output_email

output_transmit: output_transmit.c					\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_transmit.c					\
		-o output_transmit

output_grace_chart: output_grace_chart.c				\
		${EXTERNAL_LIBS} ${LIB}/grace.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_grace_chart.c					\
		${LIB}/grace.o						\
		-o output_grace_chart

output_google_chart: output_google_chart.c				\
		${EXTERNAL_LIBS} ${LIB}/google_chart.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_google_chart.c					\
		${LIB}/google_chart.o					\
		-o output_google_chart

generic_gracechart: generic_gracechart.c				\
		${EXTERNAL_LIBS} ${LIB}/grace.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_gracechart.c					\
		${LIB}/grace.o						\
		-o generic_gracechart

generic_high_average_low: generic_high_average_low.c			\
		${EXTERNAL_LIBS} ${LIB}/grace.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_high_average_low.c				\
		${LIB}/grace.o						\
		-o generic_high_average_low

generic_overlay_output: generic_overlay_output.c			\
		${EXTERNAL_LIBS}					\
		${LIB}/grace.o						\
		${LIB}/scatterplot.o					\
		${LIB}/regression.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_overlay_output.c				\
		${LIB}/grace.o						\
		${LIB}/scatterplot.o					\
		${LIB}/regression.o					\
		-o generic_overlay_output

generic_easychart: generic_easychart.c					\
		${EXTERNAL_LIBS}					\
		${LIB}/easycharts.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/easycharts.o					\
		generic_easychart.c					\
		-o generic_easychart

generic_googlechart: generic_googlechart.c				\
		${EXTERNAL_LIBS}					\
		${LIB}/google_chart.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/google_chart.o					\
		generic_googlechart.c					\
		-o generic_googlechart

output_histogram: output_histogram.c					\
		${EXTERNAL_LIBS} ${LIB}/grace.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_histogram.c					\
		${LIB}/grace.o						\
		-o output_histogram

output_group: output_group.c						\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_group.c						\
		-o output_group

generic_histogram: generic_histogram.c					\
		${EXTERNAL_LIBS} ${LIB}/grace.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_histogram.c					\
		${LIB}/grace.o						\
		-o generic_histogram

alter_column_datatype: alter_column_datatype.c				\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		alter_column_datatype.c					\
		-o alter_column_datatype

rename_table: rename_table.c						\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		rename_table.c						\
		-o rename_table

drop_column: drop_column.c						\
	${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		drop_column.c						\
		-o drop_column
	touch ${CGI_HOME}/drop_column
	/bin/rm -f ${CGI_HOME}/drop_column
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/drop_column			\
	${CGI_HOME}

drop_table: drop_table.c						\
	${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		drop_table.c						\
		-o drop_table
	touch ${CGI_HOME}/drop_table
	/bin/rm -f ${CGI_HOME}/drop_table
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/drop_table			\
	${CGI_HOME}

drop_application: drop_application.c					\
	${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		drop_application.c					\
		-o drop_application

create_table: create_table.c						\
	${EXTERNAL_LIBS} ${LIB}/appaserver_library.h
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		create_table.c						\
		-o create_table

create_application: create_application.c				\
	${EXTERNAL_LIBS} create_clone_filename.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		create_clone_filename.o					\
		create_application.c					\
		-o create_application

create_empty_application: create_empty_application.c			\
	${EXTERNAL_LIBS}						\
	${LIB}/appaserver_library.h
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		create_empty_application.c				\
		-o create_empty_application

post_prompt_edit_form: post_prompt_edit_form.c 				\
		${EXTERNAL_LIBS}					\
		${LIB}/lookup_before_drop_down.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/lookup_before_drop_down.o			\
		post_prompt_edit_form.c					\
		-o post_prompt_edit_form
	touch ${CGI_HOME}/post_prompt_edit_form
	/bin/rm -f ${CGI_HOME}/post_prompt_edit_form
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_prompt_edit_form		\
	${CGI_HOME}

get_table_name: get_table_name.c 					\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		get_table_name.c					\
		-o get_table_name

dot_process2option_value: dot_process2option_value.c			\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		dot_process2option_value.c				\
		-o dot_process2option_value

post_prompt_insert_form: post_prompt_insert_form.c 			\
			${EXTERNAL_LIBS}				\
			${LIB}/pair_one2m.o				\
			${LIB}/folder_menu.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		post_prompt_insert_form.c				\
		${LIB}/pair_one2m.o					\
		${LIB}/folder_menu.o					\
		-o post_prompt_insert_form
	touch ${CGI_HOME}/post_prompt_insert_form
	/bin/rm -f ${CGI_HOME}/post_prompt_insert_form
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_prompt_insert_form	\
	${CGI_HOME}

table_rectification: table_rectification.c 				\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		table_rectification.c					\
		-o table_rectification

generic_output_text_file: generic_output_text_file.c			\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_output_text_file.c				\
		-o generic_output_text_file

generic_statistics_report: generic_statistics_report.c			\
			${EXTERNAL_LIBS}				\
			${LIB}/query_attribute_statistics_list.o	\
			${LIB}/statistics_weighted.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_statistics_report.c				\
		${LIB}/query_attribute_statistics_list.o 		\
		${LIB}/statistics_weighted.o				\
		-o generic_statistics_report -lm

generic_output_annual_periods: generic_output_annual_periods.c		\
		${EXTERNAL_LIBS}					\
		${LIB}/grace.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_output_annual_periods.c				\
		${LIB}/grace.o						\
		-o generic_output_annual_periods

generic_output_annual_comparison: generic_output_annual_comparison.c	\
		${EXTERNAL_LIBS}					\
		${LIB}/grace.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_output_annual_comparison.c			\
		${LIB}/grace.o						\
		-o generic_output_annual_comparison

generic_annual_comparison: generic_annual_comparison.c			\
			${EXTERNAL_LIBS}				\
			generic_annual_comparison.h
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_annual_comparison.c				\
		-o generic_annual_comparison

generic_output_exceedance: generic_output_exceedance.c			\
		${EXTERNAL_LIBS}					\
		${LIB}/grace.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_output_exceedance.c				\
		${LIB}/grace.o						\
		-o generic_output_exceedance

generic_output_multi_datatype_exceedance:				\
	generic_output_multi_datatype_exceedance.c			\
		${EXTERNAL_LIBS}					\
		${LIB}/grace.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_output_multi_datatype_exceedance.c		\
		${LIB}/grace.o						\
		-o generic_output_multi_datatype_exceedance

generic_daily_moving_average: generic_daily_moving_average.c		\
			${EXTERNAL_LIBS}				\
			${LIB}/grace.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_daily_moving_average.c				\
		${LIB}/grace.o						\
		-o generic_daily_moving_average

generic_output_merged_datasets: generic_output_merged_datasets.c	\
		${EXTERNAL_LIBS}					\
		${LIB}/google_chart.o					\
		${LIB}/grace.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		generic_output_merged_datasets.c			\
		${LIB}/google_chart.o					\
		${LIB}/grace.o						\
		-o generic_output_merged_datasets

post_choose_isa_drop_down: post_choose_isa_drop_down.c 			\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		post_choose_isa_drop_down.c				\
		-o post_choose_isa_drop_down
	touch ${CGI_HOME}/post_choose_isa_drop_down
	/bin/rm -f ${CGI_HOME}/post_choose_isa_drop_down
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_choose_isa_drop_down	\
	${CGI_HOME}

output_prompt_edit_form: output_prompt_edit_form.c 			\
		${EXTERNAL_LIBS}					\
		${LIB}/lookup_before_drop_down.o			\
		${LIB}/row_security.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/lookup_before_drop_down.o			\
		${LIB}/row_security.o					\
		output_prompt_edit_form.c				\
		-o output_prompt_edit_form

output_prompt_insert_form: output_prompt_insert_form.c 			\
		${EXTERNAL_LIBS}					\
		${LIB}/lookup_before_drop_down.o			\
		${LIB}/pair_one2m.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/lookup_before_drop_down.o			\
		${LIB}/pair_one2m.o					\
		output_prompt_insert_form.c				\
		-o output_prompt_insert_form
	# Called from a window. See output_results.c
	touch ${CGI_HOME}/output_prompt_insert_form
	/bin/rm -f ${CGI_HOME}/output_prompt_insert_form
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/output_prompt_insert_form	\
	${CGI_HOME}

output_choose_isa_drop_down: output_choose_isa_drop_down.c		\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_choose_isa_drop_down.c				\
		-o output_choose_isa_drop_down

view_source: view_source.c						\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		view_source.c						\
		-o view_source

view_csv_backup: view_csv_backup.c					\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		view_csv_backup.c					\
		-o view_csv_backup

get_data: get_data.c dynamic_sql.o					\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		dynamic_sql.o						\
		get_data.c						\
		-o get_data

view_diagrams: view_diagrams.c						\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		view_diagrams.c						\
		-o view_diagrams
	touch view_pdf_files
	/bin/rm -f view_pdf_files
	ln view_diagrams view_pdf_files

documentation_list: documentation_list.c				\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		documentation_list.c					\
		-o documentation_list

detail: detail.c ${EXTERNAL_LIBS}					\
			${LIB}/lookup_before_drop_down.o		\
			${LIB}/row_security.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/row_security.o					\
		${LIB}/lookup_before_drop_down.o			\
		detail.c						\
		-o detail

insert_mysql: insert_mysql.c ${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		insert_mysql.c						\
		-o insert_mysql

append_reference_number: append_reference_number.c ${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		append_reference_number.c				\
		-o append_reference_number

output_choose_folder_process_menu: 					\
		output_choose_folder_process_menu.c 			\
		${EXTERNAL_LIBS}					\
		${LIB}/folder_menu.o					\
		${LIB}/horizontal_menu.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_choose_folder_process_menu.c			\
		${LIB}/folder_menu.o					\
		${LIB}/horizontal_menu.o				\
		-o output_choose_folder_process_menu

output_choose_role_menu: output_choose_role_menu.c 			\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_choose_role_menu.c				\
		-o output_choose_role_menu

output_choose_role_drop_down: output_choose_role_drop_down.c		\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_choose_role_drop_down.c				\
		-o output_choose_role_drop_down

output_open_html_document: output_open_html_document.c			\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_open_html_document.c				\
		-o output_open_html_document

output_close_html_document: output_close_html_document.c		\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_close_html_document.c				\
		-o output_close_html_document

post_edit_table_form: post_edit_table_form.c				\
			${EXTERNAL_LIBS}				\
			${LIB}/folder_menu.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		post_edit_table_form.c					\
		${LIB}/folder_menu.o					\
		-o post_edit_table_form
	touch ${CGI_HOME}/post_edit_table_form
	/bin/rm -f ${CGI_HOME}/post_edit_table_form
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_edit_table_form		\
	${CGI_HOME}

post_maintain_user_account: post_maintain_user_account.c		\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		post_maintain_user_account.c				\
		-o post_maintain_user_account
	touch ${CGI_HOME}/post_maintain_user_account
	/bin/rm -f ${CGI_HOME}/post_maintain_user_account
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_maintain_user_account	\
	${CGI_HOME}

get_random_session: get_random_session.c 				\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		get_random_session.c					\
		-o get_random_session

upgrade_appaserver_database: upgrade_appaserver_database.c		\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		upgrade_appaserver_database.c				\
		-o upgrade_appaserver_database
		touch upgrade-appaserver-database
		/bin/rm -f upgrade-appaserver-database
		ln upgrade_appaserver_database upgrade-appaserver-database

upload_source_file: upload_source_file.c				\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		upload_source_file.c					\
		-o upload_source_file

grant_select_to_user: grant_select_to_user.c				\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		grant_select_to_user.c					\
		-o grant_select_to_user

google_map_operation: google_map_operation.c				\
			${EXTERNAL_LIBS}				\
			${LIB}/google_map.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		google_map_operation.c					\
		${LIB}/google_map.o					\
		-o google_map_operation

post_choose_folder: post_choose_folder.c 				\
			${EXTERNAL_LIBS}				\
			${LIB}/lookup_before_drop_down.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/lookup_before_drop_down.o			\
		post_choose_folder.c					\
		-o post_choose_folder
	touch ${CGI_HOME}/post_choose_folder
	/bin/rm -f ${CGI_HOME}/post_choose_folder
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_choose_folder		\
	${CGI_HOME}

post_generic_load: post_generic_load.c					\
		${EXTERNAL_LIBS} ${LIB}/generic_load.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/generic_load.o					\
		post_generic_load.c					\
		-o post_generic_load
	touch ${CGI_HOME}/post_generic_load
	/bin/rm -f ${CGI_HOME}/post_generic_load
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_generic_load		\
	${CGI_HOME}

post_report_writer: post_report_writer.c				\
		${EXTERNAL_LIBS}					\
		${LIB}/report_writer.o					\
		${LIB}/query_attribute_statistics_list.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/report_writer.o					\
		${LIB}/query_attribute_statistics_list.o		\
		post_report_writer.c					\
		-o post_report_writer
	touch ${CGI_HOME}/post_report_writer
	/bin/rm -f ${CGI_HOME}/post_report_writer
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_report_writer		\
	${CGI_HOME}

post_merge_purge: post_merge_purge.c					\
		${EXTERNAL_LIBS}					\
		${LIB}/folder_menu.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		post_merge_purge.c					\
		${LIB}/folder_menu.o					\
		-o post_merge_purge
	touch ${CGI_HOME}/post_merge_purge
	/bin/rm -f ${CGI_HOME}/post_merge_purge
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_merge_purge		\
	${CGI_HOME}

post_choose_process: post_choose_process.c 				\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		post_choose_process.c					\
		-o post_choose_process
	touch ${CGI_HOME}/post_choose_process
	/bin/rm -f ${CGI_HOME}/post_choose_process
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_choose_process		\
	${CGI_HOME}

output_prompt_process_frameset: output_prompt_process_frameset.c	\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		output_prompt_process_frameset.c			\
		-o output_prompt_process_frameset


fix_orphans: fix_orphans.c						\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		fix_orphans.c						\
		-o fix_orphans

output_prompt_process_form: output_prompt_process_form.c		\
		${EXTERNAL_LIBS}					\
		${LIB}/lookup_before_drop_down.o
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${LIB}/lookup_before_drop_down.o			\
		output_prompt_process_form.c				\
		-o output_prompt_process_form

post_prompt_process_form: post_prompt_process_form.c			\
			${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		post_prompt_process_form.c				\
		-o post_prompt_process_form
	touch ${CGI_HOME}/post_prompt_process_form
	/bin/rm -f ${CGI_HOME}/post_prompt_process_form
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_appaserver/post_prompt_process_form	\
	${CGI_HOME}

dynamic_sql.o: dynamic_sql.c dynamic_sql.h
	${CC}	-c ${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		dynamic_sql.c

create_clone_filename.o: create_clone_filename.c			\
	create_clone_filename.h
	${CC}	-c ${CFLAGS} -I${LIB}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		create_clone_filename.c


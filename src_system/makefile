# $APPASERVER_HOME/src_system/makefile
# ------------------------------------

CC=gcc
#WARN=-Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
#WARN=-Wall -Wextra -Wtype-limits ${APPASERVER_UNUSED_PARAMETER_FLAG}
WARN=-Wall -Wextra -Wunused-parameter
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s ${WARN}

LIBRARY=${APPASERVER_HOME}/library

EXECUTABLES=								\
		create_empty_application				\
		delete_application					\
		create_table						\
		add_column						\
		alter_column_datatype					\
		attribute_list						\
		clone_folder						\
		delete_folder_row					\
		drilldown						\
		create_application					\
		drop_column						\
		export_application					\
		export_table						\
		export_process						\
		export_process_set					\
		export_subschema					\
		grant_select_to_user					\
		rename_table						\
		rename_column						\
		drop_table						\
		table_rectification					\
		change_password						\
		appaserver_user_trigger					\
		fix_orphans						\
		merge_purge						\
		post_merge_purge_choose					\
		post_merge_purge_folder					\
		view_source						\
		view_diagrams						\
		ethernet_usage_percent					\
		execute_select_statement				\
		google_map_operation					\
		appaserver_folder_list

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/float.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/String.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hash_table.o					\
		${LIBRARY}/dollar.o

APPASERVER_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/sed.o					\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/String.o 					\
		${LIBRARY}/float.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hash_table.o					\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/security.o					\
		${LIBRARY}/security_entity.o				\
		${LIBRARY}/entity.o					\
		${LIBRARY}/process_generic.o				\
		${LIBRARY}/output_medium.o				\
		${LIBRARY}/units.o					\
		${LIBRARY}/accumulate_boolean.o				\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/basename.o					\
		${LIBRARY}/session.o					\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/folder_attribute.o				\
		${LIBRARY}/environ.o					\
		${LIBRARY}/process_parameter.o				\
		${LIBRARY}/appaserver_link.o				\
		${LIBRARY}/widget.o					\
		${LIBRARY}/file.o					\
		${LIBRARY}/button.o					\
		${LIBRARY}/javascript.o					\
		${LIBRARY}/process.o					\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/operation.o					\
		${LIBRARY}/role_folder.o				\
		${LIBRARY}/folder_operation.o				\
		${LIBRARY}/form.o					\
		${LIBRARY}/dictionary_separate.o			\
		${LIBRARY}/document.o					\
		${LIBRARY}/query.o					\
		${LIBRARY}/query_isa.o					\
		${LIBRARY}/insert.o					\
		${LIBRARY}/update.o					\
		${LIBRARY}/folder_row_level_restriction.o		\
		${LIBRARY}/relation.o					\
		${LIBRARY}/relation_mto1.o				\
		${LIBRARY}/relation_one2m.o				\
		${LIBRARY}/relation_join.o				\
		${LIBRARY}/appaserver_parameter.o			\
		${LIBRARY}/aggregate_level.o				\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/application_constant.o			\
		${LIBRARY}/aggregate_statistic.o			\
		${LIBRARY}/application.o				\
		${LIBRARY}/application_log.o				\
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
		${LIBRARY}/ajax.o					\
		${LIBRARY}/spool.o					\
		${LIBRARY}/row_security_role_update.o			\
		${LIBRARY}/post_dictionary.o				\
		${LIBRARY}/folder_menu.o				\
		${LIBRARY}/menu.o					\
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
	view_pdf_files							\
	${CGI_HOME}/delete_folder_row					\
	${CGI_HOME}/drop_column						\
	${CGI_HOME}/drop_table						\
	${CGI_HOME}/post_merge_purge_choose				\
	${CGI_HOME}/post_merge_purge_folder

clone_folder: clone_folder.c ${APPASERVER_LIBRARY}			\
	${LIBRARY}/clone_folder.o					\
	${LIBRARY}/shell_script.o					\
	${LIBRARY}/html.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		clone_folder.c						\
		${LIBRARY}/clone_folder.o				\
		${LIBRARY}/shell_script.o				\
		${LIBRARY}/html.o					\
		-o clone_folder

attribute_list: attribute_list.c 					\
		${APPASERVER_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		attribute_list.c					\
		-o attribute_list

alter_column_datatype: alter_column_datatype.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/alter_datatype.o				\
		${LIBRARY}/shell_script.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/alter_datatype.o				\
		${LIBRARY}/shell_script.o				\
		alter_column_datatype.c					\
		-o alter_column_datatype

add_column: add_column.c ${APPASERVER_LIBRARY}				\
	${LIBRARY}/add_column.o						\
	${LIBRARY}/shell_script.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/add_column.o					\
		${LIBRARY}/shell_script.o				\
		add_column.c						\
		-o add_column

export_process: export_process.c ${APPASERVER_LIBRARY}			\
	${LIBRARY}/export_process.o					\
	${LIBRARY}/shell_script.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/export_process.o				\
		${LIBRARY}/shell_script.o				\
		export_process.c					\
		-o export_process

create_empty_application: create_empty_application.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/application_clone.o				\
		${LIBRARY}/application_create.o				\
		${LIBRARY}/application_backup.o				\
		${LIBRARY}/application_data.o				\
		${LIBRARY}/application_upload.o				\
		${LIBRARY}/application_login.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		create_empty_application.c				\
		${LIBRARY}/application_clone.o				\
		${LIBRARY}/application_create.o				\
		${LIBRARY}/application_backup.o				\
		${LIBRARY}/application_data.o				\
		${LIBRARY}/application_upload.o				\
		${LIBRARY}/application_login.o				\
		-o create_empty_application

delete_application: delete_application.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/application_clone.o				\
		${LIBRARY}/application_create.o				\
		${LIBRARY}/application_backup.o				\
		${LIBRARY}/application_data.o				\
		${LIBRARY}/application_upload.o				\
		${LIBRARY}/application_login.o				\
		${LIBRARY}/application_delete.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		delete_application.c					\
		${LIBRARY}/application_clone.o				\
		${LIBRARY}/application_create.o				\
		${LIBRARY}/application_backup.o				\
		${LIBRARY}/application_data.o				\
		${LIBRARY}/application_upload.o				\
		${LIBRARY}/application_login.o				\
		${LIBRARY}/application_delete.o				\
		-o delete_application

create_table: create_table.c						\
	${APPASERVER_LIBRARY}						\
	${LIBRARY}/create_table.o					\
	${LIBRARY}/shell_script.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/create_table.o				\
		${LIBRARY}/shell_script.o				\
		create_table.c						\
		-o create_table

delete_folder_row: delete_folder_row.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/delete.o					\
		${LIBRARY}/delete_primary.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		delete_folder_row.c					\
		${LIBRARY}/delete.o					\
		${LIBRARY}/delete_primary.o				\
		-o delete_folder_row
	touch ${CGI_HOME}/delete_folder_row
	/bin/rm -f ${CGI_HOME}/delete_folder_row

drilldown: drilldown.c							\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/drilldown.o					\
		${LIBRARY}/table_edit.o					\
		${LIBRARY}/form_table_edit.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		drilldown.c						\
		${LIBRARY}/drilldown.o					\
		${LIBRARY}/table_edit.o					\
		${LIBRARY}/form_table_edit.o				\
		-o drilldown

create_application: create_application.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/application_clone.o				\
		${LIBRARY}/application_create.o				\
		${LIBRARY}/application_backup.o				\
		${LIBRARY}/application_data.o				\
		${LIBRARY}/application_upload.o				\
		${LIBRARY}/application_login.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		create_application.c					\
		${LIBRARY}/application_clone.o				\
		${LIBRARY}/application_create.o				\
		${LIBRARY}/application_backup.o				\
		${LIBRARY}/application_data.o				\
		${LIBRARY}/application_upload.o				\
		${LIBRARY}/application_login.o				\
		-o create_application

drop_column: drop_column.c						\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/drop_column.o				\
		${LIBRARY}/shell_script.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		drop_column.c						\
		${LIBRARY}/drop_column.c				\
		${LIBRARY}/shell_script.c				\
		-o drop_column
	touch ${CGI_HOME}/drop_column
	/bin/rm -f ${CGI_HOME}/drop_column
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_system/drop_column			\
	${CGI_HOME}

export_application: export_application.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/export_application.o				\
		${LIBRARY}/export_table.o				\
		${LIBRARY}/shell_script.o				\
		${LIBRARY}/html.o					\
		${LIBRARY}/pkzip.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		export_application.c					\
		${LIBRARY}/export_application.o				\
		${LIBRARY}/export_table.o				\
		${LIBRARY}/shell_script.o				\
		${LIBRARY}/html.o					\
		${LIBRARY}/pkzip.o					\
		-o export_application

export_table: export_table.c						\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/export_table.o				\
		${LIBRARY}/shell_script.o				\
		${LIBRARY}/html.o					\
		${LIBRARY}/pkzip.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		export_table.c						\
		${LIBRARY}/export_table.o				\
		${LIBRARY}/shell_script.o				\
		${LIBRARY}/html.o					\
		${LIBRARY}/pkzip.o					\
		-o export_table

export_subschema: export_subschema.c ${APPASERVER_LIBRARY}		\
	${LIBRARY}/export_subschema.o					\
	${LIBRARY}/shell_script.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		export_subschema.c					\
		${LIBRARY}/export_subschema.o				\
		${LIBRARY}/shell_script.o				\
		-o export_subschema

export_process_set: export_process_set.c ${APPASERVER_LIBRARY}		\
	${LIBRARY}/export_process_set.o					\
	${LIBRARY}/shell_script.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		export_process_set.c					\
		${LIBRARY}/export_process_set.o				\
		${LIBRARY}/shell_script.o				\
		-o export_process_set

grant_select_to_user: grant_select_to_user.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/grant_select.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		grant_select_to_user.c					\
		${LIBRARY}/grant_select.o				\
		-o grant_select_to_user

rename_column: rename_column.c ${APPASERVER_LIBRARY}			\
	${LIBRARY}/rename_column.o					\
	${LIBRARY}/shell_script.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/rename_column.o				\
		${LIBRARY}/shell_script.o				\
		rename_column.c						\
		-o rename_column

rename_table: rename_table.c ${APPASERVER_LIBRARY}			\
	${LIBRARY}/rename_table.o					\
	${LIBRARY}/shell_script.o					\
	${LIBRARY}/create_table.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/rename_table.o				\
		${LIBRARY}/shell_script.o				\
		${LIBRARY}/create_table.o				\
		rename_table.c						\
		-o rename_table

drop_table: drop_table.c						\
	${APPASERVER_LIBRARY}						\
	${LIBRARY}/drop_table.o						\
	${LIBRARY}/shell_script.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		drop_table.c						\
		${LIBRARY}/drop_table.o					\
		${LIBRARY}/shell_script.o				\
		-o drop_table
	touch ${CGI_HOME}/drop_table
	/bin/rm -f ${CGI_HOME}/drop_table
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_system/drop_table			\
	${CGI_HOME}

table_rectification: table_rectification.c 				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/rectification.o				\
		${LIBRARY}/drop_table.o					\
		${LIBRARY}/drop_column.o				\
		${LIBRARY}/shell_script.o				\
		${LIBRARY}/html.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		table_rectification.c					\
		${LIBRARY}/rectification.o				\
		${LIBRARY}/drop_table.o					\
		${LIBRARY}/drop_column.o				\
		${LIBRARY}/shell_script.o				\
		${LIBRARY}/html.o					\
		-o table_rectification

appaserver_user_trigger: appaserver_user_trigger.c 			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/preupdate_change.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		appaserver_user_trigger.c				\
		${LIBRARY}/preupdate_change.o				\
		-o appaserver_user_trigger

change_password: change_password.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/change_password.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		change_password.c					\
		${LIBRARY}/change_password.o				\
		-o change_password

fix_orphans: fix_orphans.c						\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/orphan.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		fix_orphans.c						\
		${LIBRARY}/orphan.o					\
		-o fix_orphans

merge_purge: merge_purge.c ${APPASERVER_LIBRARY}			\
	${LIBRARY}/merge_purge.o					\
	${LIBRARY}/form_merge_purge.o					\
	${LIBRARY}/delete.o						\
	${LIBRARY}/delete_primary.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/merge_purge.o				\
		${LIBRARY}/form_merge_purge.o				\
		${LIBRARY}/delete.o					\
		${LIBRARY}/delete_primary.o				\
		merge_purge.c						\
		-o merge_purge

post_merge_purge_choose: post_merge_purge_choose.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/merge_purge.o				\
		${LIBRARY}/form_merge_purge.o				\
		${LIBRARY}/delete.o					\
		${LIBRARY}/delete_primary.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/merge_purge.o				\
		${LIBRARY}/form_merge_purge.o				\
		${LIBRARY}/delete.o					\
		${LIBRARY}/delete_primary.o				\
		post_merge_purge_choose.c				\
		-o post_merge_purge_choose
	touch ${CGI_HOME}/post_merge_purge_choose
	/bin/rm -f ${CGI_HOME}/post_merge_purge_choose
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_system/post_merge_purge_choose		\
	${CGI_HOME}

post_merge_purge_folder: post_merge_purge_folder.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/merge_purge.o				\
		${LIBRARY}/form_merge_purge.o				\
		${LIBRARY}/delete.o					\
		${LIBRARY}/delete_primary.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/merge_purge.o				\
		${LIBRARY}/form_merge_purge.o				\
		${LIBRARY}/delete.o					\
		${LIBRARY}/delete_primary.o				\
		post_merge_purge_folder.c				\
		-o post_merge_purge_folder
	touch ${CGI_HOME}/post_merge_purge_folder
	/bin/rm -f ${CGI_HOME}/post_merge_purge_folder
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_system/post_merge_purge_folder		\
	${CGI_HOME}

view_source: view_source.c ${APPASERVER_LIBRARY}			\
		${LIBRARY}/view_source.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		view_source.c						\
		${LIBRARY}/view_source.o				\
		-o view_source

view_diagrams: view_diagrams.c	${APPASERVER_LIBRARY}			\
		${LIBRARY}/view_source.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		view_diagrams.c						\
		${LIBRARY}/view_source.o				\
		-o view_diagrams
	touch view_pdf_files
	/bin/rm -f view_pdf_files
	ln view_diagrams view_pdf_files

ethernet_usage_percent:							\
		ethernet_usage_percent.c				\
		${APPASERVER_LIBRARY}					\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY} 					\
		${EXTERNAL_LIBS}					\
		ethernet_usage_percent.c				\
		-o ethernet_usage_percent

appaserver_folder_list:							\
		appaserver_folder_list.c				\
		${APPASERVER_LIBRARY}					\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY} 					\
		${EXTERNAL_LIBS}					\
		appaserver_folder_list.c				\
		-o appaserver_folder_list

execute_select_statement: execute_select_statement.c			\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/execute_select.o				\
		${LIBRARY}/select_statement.o				\
		${LIBRARY}/token.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		execute_select_statement.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/execute_select.o				\
		${LIBRARY}/select_statement.o				\
		${LIBRARY}/token.o					\
		-o execute_select_statement

google_map_operation: google_map_operation.c				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/google.o					\
		${LIBRARY}/google_map.o					\
		${LIBRARY}/google_map_operation.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		google_map_operation.c					\
		${LIBRARY}/google.o					\
		${LIBRARY}/google_map.o					\
		${LIBRARY}/google_map_operation.o			\
		-o google_map_operation


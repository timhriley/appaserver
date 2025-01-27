#!/bin/bash
if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "$ . set_database" 1>&2
	exit 1
fi

(
cat << shell_all_done
insert into appaserver_table (table_name,form,insert_rows_number,drillthru_yn,notepad,populate_drop_down_process,post_change_process,post_change_javascript,javascript_filename,subschema,storage_engine,exclude_application_export_yn,no_initial_capital_yn,data_directory,index_directory,create_view_statement,html_help_file_anchor) values ('storage_engine','table','5',null,null,null,null,null,null,'schema',null,null,null,null,null,null,null);
insert into relation (table_name,related_table,related_column,pair_one2m_order,omit_drillthru_yn,omit_drilldown_yn,relation_type_isa_yn,copy_common_columns_yn,automatic_preselection_yn,drop_down_multi_select_yn,join_one2m_each_row_yn,ajax_fill_drop_down_yn,omit_ajax_fill_drop_down_yn,trigger_insert_update_yn,hint_message) values ('table','storage_engine','null',null,null,null,null,null,null,null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn,default_value) values ('storage_engine','storage_engine','1',null,null,null,null,null,null,null,null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('storage_engine','text','10',null,null);
insert into table_operation (table_name,role,operation) values ('storage_engine','system','delete');
insert into table_operation (table_name,role,operation) values ('storage_engine','system','drilldown');
insert into operation (operation,output_yn) values ('delete','n');
insert into process (process,command_line) values ('delete','delete_folder_row $session $login_name $role $folder $primary_data_list n');
insert into operation (operation,output_yn) values ('drilldown','y');
insert into process (process,command_line) values ('drilldown','drilldown $session $login_name $role $target_frame $folder $primary_data_list $update_results $update_error $dictionary');
insert into table_operation (table_name,role,operation) values ('storage_engine','system','delete');
insert into table_operation (table_name,role,operation) values ('storage_engine','system','drilldown');
insert into role_table (role,table_name,permission) values ('system','storage_engine','insert');
insert into role_table (role,table_name,permission) values ('system','storage_engine','update');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

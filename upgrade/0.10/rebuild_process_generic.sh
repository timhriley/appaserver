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
insert into folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,drillthru_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,data_directory,index_directory,javascript_filename) values ('process_generic','table','5','process_generic',null,null,null,null,null,null,null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic','process','1',null,null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic','process_set','2',null,null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic','value_attribute',null,'2',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic','value_folder',null,'1',null,null,null,null,null,null,null);
insert into folder_operation (folder,role,operation) values ('process_generic','system','delete');
insert into folder_operation (folder,role,operation) values ('process_generic','system','drilldown');
insert into role_folder (role,folder,permission) values ('system','process_generic','insert');
insert into role_folder (role,folder,permission) values ('system','process_generic','update');

insert into folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,drillthru_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,data_directory,index_directory,javascript_filename) values ('process_generic_value','table','3','process_generic',null,null,null,null,null,null,null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','datatype_aggregation_sum_yn',null,'6',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','datatype_attribute',null,'3',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','datatype_bar_graph_yn',null,'7',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','datatype_folder',null,'1',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','datatype_scale_graph_zero_yn',null,'8',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','datatype_units_yn',null,'9',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','date_attribute',null,'4',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','foreign_folder',null,'2',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','foreign_units_yn',null,'10',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','time_attribute',null,'5',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','value_attribute','2',null,null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value','value_folder','1',null,null,null,null,null,null,null,null);
insert into folder_operation (folder,role,operation) values ('process_generic_value','system','delete');
insert into folder_operation (folder,role,operation) values ('process_generic_value','system','drilldown');
insert into role_folder (role,folder,permission) values ('system','process_generic_value','insert');
insert into role_folder (role,folder,permission) values ('system','process_generic_value','update');
update attribute set width = 60 where attribute = 'value_attribute';
update attribute set width = 60 where attribute = 'date_attribute';
update attribute set width = 60 where attribute = 'time_attribute';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0

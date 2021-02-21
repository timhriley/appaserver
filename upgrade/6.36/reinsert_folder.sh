#!/bin/sh
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
folder="folder"
relation="relation"
attribute="attribute"
folder_attribute="folder_attribute"
role_folder="role_folder"
row_security_role_update="row_security_role_update"
folder_row_level_restrictions="folder_row_level_restrictions"
subschemas="subschemas"
role_operation="role_operation"
javascript_folders="javascript_folders"
javascript_files="javascript_files"
process="process"
role="role"

(
cat << all_done
delete from $folder where folder = 'folder';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('folder','prompt','5','folder','folder_list',null,null,null,null,null,null,null,null,null,'y',null,null);
delete from $relation where folder = 'folder';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('folder','form','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('folder','process','populate_drop_down_process',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('folder','process','post_change_process',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('folder','subschemas','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('folder_attribute','folder','null','1',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('folder_row_level_restrictions','folder','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('process_parameter','folder','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('process_set_parameter','folder','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('relation','folder','null','2',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('relation','folder','related_folder',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('role_folder','folder','null','3',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('role_operation','folder','null','4',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('process_generic_datatype_folder','folder','datatype_folder',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('process_generic_value_folder','folder','foreign_folder',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('javascript_folders','folder','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('process_generic_value_folder','folder','value_folder',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('additional_user_drop_down_attribute','folder','drop_down_folder',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('additional_drop_down_attribute','folder','drop_down_folder',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'folder';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('folder','text','35',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'form';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('form','text','6',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'insert_rows_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('insert_rows_number','integer','2',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'subschema';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('subschema','text','30',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'populate_drop_down_process';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('populate_drop_down_process','text','40',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'post_change_process';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('post_change_process','text','40',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'post_change_javascript';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('post_change_javascript','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'lookup_before_drop_down_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('lookup_before_drop_down_yn','text','1','0',null,null,null,null,null,'y');
delete from $attribute where attribute = 'notepad';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('notepad','notepad','4096',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'no_initial_capital_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('no_initial_capital_yn','text','1','0',null,null,null,null,null,'y');
delete from $attribute where attribute = 'create_view_statement';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('create_view_statement','notepad','4096','0',null,null,null,'n','n','y');
delete from $attribute where attribute = 'lookup_email_output_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('lookup_email_output_yn','text','1',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'html_help_file_anchor';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('html_help_file_anchor','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'exclude_application_export_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('exclude_application_export_yn','text','1',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'appaserver_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('appaserver_yn','text','1',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'data_directory';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('data_directory','text','50','0',null,null,null,null,null,'y');
delete from $attribute where attribute = 'index_directory';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('index_directory','text','50','0',null,null,null,null,null,'y');
delete from $folder_attribute where folder = 'folder';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','appaserver_yn',null,'14',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','create_view_statement','0','10','n','n','n','n','n','n','n');
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','data_directory','0','15','n','n',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','exclude_application_export_yn',null,'13',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','folder','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','form',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','html_help_file_anchor',null,'12',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','index_directory','0','16','n','n',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','insert_rows_number',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','lookup_before_drop_down_yn',null,'7',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','lookup_email_output_yn',null,'11',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','notepad',null,'8',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','no_initial_capital_yn',null,'9',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','populate_drop_down_process',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','post_change_javascript',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','post_change_process',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('folder','subschema',null,'3',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('folder','system','insert');
insert into $role_folder (folder,role,permission) values ('folder','system','update');
delete from $javascript_folders where folder = 'folder';
delete from $process where process = 'folder_list';
insert into $process (process,command_line,process_group,notepad,post_change_javascript,process_set_display,preprompt_help_text,html_help_file_anchor,appaserver_yn) values ('folder_list','folder_list.sh \$application',null,null,null,null,null,null,null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('system','y','y');
insert into $subschemas (subschema) values ('folder');
insert into $role_operation (folder,role,operation) values ('folder','system','delete');
insert into $role_operation (folder,role,operation) values ('folder','system','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0

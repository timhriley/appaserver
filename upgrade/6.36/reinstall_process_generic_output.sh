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
delete from $folder where folder = 'process_generic_output';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('process_generic_output','table','5','process_generic_output',null,null,null,null,null,null,null,null,null,null,'y',null,null);
delete from $relation where folder = 'process_generic_output';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('process_generic_output','process','null',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('process_generic_output','process_generic_value_folder','value_folder',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('process_generic_output','process_set','null',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'process';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('process','text','40',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'process_set';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('process_set','text','40',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'value_folder';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('value_folder','text','50',null,null,null,null,null,null,'y');
delete from $folder_attribute where folder = 'process_generic_output';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_output','process','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_output','process_set','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_output','value_folder',null,'1',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('process_generic_output','system','insert');
insert into $role_folder (folder,role,permission) values ('process_generic_output','system','update');
delete from $javascript_folders where folder = 'process_generic_output';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('system','y','y');
insert into $subschemas (subschema) values ('process_generic_output');
insert into $role_operation (folder,role,operation) values ('process_generic_output','system','delete');
insert into $role_operation (folder,role,operation) values ('process_generic_output','system','detail');
delete from $folder where folder = 'process_generic_value_folder';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('process_generic_value_folder','table','3','process_generic_output',null,null,null,null,null,null,null,null,null,null,'y',null,null);
delete from $relation where folder = 'process_generic_value_folder';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('process_generic_value_folder','attribute','datatype_attribute',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('process_generic_value_folder','attribute','date_attribute',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('process_generic_value_folder','attribute','time_attribute',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('process_generic_value_folder','attribute','value_attribute',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('process_generic_value_folder','folder','datatype_folder',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('process_generic_value_folder','folder','foreign_folder',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('process_generic_value_folder','folder','value_folder',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('process_generic_output','process_generic_value_folder','value_folder',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'value_folder';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('value_folder','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'datatype_folder';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('datatype_folder','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'foreign_folder';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('foreign_folder','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'datatype_attribute';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('datatype_attribute','text','60',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'date_attribute';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('date_attribute','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'time_attribute';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('time_attribute','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'value_attribute';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('value_attribute','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'datatype_aggregation_sum_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('datatype_aggregation_sum_yn','text','1',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'datatype_bar_graph_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('datatype_bar_graph_yn','text','1',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'datatype_scale_graph_zero_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('datatype_scale_graph_zero_yn','text','1',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'datatype_units_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('datatype_units_yn','text','1',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'foreign_units_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('foreign_units_yn','text','1',null,null,null,null,null,null,'y');
delete from $folder_attribute where folder = 'process_generic_value_folder';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','datatype_aggregation_sum_yn',null,'7',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','datatype_attribute',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','datatype_bar_graph_yn',null,'8',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','datatype_folder',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','datatype_scale_graph_zero_yn',null,'9',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','datatype_units_yn',null,'10',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','date_attribute',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','foreign_folder',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','foreign_units_yn',null,'11',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','time_attribute',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','value_attribute',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('process_generic_value_folder','value_folder','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('process_generic_value_folder','system','insert');
insert into $role_folder (folder,role,permission) values ('process_generic_value_folder','system','update');
delete from $javascript_folders where folder = 'process_generic_value_folder';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('system','y','y');
insert into $subschemas (subschema) values ('process_generic_output');
insert into $role_operation (folder,role,operation) values ('process_generic_value_folder','system','delete');
insert into $role_operation (folder,role,operation) values ('process_generic_value_folder','system','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

echo "select process,process_set,value_folder from process_generic_output;" | sql > /tmp/process_generic_output.dat

table_name=process_generic_output
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (process char (40) not null,process_set char (40) not null,value_folder char (50)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (process,process_set);" | sql.e

cat /tmp/process_generic_output.dat | insert_statement.e table=$table_name field=process,process_set,value_folder delimiter='^' | sql

table_name=process_generic_value_folder
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (value_folder char (50) not null,datatype_folder char (50),foreign_folder char (50),datatype_attribute char (60),date_attribute char (50),time_attribute char (50),value_attribute char (50),datatype_aggregation_sum_yn char (1),datatype_bar_graph_yn char (1),datatype_scale_graph_zero_yn char (1),datatype_units_yn char (1),foreign_units_yn char (1)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (value_folder);" | sql.e

if [ "$application" = "modeland" ]
then
	echo "model_results^datatype^alternative_station_datatype^datatype^date^time^value^y^y^y^y^n" | insert_statement.e table=$table_name delimiter='^' | sql

fi

if [ "$application" = "waterquality" ]
then
	echo "results^parameter^station_parameter^parameter^collection_date^collection_time^concentration^n^n^n^n^y" | insert_statement.e table=$table_name delimiter='^' | sql

fi

exit 0

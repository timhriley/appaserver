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

if [ "$application" != "waterquality" ]
then
	exit 0
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
delete from $folder where folder = 'station_alias';
insert into $folder (folder,form,insert_rows_number,populate_drop_down_process,lookup_email_output_yn,lookup_transmit_output_yn,notepad,html_help_file_anchor,post_change_javascript,post_change_process,no_initial_capital_yn,row_access_count,exclude_application_export_yn,lookup_before_drop_down_yn,appaserver_yn,data_directory,subschema,index_directory,create_view_statement) values ('station_alias','prompt','5',null,null,null,null,null,null,null,null,null,null,null,null,null,'static',null,null);
delete from $relation where folder = 'station_alias';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('station_alias','station','null',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'station_alias';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('station_alias','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'station';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('station','text','12',null,null,null,null,null,null,'n');
delete from $folder_attribute where folder = 'station_alias';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('station_alias','station',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('station_alias','station_alias','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('station_alias','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('station_alias','supervisor','update');
delete from $javascript_folders where folder = 'station_alias';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('station_alias','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('station_alias','supervisor','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0

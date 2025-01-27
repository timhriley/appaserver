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

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
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
delete from $folder where folder = 'liability_account_entity';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('liability_account_entity','prompt','5','entity',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'liability_account_entity';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('liability_account_entity','account','null',null,null,null,null,null,'y',null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('liability_account_entity','entity','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'liability_account_entity';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('liability_account_entity','account','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('liability_account_entity','full_name',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('liability_account_entity','street_address',null,'2',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('liability_account_entity','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('liability_account_entity','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('liability_account_entity','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('liability_account_entity','supervisor','update');
delete from $javascript_folders where folder = 'liability_account_entity';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('entity');
insert into $role_operation (folder,role,operation) values ('liability_account_entity','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('liability_account_entity','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('liability_account_entity','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('liability_account_entity','supervisor','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0

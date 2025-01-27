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

# Returns 0 if ENTITY.full_name exists
# ------------------------------------
folder_attribute_exists.sh	$application		\
				entity			\
				full_name

if [ $? -eq 0 ]
then
	exit 0
fi

(
cat << shell_all_done
insert into subschema (subschema) values ('entity');
insert into folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,drillthru_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,data_directory,index_directory,javascript_filename) values ('entity','prompt','5','entity',null,null,null,null,null,null,null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','cell_number',null,'6',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','city',null,'2',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','email_address',null,'7',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','full_name','1',null,null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','notepad',null,'8',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','phone_number',null,'5',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','state_code',null,'3',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn,default_value) values ('entity','street_address','2',null,null,null,null,null,null,null,null,'null');
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','website_address',null,'9',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','website_login',null,'10',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','website_password',null,'11',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','zip_code',null,'4',null,null,null,null,null,null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('cell_number','text','15',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('city','text','20',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('email_address','text','50',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('full_name','text','60',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('notepad','notepad','4096',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('phone_number','text','15',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('state_code','text','2',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('street_address','text','60',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('website_address','text','50',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('website_login','text','50',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('website_password','password','20',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('zip_code','text','10',null,null);
insert into folder_operation (folder,role,operation) values ('entity','supervisor','delete');
insert into folder_operation (folder,role,operation) values ('entity','supervisor','drilldown');
insert into role_folder (folder,role,permission) values ('entity','supervisor','insert');
insert into role_folder (folder,role,permission) values ('entity','supervisor','update');

create table entity (full_name char (60) not null,street_address char (60) not null,city char (20),state_code char (2),zip_code char (10),phone_number char (15),cell_number char (15),email_address char (50),notepad text,website_address char (50),website_login char (50),website_password char (20)) engine MyISAM;
create unique index entity on entity (full_name,street_address);
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate


exit 0

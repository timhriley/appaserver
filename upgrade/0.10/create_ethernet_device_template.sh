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

if [ "$application" != "template" ]
then
	exit 0
fi

(
cat << shell_all_done
insert into folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,drillthru_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,data_directory,index_directory,javascript_filename) values ('ethernet_device','prompt','5','ethernet',null,null,null,null,null,null,null,null,null,null,null,null,null);
insert into relation (folder,related_folder,related_attribute,pair_one2m_order,omit_drilldown_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_drillthru_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,hint_message,join_one2m_each_row_yn) values ('ethernet_device_snapshot','ethernet_device','null',null,null,null,null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('ethernet_device','interface',null,'1',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('ethernet_device','IP_address','1',null,null,null,null,null,null,null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('interface','text','10',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('IP_address','text','15',null,null);
insert into folder_operation (folder,role,operation) values ('ethernet_device','supervisor','delete');
insert into folder_operation (folder,role,operation) values ('ethernet_device','supervisor','drilldown');
insert into role_folder (folder,role,permission) values ('ethernet_device','supervisor','insert');
insert into role_folder (folder,role,permission) values ('ethernet_device','supervisor','update');
insert into folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,drillthru_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,data_directory,index_directory,javascript_filename) values ('ethernet_device_snapshot','prompt','5','ethernet',null,null,null,null,null,null,null,null,null,null,null,null,null);
insert into relation (folder,related_folder,related_attribute,pair_one2m_order,omit_drilldown_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_drillthru_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,hint_message,join_one2m_each_row_yn) values ('ethernet_device_snapshot','ethernet_device','null',null,null,null,null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('ethernet_device_snapshot','date','1',null,null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('ethernet_device_snapshot','IP_address','2',null,null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('ethernet_device_snapshot','receive_daily_megabytes',null,'3',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('ethernet_device_snapshot','receive_megabytes',null,'1',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('ethernet_device_snapshot','transmit_daily_megabytes',null,'5',null,null,null,null,null,null,null);
insert into folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('ethernet_device_snapshot','transmit_megabytes',null,'2',null,null,null,null,null,null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('date','current_date','10',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('IP_address','text','15',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('receive_daily_megabytes','integer','5',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('receive_megabytes','integer','10',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('transmit_daily_megabytes','integer','5',null,null);
insert into attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message) values ('transmit_megabytes','integer','10',null,null);
insert into folder_operation (folder,role,operation) values ('ethernet_device_snapshot','supervisor','delete');
insert into folder_operation (folder,role,operation) values ('ethernet_device_snapshot','supervisor','drilldown');
insert into role_folder (folder,role,permission) values ('ethernet_device_snapshot','supervisor','insert');
insert into role_folder (folder,role,permission) values ('ethernet_device_snapshot','supervisor','update');
insert into subschema (subschema) values ('ethernet');
create table ethernet_device (IP_address char (15) not null,interface char (10)) engine MyISAM;
create unique index ethernet_device on ethernet_device (IP_address);
create table ethernet_device_snapshot (date date not null,IP_address char (15) not null,receive_megabytes integer,transmit_megabytes integer,receive_daily_megabytes integer,transmit_daily_megabytes integer) engine MyISAM;
create unique index ethernet_device_snapshot on ethernet_device_snapshot (date,IP_address);
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0

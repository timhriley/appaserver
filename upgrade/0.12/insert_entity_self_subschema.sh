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

# Returns 0 if self.full_name exists in TABLE_COLUMN
# --------------------------------------------------
table_column_exists.sh self full_name

if [ $? -eq 0 ]
then
	exit 0
fi

(
cat << shell_all_done
insert into appaserver_table (table_name,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,drillthru_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,data_directory,javascript_filename,index_directory) values ('self','table','1','entity',null,null,null,null,'<ol><li>One row for the entity running Appaserver<li>Must be set in order for Cloudacus to contact you<li>Must be set in order to run PredictiveBooks<li>Credit card information must be set in order for you to contract with Appahost</ol>',null,null,null,null,null,null,null,null);
insert into relation (table_name,related_table,related_column,pair_one2m_order,omit_drillthru_yn,omit_drilldown_yn,relation_type_isa_yn,copy_common_columns_yn,automatic_preselection_yn,drop_down_multi_select_yn,join_one2m_each_row_yn,ajax_fill_drop_down_yn,hint_message) values ('self','entity','null',null,null,null,'y',null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn,default_value) values ('self','credit_card_expiration_month_year',null,'2',null,null,null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn,default_value) values ('self','credit_card_number',null,'1',null,null,null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn,default_value) values ('self','credit_card_security_code',null,'3',null,null,null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn,default_value) values ('self','credit_provider',null,'4',null,'y','y',null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn,default_value) values ('self','credit_provider_not_supported_yn',null,'5',null,'y','y',null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn,default_value) values ('self','full_name','1',null,null,null,null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn,default_value) values ('self','invoice_amount_due',null,'6',null,'y','y',null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn,default_value) values ('self','street_address','2',null,null,null,null,null,null,null,null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('credit_card_expiration_month_year','text','4',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('credit_card_number','text','20',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('credit_card_security_code','text','3',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('credit_provider','text','30',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('credit_provider_not_supported_yn','text','1',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('full_name','text','60',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('invoice_amount_due','float','10','2',null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('street_address','text','60',null,null);
insert into table_operation (table_name,role,operation) values ('self','supervisor','delete');
insert into table_operation (table_name,role,operation) values ('self','supervisor','delete_isa_only');
insert into table_operation (table_name,role,operation) values ('self','supervisor','drilldown');
insert into role_table (role,table_name,permission) values ('supervisor','self','insert');
insert into role_table (role,table_name,permission) values ('supervisor','self','update');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

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

# Returns 0 if financial_institution.full_name exists in MySQL
# ------------------------------------------------------------
table_column_mysql_exists.sh financial_institution full_name

if [ $? -eq 0 ]
then
	exit 0
fi

(
cat << shell_all_done
create table financial_institution (full_name char (60) not null,street_address char (60) not null default 'unknown') engine MyISAM;
create unique index financial_institution on financial_institution (full_name,street_address);
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate

(
cat << shell_all_done
insert into appaserver_table (table_name,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,drillthru_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,data_directory,javascript_filename,index_directory) values ('financial_institution','table','5','entity',null,null,null,null,null,null,null,null,null,null,null,null,null);
insert into relation (table_name,related_table,related_column,pair_one2m_order,omit_drillthru_yn,omit_drilldown_yn,relation_type_isa_yn,copy_common_columns_yn,automatic_preselection_yn,drop_down_multi_select_yn,join_one2m_each_row_yn,ajax_fill_drop_down_yn,hint_message) values ('financial_institution','entity','null',null,null,null,'y',null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn,default_value) values ('financial_institution','full_name','1',null,null,null,null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn,default_value) values ('financial_institution','street_address','2',null,null,null,null,null,null,null,null,'unknown');
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('full_name','text','60',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('street_address','text','60',null,null);
insert into table_operation (table_name,role,operation) values ('financial_institution','supervisor','delete');
insert into table_operation (table_name,role,operation) values ('financial_institution','supervisor','delete_isa_only');
insert into table_operation (table_name,role,operation) values ('financial_institution','supervisor','drilldown');
insert into role_table (role,table_name,permission) values ('supervisor','financial_institution','insert');
insert into role_table (role,table_name,permission) values ('supervisor','financial_institution','update');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0

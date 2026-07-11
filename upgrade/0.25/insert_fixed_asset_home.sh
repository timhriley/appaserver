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

if [ "$application" = "appahost" ]
then
	exit 0
fi

# Returns 0 if ELEMENT.element exists
# -----------------------------------
table_column_exists.sh element element

if [ $? -ne 0 ]
then
	exit 0
fi

(
cat << shell_all_done
insert into appaserver_table (table_name,appaserver_form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,drillthru_yn,notepad,storage_engine,no_initial_capital_yn,create_view_statement,html_help_file_anchor,exclude_application_export_yn,data_directory,javascript_filename,index_directory) values ('fixed_asset','prompt','5','fixed_asset',null,'fixed_asset_trigger',null,null,null,null,null,null,null,null,null,'post_change_fixed_asset_purchase.js',null);
insert into relation (table_name,related_table,related_column,pair_one2m_order,omit_drillthru_yn,omit_drilldown_yn,omit_update_yn,relation_type_isa_yn,copy_common_columns_yn,automatic_preselection_yn,drop_down_multi_select_yn,join_one2m_each_row_yn,ajax_fill_drop_down_yn,hint_message) values ('fixed_asset','account','asset_account',null,null,null,null,null,null,null,null,null,null,null);
insert into relation (table_name,related_table,related_column,pair_one2m_order,omit_drillthru_yn,omit_drilldown_yn,omit_update_yn,relation_type_isa_yn,copy_common_columns_yn,automatic_preselection_yn,drop_down_multi_select_yn,join_one2m_each_row_yn,ajax_fill_drop_down_yn,hint_message) values ('fixed_asset','account','credit_account',null,null,null,null,null,null,null,null,null,null,null);
insert into relation (table_name,related_table,related_column,pair_one2m_order,omit_drillthru_yn,omit_drilldown_yn,omit_update_yn,relation_type_isa_yn,copy_common_columns_yn,automatic_preselection_yn,drop_down_multi_select_yn,join_one2m_each_row_yn,ajax_fill_drop_down_yn,hint_message) values ('fixed_asset','entity','null',null,null,null,null,null,null,null,null,null,null,null);
insert into relation (table_name,related_table,related_column,pair_one2m_order,omit_drillthru_yn,omit_drilldown_yn,omit_update_yn,relation_type_isa_yn,copy_common_columns_yn,automatic_preselection_yn,drop_down_multi_select_yn,join_one2m_each_row_yn,ajax_fill_drop_down_yn,hint_message) values ('fixed_asset','transaction','purchase_date_time',null,'y',null,'y',null,null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn,default_value) values ('fixed_asset','asset_account',null,'5',null,'n',null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn,default_value) values ('fixed_asset','asset_name','1',null,null,null,null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn,default_value) values ('fixed_asset','credit_account',null,'5',null,null,null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn,default_value) values ('fixed_asset','fixed_asset_cost',null,'3',null,null,null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn,default_value) values ('fixed_asset','full_name',null,'1',null,null,null,'n',null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn,default_value) values ('fixed_asset','notepad',null,'6',null,null,null,null,null,null,null,null);
insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn,default_value) values ('fixed_asset','purchase_date_time',null,'2',null,null,'y',null,null,null,null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('asset_account','character','60',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('asset_name','character','40',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('credit_account','character','60',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('fixed_asset_cost','float','12','2',null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('full_name','character','60',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('notepad','notepad','4096',null,null);
insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('purchase_date_time','current_date_time','19',null,null);
insert into table_operation (table_name,role,operation) values ('fixed_asset','supervisor','delete');
insert into table_operation (table_name,role,operation) values ('fixed_asset','supervisor','drilldown');
insert into operation (operation,output_yn) values ('delete','n');
insert into process (process,command_line) values ('delete','delete_folder_row \$session \$login_name \$role \$folder \$primary_data_list n');
insert into operation (operation,output_yn) values ('drilldown','y');
insert into process (process,command_line) values ('drilldown','drilldown \$session \$login_name \$role \$target_frame \$folder \$primary_data_list \$update_results \$update_error \$process_id \$operation_row_count \$dictionary');
insert into table_operation (table_name,role,operation) values ('fixed_asset','supervisor','delete');
insert into table_operation (table_name,role,operation) values ('fixed_asset','supervisor','drilldown');
insert into role_table (role,table_name,permission) values ('supervisor','fixed_asset','insert');
insert into role_table (role,table_name,permission) values ('supervisor','fixed_asset','update');
insert into subschema (subschema) values ('fixed_asset');
insert into process (process,command_line) values ('fixed_asset_trigger','fixed_asset_home_trigger asset_name \$state preupdate_full_name preupdate_purchase_date_time');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

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
process="process"
role="role"

(
cat << all_done
delete from $folder where folder = 'depreciation';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('depreciation','prompt','5','purchase',null,'depreciation_trigger',null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'depreciation';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('depreciation','fixed_asset_purchase','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('depreciation','self','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('depreciation','transaction','null',null,null,null,'n',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'asset_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('asset_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'serial_label';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('serial_label','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'depreciation_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('depreciation_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'units_produced';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('units_produced','integer','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'depreciation_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('depreciation_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'depreciation';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('depreciation','asset_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('depreciation','depreciation_amount',null,'2',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('depreciation','depreciation_date','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('depreciation','full_name',null,'3',null,'y','y',null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('depreciation','serial_label','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('depreciation','street_address',null,'4',null,'y','y',null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('depreciation','transaction_date_time',null,'5',null,'y','y',null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('depreciation','units_produced',null,'1',null,'y',null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('depreciation','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('depreciation','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('depreciation','supervisor','update');
delete from $process where process = 'depreciation_trigger';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('depreciation_trigger','depreciation_trigger asset_name serial_label depreciation_date \$state',null,null,null,null,null,null,null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('depreciation','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('depreciation','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('depreciation','supervisor','detail');
delete from $folder where folder = 'depreciation_method';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('depreciation_method','table','5','static',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'depreciation_method';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','depreciation_method','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'depreciation_method';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('depreciation_method','text','25',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'depreciation_method';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('depreciation_method','depreciation_method','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('depreciation_method','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('depreciation_method','supervisor','lookup');
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('depreciation_method','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('depreciation_method','bookkeeper','detail');
delete from $folder where folder = 'fixed_asset';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('fixed_asset','prompt','5','purchase',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'fixed_asset';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','fixed_asset','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'asset_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('asset_name','text','30',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'fixed_asset';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset','asset_name','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('fixed_asset','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('fixed_asset','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('fixed_asset','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('fixed_asset','supervisor','update');
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('fixed_asset','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('fixed_asset','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('fixed_asset','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('fixed_asset','supervisor','detail');
delete from $folder where folder = 'fixed_asset_purchase';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('fixed_asset_purchase','prompt','5','purchase',null,'post_change_fixed_asset_purchase','post_change_fixed_asset_purchase( \$row )',null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'fixed_asset_purchase';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','cost_recovery_conversion','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','cost_recovery_method','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','cost_recovery_period','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','depreciation_method','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','fixed_asset','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','vendor','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('depreciation','fixed_asset_purchase','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('cost_recovery','fixed_asset_purchase','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'asset_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('asset_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'serial_label';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('serial_label','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'purchase_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('purchase_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'service_placement_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_placement_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'fixed_asset_cost';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('fixed_asset_cost','float','12','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'units_produced_so_far';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('units_produced_so_far','integer','7',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'disposal_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('disposal_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'cost_recovery_period';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('cost_recovery_period','text','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'cost_recovery_method';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('cost_recovery_method','text','25',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'cost_recovery_conversion';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('cost_recovery_conversion','text','11',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'depreciation_method';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('depreciation_method','text','25',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'estimated_useful_life_years';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('estimated_useful_life_years','integer','3',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'estimated_useful_life_units';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('estimated_useful_life_units','integer','3',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'estimated_residual_value';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('estimated_residual_value','integer','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'declining_balance_n';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('declining_balance_n','float','3','1',null,null,null,null,null,null);
delete from $attribute where attribute = 'cost_basis';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('cost_basis','float','12','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'finance_accumulated_depreciation';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('finance_accumulated_depreciation','float','12','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_adjusted_basis';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_adjusted_basis','float','12','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'fixed_asset_purchase';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','asset_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','cost_basis',null,'16',null,'y','y',null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','cost_recovery_conversion',null,'10',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','cost_recovery_method',null,'9',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','cost_recovery_period',null,'8',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','declining_balance_n',null,'15',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','depreciation_method',null,'11',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','disposal_date',null,'7',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','estimated_residual_value',null,'14',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','estimated_useful_life_units',null,'13',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','estimated_useful_life_years',null,'12',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','finance_accumulated_depreciation',null,'17',null,'y','y',null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','fixed_asset_cost',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','full_name',null,'1',null,null,null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','purchase_date_time',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','serial_label','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','service_placement_date',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','street_address',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','tax_adjusted_basis',null,'18',null,'y','y',null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','units_produced_so_far',null,'6',null,'y',null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('fixed_asset_purchase','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('fixed_asset_purchase','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('fixed_asset_purchase','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('fixed_asset_purchase','supervisor','update');
insert into $row_security_role_update (folder,attribute_not_null) values ('fixed_asset_purchase','service_placement_date');
delete from $process where process = 'post_change_fixed_asset_purchase';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('post_change_fixed_asset_purchase','post_change_fixed_asset_purchase asset_name serial_label \$state',null,null,null,null,null,null,null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('fixed_asset_purchase','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('fixed_asset_purchase','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('fixed_asset_purchase','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('fixed_asset_purchase','supervisor','detail');
delete from $folder where folder = 'vendor';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('vendor','prompt','5','entity',null,null,null,'n',null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'vendor';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('vendor','entity','null',null,null,null,'y',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','vendor','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'vendor';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('vendor','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('vendor','street_address','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('vendor','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('vendor','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('vendor','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('vendor','supervisor','update');
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('entity');
insert into $role_operation (folder,role,operation) values ('vendor','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('vendor','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('vendor','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('vendor','supervisor','detail');
delete from $folder where folder = 'cost_recovery';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('cost_recovery','prompt','5','purchase',null,'cost_recovery_trigger',null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'cost_recovery';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('cost_recovery','fixed_asset_purchase','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'asset_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('asset_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'serial_label';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('serial_label','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_year';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_year','integer','4',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'recovery_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('recovery_amount','float','12','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'recovery_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('recovery_rate','float','7','5',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'cost_recovery';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('cost_recovery','asset_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('cost_recovery','recovery_amount',null,'1',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('cost_recovery','recovery_rate',null,'2',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('cost_recovery','serial_label','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('cost_recovery','tax_year','3',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('cost_recovery','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('cost_recovery','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('cost_recovery','supervisor','update');
delete from $process where process = 'cost_recovery_trigger';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('cost_recovery_trigger','cost_recovery_trigger asset_name serial_label tax_year \$state',null,null,null,null,null,null,null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('cost_recovery','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('cost_recovery','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('cost_recovery','supervisor','detail');
delete from $folder where folder = 'cost_recovery_conversion';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('cost_recovery_conversion','table','5','static',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'cost_recovery_conversion';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','cost_recovery_conversion','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'cost_recovery_conversion';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('cost_recovery_conversion','text','11',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'cost_recovery_conversion';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('cost_recovery_conversion','cost_recovery_conversion','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('cost_recovery_conversion','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('cost_recovery_conversion','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('cost_recovery_conversion','supervisor','lookup');
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('cost_recovery_conversion','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('cost_recovery_conversion','supervisor','detail');
delete from $folder where folder = 'cost_recovery_method';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('cost_recovery_method','table','5','static',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'cost_recovery_method';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','cost_recovery_method','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'cost_recovery_method';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('cost_recovery_method','text','25',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'cost_recovery_method';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('cost_recovery_method','cost_recovery_method','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('cost_recovery_method','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('cost_recovery_method','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('cost_recovery_method','supervisor','lookup');
insert into $role_folder (folder,role,permission) values ('cost_recovery_method','supervisor','update');
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('cost_recovery_method','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('cost_recovery_method','supervisor','detail');
delete from $folder where folder = 'cost_recovery_period';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('cost_recovery_period','table','5','static',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'cost_recovery_period';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','cost_recovery_period','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'cost_recovery_period';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('cost_recovery_period','text','10',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'cost_recovery_period';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('cost_recovery_period','cost_recovery_period','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('cost_recovery_period','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('cost_recovery_period','supervisor','update');
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('cost_recovery_period','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('cost_recovery_period','supervisor','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0

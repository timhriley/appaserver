#!/bin/bash
# -----------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/export_predictivebooks.sh
# -----------------------------------------------------------------

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
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 input_file cleanup_script" 1>&2
	exit 1
fi

# Initialize
# ----------
input_file=$1
cleanup_script=$2

# Set SELF_FULL_NAME and SELF_STREET_ADDRESS
# ------------------------------------------
. set_self.sh

output_shell="`basename.e $input_file y`".sh
source_file="$APPASERVER_HOME/library/appaserver_library.h"

echo "Here is $source_file"
grep DIFFERENT_DESTINATION_APPLICATION_OK $source_file
echo "sleeping..."
sleep 5
echo "executing..."

appaserver_config_file="/etc/appaserver.config"

# Functions
# ---------
function insert_opening_entry()
{
	input_file=$1
	output_shell=$2
	exists_fund=$3

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done13" >> $output_shell

	if [ "$exists_fund" -eq 0 ]
	then
		fund="general_fund"
	else
		fund="fund"
	fi

	insert_opening_entry.sh $input_file $fund >> $output_shell

	echo "all_done13" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell

	echo "automatic_transaction_assign.sh all process_name $fund >/dev/null" >> $output_shell
#	echo "bank_upload_sequence_propagate.sh '' | sql.e" >> $output_shell
	echo "ledger_propagate '' '' ''" >> $output_shell
#	echo "bank_upload_balance_propagate.sh '' | sql.e" >> $output_shell
	echo "" >> $output_shell
}
# insert_opening_entry()

function export_predictivebooks()
{
	application=$1
	input_file=$2
	output_shell=$3

	parameter_list=`
	cat $input_file							|
	decimal_count.e 0 n						|
	piece_shift_right.e '_'						|
	sed 's/^/folder_/'						|
	sed 's/\([1-9]\)_/\1=/1'					|
	joinlines.e '&'							|
	cat`

	export_file=`
	export_subschema x x x x x "${parameter_list}" 2>/dev/null	|
	grep Created							|
	column.e 1							|
	cat`

#	export_subschema x x x x x "${parameter_list}"; exit 1

	if [ "$export_file" = "" ]
	then
		echo "ERROR in $0: export_subschema generated an error." 1>&2
		exit 1
	fi

	cat $export_file						|
	grep -v '^exit'							|
	cat >> $output_shell

}
# export_predictivebooks()

function export_processes()
{
	application=$1
	input_file=$2
	output_shell=$3

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done3" >> $output_shell

	# role_operation
	# --------------
	cat $input_file							|
	while read folder
	do
		where="folder = '$folder'"
		select="folder,role,operation"
		echo "select $select from role_operation where $where;"	|
		sql.e							|
		insert_statement.e t=role_operation f="$select" del='^'	|
		cat >> $output_shell
	done

	# process
	# -------
	select="process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn"
	echo "select "$select" from process;"			|
	sql.e							|
	insert_statement.e t=process f="$select" del='^'	|
	cat >> $output_shell

	# process_parameter
	# -----------------
	select="process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process"
	echo "select "$select" from process_parameter;"			|
	sql.e								|
	insert_statement.e t=process_parameter f="$select" del='^'	|
	cat >> $output_shell

	# role_process
	# ------------
	select="process,role"
	echo "select "$select" from role_process;"			|
	sql.e								|
	insert_statement.e t=role_process f="$select" del='^'		|
	cat >> $output_shell

	# javascript_processes
	# --------------------
	select="process,javascript_filename"
	echo "select "$select" from javascript_processes;"		|
	sql.e								|
	insert_statement.e t=javascript_processes f="$select" del='^'	|
	cat >> $output_shell

	# javascript_files
	# ----------------
	select="javascript_filename"
	echo "select "$select" from javascript_files;"			|
	sql.e								|
	insert_statement.e t=javascript_files f="$select" del='^'	|
	cat >> $output_shell

	# prompt
	# ------
	select="prompt,
		input_width,
		hint_message,
		upload_filename_yn,
		date_yn"
	echo "select "$select" from prompt;"				|
	sql.e								|
	insert_statement.e t=prompt f="$select" del='^'			|
	cat >> $output_shell

	# drop_down_prompt
	# ----------------
	select="drop_down_prompt,hint_message,optional_display"
	echo "select "$select" from drop_down_prompt;"			|
	sql.e								|
	insert_statement.e t=drop_down_prompt f="$select" del='^'	|
	cat >> $output_shell

	# drop_down_prompt_data
	# ---------------------
	select="drop_down_prompt,drop_down_prompt_data,display_order"
	echo "select "$select" from drop_down_prompt_data;"		|
	sql.e								|
	insert_statement.e t=drop_down_prompt_data f="$select" del='^'	|
	cat >> $output_shell

	# operation
	# ---------
	select="operation,output_yn"
	echo "select $select from operation;"			|
	sql.e							|
	insert_statement.e t=operation f="$select" del='^'	|
	cat >> $output_shell

	# process_groups
	# --------------
	select="process_group"
	echo "select $select from process_groups;"		|
	sql.e							|
	insert_statement.e t=process_groups f="$select" del='^'	|
	cat >> $output_shell

	echo "all_done3" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# export_processes()

function create_predictivebooks()
{
	application=$1
	input_file=$2
	output_shell=$3

	# ttable misspelling on purpose
	# -----------------------------
	command="create_table x n x x x $application ttable n mysql"

	cat $input_file							|
	while read table
	do
		new_command=`echo $command | sed "s/ttable/$table/"`
		create_file=`
		$new_command						|
		grep Created						|
		column.e 1						|
		cat`

		cat $create_file					|
		egrep "^echo|^table_name="				|
		grep -v 'drop table'					|
		cat >> $output_shell
	done
}
# create_predictivebooks()

function extract_investment()
{
	application=$1
	output_shell=$2

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done5" >> $output_shell

	folder=investment_classification
	columns=classification
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e table=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=investment_operation
	columns=investment_operation
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	echo "all_done5" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_investment()

function extract_static_tables()
{
	application=$1
	input_file=$2
	output_shell=$3
	exists_fund=$4

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done2" >> $output_shell

	# --------------
	# General ledger
	# --------------
	folder=element
	columns=element,accumulate_debit_yn
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e table=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=subclassification
	columns=subclassification,element,display_order
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	# Extract folder=account
	# ----------------------
	folder=account

	if [ "$exists_fund" -eq 0 ]
	then
		columns=account,fund,subclassification,hard_coded_account_key
	else
		columns=account,subclassification,hard_coded_account_key
	fi

	where="( hard_coded_account_key not in ( 'cash_key', 'closing_key,contributed_capital_key' ) ) or ( account in ( select account from account where subclassification in ( 'operating_expense', 'donation', 'entertainment', 'home', 'transportation', 'revenue' ) ) )"

	get_folder_data a=$application f=$folder s=$columns w="$where"	|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	# Extract folder=fund (maybe)
	# ---------------------------
	if [ "$exists_fund" -eq 0 ]
	then
		folder=fund
		columns=fund
		get_folder_data a=$application f=$folder s=$columns	|
		insert_statement.e t=$folder field=$columns del='^'	|
		cat >> $output_shell
	fi

	folder=depreciation_method
	columns=depreciation_method
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=day
	columns=day
	get_folder_data a=$application f=$folder s=$columns	|
	insert_statement.e t=$folder field=$columns del='^'	|
	cat >> $output_shell

	# Payroll
	# -------
	if [ "$input_file" != "predictivebooks_rentalproperty.dat" -a	\
	     "$input_file" != "predictivebooks_home.dat" ]
	then
		folder=payroll_pay_period
		columns=payroll_pay_period
		get_folder_data a=$application f=$folder s=$columns	|
		insert_statement.e t=$folder field=$columns del='^'	|
		cat >> $output_shell
	fi

#	folder=tax_form_line_account
#	columns=tax_form,tax_form_line,account
#	get_folder_data a=$application f=$folder s=$columns		|
#	insert_statement.e t=$folder field=$columns del='^'		|
#	cat >> $output_shell

	folder=tax_form_line
	columns="	tax_form,
			tax_form_line,
			tax_form_description,
			itemize_accounts_yn"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e t=$folder field="$columns" del='^'		|
	cat >> $output_shell

	folder=tax_form
	columns=tax_form
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	# Tax recovery
	# ------------
	if [ "$input_file" != "predictivebooks_nonprofit.dat" -a	\
	     "$input_file" != "predictivebooks_communityband.dat" -a	\
	     "$input_file" != "predictivebooks_home.dat" ]
	then
		folder=tax_recovery_table
		columns="	tax_recovery_period,
				recovery_year,
				recovery_percent"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell
	
		folder=tax_recovery_period
		columns=tax_recovery_period
		get_folder_data a=$application f=$folder s=$columns	|
		insert_statement.e t=$folder field=$columns del='^'	|
		cat >> $output_shell
	fi

	# Inventory
	# ---------
	if [ "$input_file" != "predictivebooks_rentalproperty.dat" -a	\
	     "$input_file" != "predictivebooks_home.dat" ]
	then
		folder=inventory_cost_method
		columns=inventory_cost_method
		get_folder_data a=$application f=$folder s=$columns	|
		insert_statement.e t=$folder field=$columns del='^'	|
		cat >> $output_shell
	fi

	# Payroll
	# -------
	if [ "$input_file" != "predictivebooks_rentalproperty.dat" -a	\
	     "$input_file" != "predictivebooks_home.dat" ]
	then
		folder=federal_marital_status
		columns=federal_marital_status
		get_folder_data a=$application f=$folder s=$columns	|
		insert_statement.e t=$folder field=$columns del='^'	|
		cat >> $output_shell
	
		folder=state_marital_status
		columns=state_marital_status
		get_folder_data a=$application f=$folder s=$columns	|
		insert_statement.e t=$folder field=$columns del='^'	|
		cat >> $output_shell
	
		folder=federal_income_tax_withholding
		columns="federal_marital_status,
			 income_over,
			 income_not_over,
			 tax_fixed_amount,
			 tax_percentage_amount"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell
	
		folder=state_income_tax_withholding
		columns="state_marital_status,
			 income_over,
			 income_not_over,
			 tax_fixed_amount,
			 tax_percentage_amount"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell
	
		folder=state_standard_deduction_table
		columns="state_marital_status,
			 state_withholding_allowances,
			 state_standard_deduction_amount"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell
	
		folder=fixed_service_category
		columns="service_category"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell
	
		folder=hourly_service_category
		columns="service_category"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell
	fi

	if [ "$input_file" = "predictivebooks_communityband.dat" ]
	then
		folder=composition_location
		columns="composition_location"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell

		folder=status
		columns="status,sort_order"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell

		folder=position
		columns="position"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell

		folder=section
		columns="section"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell

	fi

	echo "all_done2" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_static_tables()

function extract_self()
{
	application=$1
	input_file=$2
	output_shell=$3

	full_name="$SELF_FULL_NAME"
	street_address="$SELF_STREET_ADDRESS"

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done4" >> $output_shell

	folder=self

	if [ "$input_file" = "predictivebooks_home.dat" ]
	then
		select="'$full_name',
			'$street_address'"
	
		fields="full_name,
			street_address"
	else
		select="'$full_name',
			'$street_address',
			inventory_cost_method,
			payroll_pay_period,
			payroll_beginning_day,
			social_security_combined_tax_rate,
			social_security_payroll_ceiling,
			medicare_combined_tax_rate,
			medicare_additional_withholding_rate,
			medicare_additional_gross_pay_floor,
			federal_withholding_allowance_period_value,
			federal_nonresident_withholding_income_premium,
			state_withholding_allowance_period_value,
			state_itemized_allowance_period_value,
			federal_unemployment_wage_base,
			federal_unemployment_tax_standard_rate,
			federal_unemployment_threshold_rate,
			federal_unemployment_tax_minimum_rate,
			state_unemployment_wage_base,
			state_unemployment_tax_rate,
			state_sales_tax_rate"
	
		fields="full_name,
			street_address,
			inventory_cost_method,
			payroll_pay_period,
			payroll_beginning_day,
			social_security_combined_tax_rate,
			social_security_payroll_ceiling,
			medicare_combined_tax_rate,
			medicare_additional_withholding_rate,
			medicare_additional_gross_pay_floor,
			federal_withholding_allowance_period_value,
			federal_nonresident_withholding_income_premium,
			state_withholding_allowance_period_value,
			state_itemized_allowance_period_value,
			federal_unemployment_wage_base,
			federal_unemployment_tax_standard_rate,
			federal_unemployment_threshold_rate,
			federal_unemployment_tax_minimum_rate,
			state_unemployment_wage_base,
			state_unemployment_tax_rate,
			state_sales_tax_rate"
	fi

	get_folder_data a=$application f=$folder s="$select"		|
	insert_statement.e table=$folder field="$fields" del='^'	|
	cat >> $output_shell

	folder=entity
	fields="full_name,street_address"
	echo "$full_name^$street_address"				|
	insert_statement.e table=$folder field=$fields del='^'		|
	cat >> $output_shell

	if [ "$input_file" = "predictivebooks_autorepair.dat" ]
	then
		echo "insert into role_appaserver_user (login_name,role) values ( '\$login_name','mechanic');"					|
		cat >> $output_shell
	fi

	if [ "$input_file" = "predictivebooks_communityband.dat" ]
	then
		echo "insert into role_appaserver_user (login_name,role) values ( '\$login_name','librarian');"					|
		cat >> $output_shell
	fi

	echo "all_done4" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_self()

function extract_inventory()
{
	application=$1
	output_shell=$2

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done8" >> $output_shell

	folder=inventory_category
	columns="inventory_category"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e table=$folder field="$columns" del='^'	|
	cat >> $output_shell

	folder=inventory
	columns="inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e table=$folder field="$columns" del='^'	|
	cat >> $output_shell

	echo "all_done8" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_inventory()

function extract_fixed_asset()
{
	application=$1
	output_shell=$2

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done9" >> $output_shell

	folder=fixed_asset
	columns="asset_name,account"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e table=$folder field="$columns" del='^'	|
	cat >> $output_shell

	folder=prior_fixed_asset
	columns="asset_name,serial_number,service_placement_date,extension,estimated_useful_life_years,estimated_residual_value,depreciation_method,tax_cost_basis,tax_recovery_period"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e table=$folder field="$columns" del='^'	|
	cat >> $output_shell

	echo "all_done9" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_fixed_asset()

function extract_activity()
{
	application=$1
	output_shell=$2

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done12" >> $output_shell

	folder=activity
	columns="activity"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e table=$folder field="$columns" del='^'	|
	cat >> $output_shell

	echo "all_done12" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_activity()

function extract_subsidiary_transaction()
{
	application=$1
	output_shell=$2

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done11" >> $output_shell

	folder=subsidiary_transaction
	columns="folder,attribute,debit_account,debit_account_folder,credit_account"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e table=$folder field="$columns" del='^'	|
	cat >> $output_shell

	echo "all_done11" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_subsidiary_transaction()

function extract_customer_sale()
{
	application=$1
	output_shell=$2

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done10" >> $output_shell

	folder=hourly_service
	columns="service_name,service_category,hourly_rate,credit_account"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e table=$folder field="$columns" del='^'	|
	cat >> $output_shell

	folder=fixed_service
	columns="service_name,service_category,retail_price,credit_account"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e table=$folder field="$columns" del='^'	|
	cat >> $output_shell

	echo "all_done10" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_customer_sale()

function extract_vehicle()
{
	application=$1
	output_shell=$2

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done7" >> $output_shell

	folder=vehicle_maker
	columns="vehicle_make"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e table=$folder field="$columns" del='^'	|
	cat >> $output_shell

	folder=vehicle
	columns="vehicle_make,vehicle_model,vehicle_trim,vehicle_year,assessment"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e table=$folder field="$columns" del='^'	|
	cat >> $output_shell

	echo "all_done7" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_vehicle()

function extract_foreign_attribute()
{
	application=$1
	output_shell=$2

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done6" >> $output_shell

	folder=foreign_attribute
	columns="folder,related_folder,related_attribute,foreign_attribute,foreign_key_index"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e table=$folder field=$columns del='^'		|
	cat >> $output_shell

	echo "all_done6" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_foreign_attribute()

function append_customer_sale_relation()
{
	output_shell=$1

	echo "table=\`get_table_name ignored relation\`" >> $output_shell
	echo "echo \"insert into \$table (folder,related_folder,related_attribute) values ('customer_sale','customer','null');\" | sql.e" >> $output_shell
}

function prepend_src_rentalproperty()
{
	output_shell=$1

	echo "table=\`get_table_name ignored application\`" >> $output_shell
	echo "results=\`echo \"select relative_source_directory from \$table;\" | sql.e\`" >> $output_shell
	echo "relative_source_directory=src_rentalproperty:\${results}" >> $output_shell
	echo "echo \"update \$table set relative_source_directory = '\$relative_source_directory';\" | sql.e" >> $output_shell
}
# prepend_src_rentalproperty()

function prepend_src_autorepair()
{
	output_shell=$1

	echo "table=\`get_table_name ignored application\`" >> $output_shell
	echo "results=\`echo \"select relative_source_directory from \$table;\" | sql.e\`" >> $output_shell
	echo "relative_source_directory=src_autorepair:\${results}" >> $output_shell
	echo "echo \"update \$table set relative_source_directory = '\$relative_source_directory';\" | sql.e" >> $output_shell
}
# prepend_src_autorepair()

function prepend_src_communityband()
{
	output_shell=$1

	echo "table=\`get_table_name ignored application\`" >> $output_shell
	echo "results=\`echo \"select relative_source_directory from \$table;\" | sql.e\`" >> $output_shell
	echo "relative_source_directory=src_communityband:\${results}" >> $output_shell
	echo "echo \"update \$table set relative_source_directory = '\$relative_source_directory';\" | sql.e" >> $output_shell
}
# prepend_src_communityband()

# Main
# ----
echo ":" > $output_shell
echo "# Created by $0" >> $output_shell
echo "" >> $output_shell

folder_attribute_exists.sh $application account fund

# Zero means true
# ---------------
exists_fund=$?

create_predictivebooks $application $input_file $output_shell

export_predictivebooks $application $input_file $output_shell

extract_static_tables $application $input_file $output_shell $exists_fund

#extract_investment $application $output_shell

export_processes $application $input_file $output_shell

extract_self $application $input_file $output_shell

extract_subsidiary_transaction $application $output_shell

extract_activity $application $output_shell

insert_opening_entry $input_file $output_shell $exists_fund

if [ "$input_file" = "predictivebooks_communityband.dat" ]
then
	prepend_src_communityband $output_shell
fi

if [ "$input_file" = "predictivebooks_autorepair.dat" ]
then
	# See CUSTOMER_SALE->MECHANIC relation.
	# -------------------------------------
	extract_foreign_attribute $application $output_shell

	extract_vehicle $application $output_shell

	extract_inventory $application $output_shell

	prepend_src_autorepair $output_shell
fi

if [	"$input_file" = "predictivebooks_autorepair.dat" ]
then
	extract_inventory $application $output_shell
	extract_fixed_asset $application $output_shell
	extract_customer_sale $application $output_shell
fi

if [	"$input_file" = "predictivebooks_enterprise.dat" ]
then
	extract_inventory $application $output_shell
	extract_fixed_asset $application $output_shell
	extract_customer_sale $application $output_shell
	append_customer_sale_relation $output_shell
fi

if [	"$input_file" = "predictivebooks_rentalproperty.dat" ]
then
	prepend_src_rentalproperty $output_shell
fi

echo "exit 0" >> $output_shell

if [ "$cleanup_script" != "" -a "$cleanup_script" != "cleanup_script" ]
then
	$cleanup_script $output_shell
fi

chmod +x $output_shell

echo Created $output_shell

exit 0

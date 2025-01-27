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

if [ "$application" != "donner" ]
then
	exit 0
fi

folder="role_folder"

(
cat << all_done
delete from $folder where folder = 'activity';
delete from $folder where folder = 'bank_upload_activity_work';
delete from $folder where folder = 'role_activity';
delete from $folder where folder = 'camp';
delete from $folder where folder = 'camp_enrollment';
delete from $folder where folder = 'enrollment_payment';
delete from $folder where folder = 'service_enrollment';
delete from $folder where folder = 'student';
delete from $folder where folder = 'employee';
delete from $folder where folder = 'financial_institution';
delete from $folder where folder = 'vendor';
delete from $folder where folder = 'account_balance';
delete from $folder where folder = 'investment_account';
delete from $folder where folder = 'receipt_upload';
delete from $folder where folder = 'employee_work_day';
delete from $folder where folder = 'employee_work_period';
delete from $folder where folder = 'federal_income_tax_withholding';
delete from $folder where folder = 'payroll_posting';
delete from $folder where folder = 'state_income_tax_withholding';
delete from $folder where folder = 'state_standard_deduction_table';
delete from $folder where folder = 'cost_recovery';
delete from $folder where folder = 'depreciation';
delete from $folder where folder = 'fixed_asset';
delete from $folder where folder = 'fixed_asset_purchase';
delete from $folder where folder = 'adult_tee_shirt_size';
delete from $folder where folder = 'camp_title';
delete from $folder where folder = 'cost_recovery_conversion';
delete from $folder where folder = 'cost_recovery_method';
delete from $folder where folder = 'cost_recovery_period';
delete from $folder where folder = 'county';
delete from $folder where folder = 'day';
delete from $folder where folder = 'depreciation_method';
delete from $folder where folder = 'elective';
delete from $folder where folder = 'federal_marital_status';
delete from $folder where folder = 'grade_level';
delete from $folder where folder = 'instrument';
delete from $folder where folder = 'investment_classification';
delete from $folder where folder = 'investment_operation';
delete from $folder where folder = 'investment_purpose';
delete from $folder where folder = 'name_prefix';
delete from $folder where folder = 'name_suffix';
delete from $folder where folder = 'payroll_pay_period';
delete from $folder where folder = 'private_lesson_instrument';
delete from $folder where folder = 'school';
delete from $folder where folder = 'service_category';
delete from $folder where folder = 'service_offering';
delete from $folder where folder = 'state_marital_status';
delete from $folder where folder = 'subsidiary_transaction';
delete from $folder where folder = 'reoccurring_transaction';
all_done
) | sql.e 2>&1 | grep -iv duplicate

folder="role_process"

(
cat << all_done
delete from $folder where process = 'depreciate_fixed_assets';
delete from $folder where process = 'direct_transaction_assign';
delete from $folder where process = 'post_payroll';
delete from $folder where process = 'post_reoccurring_transaction_accrual';
delete from $folder where process = 'post_reoccurring_transaction_recent';
delete from $folder where process = 'transaction_balance_report';
delete from $folder where process = 'feeder_upload_missing';
delete from $folder where process = 'generate_invoice';
delete from $folder where process = 'generic_load';
delete from $folder where process = 'load_institution_balance';
delete from $folder where process = 'load_receipt_scan';
delete from $folder where process = 'report_writer';
delete from $folder where process = 'tax_cost_recover_fixed_assets';
delete from $folder where process = 'transaction_balance_report';
all_done
) | sql.e 2>&1 | grep -iv duplicate


exit 0

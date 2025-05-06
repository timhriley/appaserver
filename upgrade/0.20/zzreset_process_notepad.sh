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

(
cat << shell_all_done
update process set notepad = 'This process creates a new application that is a clone of the current application. For security on a public facing website, the source code must be compiled with NON_TEMPLATE_APPLICATION_OKAY set to 1.' where process = 'create_application';
update process set notepad = 'This process compares the Appaserver attributes with the Mysql table columns. It then gives you the opportunity to drop the residual columns.' where process = 'table_rectification';
update process set notepad = 'For a list of processes, this process exports from the following: process, role_process, process_parameter, prompt, drop_down_prompt, and drop_down_prompt_data.' where process = 'export_process';
update process set notepad = 'For a list of tables, this process exports from the following: table, relation, table_column, column, table_operation, role_table, row_security_role_update, table_row_level_restriction, and foreign_column.' where process = 'export_subschema';
update process set notepad = 'This process traverses the many-to-one relationships for a table. It inserts the missing primary keys that contain foreign keys in the selected table.' where process = 'fix_orphans';
update process set notepad = 'For a list of process sets, this process exports from the following: process_set, role_process_set_member, process_set_parameter, prompt, drop_down_prompt, and drop_down_prompt_data.' where process = 'export_process_set';
update process set notepad = 'This process allows you to provide command-line access to a user with select-only permission to sql.' where process = 'grant_select_to_user';
update process set notepad = 'This process imports the PredictBooks home edition.' where process = 'import_predictbooks';
update process set notepad = 'This process creates an empty application. It creates a new database, the Appaserver tables, the data directories, among other application objects. Following this process, you can begin inserting the table rows.' where process = 'create_empty_application';
update process set notepad = 'This process allows you to remove duplicate rows. It may be that two rows should really be one row because a spelling is slightly different in one of them.' where process = 'merge_purge';
update process set notepad = '<h3>Input format:</h3><table border=1><tr><th>Account Number<th>Balance</table>' where process = 'load_institution_balance';
update process set notepad = 'This process allows you to upload non-executable source files, like javascript. The destination directory is likely $APPASERVER_HOME/src_$application.' where process = 'upload_source_file';
update process set notepad = 'This process imports the PredictiveBooks application.' where process = 'import_predictivebooks';
update process set notepad = '<h2>Print Checks</h2><ol><li>Open in Evince, also known as Document Viewer (recommended)<li>Papersize width = 8.5 inches<li>Papersize height = 3 inches<li>Orientation = Portrait<li>Page order = Back to front<li>Reverse = Yes<li>Place first check face down on white paper<li>Print the checks<li>Print a blank page to eject the last check</ol>' where process = 'pay_liabilities';
update process set notepad = 'Most error messages are logged to this file. If you get the Server Error screen, then this file is the first place to find a clue. <big><bold>Warning:</bold></big> this process exposes password changes and session numbers.' where process = 'view_appaserver_log_file';
update process set notepad = '<ul><li>When you execute a feeder upload process, there are multiple attempts to ensure your PredictBooks'' transactions are synchronized with your Financial Institution''s transactions. If your Financial Institution''s download file contains a running balance column, then there are two safeguards in place. If your Financial Institution''s download file lacks a running balance column, then there is one safeguard in place. This process joins the PredictBooks'' Journal rows with the Financial Institution''s Feeder Rows. Three running balance columns are displayed: File Row Balance, Calculate Balance, and Ledger Balance.<li>It is natural for a temporary difference to appear when a check clears. A temporary difference will appear between the date you cut the check until the date the check clears. However, the ledger balance Difference should be zero as of 11:59:59 PM yesterday.</ul>' where process = 'feeder_row_journal_audit';
update process set notepad = 'This process outputs Journal rows connected to a Feeder Account that lack a corresponding Feeder Row. Newly inserted transactions are expected to lack a corresponding Feeder Row. Older transactions that lack a corresponding Feeder Row are probably the result of plug transactions used to force a balance with the Financial Institution. A plug transaction is needed if Financial Institution transactions don''t get included in the Upload File. An inaccurate Financial Institution download date range will cause an incomplete Upload File.' where process = 'feeder_journal_orphan_audit';
update process set notepad = '<ol><li>One Transaction has two or more Journal entries. Each Journal entry has a debit amount or credit amount, but not both. Also, the total of the debit amounts must equal the total of the credit amounts. Unfortunately, it is a common mistake for Total Debit to not equal Total Credit. If this mistake occurs, then the last line of your Trial Balance will be off. If your Trial Balance is off, then run this process to find the out-of-balance Transactions.<li>It is also a common mistake to start entering a Transaction but not finish it. If you need to assign a Journal entry to an Account that is not there, then you need to interrupt your flow to insert the new Account. When you return to the insert Transaction screen, remember to press the [Recall] button. If you forget to press the [Recall] button, then the unfinished Transaction will linger. Not to worry, select Yes for ''Delete No Journal yn''. Unfinished Transactions will be deleted.</ol>' where process = 'ledger_debit_credit_audit';
update process set notepad = '<ul><li>This is a shortcut process instead of executing Insert --> Feeder --> Feeder Phrase<li>If your account isn''t in the list below, then insert it using the path:<br>Insert --> Transaction --> Account.</ul>' where process = 'insert_feeder_phrase';
update process set notepad = '<ol><li>Download your transactions from your Financial Institution using a begin date as the end date of your last download. The end date of your last download will display if you submit this form without a file. Use ''Final Feeder Date'' as the begin date.<li>Hint: [Right Click] the browser''s tab above and select ''Duplicate Tab''. Use the new PredictBooks instance to:<ul><li>Feeder --> Insert Expense Transaction or<li>Feeder --> Insert Feeder Phrase</ul></ol>' where process = 'feeder_upload';
update process set notepad = '<ul><li>This is a shortcut process instead of executing:<br>Insert --> Feeder --> Feeder Phrase<li>If your account isn''t in the list below, then insert it using the path:<br>Insert --> Transaction --> Account.<li>Both the feeder phrase and the full name may be the vendor''s name copied from the feeder file''s description cell.<br>However, the feeder phrase may need to include an adjoining keyword to remove ambiguity.<li>If the vendor already exists as an entity and already has an existing, but sightly different, feeder phrase, then:<ol><li>Lookup --> Feeder --> Feeder Phrase<li>query the entity<li>set the additional feeder phrase to the right of the existing feeder phrase, separated with a ''|''</ol></ul>' where process = 'insert_feeder_phrase';
update process set notepad = '<ul><li>This is a shortcut process instead of executing:<br>Insert --> Transaction --> Transaction<li>If your account isn''t in the list below, then insert it using the path:<br>Insert --> Transaction --> Account.</ul>' where process = 'insert_expense_transaction';
update process set notepad = '<ol><li>This process generates an opening entry transaction.<li>The transaction is assigned to the entity stored in the Self table. When you first install Appaserver, the Self table has a placeholder full name. Change this placeholder to your full name. The menu path is:<br>Lookup --> Entity --> Self<li>This process creates: <ul><li>an asset account, if the Account Type is Checking or<li>a liability account, if the Account Type is Credit Card</ul> The account name is derived from the financial institution''s full name. When you first import PredictBooks, the Financial Institution table has a placeholder full name. Change this placeholder to your financial institution''s full name. The menu path is:<br>Lookup --> Entity --> Financial Institution<li>This process also generates useful bookkeeping tips. You can view the tips without generating an opening entry by leaving at least one choice blank.</ol>' where process = 'initialize_feeder_account';
update process set notepad = null where process = 'add_column';
update process set notepad = null where process = 'alter_column_datatype';
update process set notepad = null where process = 'clone_application';
update process set notepad = null where process = 'clone_folder';
update process set notepad = null where process = 'create_table';
update process set notepad = null where process = 'drilldown';
update process set notepad = null where process = 'drop_column';
update process set notepad = null where process = 'null';
update process set notepad = null where process = 'rename_table';
update process set notepad = null where process = 'view_documentation';
update process set notepad = null where process = 'view_source';
update process set notepad = null where process = 'rename_column';
update process set notepad = null where process = 'export_table';
update process set notepad = null where process = 'delete';
update process set notepad = null where process = 'delete_isa_only';
update process set notepad = null where process = 'appaserver_info';
update process set notepad = null where process = 'orphans_process_folder_list';
update process set notepad = null where process = 'export_application';
update process set notepad = null where process = 'attribute_list';
update process set notepad = null where process = 'change_password';
update process set notepad = null where process = 'google_map';
update process set notepad = null where process = 'account_balance';
update process set notepad = null where process = 'execute_select_statement';
update process set notepad = null where process = 'google_map_site';
update process set notepad = null where process = 'cost_recovery_trigger';
update process set notepad = null where process = 'depreciation_trigger';
update process set notepad = null where process = 'populate_account';
update process set notepad = null where process = 'populate_inventory';
update process set notepad = null where process = 'populate_print_checks_entity';
update process set notepad = null where process = 'populate_tax_form_line';
update process set notepad = null where process = 'post_change_bank_upload';
update process set notepad = null where process = 'post_change_customer_payment';
update process set notepad = null where process = 'post_change_customer_sale';
update process set notepad = null where process = 'post_change_employee_work_day';
update process set notepad = null where process = 'prior_fixed_asset_trigger';
update process set notepad = null where process = 'post_change_fixed_service_sale';
update process set notepad = null where process = 'post_change_fixed_service_work';
update process set notepad = null where process = 'post_change_hourly_service_sale';
update process set notepad = null where process = 'post_change_hourly_service_work';
update process set notepad = null where process = 'post_change_inventory_purchase';
update process set notepad = null where process = 'post_change_inventory_sale';
update process set notepad = null where process = 'post_change_journal_ledger';
update process set notepad = null where process = 'post_change_purchase_order';
update process set notepad = null where process = 'post_change_self';
update process set notepad = null where process = 'post_change_supply_purchase';
update process set notepad = null where process = 'post_change_vendor_payment';
update process set notepad = null where process = 'recalculate_cost';
update process set notepad = null where process = 'select_closed_customer_sale';
update process set notepad = null where process = 'select_customer_sale_collect_payment';
update process set notepad = null where process = 'select_open_customer_sale';
update process set notepad = null where process = 'select_open_fixed_service_sale';
update process set notepad = null where process = 'select_open_fixed_service_work';
update process set notepad = null where process = 'select_open_hourly_service_sale';
update process set notepad = null where process = 'select_open_hourly_service_work';
update process set notepad = null where process = 'tax_form_report';
update process set notepad = null where process = 'post_change_prior_fixed_asset';
update process set notepad = null where process = 'populate_investment_account';
update process set notepad = null where process = 'post_change_account_balance';
update process set notepad = null where process = 'post_change_activity_work';
update process set notepad = null where process = 'graphviz_database_schema';
update process set notepad = null where process = 'populate_expense_account';
update process set notepad = null where process = 'populate_bank_upload_pending';
update process set notepad = null where process = 'post_change_bank_upload_transaction';
update process set notepad = null where process = 'populate_bank_upload_transaction_pending';
update process set notepad = null where process = 'folder_list';
update process set notepad = null where process = 'trial_balance';
update process set notepad = null where process = 'financial_position';
update process set notepad = null where process = 'statement_of_activities';
update process set notepad = null where process = 'appaserver_user_trigger';
update process set notepad = null where process = 'budget_forecast';
update process set notepad = null where process = 'close_nominal_accounts';
update process set notepad = null where process = 'drop_application';
update process set notepad = null where process = 'drop_table';
update process set notepad = null where process = 'populate_revenue_account';
update process set notepad = null where process = 'self_trigger';
shell_all_done
) | sql.e

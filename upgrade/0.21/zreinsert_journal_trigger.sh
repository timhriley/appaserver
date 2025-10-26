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

# Returns 0 if ELEMENT.element exists
# -----------------------------------
table_column_exists.sh element element

if [ $? -ne 0 ]
then
	exit 0
fi

(
cat << shell_all_done
delete from process where process = 'post_change_journal_ledger';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('post_change_journal_ledger','journal_trigger fund full_name street_address transaction_date_time account debit_amount credit_amount \$state preupdate_fund preupdate_transaction_date_time preupdate_account preupdate_debit_amount preupdate_credit_amount',null,null,null,null,null,null,null,null);
delete from role_process where process = 'post_change_journal_ledger';
delete from process_parameter where process = 'post_change_journal_ledger';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

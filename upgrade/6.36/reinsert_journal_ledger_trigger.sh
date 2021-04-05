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

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
	exit 1
fi

process="process"
operation="operation"
process_parameter="process_parameter"
javascript_processes="javascript_processes"
process_generic_output="process_generic_output"
javascript_files="javascript_files"
prompt="prompt"
process_groups="process_groups"
drop_down_prompt="drop_down_prompt"
drop_down_prompt_data="drop_down_prompt_data"
role_process="role_process"
role_process_set_member="role_process_set_member"
role_operation="role_operation"
(
cat << all_done
delete from $process where process = 'post_change_journal_ledger';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('post_change_journal_ledger','journal_ledger_trigger full_name street_address transaction_date_time account \$state preupdate_transaction_date_time preupdate_account',null,null,null,null,null,null,null);
delete from $process_parameter where process = 'post_change_journal_ledger';
delete from $javascript_processes where process = 'post_change_journal_ledger';
delete from $process_generic_output where process = 'post_change_journal_ledger';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0

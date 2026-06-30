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

# Returns 0 if ACCOUNT_BALANCE.account_number exists
# --------------------------------------------------
table_column_exists.sh account_balance account_number

if [ $? -ne 0 ]
then
	exit 0
fi

(
cat << shell_all_done
delete from process where process = 'post_change_account_balance';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('post_change_account_balance','post_change_account_balance full_name contact_key account_number date \$state',null,null,null,null,null,null,null,null);
delete from role_process where process = 'post_change_account_balance';
delete from process_parameter where process = 'post_change_account_balance';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

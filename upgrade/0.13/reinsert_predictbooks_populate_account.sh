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

# Returns 0 if account.account exists in TABLE_COLUMN
# ---------------------------------------------------
table_column_exists.sh account account

if [ $? -eq 0 ]
then
	exit 0
fi

(
cat << shell_all_done
delete from process where process = 'populate_account';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('populate_account','populate_account.sh \$many_table subclassification',null,null,null,null,null,null,null,null);
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

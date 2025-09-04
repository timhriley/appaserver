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
delete from process where process = 'populate_account';
insert into process (process,command_line,process_group,notepad,post_change_javascript,javascript_filename,execution_count,preprompt_help_text,html_help_file_anchor,process_set_display) values ('populate_account','populate_account.sh \$many_table \$related_column subclassification',null,null,null,null,null,null,null,null);
delete from role_process where process = 'populate_account';
delete from process_parameter where process = 'populate_account';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

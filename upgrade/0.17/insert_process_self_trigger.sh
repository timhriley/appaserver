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
delete from process where process = 'self_trigger';
insert into process (process,command_line,notepad,execution_count,process_group,post_change_javascript,javascript_filename,preprompt_help_text,html_help_file_anchor,process_set_display) values ('self_trigger','self_trigger \$state full_name street_address preupdate_credit_card_number credit_card_number',null,null,null,null,null,null,null,null);
delete from process_parameter where process = 'self_trigger';
update appaserver_table set post_change_process = 'self_trigger' where table_name = 'self';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

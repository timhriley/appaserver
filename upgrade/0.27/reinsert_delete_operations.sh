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
delete from process where process = 'delete';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,javascript_filename,preprompt_help_text) values ('delete','delete_folder_row \$session \$login_name \$role \$folder \$primary_data_list \$row_number \$row_count n',null,null,null,null,null,null,null);
delete from role_process where process = 'delete';
delete from process_parameter where process = 'delete';
delete from process where process = 'delete_isa_only';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,javascript_filename,preprompt_help_text) values ('delete_isa_only','delete_folder_row \$session \$login_name \$role \$folder \$primary_data_list \$row_number \$row_count y',null,null,null,null,null,null,null);
delete from role_process where process = 'delete_isa_only';
delete from process_parameter where process = 'delete_isa_only';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

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
delete from process where process = 'drilldown';
insert into process (process,command_line,notepad,execution_count,process_group,post_change_javascript,javascript_filename,preprompt_help_text,html_help_file_anchor,process_set_display) values ('drilldown','drilldown \$session \$login_name \$role \$target_frame \$folder \$primary_data_list \$update_results \$update_error \$process_id \$operation_row_count \$dictionary',null,null,null,null,null,null,null,null);
delete from role_process where process = 'drilldown';
delete from process_parameter where process = 'drilldown';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

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

if [ "$application" != "appahost" ]
then
	exit 0
fi

(
cat << shell_all_done
delete from process where process = 'post_change_fixed_service_work';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('post_change_fixed_service_work','post_change_fixed_service_work fund_name full_name contact_key sale_date_time service_name begin_work_date_time \$state',null,null,null,null,null,null,null,null);
delete from process_parameter where process = 'post_change_fixed_service_work';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

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

if [ "$application" != "discovery" ]
then
	exit 0
fi

(
cat << shell_all_done
delete from process where process = 'canvass_plan';
insert into process (process,command_line,notepad,execution_count,process_group,post_change_javascript,javascript_filename,preprompt_help_text,html_help_file_anchor,process_set_display) values ('canvass_plan','canvass_plan.sh \$process maximum_weight',null,null,'output',null,null,null,null,null);
delete from role_process where process = 'canvass_plan';
insert into role_process (role,process) values ('supervisor','canvass_plan');
delete from process_parameter where process = 'canvass_plan';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('canvass_plan','null','null','null','maximum_weight','1',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('maximum_weight',null,null,null,'7');
insert into process_group (process_group) values ('output');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

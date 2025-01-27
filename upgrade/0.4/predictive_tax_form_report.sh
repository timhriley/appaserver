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

# Returns 0 if FEEDER_PHRASE.feeder_phrase exists
# -----------------------------------------------
folder_attribute_exists.sh $application feeder_phrase feeder_phrase

if [ $? -ne 0 ]
then
	exit 0
fi

process="process"
operation="operation"
process_parameter="process_parameter"
prompt="prompt"
process_groups="process_groups"
drop_down_prompt="drop_down_prompt"
drop_down_prompt_data="drop_down_prompt_data"
(
cat << all_done
delete from $process where process = 'tax_form_report';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('tax_form_report','tax_form_report \$process tax_form tax_year 0 output_medium',null,null,null,null,'output',null,null);
delete from $process_parameter where process = 'tax_form_report';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('tax_form_report','null','null','null','tax_report_output_medium','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('tax_form_report','null','null','tax_year','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('tax_form_report','tax_form','null','null','null','1',null,null,null,null);
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('tax_year',null,null,null,'4');
insert into $process_groups (process_group) values ('output');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('tax_report_output_medium',null,'output_medium');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('tax_report_output_medium','PDF','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('tax_report_output_medium','table','1');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0

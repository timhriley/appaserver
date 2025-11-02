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
delete from process where process = 'feeder_upload';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('feeder_upload','feeder_upload \$process \$login_name fund_name feeder_account exchange_format_filename execute_yn','<ol><li>Download your transactions from your Financial Institution using a begin date as the end date of your last download. The end date of your last download will display if you submit this form without a file. Use \'Final Feeder Date\' as the begin date.<li>Hint: [Right Click] the browser\'s tab above and select \'Duplicate Tab\'. Use the new PredictBooks instance to:<ul><li>Feeder --> Insert Expense Transaction or<li>Feeder --> Insert Feeder Phrase</ul></ol>',null,'73',null,'feeder',null,null,null);
delete from process_parameter where process = 'feeder_upload';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('feeder_upload','feeder_account','null','null','null','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('feeder_upload','null','null','null','exchange_format_filename','2',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('feeder_upload','null','null','null','execute_yn','9',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('exchange_format_filename','Log into your bank\'s website and download your transactions. Choose the format option likely to be the Exchange Format. It will probably be the top row in the format selection list. It will also probably have a filename that ends with xf.','y',null,'255');
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Omit execute for display.',null,null,'1');
insert into process_group (process_group) values ('feeder');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

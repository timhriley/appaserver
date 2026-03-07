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
delete from process where process = 'feeder_journal_orphan_audit';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('feeder_journal_orphan_audit','feeder_journal_orphan_audit.sh \$process feeder_account minimum_transaction_date','This process outputs Journal rows connected to a Feeder Account that lack a corresponding Feeder Row. After executing a feeder load process, you may get a Status of "Different" instead of "Okay". The cause is a duplicated transaction. Two possible duplication vulnerabilities are: <ol><li>You manually inserted in a restaurant transaction using a receipt that didn\'t include the tip. The feeder load then inserted a duplicate transaction by matching the feeder phrase.<ul><li>The solution is to seek out the manually inserted transaction and delete it.</ul><li>A transaction was inserted on a prior feeder load. However, on your last feeder load, the bank changed the description column a little. Therefore, the feeder phrase matching algorithm didn\'t match the existing transaction and inserted a new one.<ul><li>The solution is to seek out either transaction and delete it.</ul></ol>',null,null,null,'audit',null,null,null);
delete from process_parameter where process = 'feeder_journal_orphan_audit';
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('feeder_journal_orphan_audit','feeder_account','null','null','null','1',null,null,null,null);
insert into process_parameter (process,table_name,column_name,drop_down_prompt,prompt,display_order,drop_down_multi_select_yn,drillthru_yn,populate_drop_down_process,populate_helper_process) values ('feeder_journal_orphan_audit','null','null','null','minimum_transaction_date','1',null,null,null,null);
insert into prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('minimum_transaction_date',null,null,'y','10');
insert into process_group (process_group) values ('audit');
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

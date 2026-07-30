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

# Returns 0 if HOURLY_SERVICE_SALE.full_name exists
# -------------------------------------------------
table_column_exists.sh hourly_service_sale full_name

if [ $? -ne 0 ]
then
	exit 0
fi

(
cat << shell_all_done
delete from process where process = 'hourly_service_sale_trigger';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,javascript_filename,preprompt_help_text) values ('hourly_service_sale_trigger','hourly_service_sale_trigger fund_name full_name contact_key sale_date_time service_name service_description \$state',null,null,null,null,null,null,null);
delete from role_process where process = 'hourly_service_sale_trigger';
delete from process_parameter where process = 'hourly_service_sale_trigger';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

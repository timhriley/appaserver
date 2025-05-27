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
delete from process where process = 'prior_fixed_asset_trigger';
insert into process (process,command_line,notepad,html_help_file_anchor,execution_count,post_change_javascript,process_group,process_set_display,javascript_filename,preprompt_help_text) values ('prior_fixed_asset_trigger','prior_fixed_asset_trigger asset_name \$state preupdate_full_name preupdate_street_address preupdate_purchase_date_time',null,null,null,null,null,null,null,null);
delete from process_parameter where process = 'prior_fixed_asset_trigger';
shell_all_done
) | sql.e 2>&1 | grep -iv duplicate
exit 0

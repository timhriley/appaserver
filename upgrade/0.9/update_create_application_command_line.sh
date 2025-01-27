#!/bin/sh

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

echo "update process set command_line = 'create_application \$login_name \$process destination_application new_application_title execute_yn' where process = 'create_application';" | sql.e

echo "update process set notepad = 'This process creates a new application that is a clone of the current application. For security on a public facing website, the source code must be compiled with NON_TEMPLATE_APPLICATION_OKAY set to 1.' where process = 'create_application';" | sql.e

echo "delete from process_parameter where process = 'create_application';" | sql.e

echo "insert into process_parameter( process,folder,attribute,drop_down_prompt,prompt,display_order ) values ( 'create_application','null','null','null','destination_application',1 );" | sql.e
echo "insert into process_parameter( process,folder,attribute,drop_down_prompt,prompt,display_order ) values ( 'create_application','null','null','null','new_application_title',2 );" | sql.e
echo "insert into process_parameter( process,folder,attribute,drop_down_prompt,prompt,display_order ) values ( 'create_application','null','null','null','execute_yn',9 );" | sql.e

exit 0

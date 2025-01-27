:

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
sql.e << all_done
insert into process (process,command_line) values ('appaserver_user_trigger','appaserver_user_trigger login_name \$state');
update folder set post_change_process = 'appaserver_user_trigger' where folder = 'appaserver_user';
all_done
) 2>&1 | grep -vi duplicate
exit 0

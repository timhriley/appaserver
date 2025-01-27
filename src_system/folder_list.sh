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
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -eq 1 ]
then
	application=$1
fi

select.e				\
	application=$application	\
	folder=appaserver_table		\
	select=table_name 		|
sort					|
cat

exit 0

:
# $APPASERVER_HOME/src_appaserver/select.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

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

if [ "$#" -lt 2 ]
then
	echo "Usage: `basename.e $0 n` select table [where] [order]" 1>&2
	exit 1
fi

select="$1"

if [ "$select" = "" -o "$select" = "select" ]
then
	select="*"
fi

table="$2"

if [ "$#" -gt 2 ]
then
	where="$3"
else
	where="1=1"
fi

if [ "$#" -gt 3 ]
then
	order="$4"
else
	order="none"
fi

get_folder_data		application="$application"		\
			select="$select"			\
			folder="$table"				\
			where="$where"				\
			order="$order"

exit 0


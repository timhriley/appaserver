:
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_system/create_index.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: APPASERVER_DATABASE is not defined." 1>&2
	exit 1
fi

if [ $# -ne 1 ]
then
	echo "Usage: `basename.e $0 n` table" 1>&2
	exit 1
fi

table=$1

create_table '' $table n 2>/dev/null	|
grep 'create'				|
grep 'index'				|
sed 's/<td.*>//'

exit 0

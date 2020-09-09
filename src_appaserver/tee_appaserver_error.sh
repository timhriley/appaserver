:
# $APPASERVER_HOME/src_appaserver/tee_appaserver_error.sh
# -------------------------------------------------------
# Freely available software. See appaserver.org
# -------------------------------------------------------

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

appaserver_error_directory=`get_appaserver_error_directory`

tee_file="${appaserver_error_directory}/appaserver_${application}.err"

tee -a $tee_file

exit 0


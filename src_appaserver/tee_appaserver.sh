:
# -------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/tee_appaserver.sh
# -------------------------------------------------------------
# No warranty and freely available software. See appaserver.org
# -------------------------------------------------------------

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

appaserver_error_filename=`appaserver_error_filename $application`

if [ "$appaserver_error_filename" = "" ]
then
	echo "Error in `basename.e $0 n`: appaserver_error_filename returned empty" 1>&2
fi

tee -a $appaserver_error_filename

exit 0


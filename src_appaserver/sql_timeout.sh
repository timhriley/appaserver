#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/sql_timeout.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must . set_database first." 1>&2
	exit 1
fi

application_name=$APPASERVER_DATABASE

if [ "$#" -eq 1 ]
then
	run_seconds=$1
fi

if [ "$run_seconds" -ge 1 ]
then
	shift
	timeout $run_seconds sql $*
else
	sql
fi

exit $?

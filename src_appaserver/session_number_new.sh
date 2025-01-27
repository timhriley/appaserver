#!/bin/sh
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/session_number_new.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

#echo Starting: $0 $* 1>&2

if [ $# -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

application_table=appaserver_application

# Note: APPLICATION.next_session_number is really the session seed.
# -----------------------------------------------------------------
session_seed=`echo "	select next_session_number	 		 \
			from $application_table				 \
			where application = '$application';"		|\
		sql.e`

if [ "$session_seed" = "" ]
then
	session_seed=1
	echo "	update $application_table				 \
		set next_session_number = $session_seed			 \
		where application = '$application';"			 |
	sql.e
fi

echo "	update $application_table 					 \
	set next_session_number = next_session_number + 1		 \
	where application = '$application';"		 		 |
sql.e

while [ true ]
do
	session_number=`random_session $session_seed`

	results=`echo "	select session					\
			from session					\
			where session = '$session_number';"		|
		 sql.e`

	if [ "$results" = "" ]
	then
		break
	fi
done

echo $session_number

exit 0


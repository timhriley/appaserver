:
if [ "$APPASERVER_HOME" = "" ]
then
	echo "Error in $0: APPASERVER_HOME not defined" 1>&2
	exit 1
fi

if [ "$CGI_HOME" = "" ]
then
	echo "Error in $0: CGI_HOME not defined" 1>&2
	exit 1
fi

cd $APPASERVER_HOME/library && pwd && make
if [ "$?" -ne 0 ]
then
	echo "$0 exiting early"
	exit 1
fi

cd $APPASERVER_HOME/utility && pwd && make
if [ "$?" -ne 0 ]
then
	echo "$0 exiting early"
	exit 1
fi

cd $APPASERVER_HOME/src_appaserver && pwd && make
if [ "$?" -ne 0 ]
then
	echo "$0 exiting early"
	exit 1
fi

if [ -d $APPASERVER_HOME/src_hydrology ]
then
	cd $APPASERVER_HOME/src_hydrology && pwd && make
fi

if [ -d $APPASERVER_HOME/src_hydrology/reg_sched ]
then
	cd $APPASERVER_HOME/src_hydrology/reg_sched && pwd && make
fi

exit 0

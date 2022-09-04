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

cd $APPASERVER_HOME/library; make clean
cd $APPASERVER_HOME/utility; make clean
cd $APPASERVER_HOME/src_appaserver; make clean
cd $APPASERVER_HOME/src_predictive; make clean

cd $APPASERVER_HOME

for application in $(ls -1 -d src_* | sed 's/src_//')
do
	if [ "$application" = "appaserver" -o	\
	     "$application" = "predictive" ]
	then
		continue
	fi

	directory="$APPASERVER_HOME/src_${application}"

	if [ -f $directory/makefile ]
	then
		cd $directory; pwd; make clean
	fi
done

if [ -d $APPASERVER_HOME/src_hydrology/reg_sched ]
then
	cd $APPASERVER_HOME/src_hydrology/reg_sched

	if [ -f regulation_schedule_dvr ]
	then
		make clean
	fi

fi

exit 0

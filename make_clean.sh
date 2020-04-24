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
cd $APPASERVER_HOME/src_predictive; make clean

for application in `application_list.sh`
do
	directory="$APPASERVER_HOME/src_${application}"

echo "$directory" 1>&2

	if [ -f $directory/makefile ]
	then
		cd $directory; make clean
	fi
done

if [ -d $APPASERVER_HOME/src_hydrology/reg_sched ]
then
	cd $APPASERVER_HOME/src_hydrology/reg_sched && make clean
fi

exit 0

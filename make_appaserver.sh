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

(
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

if [ -d $APPASERVER_HOME/src_predictive ]
then
	cd $APPASERVER_HOME/src_predictive 2>/dev/null && pwd && make
	if [ "$?" -ne 0 ]
	then
		echo "$0 exiting early"
		exit 1
	fi
fi

for application in `application_list.sh`
do
	directory="$APPASERVER_HOME/src_${application}"

	if [ -f $directory/makefile ]
	then
		cd $directory && pwd && make

		if [ "$?" -ne 0 ]
		then
			echo "$0 exiting early"
			exit 1
		fi
	fi
done

if [ -d $APPASERVER_HOME/src_hydrology/reg_sched ]
then
	cd $APPASERVER_HOME/src_hydrology/reg_sched \
	&& pwd && make
fi

) 2>&1						|
grep -v "length character with"			|
grep -v "subscript has"				|
grep -v "set but not used"			|
grep -v "cast from pointer to integer"		|
grep -v "warning: ignoring return value"	|
grep -v "int format,"

exit 0

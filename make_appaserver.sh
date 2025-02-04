:
if [ "$#" -eq 1 -a "$1" = "force" ]
then
	force_compile=1
else
	force_compile=0
fi

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

cd $APPASERVER_HOME/src_appaserver && pwd && make
if [ "$?" -ne 0 ]
then
	echo "$0 exiting early"
	exit 1
fi

if [	-d $APPASERVER_HOME/src_system -a \
	-f $APPASERVER_HOME/src_system/makefile ]
then
	cd $APPASERVER_HOME/src_system 2>/dev/null && pwd && make
	if [ "$?" -ne 0 ]
	then
		echo "$0 exiting early"
		exit 1
	fi
fi

if [	-d $APPASERVER_HOME/src_admin -a \
	-f $APPASERVER_HOME/src_admin/makefile ]
then
	cd $APPASERVER_HOME/src_admin 2>/dev/null && pwd && make
	if [ "$?" -ne 0 ]
	then
		echo "$0 exiting early"
		exit 1
	fi
fi

if [	-d $APPASERVER_HOME/src_predictive -a \
	-f $APPASERVER_HOME/src_predictive/makefile ]
then
	cd $APPASERVER_HOME/src_predictive 2>/dev/null && pwd && make
	if [ "$?" -ne 0 ]
	then
		echo "$0 exiting early"
		exit 1
	fi
fi

cd $APPASERVER_HOME

if [ "$force_compile" -eq 1 ]
then
	for application in $(ls -1 -d src_* | sed 's/src_//')
	do
		if [ "$application" = "appaserver" -o	\
		     "$application" = "system" -o	\
		     "$application" = "admin" -o	\
		     "$application" = "predictive" ]
		then
			continue
		fi

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
else
	for application in `application_list.sh`
	do
		if [ "$application" = "appaserver" -o	\
		     "$application" = "predictive" ]
		then
			continue
		fi

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
fi

if [	-d $APPASERVER_HOME/src_hydrology/reg_sched -a		\
	-f $APPASERVER_HOME/src_hydrology/reg_sched/makefile ]
then
	cd $APPASERVER_HOME/src_hydrology/reg_sched 		\
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

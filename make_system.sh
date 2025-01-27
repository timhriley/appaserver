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

cd $APPASERVER_HOME/src_appaserver && pwd && make
if [ "$?" -ne 0 ]
then
	echo "$0 exiting early"
	exit 1
fi

cd $APPASERVER_HOME/src_system && pwd && make
if [ "$?" -ne 0 ]
then
	echo "$0 exiting early"
	exit 1
fi

) 2>&1						|
grep -v "length character with"			|
grep -v "subscript has"				|
grep -v "set but not used"			|
grep -v "cast from pointer to integer"		|
grep -v "warning: ignoring return value"	|
grep -v "int format,"

exit 0

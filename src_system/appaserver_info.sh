:
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_system/appaserver_info.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------


if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: APPASERVER_DATABASE is not defined." 1>&2
	exit 1
fi

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 process" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

process=$1

process_display=`echo $process | string_initial_capital.e ',' 0`

cat << all_done
<html><head>

<link rel=stylesheet type="text/css" href=/appaserver/template/style.css>
<TITLE>$process_display</TITLE>
</head>
<body bgcolor="white" onload="window.open('https://appaserver.org','appaserver_info','menubar=no,resizeable=yes,scrollbars=yes,status=no,toolbar=no,location=no', 'false');">
<h1>$process_display</h1>
<p><a href=https://appaserver.org target=_new>Click here</a> to view the Appaserver home page.
</body>
</html>
all_done

exit 0

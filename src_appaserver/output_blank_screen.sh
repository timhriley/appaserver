#!/bin/sh
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/output_blank_screen.sh
# ---------------------------------------------------------------
#
# This is used as a filler for frames.
#
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

# echo "starting $0 $*" 1>&2

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 application bgcolor title output_content_type_yn" 1>&2
	exit 1
fi

application=$1
bgcolor=$2

if [ "$3" != "" ]
then
	title="<h1>$3</h1>"
else
	title=""
fi

if [ "$4" = "y" ]
then
	content_type_cgi.sh
fi

cat << all_done
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<link rel=stylesheet type="text/css" href=/appaserver_home/template/style.css>
</head>

<body bgcolor=$bgcolor>
$title
</body>
</html>
all_done

exit 0

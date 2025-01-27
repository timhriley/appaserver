#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/folder_attribute_exists.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application folder attribute" 1>&2
	exit 1
fi

application=$1
folder=$2
attribute=$3

export DATABASE=$application

where="folder = '$folder' and attribute = '$attribute'"

result=$(select.sh 'count(1)' folder_attribute "$where")

if [ "$result" -eq 0 ]
then
	exit 1
fi

exit 0

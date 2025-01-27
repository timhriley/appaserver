#!/bin/sh

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
   echo "Error in `basename.e $0 n`: you must first:" 1>&2
   echo "$ . set_database" 1>&2
   exit 1
fi

if [ $? -ne 0 ]
then
	exit 0
fi

echo "update attribute_datatype set attribute_datatype = 'upload_file' where attribute_datatype = 'http_filename';" | sql

echo "update attribute set attribute_datatype = 'upload_file' where attribute_datatype = 'http_filename';" | sql

exit 0


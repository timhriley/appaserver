#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/appaserver_trim_log.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

# -----------------------------------------------------------------
# Exposure: This script is executed by mysqldump_all.sh. If you run
# it multiple times in a single day, you may lose log entries.
# -----------------------------------------------------------------
maximum_lines=2000

tmpfile=$(tmpfile.sh)

application_list.sh |
while read application
do
	filename=$(appaserver_log_filename $application)

	if [ "$filename" = "" -o ! -r $filename ]
	then
		echo "Skipping $application" 1>&2
		continue
	fi

	number_lines=$(cat $filename | wc -l)

	if [ "$number_lines" -gt $maximum_lines ]
	then
		tail -$maximum_lines $filename > $tmpfile
		cp $tmpfile $filename
	fi
done

rm $tmpfile

exit 0

#!/bin/bash
# ------------------------------------------------------
# $APPASERVER_HOME/utility/diff_source_code.sh
# ------------------------------------------------------
# Freely available software: see Appaserver.org
# ------------------------------------------------------

if [ $# -ne 2 ]
then
      echo "Usage: $0 destination_directory with_copy_yn" 1>&2
      exit 1
fi

destination_directory=$1
with_copy_yn=$2

function directory_diff_source_code()
{
	filename=$1

	without_directory_filename=`basename "$filename"`

	destination_filename=$destination_directory/$without_directory_filename

	if [ -f $destination_filename ]
	then
		diff $filename $destination_filename >/dev/null

		if [ $? != 0 ]
		then
			echo "Changed: $filename"

			if [ "$with_copy_yn" = y ]
			then
				cp $filename $destination_directory
			fi
		fi
	else
		echo "Not found: $filename"

		if [ "$with_copy_yn" = y ]
		then
			cp $filename $destination_directory
		fi
	fi
}

find . -maxdepth 1 -name '*.[ch]' -print 2>/dev/null			|
while read filename
do
	directory_diff_source_code $filename
done

find . -maxdepth 1 -name '*.sh' -print 2>/dev/null			|
while read filename
do
	directory_diff_source_code $filename
done

find . -maxdepth 1 -name '*.py' -print 2>/dev/null			|
while read filename
do
	directory_diff_source_code $filename
done

find . -maxdepth 1 -name '[Mm]akefile' -print 2>/dev/null		|
while read filename
do
	directory_diff_source_code $filename
done

exit 0

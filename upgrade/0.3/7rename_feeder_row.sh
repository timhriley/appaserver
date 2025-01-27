#!/bin/sh
#create feeder_row

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

# Returns 0 if FEEDER_PHRASE.feeder_phrase exists
# -----------------------------------------------
folder_attribute_exists.sh $application feeder_phrase feeder_phrase

if [ $? -ne 0 ]
then
	exit 0
fi

(
cat << all_done
update attribute set attribute = 'feeder_row_number' where attribute = 'row_number';
update folder_attribute set attribute = 'feeder_row_number' where attribute = 'row_number';
alter table feeder_row change column \`row_number\` feeder_row_number int not null;
all_done
) | sql.e

exit 0

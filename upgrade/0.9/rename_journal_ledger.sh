#!/bin/bash
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

sql.e << all_done
alter table journal_ledger rename as journal;

update relation set folder = 'journal' where folder = 'journal_ledger';
update relation set related_folder = 'journal' where related_folder = 'journal_ledger';
update role_folder set folder = 'journal' where folder = 'journal_ledger';
update folder_operation set folder = 'journal' where folder = 'journal_ledger';
update folder_attribute set folder = 'journal' where folder = 'journal_ledger';
update foreign_attribute set related_folder = 'journal' where related_folder = 'journal_ledger';
update folder set folder = 'journal' where folder = 'journal_ledger';
alter table journal drop index journal_ledger;
create unique index journal on journal (full_name,street_address,transaction_date_time,account);
update process set command_line = 'journal_trigger full_name street_address transaction_date_time account \$state preupdate_transaction_date_time preupdate_account' where process = 'post_change_journal_ledger';
all_done

exit 0

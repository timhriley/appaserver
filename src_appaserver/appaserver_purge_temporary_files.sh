#!/bin/sh
# -------------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/appaserver_purge_temporary_files.sh
# -------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# -------------------------------------------------------------------

spreadsheet_delay_minutes=30
update_delay_minutes=180

# First, purge $document_root/appaserver/*/data/
# ----------------------------------------------
document_root=`document_root`
appaserver_directory=${document_root}/appaserver

cd $appaserver_directory

find . -type d -name data					|
while read directory
do
	find	$directory					\
		-type f						\
		-name '*_[1-9][0-9][0-9]*'			\
		-mmin +$spreadsheet_delay_minutes		\
		-exec rm -f {} \; 2>/dev/null
done

# Second, purge appaserver data temporary files
# ---------------------------------------------
appaserver_data_directory=`appaserver_data_directory`

find	$appaserver_data_directory				\
	-type f							\
	-name '*_[1-9][0-9][0-9]*'				\
	-mmin +$update_delay_minutes				\
	-exec rm -f {} \; 2>/dev/null

exit 0

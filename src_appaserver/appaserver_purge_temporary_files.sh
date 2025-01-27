#!/bin/sh
# -------------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/appaserver_purge_temporary_files.sh
# -------------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# -------------------------------------------------------------------

post_login_delay_minutes=5
non_data_delay_minutes=30
data_delay_minutes=180
everything_delay_minutes=360

. /etc/profile

appaserver_data_directory=`appaserver_data_directory`

if [ "$appaserver_data_directory" = "" ]
then
	echo "ERROR in $0: appaserver_data_directory is empty" 1>&2
	exit 1
fi

cd $appaserver_data_directory

if [ $? -ne 0 ]
then
	echo "ERROR in $0: cd $appaserver_data_directory failed" 1>&2
	exit 1
fi

# purge post_login files
# ----------------------
find	.					\
	-type f					\
	-name 'post_login_*.html'		\
	-mmin +$post_login_delay_minutes	\
	-exec rm -f {} \; 2>/dev/null

# purge non-data files
# --------------------
find	.					\
	-type f					\
	-name '*.csv'				\
	-mmin +$non_data_delay_minutes		\
	-exec rm -f {} \; 2>/dev/null

find	.					\
	-type f					\
	-name '*.sh'				\
	-mmin +$non_data_delay_minutes		\
	-exec rm -f {} \; 2>/dev/null

find	.					\
	-type f					\
	-name '*.zip'				\
	-mmin +$non_data_delay_minutes		\
	-exec rm -f {} \; 2>/dev/null

find	.					\
	-type f					\
	-name '*.aux'				\
	-mmin +$non_data_delay_minutes		\
	-exec rm -f {} \; 2>/dev/null

find	.					\
	-type f					\
	-name '*.pdf'				\
	-mmin +$non_data_delay_minutes		\
	-exec rm -f {} \; 2>/dev/null

find	.					\
	-type f					\
	-name '*.tex'				\
	-mmin +$non_data_delay_minutes		\
	-exec rm -f {} \; 2>/dev/null

find	.					\
	-type f					\
	-name '*.log'				\
	-mmin +$non_data_delay_minutes		\
	-exec rm -f {} \; 2>/dev/null

find	.					\
	-type f					\
	-name '*.ps'				\
	-mmin +$non_data_delay_minutes		\
	-exec rm -f {} \; 2>/dev/null

find	.					\
	-type f					\
	-name '*.gv'				\
	-mmin +$non_data_delay_minutes		\
	-exec rm -f {} \; 2>/dev/null

# purge appaserver data temporary files
# -------------------------------------
find	.								\
	-type f,l							\
	-name '*_[1-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]*'	\
	-mmin +$data_delay_minutes					\
	-exec rm -f {} \; 2>/dev/null

# purge everything
# ----------------
find	.								\
	-type f								\
	-mmin +$everything_delay_minutes				\
	-exec rm -f {} \; 2>/dev/null

exit 0

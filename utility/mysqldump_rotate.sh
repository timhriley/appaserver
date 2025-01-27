:
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_rotate.sh
#
# This script rotates /var/backups/appaserver/$database/
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------
if [ "$#" -ne 6 ]
then
	echo "Usage: $0 date_yyyymmdd son father grandfather greatgrandfather greatgreatgrandfather" 1>&2
	exit 1
fi

date_yyyymmdd=$1
son=$2
father=$3
grandfather=$4
greatgrandfather=$5
greatgreatgrandfather=$6

son_keep_days=14
father_keep_days=31
grandfather_keep_days=185
greatgrandfather_keep_days=1100
greatgreatgrandfather_keep_days=2555

link_files()
{
	son=$1
	destination_directory=$2
	date_yyyymmdd=$3

	pwd=`pwd`
	cd $son

	for file in `	ls -1 2>&1 | grep -vi 'no such file'	|\
			grep $date_yyyymmdd`
	do
		if [ ! -f $destination_directory/$file ]
		then
			ln $file $destination_directory/$file
		fi
	done

	cd $pwd
}

# Manage son
# ----------
purge_old_filenames.sh $son $son_keep_days

# Manage father 
# -------------
results=`date_grandfather_father_son.sh $date_yyyymmdd father`

if [ $results -eq 1 ]
then
	link_files $son $father $date_yyyymmdd
fi

purge_old_filenames.sh $father $father_keep_days

# Manage grandfather 
# ------------------
results=`date_grandfather_father_son.sh $date_yyyymmdd grandfather`

if [ $results -eq 1 ]
then
	link_files $son $grandfather $date_yyyymmdd
fi

purge_old_filenames.sh $grandfather $grandfather_keep_days

# Manage greatgrandfather 
# -----------------------
results=`date_grandfather_father_son.sh $date_yyyymmdd greatgrandfather`

if [ $results -eq 1 ]
then
	link_files $son $greatgrandfather $date_yyyymmdd
fi

purge_old_filenames.sh $greatgrandfather $greatgrandfather_keep_days

# Manage greatgreatgrandfather 
# ----------------------------
results=`date_grandfather_father_son.sh $date_yyyymmdd greatgreatgrandfather`

if [ $results -eq 1 ]
then
	link_files $son	$greatgreatgrandfather $date_yyyymmdd
fi

purge_old_filenames.sh	$greatgreatgrandfather			\
			$greatgreatgrandfather_keep_days

exit 0

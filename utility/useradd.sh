:
# $APPASERVER_HOME/utility/useradd.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -lt 2 ]
then
	echo "Usage: `echo $0 | basename.e` username sudo_yn [additional_group]" 1>&2
	echo "Note: a default group named username will be assigned" 1>&2
	exit 1
fi

username=$1
sudo_yn=$2

if [ "$#" -eq 3 ]
then
	additional_group=$3
fi

# Create the username and default group
# -------------------------------------
sudo useradd -m $username
sudo usermod -a -G $username $username
sudo usermod $username -s /bin/bash

if [ "$sudo_yn" = 'y' -o "$sudo_yn" = 'Y' ]
then
	sudo usermod -a -G sudo $username
fi

# Assign to additional group
# --------------------------
if [ "$additional_group" != "" ]
then
	sudo addgroup $additional_group
	sudo usermod -a -G $additional_group $username
fi

sudo passwd $username

exit 0


#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/host-authorize.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: sudo `basename.e $0 n` username hostname" 1>&2
	exit 1
fi

if [ $UID -ne 0 ]
then
	sudo -p "Restarting with sudo. Password: " bash $0 $*
	exit $?
fi

username=$1
hostname=$2

ssh $username@$hostname 			\
	"cat >> ~/.ssh/authorized_keys"		\
	< /home/$username/.ssh/id_rsa.pub

exit $?

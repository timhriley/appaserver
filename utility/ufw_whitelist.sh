#!/bin/bash
# -----------------------------------------
# $APPASERVER_HOME/utility/ufw_whitelist.sh
# -----------------------------------------


if [ "$#" -ne 1 ]
then
	echo "Usage: $0 ip_address" 1>&2
	exit 1
fi

ip_address=$1

whitelistfile="/usr2/ufw/ufw_whitelist.dat"

if [ ! -r $whitelistfile ]
then
	echo "Warning in $0: cannot read from $whitelistfile" 1>&2

	# Shell okay
	# ----------
	exit 0
fi

result=`ufw_whitelist.e | grep "^${ip_address}$"`

if [ "$result" != "" ]
then
	# Shell okay
	# ----------
	exit 0
else
	exit 1
fi


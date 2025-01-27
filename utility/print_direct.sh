:
# utility/print_direct.sh
# -----------------------

if [ "$1" = "ff" ]
then
	nl.e ff > /dev/lp0
else
	nl.e > /dev/lp0
fi
exit 0

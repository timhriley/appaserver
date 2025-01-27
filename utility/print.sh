:

printer=/dev/lp0

if [ "$#" -eq 0 ]
then
	/usr2/utility/nl.e ff > $printer
else
	while [ "$#" -gt 0 ]
	do
		/usr2/utility/nl.e ff < $1 > $printer
		shift
	done
fi

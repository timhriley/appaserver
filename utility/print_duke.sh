:
# /usr2/utility/print_duke.sh
# ---------------------------

# printer=/dev/lp1
# /usr2/utility/nl.e ff > $printer

printer_flags="-h -Pps"

if [ "$#" -gt 0 ]
then
	while true
	do
		lpr $printer_flags $1
		shift
		if [ "$#" -eq 0 ]
		then
			break
		fi
	done
else
	lpr $printer_flags
fi


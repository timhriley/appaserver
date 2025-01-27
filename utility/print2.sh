:

destination=hp_laserjet_2

if [ "$#" -eq 0 ]
then
	lp -d $destination
else
	while [ "$#" -gt 0 ]
	do
		lp -d $destination $1
		shift
	done
fi

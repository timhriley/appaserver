:
make

if [ "$?" -eq 0 ]
then
	cd ../src_hydrology
	make post_hydrology_quick_measurement_form
fi


:
make

if [ "$?" -eq 0 ]
then
	cd ../src_hydrology
	touch hydrology_quick_measurement_form.c
	make hydrology_quick_measurement_form
fi


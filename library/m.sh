:

make

if [ "$?" -eq 0 ]
then
	cd ../src_predictive
	make balance_sheet_output
fi


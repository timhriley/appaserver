:

make

if [ "$?" -eq 0 ]
then
	cd ../src_predictive
	make budget_report
fi


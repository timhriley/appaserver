:

make

if [ "$?" -eq 0 ]
then
	cd ../src_predictive
	make tax_form_report
fi


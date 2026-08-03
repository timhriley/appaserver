:

make

if [ "$?" -eq 0 ]
then
	cd ../src_predictive
	make generate_invoice
fi


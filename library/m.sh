:
make

if [ "$?" -eq 0 ]
then
	cd ../src_education
	make generate_invoice
fi


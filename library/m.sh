:

make

if [ "$?" -eq 0 ]
then
	cd ../src_predictive
	make income_statement_output
fi


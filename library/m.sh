:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_grace_chart
fi


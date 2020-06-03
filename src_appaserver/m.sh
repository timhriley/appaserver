:
cd ../library
make
if [ "$?" -eq 0 ]
then
	cd -
	make generic_output_merged_datasets
fi


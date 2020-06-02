:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	touch generic_output_merged_datasets.c
	make generic_output_merged_datasets
fi


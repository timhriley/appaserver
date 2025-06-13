:

make

if [ "$?" -eq 0 ]
then
	cd ../src_predictive
	make prior_fixed_asset_trigger
fi


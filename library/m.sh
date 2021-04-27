:
make

if [ "$?" -eq 0 ]
then
	cd ../src_predictive
	make trial_balance
fi


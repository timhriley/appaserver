:
if [ "$#" -eq 1 ]
then
	input_agr_file=$1
fi

cd /usrb2/grace-5.1.3

if [ "$input_agr_file" != "" ]
then
	/usrb2/grace-5.1.3/bin/xmgrace -pipe < $input_agr_file
else
	/usrb2/grace-5.1.3/bin/xmgrace
fi

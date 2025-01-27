:
# $APPASERVER_HOME/utility/generate_password.sh
# ---------------------------------------------

if [ "$#" -eq 1 ]
then
	how_many=$1
else
	how_many=1
fi

while [ $how_many -gt 0 ]
do
	first_word=`random_word.sh`
	middle_number=`random.e 99`
	second_word=`random_word.sh`
	
	echo ${first_word}${middle_number}${second_word}

	how_many=`expr $how_many - 1`
done

exit 0

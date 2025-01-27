:
# Son = everyday
# Father = Sunday
# Grandfather = First of the month
# GreatGrandfather = January, April, July, and October 1
# GreatGreatGrandfather = January 1

ancestor="father|grandfather|greatgrandfather|greatgreatgrandfather"

if [ "$#" -ne 2 ]
then
	echo "Usage: `basename.e $0 n` date_yyyymmdd ancestor" 1>&2
	echo "ancestor is $ancestor" 1>&2
	exit 1
fi

date=$1
ancestor=$2

result=`date_grandfather_father_son.e $date | grep "^${ancestor}$"`

if [ "$result" = "$ancestor" ]
then
	echo 1
else
	echo 0
fi

# Stub
exit 0

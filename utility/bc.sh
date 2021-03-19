:

# This is a wrapper around bc
# ---------------------------

if [ $# -ne 3 ]
then
	echo "Usage: operand1 operation operand2" 1>&2
	exit 1
fi

operand1=$1
operation=$2
operand2=$3

echo "scale=4; $1 $2 $3" | bc -l

exit $?

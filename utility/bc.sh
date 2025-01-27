:

# This is a wrapper around bc
# ---------------------------

if [ $# -ne 3 ]
then
	echo "Usage: $0 operand1 operation operand2" 1>&2
	exit 1
fi

operand1=$1
operation=$2
operand2=$3

if [ "$operation" = 'x' -o "$operation" = 'X' ]
then
	operation='*'
fi

echo "scale=4; $operand1 $operation $operand2" | bc -l

exit $?

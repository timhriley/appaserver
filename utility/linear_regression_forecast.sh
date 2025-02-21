:
# $APPASERVER_HOME/utility/linear_regression_forecast.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 julian_csv regression_out date_label balance_label forecast_julian" 1>&2
	exit 1
fi

julian_csv=$1
regression_out=$2
date_label=$3
balance_label=$4
forecast_julian=$5

linear_regression.sh $julian_csv $date_label $balance_label |
cat > $regression_out

a=`grep ^a $regression_out | column.e 1`
b=`grep ^b $regression_out | column.e 1`

slope_expression=`bc.sh $forecast_julian x $a`
forecast=`bc.sh $slope_expression + $b`

confidence_double=`grep ^linear_coefficient_of_determination $regression_out |\
		   column.e 1`

confidence_integer=`bc.sh $confidence_double x 100.0 | round_integer.e 0 ^`

echo "$forecast,$confidence_integer"

exit 0

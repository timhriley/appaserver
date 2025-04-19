:
# $APPASERVER_HOME/utility/linear_regression.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

#echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 filename xlabel ylabel" 1>&2
	exit 1
fi

filename=$1
xlabel=$2
ylabel=$3

(
R --quiet --no-save --slave << done

# Input
# -----
table = read.csv(file="$filename",head=T)

# Process
# -------
x = table\$${xlabel}
y = table\$${ylabel}
fit = lm( y ~ x )

y_intercept = fit\$coefficients[[1]]
slope = fit\$coefficients[[2]]
linear_coefficient_of_determination = summary(fit)\$r.squared
degrees_of_freedom = df.residual( fit )
sum_squared_residuals = deviance( fit )
residual_standard_deviation =
	sqrt( sum_squared_residuals / degrees_of_freedom )
slope_standard_deviation = summary( fit )[[4]][4]
slope_t_value = abs( summary( fit )[[4]][6] )
slope_test_statistic = abs( qt( 0.05 / 2, degrees_of_freedom ) )

slope_is_significant = FALSE
if ( slope_t_value >= slope_test_statistic )
	slope_is_significant = TRUE

minimum_slope = slope -
			( slope_test_statistic *
			  slope_standard_deviation )
maximum_slope = slope +
			( slope_test_statistic *
			  slope_standard_deviation )

y_intercept_minimum = confint( fit, level = 0.95 )[ 1,1 ]
y_intercept_maximum = confint( fit, level = 0.95 )[ 1,2 ]

# Output
# ------
results = sprintf( "a %s", slope )
print( results )
results = sprintf( "b %s", y_intercept )
print( results )
results = sprintf( "error %s",
			residual_standard_deviation )
print( results )

results = sprintf( "slope %s", slope )
print( results )

results = sprintf( "y_intercept %s", y_intercept )
print( results )

results = sprintf( "linear_coefficient_of_determination %s",
			linear_coefficient_of_determination )
print( results )

results = sprintf( "r_squared %s", linear_coefficient_of_determination )
print( results )

results = sprintf( "residual_standard_deviation %s",
			residual_standard_deviation )
print( results )

results = sprintf( "slope_standard_deviation %s",
			slope_standard_deviation )
print( results )

results = sprintf( "slope_t_value %s", slope_t_value )
print( results )

results = sprintf( "slope_test_statistic %s",
			slope_test_statistic )
print( results )

results = sprintf( "slope_is_significant %s",
			slope_is_significant )
print( results )

results = sprintf( "minimum_slope %s",
			minimum_slope )
print( results )

results = sprintf( "maximum_slope %s",
			maximum_slope )
print( results )

results = sprintf( "y_intercept_minimum %s",
			y_intercept_minimum )
print( results )

results = sprintf( "y_intercept_maximum %s",
			y_intercept_maximum )
print( results )

quit()
done
) 									|
sed 's/^\[1\] //g'							|
sed 's/"//g'								|
times_ten_to_power.e ' ' 1						|
cat

exit 0

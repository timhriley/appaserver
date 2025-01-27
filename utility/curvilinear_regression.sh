:
# $APPASERVER_HOME/utility/curvilinear_regression.sh
# -------------------------------------------------------------
# No warranty and freely available software: see Appaserver.org
# -------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

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
x_poly = I( x ^ 2 )
fit = lm( y ~ x + x_poly )

y_intercept = fit\$coefficients[[1]]
beta = fit\$coefficients[[2]]
quadratic_beta = fit\$coefficients[[3]]
curvilinear_coefficient_of_determination = summary(fit)\$r.squared
degrees_of_freedom = df.residual( fit )
sum_squared_residuals = deviance( fit )
residual_standard_deviation =
	sqrt( sum_squared_residuals / degrees_of_freedom )
quadratic_beta_standard_deviation = summary( fit )[[4]][6]
quadratic_beta_t_value = abs( summary( fit )[[4]][9] )
quadratic_beta_test_statistic = abs( qt( 0.05 / 2, degrees_of_freedom ) )

quadratic_beta_is_significant = 0
if ( quadratic_beta_t_value >= quadratic_beta_test_statistic )
	quadratic_beta_is_significant = 1

minimum_quadratic_beta = quadratic_beta -
				( quadratic_beta_test_statistic *
				  quadratic_beta_standard_deviation )

maximum_quadratic_beta = quadratic_beta +
					( quadratic_beta_test_statistic *
					  quadratic_beta_standard_deviation )

# Output
# ------
results = sprintf( "a %s", quadratic_beta )
print( results )
results = sprintf( "b %s", beta )
print( results )
results = sprintf( "c %s", y_intercept )
print( results )
results = sprintf( "error %s",
			residual_standard_deviation )
print( results )

results = sprintf( "quadratic_beta %s", quadratic_beta )
print( results )
results = sprintf( "beta %s", beta )
print( results )
results = sprintf( "y_intercept %s", y_intercept )
print( results )

results = sprintf( "curvilinear_coefficient_of_determination %s",
			curvilinear_coefficient_of_determination )
print( results )

results = sprintf( "r_squared %s", curvilinear_coefficient_of_determination )
print( results )

results = sprintf( "residual_standard_deviation %s",
			residual_standard_deviation )
print( results )

results = sprintf( "quadratic_beta_standard_deviation %s",
			quadratic_beta_standard_deviation )
print( results )

results = sprintf( "quadratic_beta_t_value %s", quadratic_beta_t_value )
print( results )

results = sprintf( "quadratic_beta_test_statistic %s",
			quadratic_beta_test_statistic )
print( results )

results = sprintf( "quadratic_beta_is_significant %s",
			quadratic_beta_is_significant )
print( results )

results = sprintf( "minimum_quadratic_beta %s",
			minimum_quadratic_beta )
print( results )

results = sprintf( "maximum_quadratic_beta %s",
			maximum_quadratic_beta )
print( results )

quit()
done
) 									|
sed 's/^\[1\] //g'							|
sed 's/"//g'								|
times_ten_to_power.e ' ' 1						|
cat

exit 0

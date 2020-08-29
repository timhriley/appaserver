/* covariance_dvr.c */
/* ---------------- */

#include <stdio.h>
#include <stdlib.h>
#include "float.h"
#include "covariance.h"

int main()
{
	COVARIANCE *covariance;
	double results;
	int count;
	double average;
	double standard_deviation;
	double correlation;

	covariance = covariance_new_covariance(	"team_batting_average" );

	covariance_set_double(	covariance->anchor_datatype_group,
				covariance->compare_covariance_datatype_list,
				"team_batting_average",
				"atlanta",
				0.276 );
	covariance_set_double(	covariance->anchor_datatype_group,
				covariance->compare_covariance_datatype_list,
				"team_earned_run_average",
				"atlanta",
				3.33 );
	covariance_set_double(	covariance->anchor_datatype_group,
				covariance->compare_covariance_datatype_list,
				"winning_percentage",
				"atlanta",
				0.625 );
	covariance_set_double(	covariance->anchor_datatype_group,
				covariance->compare_covariance_datatype_list,
				"team_batting_average",
				"los_angeles",
				0.249 );
	covariance_set_double(	covariance->anchor_datatype_group,
				covariance->compare_covariance_datatype_list,
				"team_earned_run_average",
				"los_angeles",
				3.51 );
	covariance_set_double(	covariance->anchor_datatype_group,
				covariance->compare_covariance_datatype_list,
				"winning_percentage",
				"los_angeles",
				0.512 );

	if ( covariance_get_results(	
				&results,
				&count,
				&average,
				&standard_deviation,
				&correlation,
				covariance->anchor_datatype_group,
				covariance->compare_covariance_datatype_list,
				"team_earned_run_average" ) )
	{
		printf(
"covariance between team batting average and team earned run average = %lf\n",
			results );
	}
	else
	{
		fprintf( stderr, "%s ERROR occurred\n", __FILE__ );
		return 1;
	}

	return 0;
} /* main() */


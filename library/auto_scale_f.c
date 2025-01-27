#include <math.h>

void auto_scale_f( double *scale_min, double *scale_max, double *major_interval, int *minor_intervals, double min, double max )
{

double range ;

double log_min ; 
double log_max ;

double floor_log_min ; 
double floor_log_max ;

double scale_min_sign = 1.0 ;  /* sign as in a positive sign + */
double scale_max_sign = 1.0 ; 

double increment ; 

/*
**	Initializations
*/

log_min = -99.0 ;
log_max = -99.0 ;

floor_log_min = -99.0 ;
floor_log_max = -99.0 ;

/*
**	We are going to use logs to determine magnitudes, so 
**	we will have trouble with negative values.  so...
**	check and set the sign and use abs
*/

if( min < 0 ) scale_min_sign = -1.0 ;
if( max < 0 ) scale_max_sign = -1.0 ;

/*
	min = fabs( min ) ;
	max = fabs( max ) ;
*/


/* 
**	get magnitudes 
**	get floors of magnitudes
**	and starting points for scale min and max 
*/

range = max -  min ;

if( min > 0 ) 
{
	log_min = log10( min ) ; 
	floor_log_min = floor( log_min ) ; 
 	*scale_min = floor_log_min * ( pow( 10.0, floor_log_min ) ) ;
}

if( max > 0 ) 
{
	log_max = log10( max ) ; 
	floor_log_max = floor( log_max ) + 1 ; 
	*scale_max = floor_log_max * ( pow( 10.0, floor_log_max ) ) ;
}


/* 
**	Set the min and max increments based on the range
*/

if( range > 0.0 && range <= 1.0 )
{
	increment=0.1 ; 
	*major_interval = 0.2 ; 
        *minor_intervals = 1 ;
}
else if( range > 1.0 && range <= 3.0 )
{
	increment=0.2 ;  
	*major_interval = 0.2 ; 
	*minor_intervals = 1 ;
}
else if( range > 3.0 && range <= 5.0 )
{
	increment=0.5 ;  
	*major_interval = 0.5 ; 
	*minor_intervals = 4 ;
}
else if( range > 5.0 && range <= 10.0 )
{
	increment=1.0 ;  
	*major_interval = 0.5 ; 
	*minor_intervals =  1 ;
}
else if( range > 10.0 && range <= 20.0 )
{
	increment=2.0 ;  
	*major_interval = 2.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 20.0 && range <= 50.0 )
{
	increment=5.0 ;  
	*major_interval = 10.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 50.0 && range <= 100.0 )
{
	increment=5.0 ;  
	*major_interval = 20.0 ; 
	*minor_intervals = 3 ;
}
else if( range > 100.0 && range <= 150.0 )
{
	increment=10.0 ;  
	*major_interval = 10.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 150.0 && range <= 200.0 )
{
	increment=10.0 ;  
	*major_interval = 20.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 200.0 && range <= 250.0 )
{
	increment=10.0 ;  
	*major_interval = 40.0 ; 
	*minor_intervals = 3 ;
}
else if( range > 250.0 && range <= 300.0 )
{
	increment=10.0 ;  
	*major_interval = 40.0 ; 
	*minor_intervals = 2 ;
}
else if( range > 300.0 && range <= 400.0 )
{
	increment=20.0 ;  
	*major_interval = 50.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 400.0 && range <= 500.0 )
{
	increment=20.0 ;  
	*major_interval = 50.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 500.0 && range <= 600.0 )
{
	increment=20.0 ;  
	*major_interval = 60.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 600.0 && range <= 800.0 )
{
	increment=50.0 ;  
	*major_interval = 100.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 800.0 && range <= 1000.0 )
{
	increment=50.0 ;  
	*major_interval = 200.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 1000.0 && range <= 1500.0 )
{
	increment=50.0 ;  
	*major_interval = 300.0 ; 
	*minor_intervals = 2 ;
}
else if( range > 1500.0 && range <= 2000.0 )
{
	increment=50.0 ;  
	*major_interval = 300.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 2000.0 && range <= 2500.0 )
{
	increment=50.0 ;  
	*major_interval = 400.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 2500.0 && range <= 3000.0 )
{
	increment=50.0 ;  
	*major_interval = 400.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 3000.0 && range <= 4000.0 )
{
	increment=50.0 ;  
	*major_interval = 500.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 4000.0 && range <= 5000.0 )
{
	increment=50.0 ;  
	*major_interval = 500.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 5000.0 && range <= 6000.0 )
{
	increment=50.0 ;  
	*major_interval = 500.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 6000.0 && range <= 7000.0 )
{
	increment=50.0 ;  
	*major_interval = 500.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 7000.0 && range <= 8000.0 )
{
	increment=50.0 ;  
	*major_interval = 500.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 8000.0 && range <= 10000.0 )
{
	increment=50.0 ;  
	*major_interval = 1000.0 ; 
	*minor_intervals = 4 ;
}

else if( range > 10000.0 && range <= 15000.0 )
{
	increment=1000.0 ;  
	*major_interval = 1000.0 ; 
	*minor_intervals = 4 ;
}

else if( range > 15000.0 && range <= 20000.0 )
{
	increment=500.0 ;  
	*major_interval = 1000.0 ; 
	*minor_intervals = 1 ;
}
else if( range > 20000.0 && range <= 40000.0 )
{
	increment=1000.0 ;  
	*major_interval = 2000.0 ; 
	*minor_intervals = 4 ;
}
else if( range > 40000.0 && range <= 100000.0 )
{
	increment=2000.0 ;  
	*major_interval = 4000.0 ; 
	*minor_intervals = 4 ;
}

else if( range > 100000.0 )
{
	increment=5000.0 ;  
	*major_interval = 5000.0 ; 
	*minor_intervals = 1 ;
}


if( max > 0 )
{ 
	while( *scale_max > max )
	{ 
		*scale_max -= increment  ; 
	}

	/*
	**	when scale_max is < max  then 
	**	add back one increment and break out
	*/

		if( *scale_max < max ) *scale_max += increment ;
}
else if (max < 0 )
{
	*scale_max = 0 ;

	while( *scale_max > max )
	{ 
		*scale_max -= increment  ; 
	}

		if( *scale_max < max ) *scale_max += increment ;
}
else
{
	*scale_max = 0 ;
}

if( min > 0 )
{
	while( *scale_min < min )
	{ 
		*scale_min += increment  ; 
	}

		if( *scale_min > min ) *scale_min -= increment ;
}

else if (min < 0 )
{
	*scale_min = 0 ;

	while( *scale_min > min )
	{ 
		*scale_min -= increment  ; 
	}

}
else
{
	*scale_min = 0 ;
}
 
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "date.h"
#include "tax_recovery.h"

void test_recovery( void );
void test_recovery2( void );
void test_recovery_loop1( void );
void test_recovery_loop2( void );

void test_recovery_loop3(	double cash_basis,
				double recovery_period_years,
				int start_month,
				int start_year,
				int years );

void test_recovery_loop4(	double cash_basis,
				double recovery_period_years,
				int start_month,
				int start_year,
				int years );

int main( void )
{
	/* test_recovery(); */
	/* test_recovery_loop1(); */
	/* test_recovery_loop2(); */
	/* test_recovery2(); */

	test_recovery_loop3(	1200000.0 /* cash_basis */,
				39.0 /* recovery_period_years */,
				1 /* start_month */,
				2007 /* start_year */,
				41 /*  years */ );

#ifdef NOT_DEFINED
	test_recovery_loop4(	1200000.0 /* cash_basis */,
				39.0 /* recovery_period_years */,
				1 /* start_month */,
				2007 /* start_year */,
				41 /*  years */ );

	test_recovery_loop3(	1200000.0 /* cash_basis */,
				39.0 /* recovery_period_years */,
				1 /* start_month */,
				2007 /* start_year */,
				41 /*  years */ );

	test_recovery_loop3(	1200000.0 /* cash_basis */,
				39.0 /* recovery_period_years */,
				6 /* start_month */,
				2007 /* start_year */,
				41 /*  years */ );

	test_recovery_loop3(	1200000.0 /* cash_basis */,
				39.0 /* recovery_period_years */,
				12 /* start_month */,
				2007 /* start_year */,
				41 /*  years */ );

	test_recovery_loop3(	800000.0 /* cash_basis */,
				27.5 /* recovery_period_years */,
				1 /* start_month */,
				1993 /* start_year */,
				29 /*  years */ );

	test_recovery_loop3(	800000.0 /* cash_basis */,
				27.5 /* recovery_period_years */,
				4 /* start_month */,
				1993 /* start_year */,
				29 /*  years */ );

	test_recovery_loop3(	800000.0 /* cash_basis */,
				27.5 /* recovery_period_years */,
				5 /* start_month */,
				1993 /* start_year */,
				29 /*  years */ );

	test_recovery_loop3(	800000.0 /* cash_basis */,
				27.5 /* recovery_period_years */,
				6 /* start_month */,
				1993 /* start_year */,
				29 /*  years */ );

	test_recovery_loop3(	800000.0 /* cash_basis */,
				27.5 /* recovery_period_years */,
				7 /* start_month */,
				1993 /* start_year */,
				29 /*  years */ );

	test_recovery_loop3(	800000.0 /* cash_basis */,
				27.5 /* recovery_period_years */,
				12 /* start_month */,
				1993 /* start_year */,
				29 /*  years */ );
#endif

	return 0;
}

void test_recovery2()
{
	printf( "should be %.2lf, is %.2lf\n",
		30769.23,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				1200000.0,
				"2007-01-01"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				39.0 /* recovery_period_years */,
				2045 /* current_year */ ) );
}

void test_recovery()
{
	printf( "should be %.2lf, is %.2lf\n",
		1212.12,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				800000.0,
				"1993-01-01"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				27.5 /* recovery_period_years */,
				2020 /* current_year */ ) );

#ifdef NOT_DEFINED
	printf( "should be %.2lf, is %.2lf\n",
		29091.20,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				800000.0,
				"1993-04-01"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				27.5 /* recovery_period_years */,
				2007 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		29091.20,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				800000.0,
				"1993-04-01"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				27.5 /* recovery_period_years */,
				2019 /* current_year */ ) );


	printf( "should be %.2lf, is %.2lf\n",
		23031.20,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				800000.0,
				"1993-04-01"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				27.5 /* recovery_period_years */,
				2020 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		29091.20,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				800000.0,
				"1993-04-01"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				27.5 /* recovery_period_years */,
				2019 /* current_year */ ) );


	printf( "should be %.2lf, is %.2lf\n",
		23031.04,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				800000.0,
				"1993-04-01"
					/* service_placement_date_string */,
				"2007-10-07" /* sale_date_string */,
				27.5 /* recovery_period_years */,
				2007 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		31746.0,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				1000000.0,
				"1993-03-02"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				31.5 /* recovery_period_years */,
				2007 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		1322.8,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				1000000.0,
				"1993-03-02"
					/* service_placement_date_string */,
				"2007-01-05" /* sale_date_string */,
				31.5 /* recovery_period_years */,
				2007 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		3846.15,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				1200000.0,
				"2007-11-19"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				39.0 /* recovery_period_years */,
				2007 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		30769.0,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				1200000.0,
				"2007-11-19"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				39.0 /* recovery_period_years */,
				2008 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		11538.46,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				1200000.0,
				"2007-11-19"
					/* service_placement_date_string */,
				"2008-05-21" /* sale_date_string */,
				39.0 /* recovery_period_years */,
				2008 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		30769.0,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				1200000.0,
				"2007-11-19"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				39.0 /* recovery_period_years */,
				2044 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		30769.0,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				1200000.0,
				"2007-11-19"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				39.0 /* recovery_period_years */,
				2044 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		30769.0,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				1200000.0,
				"2007-11-19"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				39.0 /* recovery_period_years */,
				2045 /* current_year */ ) );


	printf( "should be %.2lf, is %.2lf\n",
		26923.0,
		tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				1200000.0,
				"2007-11-19"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				39.0 /* recovery_period_years */,
				2046 /* current_year */ ) );
#endif

}

void test_recovery_loop1()
{
	DATE *annual;
	int year;
	char beginning[ 16 ];
	int y;
	int years;
	double recovery;
	double total = 0.0;
	double cash_basis;

	cash_basis = 800000.0;
	year = 1993;
	years = 30;
	sprintf( beginning, "%d-12-31", year );

	annual = date_yyyy_mm_dd_new( beginning );

	for( y = 0; y < years; y++ )
	{
		recovery =
			 tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				cash_basis,
				"1993-04-01"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				27.5 /* recovery_period_years */,
				date_get_year( annual ) /* current_year */ );

		printf( "%.2d: %s: %.2lf\n",
			y + 1,
			date_display( annual ),
			recovery );

		date_increment_months( annual, 12 );
		total += recovery;
	}

	printf( "Cash basis should be %.2lf, is %.2lf\n", cash_basis, total );
}

void test_recovery_loop2()
{
	DATE *annual;
	int start_year;
	char beginning[ 16 ];
	int y;
	int years;
	double recovery;
	double recovery_period_years;
	double total;
	double cash_basis;

	cash_basis = 1200000.0;
	recovery_period_years = 39.0;
	start_year = 2007;
	years = 40;
	sprintf( beginning, "%d-12-31", start_year );

	annual = date_yyyy_mm_dd_new( beginning );

	total = 0.0;
	for( y = 0; y < years; y++ )
	{
		printf( "%.2d: %s:",
			y + 1,
			date_display( annual ) );

		recovery =
			 tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				1200000.0,
				"2007-11-19"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				recovery_period_years,
				date_get_year( annual ) /* current_year */ );

		printf( " %.2lf\n", recovery );

		date_increment_months( annual, 12 );
		total += recovery;
	}

	printf( "Cash basis should be %.2lf, is %.2lf\n", cash_basis, total );
}

void test_recovery_loop3(	double cash_basis,
				double recovery_period_years,
				int start_month,
				int start_year,
				int years )
{
	DATE *annual;
	char beginning[ 16 ];
	char service_placement_date_string[ 16 ];
	int y;
	double recovery;
	double total;

	sprintf( beginning, "%d-12-31", start_year );

	sprintf(	service_placement_date_string,
			"%d-%.2d-01",
			start_year,
			start_month );

	annual = date_yyyy_mm_dd_new( beginning );

	total = 0.0;

	for( y = 0; y < years; y++ )
	{
		printf( "%.2d: %s:",
			y + 1,
			date_display( annual ) );

		recovery =
			 tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				cash_basis,
				service_placement_date_string,
				(char *)0 /* sale_date_string */,
				recovery_period_years,
				date_get_year( annual ) /* current_year */ );

		printf( " %.2lf\n", recovery );

		date_increment_months( annual, 12 );
		total += recovery;
	}

	printf( "Cash basis should be %.2lf, is %.2lf\n", cash_basis, total );
}

void test_recovery_loop4(	double cash_basis,
				double recovery_period_years,
				int start_month,
				int start_year,
				int years )
{
	DATE *annual;
	char beginning[ 16 ];
	char service_placement_date_string[ 16 ];
	int y;
	double recovery;
	double total;

	sprintf( beginning, "%d-12-31", start_year );

	sprintf(	service_placement_date_string,
			"%d-%.2d-01",
			start_year,
			start_month );

	annual = date_yyyy_mm_dd_new( beginning );

	total = 0.0;

	for( y = 0; y < years; y++ )
	{
		if ( y == 38 )
		{
			printf( "%.2d: %s:",
				y + 1,
				date_display( annual ) );

			recovery =
			 tax_recovery_calculate_recovery_amount(
				(double *)0 /* recovery_percent */,
				cash_basis,
				service_placement_date_string,
				(char *)0 /* sale_date_string */,
				recovery_period_years,
				date_get_year( annual ) /* current_year */ );

			printf( " %.2lf\n", recovery );
		}

		date_increment_years( annual, 1 );
		total += recovery;
	}

/*
	printf( "Cash basis should be %.2lf, is %.2lf\n", cash_basis, total );
*/
}


/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_payroll.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "list.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "date.h"
#include "employee.h"
#include "entity.h"
#include "boolean.h"

void post_payroll_transaction_insert(
		char *application_name,
		LIST *employee_work_period_list );

void post_payroll_payroll_posting_insert(
		char *application_name,
		PAYROLL_POSTING *payroll_posting );

void post_payroll_update(
		LIST *employee_work_period_list,
		int payroll_year );

void post_payroll_delete_execute(
		char *application_name,
		int payroll_year,
		int payroll_period_number,
		LIST *employee_work_period_list );

void post_payroll_delete(
		char *application_name,
		int payroll_year,
		int payroll_period_number,
		boolean execute );

void post_payroll_propagate(
		char *application_name,
		char *salary_wage_expense_account,
		char *payroll_tax_account,
		char *payroll_payable_account,
		char *federal_withholding_payable_account,
		char *state_withholding_payable_account,
		char *social_security_payable_account,
		char *medicare_payable_account,
		char *retirement_plan_payable_account,
		char *health_insurance_payable_account,
		char *union_dues_payable_account,
		char *federal_unemployment_tax_payable_account,
		char *state_unemployment_tax_payable_account,
		char *propagate_transaction_date_time );

void post_payroll_insert(
		char *application_name,
		LIST *employee_work_period_list );

void post_insert(
		char *application_name,
		PAYROLL_POSTING *payroll_posting );

void post_payroll_journal_display(
		char *application_name,
		LIST *employee_work_period_list );

double calculate_payroll_tax_percent(
		double gross_pay,
	 	double payroll_tax_amount );

void post_payroll(
		char *application_name,
		LIST *employee_list,
		int payroll_year,
		int payroll_period_number,
		char *begin_work_date,
		char *end_work_date,
		ENTITY_SELF *self );

void post_payroll_employee_display(
		boolean delete,
		int payroll_year,
		int payroll_period_number,
		LIST *employee_work_period_list );

void post_payroll_display(
		char *application_name,
		boolean delete,
		LIST *employee_list,
		int payroll_year,
		int payroll_period_number,
		char *begin_work_date,
		char *end_work_date,
		ENTITY_SELF *self );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *begin_work_date = {0};
	char *end_work_date = {0};
	int payroll_year = 0;
	int payroll_period_number = 0;
	boolean delete;
	boolean with_html;
	boolean execute;
	APPASERVER_PARAMETER *appaserver_parameter;
	ENTITY_SELF *self;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s process payroll_year period_number delete_yn withhtml_yn execute_yn\n",
			 argv[ 0 ] );
		fprintf( stderr,
"Note: if payroll_year and period_number are missing, then the prior period will be posted.\n" );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	payroll_year = atoi( argv[ 2 ] );
	payroll_period_number = atoi( argv[ 3 ] );
	delete = (*argv[ 4 ] == 'y');
	with_html = (*argv[ 5 ] == 'y');
	execute = (*argv[ 6 ] == 'y');

	appaserver_parameter = appaserver_parameter_new();

	if ( with_html )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name /* title */ );
	}

	if ( ! ( self =
			entity_self_fetch(
				0 /* not fetch_entity_boolean */ ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: entity_self_fetch() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !payroll_year )
	{
		employee_get_prior_period(
			&begin_work_date,
			&end_work_date,
			&payroll_year,
			&payroll_period_number,
			self->payroll_pay_period,
			self->payroll_beginning_day );
	}
	else
	if ( !employee_get_payroll_begin_end_work_dates(
			&begin_work_date,
			&end_work_date,
			entity_get_payroll_pay_period_string(
				self->payroll_pay_period ),
			payroll_year,
			payroll_period_number,
			self->payroll_beginning_day ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get begin/end dates.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( delete )
	{
		post_payroll_delete(
			application_name,
			payroll_year,
			payroll_period_number,
			execute );

		if ( with_html )
		{
			if ( execute )
			{
				printf( "<h3>Process complete.</h3>\n" );
			}

			document_close();
		}

		exit( 0 );
	}

	self->employee_list =
		employee_fetch_list(
			application_name,
			begin_work_date,
			end_work_date );

	if ( !execute )
	{
		post_payroll_display(
			application_name,
			0 /* not delete */,
			self->employee_list,
			payroll_year,
			payroll_period_number,
			begin_work_date,
			end_work_date,
			self );
	}
	else
	{
		post_payroll(
			application_name,
			self->employee_list,
			payroll_year,
			payroll_period_number,
			begin_work_date,
			end_work_date,
			self );

		if ( with_html )
		{
			printf( "<h3>Process complete.</h3>\n" );
		}
	}

	if ( with_html ) document_close();

	exit( 0 );

} /* main() */

void post_payroll_display(
			char *application_name,
			boolean delete,
			LIST *employee_list,
			int payroll_year,
			int payroll_period_number,
			char *begin_work_date,
			char *end_work_date,
			ENTITY_SELF *self )
{
	PAYROLL_POSTING *payroll_posting;
	EMPLOYEE_TAX_WITHHOLDING_TABLE *employee_tax_withholding_table;

	employee_tax_withholding_table =
		employee_tax_withholding_table_new(
			application_name );

	if ( ! ( payroll_posting =
			employee_get_payroll_posting(
				employee_list,
				payroll_year,
				payroll_period_number,
				begin_work_date,
				end_work_date,
				self,
				employee_tax_withholding_table ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get payroll_posting.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	post_payroll_employee_display(
		delete,
		payroll_year,
		payroll_period_number,
		payroll_posting->employee_work_period_list );

	post_payroll_journal_display(
		application_name,
		payroll_posting->employee_work_period_list );

} /* post_payroll_display() */

void post_payroll_employee_display(
			boolean delete,
			int payroll_year,
			int payroll_period_number,
			LIST *employee_work_period_list )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *format_string;
	char *heading_list_string;
	char *justify_list_string;
	EMPLOYEE_WORK_PERIOD *e;
	char delete_message[ 16 ];
	char sub_title[ 128 ];
	double payroll_tax_percent;

	format_string =
"%s^%.1lf^%.1lf^%.2lf^%.2lf^%.2lf^%.2lf%c^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf\n";

	heading_list_string = "full_name,regular_hours,overtime_hours,gross_pay,net_pay,payroll_tax,tax_percent,federal_withhold,state_withhold,social_employee,social_employer,medicare_employee,medicare_employer,federal_unemployment,state_unemployment";

	justify_list_string = "left,right";

	if ( delete )
		strcpy( delete_message, "Delete " );
	else
		*delete_message = '\0';

	sprintf(	sub_title,
			"%sYear: %d Period: %d",
			delete_message,
			payroll_year,
			payroll_period_number );

	sprintf( sys_string,
		 "html_table.e '^%s' '%s' '^' '%s'",
		 sub_title,
		 heading_list_string,
		 justify_list_string );

	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( employee_work_period_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		e = list_get_pointer( employee_work_period_list );

		if ( !e->gross_pay ) continue;

		payroll_tax_percent =
			calculate_payroll_tax_percent(
				e->gross_pay,
			 	e->payroll_tax_amount );

		fprintf( output_pipe,
			 format_string,
			 e->full_name,
			 e->regular_work_hours,
			 e->overtime_work_hours,
			 e->gross_pay,
			 e->net_pay,
			 e->payroll_tax_amount,
			 payroll_tax_percent,
			 '%',
			 e->federal_tax_withholding_amount,
			 e->state_tax_withholding_amount,
			 e->social_security_employee_tax_amount,
			 e->social_security_employer_tax_amount,
			 e->medicare_employee_tax_amount,
			 e->medicare_employer_tax_amount,
			 e->federal_unemployment_tax_amount,
			 e->state_unemployment_tax_amount );

	} while( list_next( employee_work_period_list ) );

	pclose( output_pipe );
}

void post_payroll(
			char *application_name,
			LIST *employee_list,
			int payroll_year,
			int payroll_period_number,
			char *begin_work_date,
			char *end_work_date,
			ENTITY_SELF *self )
{
	PAYROLL_POSTING *payroll_posting;
	EMPLOYEE_TAX_WITHHOLDING_TABLE *employee_tax_withholding_table;
	LIST *open_work_day_list;

	open_work_day_list =
		employee_fetch_open_work_day_list(
			application_name );

	if ( list_length( open_work_day_list ) )
	{
		employee_close_employee_work_list_set(
			open_work_day_list,
			end_work_date );

		employee_close_employee_work_list_update(
			application_name,
			open_work_day_list );

		employee_close_employee_work_list_insert(
			application_name,
			open_work_day_list,
			end_work_date );
	}

	employee_tax_withholding_table =
		employee_tax_withholding_table_new(
			application_name );

	payroll_posting =
		employee_get_payroll_posting(
			employee_list,
			payroll_year,
			payroll_period_number,
			begin_work_date,
			end_work_date,
			self,
			employee_tax_withholding_table );

	/* -------------------------------------- */
	/* Sets employee_work_period->transaction */
	/* -------------------------------------- */
	employee_work_period_set_transaction(
		payroll_posting->employee_work_period_list );

	post_insert( application_name, payroll_posting );

	post_payroll_update(
			payroll_posting->employee_work_period_list,
			payroll_year );

} /* post_payroll() */

double calculate_payroll_tax_percent(
				double gross_pay,
			 	double payroll_tax_amount )
{
	if ( timlib_double_virtually_same(
		gross_pay, 0.0 ) )
	{
		return 0.0;
	}

	return ( payroll_tax_amount / gross_pay ) * 100.0;

} /* calculate_payroll_tax_percent() */

void post_payroll_journal_display(
			char *application_name,
			LIST *employee_work_period_list )
{
	char *heading;
	char *justify;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	double salary_wage_expense;
	EMPLOYEE_WORK_PERIOD *e;
	double total_debit;
	double total_credit;
	char buffer[ 128 ];
	char *salary_wage_expense_account = {0};
	char *payroll_tax_account = {0};
	char *payroll_payable_account = {0};
	char *federal_withholding_payable_account = {0};
	char *state_withholding_payable_account = {0};
	char *social_security_payable_account = {0};
	char *medicare_payable_account = {0};
	char *retirement_plan_payable_account = {0};
	char *health_insurance_payable_account = {0};
	char *union_dues_payable_account = {0};
	char *federal_unemployment_tax_payable_account = {0};
	char *state_unemployment_tax_payable_account = {0};

	heading = "Employee,Account,Debit,Credit";
	justify = "left,left,right,right";

	sprintf(sys_string,
		"html_table.e '' %s '^' %s",
		heading,
		justify );

	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( employee_work_period_list ) )
	{
		pclose( output_pipe );
		return;
	}

	ledger_get_payroll_account_names(
		&salary_wage_expense_account,
		&payroll_tax_account,
		&payroll_payable_account,
		&federal_withholding_payable_account,
		&state_withholding_payable_account,
		&social_security_payable_account,
		&medicare_payable_account,
		&retirement_plan_payable_account,
		&health_insurance_payable_account,
		&union_dues_payable_account,
		&federal_unemployment_tax_payable_account,
		&state_unemployment_tax_payable_account,
		application_name,
		(char *)0 /* fund_name */ );

	do {
		e = list_get_pointer( employee_work_period_list );

		if ( !e->gross_pay ) continue;

		/* Debit */
		/* ----- */
		salary_wage_expense =
			e->gross_pay +
			(double)e->
				retirement_contribution_plan_employer_amount +
			(double)e->health_insurance_employer_amount;

		fprintf(output_pipe,
		 	"%s^%s^%.2lf^\n",
		 	e->full_name,
			format_initial_capital(
				buffer,
				salary_wage_expense_account ),
		 	salary_wage_expense );

		total_debit = salary_wage_expense;

		fprintf(output_pipe,
		 	"^%s^%.2lf^\n",
			format_initial_capital(
				buffer,
				payroll_tax_account ),
		 	e->payroll_tax_amount );

		total_debit += e->payroll_tax_amount;

		/* Credit */
		/* ------ */
		fprintf(output_pipe,
			"^%s^^%.2lf\n",
			format_initial_capital(
				buffer,
				payroll_payable_account ),
			e->net_pay );

		total_credit = e->net_pay;

		fprintf(output_pipe,
		 	"^%s^^%.2lf\n",
			format_initial_capital(
				buffer,
				federal_withholding_payable_account ),
		 	e->federal_tax_withholding_amount );

		total_credit += e->federal_tax_withholding_amount;

		if ( e->state_tax_withholding_amount )
		{
			if ( !state_withholding_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: state_withholding_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(output_pipe,
		 		"^%s^^%.2lf\n",
				format_initial_capital(
					buffer,
					state_withholding_payable_account ),
		 		e->state_tax_withholding_amount );

			total_credit += e->state_tax_withholding_amount;
		}

		fprintf(output_pipe,
		 	"^%s^^%.2lf\n",
			format_initial_capital(
				buffer,
				social_security_payable_account ),
		 	e->social_security_employee_tax_amount +
		 	e->social_security_employer_tax_amount );

		total_credit +=
			(e->social_security_employee_tax_amount +
			 e->social_security_employer_tax_amount );

		fprintf(output_pipe,
		 	"^%s^^%.2lf\n",
			format_initial_capital(
				buffer,
				medicare_payable_account ),
		 	e->medicare_employee_tax_amount +
		 	e->medicare_employer_tax_amount );

		total_credit +=
			(e->medicare_employee_tax_amount +
			 e->medicare_employer_tax_amount );

		if ( e->retirement_contribution_plan_employee_amount
		||   e->retirement_contribution_plan_employer_amount )
		{
			if ( !retirement_plan_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: retirement_plan_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(
			      output_pipe,
		 	      "^%s^^%d\n",
			      format_initial_capital(
					buffer,
					retirement_plan_payable_account ),
			      e->retirement_contribution_plan_employee_amount +
			      e->retirement_contribution_plan_employer_amount );

			total_credit +=
			     (e->retirement_contribution_plan_employee_amount +
			      e->retirement_contribution_plan_employer_amount );

		}

		if ( e->health_insurance_employee_amount
		||   e->health_insurance_employer_amount )
		{
			if ( !health_insurance_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: health_insurance_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(
			      output_pipe,
		 	      "^%s^^%d\n",
			      format_initial_capital(
					buffer,
				    	health_insurance_payable_account ),
			      e->health_insurance_employee_amount +
			      e->health_insurance_employer_amount );

			total_credit +=
			     (e->health_insurance_employee_amount +
			      e->health_insurance_employer_amount );
		}

		if ( e->union_dues_amount )
		{
			if ( !union_dues_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: union_dues_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(output_pipe,
		 		"^%s^^%d\n",
				format_initial_capital(
					buffer,
					union_dues_payable_account ),
				e->union_dues_amount );

			total_credit += e->union_dues_amount;
		}

		fprintf(output_pipe,
		 	"^%s^^%.2lf\n",
			format_initial_capital(
				buffer,
				federal_unemployment_tax_payable_account ),
		 	e->federal_unemployment_tax_amount );

		total_credit += e->federal_unemployment_tax_amount;

		if ( e->state_unemployment_tax_amount )
		{
			if ( !state_unemployment_tax_payable_account )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: state_unemployment_tax_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			fprintf(output_pipe,
		 		"^%s^^%.2lf\n",
				format_initial_capital(
					buffer,
					state_unemployment_tax_payable_account),
		 		e->state_unemployment_tax_amount );

			total_credit += e->state_unemployment_tax_amount;
		}

		fprintf(output_pipe,
		 	"^Total^%.2lf^%.2lf\n",
		 	total_debit,
			total_credit );

	} while( list_next( employee_work_period_list ) );

	pclose( output_pipe );

} /* post_payroll_journal_display() */

void post_insert(	char *application_name,
			PAYROLL_POSTING *payroll_posting )
{
	post_payroll_payroll_posting_insert(
			application_name,
			payroll_posting );

	post_payroll_insert(
			application_name,
			payroll_posting->employee_work_period_list );

	post_payroll_transaction_insert(
			application_name,
			payroll_posting->employee_work_period_list );

} /* post_payroll_insert() */

void post_payroll_payroll_posting_insert(
			char *application_name,
			PAYROLL_POSTING *payroll_posting )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *field;
	char *format;
	char *table;

	table = get_table_name( application_name, "payroll_posting" );

	field =
"payroll_year,payroll_period_number,begin_work_date,end_work_date,regular_work_hours,overtime_work_hours,gross_pay,federal_tax_withholding_amount,state_tax_withholding_amount,social_security_employee_tax_amount,social_security_employer_tax_amount,medicare_employee_tax_amount,medicare_employer_tax_amount,retirement_contribution_plan_employee_amount,retirement_contribution_plan_employer_amount,health_insurance_employee_amount,health_insurance_employer_amount,federal_unemployment_tax_amount,state_unemployment_tax_amount,union_dues_amount,commission_sum_extension,net_pay,payroll_tax_amount";

	format =
"%d^%d^%s^%s^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%d^%d^%d^%d^%.2lf^%.2lf^%d^%.2lf^%.2lf^%.2lf\n";

	sprintf( sys_string,
		 "insert_statement.e	table=payroll_posting	 "
		 "			field=\"%s\"		 "
		 "			table=%s		 "
		 "			delimiter='^'		|"
		 "sql.e						 ",
		 field,
		 table );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 format,
		 payroll_posting->payroll_year,
		 payroll_posting->payroll_period_number,
		 payroll_posting->begin_work_date,
		 payroll_posting->end_work_date,
		 payroll_posting->regular_work_hours,
		 payroll_posting->overtime_work_hours,
		 payroll_posting->gross_pay,
		 payroll_posting->federal_tax_withholding_amount,
		 payroll_posting->state_tax_withholding_amount,
		 payroll_posting->social_security_employee_tax_amount,
		 payroll_posting->social_security_employer_tax_amount,
		 payroll_posting->medicare_employee_tax_amount,
		 payroll_posting->medicare_employer_tax_amount,
		 payroll_posting->retirement_contribution_plan_employee_amount,
		 payroll_posting->retirement_contribution_plan_employer_amount,
		 payroll_posting->health_insurance_employee_amount,
		 payroll_posting->health_insurance_employer_amount,
		 payroll_posting->federal_unemployment_tax_amount,
		 payroll_posting->state_unemployment_tax_amount,
		 payroll_posting->union_dues_amount,
		 payroll_posting->commission_sum_extension,
		 payroll_posting->net_pay,
		 payroll_posting->payroll_tax_amount );

	pclose( output_pipe );

} /* post_payroll_payroll_posting_insert() */

void post_payroll_insert(
			char *application_name,
			LIST *employee_work_period_list )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *field;
	char *format;
	char *table;
	EMPLOYEE_WORK_PERIOD *e;

	if ( !list_rewind( employee_work_period_list ) ) return;

	table = get_table_name( application_name, "employee_work_period" );

	field =
"full_name,street_address,payroll_year,payroll_period_number,begin_work_date,end_work_date,regular_work_hours,gross_pay,federal_tax_withholding_amount,state_tax_withholding_amount,social_security_employee_tax_amount,social_security_employer_tax_amount,medicare_employee_tax_amount,medicare_employer_tax_amount,retirement_contribution_plan_employee_amount,retirement_contribution_plan_employer_amount,health_insurance_employee_amount,health_insurance_employer_amount,federal_unemployment_tax_amount,state_unemployment_tax_amount,union_dues_amount,transaction_date_time,commission_sum_extension,overtime_work_hours,net_pay,payroll_tax_amount";

	format =
"%s^%s^%d^%d^%s^%s^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%d^%d^%d^%d^%.2lf^%.2lf^%d^%s^%.2lf^%.2lf^%.2lf^%.2lf\n";

	sprintf( sys_string,
		 "insert_statement.e	table=payroll_posting	 "
		 "			field=\"%s\"		 "
		 "			table=%s		 "
		 "			delimiter='^'		|"
		 "sql.e						 ",
		 field,
		 table );

	output_pipe = popen( sys_string, "w" );

	do {
		e = list_get_pointer( employee_work_period_list );

		if ( !e->gross_pay ) continue;

		fprintf(output_pipe,
		 	format,
			e->full_name,
			e->street_address,
			e->payroll_year,
			e->payroll_period_number,
			e->begin_work_date,
			e->end_work_date,
			e->regular_work_hours,
			e->gross_pay,
			e->federal_tax_withholding_amount,
			e->state_tax_withholding_amount,
			e->social_security_employee_tax_amount,
			e->social_security_employer_tax_amount,
			e->medicare_employee_tax_amount,
			e->medicare_employer_tax_amount,
			e->retirement_contribution_plan_employee_amount,
			e->retirement_contribution_plan_employer_amount,
			e->health_insurance_employee_amount,
			e->health_insurance_employer_amount,
			e->federal_unemployment_tax_amount,
			e->state_unemployment_tax_amount,
			e->union_dues_amount,
			e->transaction->transaction_date_time,
			e->commission_sum_extension,
			e->overtime_work_hours,
			e->net_pay,
			e->payroll_tax_amount );

	} while( list_next( employee_work_period_list ) );

	pclose( output_pipe );

} /* post_payroll_insert() */

void post_payroll_transaction_insert(
			char *application_name,
			LIST *employee_work_period_list )
{
	FILE *transaction_output_pipe;
	FILE *debit_account_pipe = {0};
	FILE *credit_account_pipe = {0};
	double salary_wage_expense;
	EMPLOYEE_WORK_PERIOD *e;
	char *salary_wage_expense_account = {0};
	char *payroll_tax_account = {0};
	char *payroll_payable_account = {0};
	char *federal_withholding_payable_account = {0};
	char *state_withholding_payable_account = {0};
	char *social_security_payable_account = {0};
	char *medicare_payable_account = {0};
	char *retirement_plan_payable_account = {0};
	char *health_insurance_payable_account = {0};
	char *union_dues_payable_account = {0};
	char *federal_unemployment_tax_payable_account = {0};
	char *state_unemployment_tax_payable_account = {0};
	char *propagate_transaction_date_time = {0};

	if ( !list_rewind( employee_work_period_list ) ) return;

	transaction_output_pipe =
		ledger_transaction_insert_open_stream(
			application_name );

	ledger_journal_insert_open_stream(
		&debit_account_pipe,
		&credit_account_pipe,
		application_name );

	if ( !transaction_output_pipe
	||   !debit_account_pipe
	||   !credit_account_pipe )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: did open an output pipe.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ledger_get_payroll_account_names(
		&salary_wage_expense_account,
		&payroll_tax_account,
		&payroll_payable_account,
		&federal_withholding_payable_account,
		&state_withholding_payable_account,
		&social_security_payable_account,
		&medicare_payable_account,
		&retirement_plan_payable_account,
		&health_insurance_payable_account,
		&union_dues_payable_account,
		&federal_unemployment_tax_payable_account,
		&state_unemployment_tax_payable_account,
		application_name,
		(char *)0 /* fund_name */ );

	do {
		e = list_get_pointer( employee_work_period_list );

		if ( !e->gross_pay ) continue;

		ledger_transaction_insert_stream(
			transaction_output_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
			e->net_pay /* transaction_amount */,
			e->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */ );

		/* Debit */
		/* ----- */
		salary_wage_expense =
			e->gross_pay +
			(double)e->
				retirement_contribution_plan_employer_amount +
			(double)e->health_insurance_employer_amount;

		ledger_journal_insert_stream(
			debit_account_pipe,
			(FILE *)0 /* credit_account_pipe */,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
			salary_wage_expense,
			salary_wage_expense_account /* debit_account_name */,
			(char *)0 /* credit_account_name */ );

		ledger_journal_insert_stream(
			debit_account_pipe,
			(FILE *)0 /* credit_account_pipe */,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
			e->payroll_tax_amount,
			payroll_tax_account /* debit_account_name */,
			(char *)0 /* credit_account_name */ );

		/* Credit */
		/* ------ */
		ledger_journal_insert_stream(
			(FILE *)0 /* debit_account_pipe */,
			credit_account_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
			e->net_pay,
			(char *)0 /* debit_account_name */,
			payroll_payable_account /* credit_account_name */ );

		ledger_journal_insert_stream(
			(FILE *)0 /* debit_account_pipe */,
			credit_account_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
			e->federal_tax_withholding_amount,
			(char *)0 /* debit_account_name */,
			federal_withholding_payable_account
				/* credit_account_name */ );

		if ( e->state_tax_withholding_amount )
		{
			ledger_journal_insert_stream(
				(FILE *)0 /* debit_account_pipe */,
				credit_account_pipe,
				e->full_name,
				e->street_address,
				e->transaction->transaction_date_time,
				e->state_tax_withholding_amount,
				(char *)0 /* debit_account_name */,
				state_withholding_payable_account
					/* credit_account_name */ );
		}

		ledger_journal_insert_stream(
			(FILE *)0 /* debit_account_pipe */,
			credit_account_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
		 	e->social_security_employee_tax_amount +
		 	e->social_security_employer_tax_amount,
			(char *)0 /* debit_account_name */,
			social_security_payable_account
				/* credit_account_name */ );

		ledger_journal_insert_stream(
			(FILE *)0 /* debit_account_pipe */,
			credit_account_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
		 	e->medicare_employee_tax_amount +
		 	e->medicare_employer_tax_amount,
			(char *)0 /* debit_account_name */,
			medicare_payable_account /* credit_account_name */ );

		if ( e->retirement_contribution_plan_employee_amount
		||   e->retirement_contribution_plan_employer_amount )
		{
			if ( !retirement_plan_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: retirement_plan_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			ledger_journal_insert_stream(
			      (FILE *)0 /* debit_account_pipe */,
			      credit_account_pipe,
			      e->full_name,
			      e->street_address,
			      e->transaction->transaction_date_time,
			      e->retirement_contribution_plan_employee_amount +
			      e->retirement_contribution_plan_employer_amount,
			      (char *)0 /* debit_account_name */,
			      retirement_plan_payable_account
					/* credit_account_name */ );
		}

		if ( e->health_insurance_employee_amount
		||   e->health_insurance_employer_amount )
		{
			if ( !health_insurance_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: health_insurance_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			ledger_journal_insert_stream(
			      (FILE *)0 /* debit_account_pipe */,
			      credit_account_pipe,
			      e->full_name,
			      e->street_address,
			      e->transaction->transaction_date_time,
			      e->health_insurance_employee_amount +
			      e->health_insurance_employer_amount,
			      (char *)0 /* debit_account_name */,
			      health_insurance_payable_account
					/* credit_account_name */ );
		}

		if ( e->union_dues_amount )
		{
			if ( !union_dues_payable_account )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: union_dues_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			ledger_journal_insert_stream(
				(FILE *)0 /* debit_account_pipe */,
				credit_account_pipe,
				e->full_name,
				e->street_address,
				e->transaction->transaction_date_time,
				e->union_dues_amount,
				(char *)0 /* debit_account_name */,
				union_dues_payable_account
					/* credit_account_name */ );
		}

		ledger_journal_insert_stream(
			(FILE *)0 /* debit_account_pipe */,
			credit_account_pipe,
			e->full_name,
			e->street_address,
			e->transaction->transaction_date_time,
		 	e->federal_unemployment_tax_amount,
			(char *)0 /* debit_account_name */,
			federal_unemployment_tax_payable_account
				/* credit_account_name */ );

		if ( e->state_unemployment_tax_amount )
		{
			if ( !state_unemployment_tax_payable_account )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: state_unemployment_tax_payable_account not set.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			ledger_journal_insert_stream(
				(FILE *)0 /* debit_account_pipe */,
				credit_account_pipe,
				e->full_name,
				e->street_address,
				e->transaction->transaction_date_time,
		 		e->state_unemployment_tax_amount,
				(char *)0 /* debit_account_name */,
				state_unemployment_tax_payable_account
					/* credit_account_name */ );
		}

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				e->transaction->transaction_date_time;
		}

	} while( list_next( employee_work_period_list ) );

	pclose( transaction_output_pipe );
	pclose( debit_account_pipe );
	pclose( credit_account_pipe );

	post_payroll_propagate(
		application_name,
		salary_wage_expense_account,
		payroll_tax_account,
		payroll_payable_account,
		federal_withholding_payable_account,
		state_withholding_payable_account,
		social_security_payable_account,
		medicare_payable_account,
		retirement_plan_payable_account,
		health_insurance_payable_account,
		union_dues_payable_account,
		federal_unemployment_tax_payable_account,
		state_unemployment_tax_payable_account,
		propagate_transaction_date_time );

} /* post_payroll_transaction_insert() */

void post_payroll_propagate(
			char *application_name,
			char *salary_wage_expense_account,
			char *payroll_tax_account,
			char *payroll_payable_account,
			char *federal_withholding_payable_account,
			char *state_withholding_payable_account,
			char *social_security_payable_account,
			char *medicare_payable_account,
			char *retirement_plan_payable_account,
			char *health_insurance_payable_account,
			char *union_dues_payable_account,
			char *federal_unemployment_tax_payable_account,
			char *state_unemployment_tax_payable_account,
			char *propagate_transaction_date_time )
{
	if ( salary_wage_expense_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			salary_wage_expense_account );
	}

	if ( payroll_tax_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			payroll_tax_account );
	}

	if ( payroll_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			payroll_payable_account );
	}

	if ( federal_withholding_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			federal_withholding_payable_account );
	}

	if ( state_withholding_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			state_withholding_payable_account );
	}

	if ( social_security_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			social_security_payable_account );
	}

	if ( medicare_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			medicare_payable_account );
	}

	if ( retirement_plan_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			retirement_plan_payable_account );
	}

	if ( health_insurance_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			health_insurance_payable_account );
	}

	if ( union_dues_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			union_dues_payable_account );
	}

	if ( federal_unemployment_tax_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			federal_unemployment_tax_payable_account );
	}

	if ( state_unemployment_tax_payable_account )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			state_unemployment_tax_payable_account );
	}

} /* post_payroll_propagate() */

void post_payroll_delete(
			char *application_name,
			int payroll_year,
			int payroll_period_number,
			boolean execute )
{
	LIST *employee_work_period_list;

	employee_work_period_list =
		employee_fetch_work_period_list(
				application_name,
				payroll_year,
				payroll_period_number );

	if ( execute )
	{
		post_payroll_delete_execute(
			application_name,
			payroll_year,
			payroll_period_number,
			employee_work_period_list );
	}
	else
	{
		post_payroll_employee_display(
			1 /* delete */,
			payroll_year,
			payroll_period_number,
			employee_work_period_list );
	}

} /* post_payroll_delete() */

void post_payroll_delete_execute(
			char *application_name,
			int payroll_year,
			int payroll_period_number,
			LIST *employee_work_period_list )
{
	EMPLOYEE_WORK_PERIOD *e;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char *propagate_transaction_date_time = {0};
	FILE *delete_pipe;
	char *table_name;
	char *field;

	sprintf( where,
		 "payroll_year = %d and	"
		 "payroll_period_number = %d",
		 payroll_year,
		 payroll_period_number );

	/* Delete from PAYROLL_POSTING */
	/* --------------------------- */
	sprintf( sys_string,
		 "echo \"delete from payroll_posting where %s;\" | sql.e",
		 where );

	if ( system( sys_string ) ){};

	/* Delete from EMPLOYEE_WORK_PERIOD */
	/* -------------------------------- */
	if ( !list_rewind( employee_work_period_list ) ) return;

	table_name = "employee_work_period";
	field = "full_name,street_address,payroll_year,payroll_period_number";

	sprintf( sys_string,
		 "delete_statement.e table=%s field=%s | sql.e",
		 table_name,
		 field );

	delete_pipe = popen( sys_string, "w" );

	do {
		e = list_get_pointer( employee_work_period_list );

		fprintf( delete_pipe,
			 "%s|%s|%d|%d\n",
			 e->full_name,
			 e->street_address,
			 payroll_year,
			 payroll_period_number );

		ledger_delete(	application_name,
				TRANSACTION_FOLDER_NAME,
				e->full_name,
				e->street_address,
				e->transaction_date_time );

		ledger_delete(	application_name,
				LEDGER_FOLDER_NAME,
				e->full_name,
				e->street_address,
				e->transaction_date_time );

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				e->transaction_date_time;
		}

	} while( list_next( employee_work_period_list ) );

	pclose( delete_pipe );

	post_payroll_update(
		employee_work_period_list,
		payroll_year );

} /* post_payroll_delete_execute() */

void post_payroll_update(
			LIST *employee_work_period_list,
			int payroll_year )
{
	EMPLOYEE_WORK_PERIOD *e;
	char sys_string[ 1024 ];
	char buffer[ 256 ];

	if ( !list_rewind( employee_work_period_list ) ) return;

	do {
		e = list_get_pointer( employee_work_period_list );

		if ( !e->gross_pay ) continue;

		sprintf( sys_string,
"echo \"update employee set gross_pay_year_to_date = ( select sum( gross_pay ) from employee_work_period where employee.full_name = employee_work_period.full_name and employee.street_address = employee_work_period.street_address and payroll_year = %d ) where full_name = '%s' and street_address = '%s';\" | sql.e",
			 payroll_year,
		 	 escape_character(
					buffer,
					e->full_name,
					'\'' ),
			 e->street_address );

		if ( system( sys_string ) ){};

		sprintf( sys_string,
"echo \"update employee set net_pay_year_to_date = ( select sum( net_pay ) from employee_work_period where employee.full_name = employee_work_period.full_name and employee.street_address = employee_work_period.street_address and payroll_year = %d ) where full_name = '%s' and street_address = '%s';\" | sql.e",
			 payroll_year,
		 	 escape_character(
					buffer,
					e->full_name,
					'\'' ),
			 e->street_address );

		if ( system( sys_string ) ){};

	} while( list_next( employee_work_period_list ) );

} /* post_payroll_update() */


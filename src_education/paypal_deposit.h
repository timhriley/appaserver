/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/paypal_deposit.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PAYPAL_DEPOSIT_H
#define PAYPAL_DEPOSIT_H

#include "boolean.h"
#include "list.h"
#include "semester.h"
#include "entity.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PAYPAL_DEPOSIT_TABLE	"paypal"

#define PAYPAL_PRIMARY_KEY	"payor_full_name,"		\
				"payor_street_address,"		\
				"season_name,"			\
				"year,"				\
				"deposit_date_time"

#define PAYPAL_INSERT_COLUMNS	"payor_full_name,"		\
				"payor_street_address,"		\
				"season_name,"			\
				"year,"				\
				"deposit_date_time,"		\
				"deposit_amount,"		\
				"transaction_fee,"		\
				"net_revenue,"			\
				"account_balance,"		\
				"transaction_ID,"		\
				"invoice_number,"		\
				"from_email_address"

/* Structures */
/* ---------- */
typedef struct
{
	/* ----------- */
	/* Primary key */
	/* ----------- */
	ENTITY *payor_entity;
	SEMESTER *semester;
	char *deposit_date_time;
	/* ---------- */
	/* Attributes */
	/* ---------- */
	double deposit_amount;
	double transaction_fee;
	double account_balance;
	char *transaction_ID;
	char *invoice_number;
	char *from_email_address;
	/* ------- */
	/* Process */
	/* ------- */
	double net_revenue;
	LIST *paypal_item_list;
	LIST *paypal_item_steady_state_list;
	double paypal_item_expected_revenue;
	double paypal_item_expected_revenue_total;
	int paypal_item_nonexpected_revenue_length;
	int paypal_item_expected_revenue_length;
	LIST *tuition_payment_list;
	LIST *program_payment_list;
	LIST *product_payment_list;
	LIST *tuition_refund_list;
	LIST *product_refund_list;
	LIST *enrollment_list;
	LIST *registration_list;
	LIST *course_list;
	double tuition_payment_total;
	double program_payment_total;
	double product_payment_total;
	double tuition_refund_total;
	double product_refund_total;
	double gain_donation;
	double registration_tuition;
	int row_number;
} PAYPAL_DEPOSIT;

LIST *paypal_deposit_registration_list(
			LIST *paypal_deposit_tuition_payment_list,
			LIST *paypal_deposit_tuition_refund_list );

LIST *paypal_deposit_fetch_program_payment_list(
			char *payor_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_program,
			boolean fetch_paypal );

LIST *paypal_deposit_fetch_tuition_payment_list(
			char *payor_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_paypal,
			boolean fetch_enrollment );

LIST *paypal_deposit_fetch_tuition_refund_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_paypal,
			boolean fetch_enrollment );

double paypal_gain_donation(
			double deposit_amount,
			double paypal_registration_tuition );

double paypal_net_revenue(
			double deposit_amount,
			double transaction_fee );

/* Returns static memory */
/* --------------------- */
char *paypal_deposit_primary_where(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

PAYPAL_DEPOSIT *paypal_deposit_calloc(
			void );

PAYPAL_DEPOSIT *paypal_deposit_new(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

LIST *paypal_deposit_system_list(
			char *sys_string,
			boolean fetch_tuition_payment_list,
			boolean fetch_program_payment_list,
			boolean fetch_product_payment_list,
			boolean fetch_tuition_refund_list );

PAYPAL_DEPOSIT *paypal_deposit_fetch(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_tuition_payment_list,
			boolean fetch_program_payment_list,
			boolean fetch_product_payment_list,
			boolean fetch_tuition_refund_list );

PAYPAL_DEPOSIT *paypal_deposit_parse(
			char *input,
			boolean fetch_tuition_payment_list,
			boolean fetch_program_payment_list,
			boolean fetch_product_payment_list,
			boolean fetch_tuition_refund_list );

/* Safely returns heap memory */
/* -------------------------- */
char *paypal_deposit_sys_string(
			char *where );

PAYPAL_DEPOSIT *paypal_deposit_steady_state(
			PAYPAL_DEPOSIT *paypal_deposit,
			LIST *semester_offering_list );

void paypal_deposit_update(
			double paypal_tuition_payment_total,
			double paypal_program_payment_total,
			double paypal_net_revenue,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

FILE *paypal_deposit_update_open(
			void );

LIST *paypal_enrollment_list(
			LIST *paypal_tuition_payment_list );

double paypal_registration_tuition(
			LIST *paypal_registration_list,
			LIST *semester_offering_list );

void paypal_deposit_list_insert(
			LIST *paypal_deposit_list );

FILE *paypal_deposit_insert_open(
			char *error_filename );

void paypal_deposit_insert_pipe(
			FILE *insert_pipe,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			double deposit_amount,
			double transaction_fee,
			double net_revenue,
			double account_balance,
			char *transaction_ID,
			char *invoice_number,
			char *from_email_address );

void paypal_list_tuition_payment_insert(
			LIST *paypal_deposit_list );

void paypal_list_program_payment_insert(
			LIST *paypal_deposit_list );

void paypal_list_tuition_refund_insert(
			LIST *paypal_deposit_list );

void paypal_list_enrollment_insert(
			LIST *paypal_deposit_list );

void paypal_list_registration_insert(
			LIST *paypal_deposit_list );

void paypal_list_student_insert(
			LIST *paypal_deposit_list );

void paypal_list_student_entity_insert(
			LIST *paypal_deposit_list );

void paypal_list_payor_entity_insert(
			LIST *paypal_deposit_list );

void paypal_list_enrollment_trigger(
			LIST *paypal_deposit_list );

void paypal_list_tuition_payment_trigger(
			LIST *paypal_deposit_list );

void paypal_list_program_payment_trigger(
			LIST *paypal_deposit_list );

LIST *paypal_deposit_course_name_list(
			LIST *paypal_deposit_list );

void paypal_deposit_trigger(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			char *state );

LIST *paypal_deposit_list_transaction_list(
			LIST *paypal_deposit_list );

LIST *paypal_deposit_list_steady_state(
			LIST *paypal_deposit_list,
			LIST *semester_offering_list );

LIST *paypal_deposit_list_registration_fetch_update(
			LIST *paypal_deposit_list,
			char *season_name,
			int year );

LIST *paypal_deposit_list_offering_fetch_update(
			LIST *paypal_deposit_list,
			char *season_name,
			int year );

LIST *paypal_deposit_transaction_list(
			LIST *paypal_deposit_tuition_payment_list,
			LIST *paypal_deposit_program_payment_list,
			LIST *paypal_deposit_product_payment_list,
			LIST *paypal_deposit_tuition_refund_list,
			LIST *paypal_deposit_product_refund_list );

LIST *paypal_deposit_list_enrollment_update(
			LIST *paypal_deposit_list,
			char *season_name,
			int year );

LIST *paypal_list_refund_enrollment_list(
			LIST *paypal_deposit_list );

LIST *paypal_tuition_refund_list(
			char *season_name,
			int year,
			char *item_title_P,
			LIST *semester_offering_list,
			PAYPAL_DEPOSIT *paypal_deposit );

double paypal_overpayment_loss(
			double deposit_amount,
			double paypal_deposit_registration_tuition );

LIST *paypal_fetch_product_payment_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_product,
			boolean fetch_paypal );

LIST *paypal_fetch_product_refund_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_product,
			boolean fetch_paypal );

void paypal_list_product_payment_insert(
			LIST *paypal_deposit_list );

LIST *paypal_product_payment_list(
			char *item_title_P,
			LIST *education_product_list,
			PAYPAL_DEPOSIT *paypal_deposit );

LIST *paypal_deposit_steady_state_paypal_item_list(
			LIST *paypal_item_list,
			double deposit_amount,
			double transaction_fee,
			double paypal_deposit_expected_revenue_total,
			int nonexpected_revenue_list_length );

void paypal_deposit_set_paypal_item_expected_revenue(
			LIST *paypal_item_list,
			LIST *semester_offering_list );

/* Returns paypal_deposit_list */
/* --------------------------- */
LIST *paypal_deposit_list_set_transaction(
			LIST *paypal_deposit_list );

void paypal_deposit_set_transaction(
			PAYPAL_DEPOSIT *paypal_deposit );

#endif


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
#include "paypal_dataset.h"
#include "paypal_sweep.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PAYPAL_DEPOSIT_TABLE	"paypal"

#define PAYPAL_PRIMARY_KEY	"payor_full_name,"		\
				"payor_street_address,"		\
				"paypal_date_time"

#define PAYPAL_INSERT_COLUMNS	"payor_full_name,"		\
				"payor_street_address,"		\
				"paypal_date_time,"		\
				"row_number,"			\
				"season_name,"			\
				"year,"				\
				"paypal_amount,"		\
				"transaction_fee,"		\
				"net_revenue,"			\
				"tuition_payment_total,"	\
				"program_donation_total,"	\
				"ticket_sale_total,"		\
				"product_sale_total,"		\
				"sweep_amount,"			\
				"tuition_refund_total,"		\
				"ticket_refund_total,"		\
				"product_refund_total,"		\
				"from_email_address,"		\
				"account_balance,"		\
				"transaction_ID,"		\
				"invoice_number"

/* Structures */
/* ---------- */
typedef struct
{
	/* ----------- */
	/* Primary key */
	/* ----------- */
	ENTITY *payor_entity;
	SEMESTER *semester;
	char *paypal_date_time;
	/* ---------- */
	/* Attributes */
	/* ---------- */
	double paypal_amount;
	double transaction_fee;
	double account_balance;
	char *transaction_ID;
	char *invoice_number;
	char *from_email_address;
	/* ------- */
	/* Process */
	/* ------- */
	double net_revenue;
	double tuition_payment_total;
	double program_donation_total;
	double ticket_sale_total;
	double product_sale_total;
	double sweep_amount;
	double tuition_refund_total;
	double ticket_refund_total;
	double product_refund_total;
	LIST *paypal_item_list;
	LIST *paypal_item_list_steady_state;
	double paypal_item_expected_revenue;
	double paypal_item_expected_revenue_total;
	int paypal_item_nonexpected_revenue_length;
	int paypal_item_expected_revenue_length;

	LIST *tuition_payment_list;
	LIST *program_donation_list;
	LIST *product_sale_list;
	LIST *ticket_sale_list;
	LIST *tuition_refund_list;
	LIST *product_refund_list;
	LIST *ticket_refund_list;
	PAYPAL_SWEEP *paypal_sweep;
	int row_number;
} PAYPAL_DEPOSIT;

LIST *paypal_deposit_registration_list(
			LIST *paypal_deposit_tuition_payment_list,
			LIST *paypal_deposit_tuition_refund_list );

LIST *paypal_fetch_program_donation_list(
			char *payor_full_name,
			char *street_address,
			char *paypal_date_time,
			boolean fetch_program );

LIST *paypal_fetch_tuition_payment_list(
			char *payor_full_name,
			char *street_address,
			char *paypal_date_time,
			boolean fetch_enrollment );

LIST *paypal_fetch_tuition_refund_list(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time,
			boolean fetch_enrollment );

LIST *paypal_fetch_product_sale_list(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time,
			boolean fetch_product );

double paypal_net_revenue(
			double paypal_amount,
			double transaction_fee );

/* Returns static memory */
/* --------------------- */
char *paypal_deposit_primary_where(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time );

PAYPAL_DEPOSIT *paypal_deposit_calloc(
			void );

PAYPAL_DEPOSIT *paypal_deposit_new(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time );

LIST *paypal_deposit_system_list(
			char *sys_string );

PAYPAL_DEPOSIT *paypal_deposit_fetch(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time );

PAYPAL_DEPOSIT *paypal_deposit_parse(
			char *input );

/* Safely returns heap memory */
/* -------------------------- */
char *paypal_deposit_sys_string(
			char *where );

PAYPAL_DEPOSIT *paypal_deposit_steady_state(
			PAYPAL_DEPOSIT *paypal_deposit );

FILE *paypal_deposit_update_open(
			void );

LIST *paypal_enrollment_list(
			LIST *paypal_tuition_payment_list );

double paypal_registration_tuition(
			LIST *paypal_registration_list,
			LIST *semester_offering_list );

void paypal_deposit_insert_pipe(
			FILE *insert_pipe,
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time,
			int row_number,
			char *season_name,
			int year,
			double paypal_amount,
			double transaction_fee,
			double net_revenue,
			double tuition_payment_total,
			double program_donation_total,
			double ticket_sale_total,
			double product_sale_total,
			double sweep_amount,
			double tuition_refund_total,
			double ticket_refund_total,
			double product_refund_total,
			double account_balance,
			char *transaction_ID,
			char *invoice_number,
			char *from_email_address );

void paypal_list_enrollment_trigger(
			LIST *paypal_deposit_list );

void paypal_list_tuition_payment_trigger(
			LIST *paypal_deposit_list );

void paypal_list_program_donation_trigger(
			LIST *paypal_deposit_list );

LIST *paypal_deposit_course_name_list(
			LIST *paypal_deposit_list );

LIST *paypal_deposit_list_transaction_list(
			LIST *paypal_deposit_list );

LIST *paypal_deposit_list_steady_state(
			LIST *paypal_deposit_list );

void paypal_deposit_list_registration_fetch_update(
			LIST *paypal_deposit_list,
			char *season_name,
			int year );

LIST *paypal_deposit_transaction_list(
			LIST *tuition_payment_list,
			LIST *product_sale_list,
			LIST *ticket_sale_list,
			LIST *tuition_refund_list,
			LIST *product_refund_list,
			LIST *ticket_refund_list,
			LIST *program_donation_list );

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

LIST *paypal_product_payment_list(
			char *item_title_P,
			LIST *education_product_list,
			PAYPAL_DEPOSIT *paypal_deposit );

LIST *paypal_deposit_steady_state_paypal_item_list(
			LIST *paypal_item_list,
			double paypal_amount,
			double transaction_fee,
			double paypal_deposit_expected_revenue_total,
			int nonexpected_revenue_list_length );

void paypal_deposit_set_paypal_item_expected_revenue(
			LIST *paypal_item_list,
			LIST *semester_offering_list,
			LIST *product_list,
			LIST *event_list );

/* Returns paypal_deposit_list */
/* --------------------------- */
LIST *paypal_deposit_list_set_transaction(
			LIST *paypal_deposit_list );

void paypal_deposit_set_transaction(
			PAYPAL_DEPOSIT *paypal_deposit );

PAYPAL_DEPOSIT *paypal_deposit_education(
			char *season_name,
			int year,
			LIST *semester_offering_list,
			LIST *program_list,
			LIST *product_list,
			LIST *event_list,
			PAYPAL_DATASET *paypal_dataset,
			int row_number );

PAYPAL_DEPOSIT *paypal_deposit_sweep(
			PAYPAL_DATASET *paypal_dataset,
			int row_number );

void paypal_deposit_paypal_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_tuition_payment_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_program_donation_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_tuition_refund_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_paypal_sweep_insert(
			LIST *education_paypal_list );

void paypal_deposit_enrollment_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_registration_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_student_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_student_entity_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_payor_entity_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_product_sale_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_ticket_sale_insert(
			LIST *paypal_deposit_list );

FILE *paypal_deposit_paypal_insert_open(
			char *error_filename );

void paypal_list_tuition_payment_insert(
			LIST *paypal_deposit_list );

void paypal_list_program_donation_insert(
			LIST *paypal_deposit_list );

void paypal_list_product_sale_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_ticket_refund_insert(
			LIST *paypal_deposit_list );

void paypal_deposit_product_refund_insert(
			LIST *paypal_deposit_list );

char *paypal_deposit_date_time(
			char *date_A,
			char *time_B );

#endif


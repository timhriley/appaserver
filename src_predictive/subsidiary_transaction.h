/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/subsidiary_transaction.h		*/
/* -------------------------------------------------------------------- */
/* This is the appaserver subsidiary_transaction ADT.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef SUBSIDIARY_H
#define SUBSIDIARY_H

#include "folder.h"
#include "list.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	TRANSACTION *transaction;
} SUBSIDIARY_OUTPUT;

typedef struct
{
	char *attribute_name;
	char *credit_account_name;
	LIST *primary_key_list;
	double transaction_amount;
	char *debit_account_name;
	FOLDER *subsidiary_transaction_folder;
	FOLDER *debit_account_folder;
	char *memo;
} SUBSIDIARY_PROCESS;

typedef struct
{
	char *folder_name;
	LIST *primary_data_list;
	char *full_name;
	char *street_address;
} SUBSIDIARY_INPUT;

typedef struct
{
	SUBSIDIARY_INPUT input;
	SUBSIDIARY_PROCESS process;
	SUBSIDIARY_OUTPUT output;
} SUBSIDIARY_TRANSACTION;

/* Operations */
/* ---------- */
SUBSIDIARY_TRANSACTION *subsidiary_new(	char *application_name,
					char *folder_name,
					LIST *primary_data_list,
					char *full_name,
					char *street_address,
					double transaction_amount );

SUBSIDIARY_TRANSACTION *subsidiary_transaction_calloc(
					void );

SUBSIDIARY_TRANSACTION *subsidiary_calloc(
					void );

boolean subsidiary_transaction_fetch(
					char **attribute_name,
					char **debit_account_name,
					char **credit_account_name,
					char **debit_account_folder_name,
					char *application_name,
					char *input_folder_name );

char *subsidiary_process_fetch_debit_account_name(
		char *application_name,
		char *debit_account_folder_name,
		char *debit_account_folder_primary_key,
		char *subsidiary_transaction_folder_name,
		LIST *subsidiary_transaction_folder_primary_key_list,
		LIST *subsidiary_transaction_folder_primary_data_list );

double subsidiary_fetch_transaction_amount(
				char *application_name,
				char *subsidiary_transaction_folder_name,
				char *subsidiary_transaction_attribute_name,
				LIST *primary_key_list,
				LIST *primary_data_list );

TRANSACTION *subsidiary_get_transaction(
				char *full_name,
				char *street_address,
				char *debit_account_name,
				char *credit_account_name,
				double transaction_amount,
				char *memo );

#endif

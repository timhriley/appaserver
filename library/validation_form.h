/* $APPASERVER_HOME/library/validation_form.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver validation_form ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */


#ifndef VALIDATION_FORM_H
#define VALIDATION_FORM_H

#include "list.h"
#include "boolean.h"
#include "hash_table.h"

/* Constants */
/* --------- */
#define DEFAULT_ATTRIBUTE_WIDTH 		10
#define NO_VERIFY_PUSH_BUTTON_PREFIX		"no_verify_push_button_"
#define NO_VERIFY_LABEL				"Not Validated"

/* Data Structures */
/* --------------- */
typedef struct
{
	char *primary_data_column_list_string;
	HASH_TABLE *primary_data_hash_table;
	HASH_TABLE *datatype_data_hash_table;
} VALIDATION_FORM_ROW;

/* VALIDATION_FORM_ROW operations */
/* ------------------------------ */
LIST *validation_form_row_list(
			LIST *primary_column_name_list,
			char *system_string );

VALIDATION_FORM_ROW *validation_form_row_new(
			char *primary_data_column_list_string );

void validation_form_row_set_primary_data_hash_table(
			HASH_TABLE *primary_data_hash_table,
			LIST *primary_name_list,
			char *primary_data_comma_list_string );

void validation_form_row_set_datatype_data_hash_table(
			HASH_TABLE *datatype_hash_table,
			char *datatype_name,
			char *datatype_value,
			boolean validated );

typedef struct
{
	char *datatype_data;
	boolean validated;
} VALIDATION_DATATYPE_DATA;

/* VALIDATION_DATATYPE_DATA operations */
/* ----------------------------------- */
VALIDATION_DATATYPE_DATA *validation_datatype_data_calloc(
			void );

VALIDATION_DATATYPE_DATA *validation_datatype_data_new(
			char *datatype_data,
			boolean validated );

typedef struct
{
	/* Input */
	/* ----- */
	char *table_title;
	char *target_frame;
	LIST *primary_name_list;
	LIST *datatype_name_list;
	char *action_string;
	boolean table_border;

	/* Process */
	/* ------- */
	LIST *row_list;
	LIST *element_list;
} VALIDATION_FORM;

/* VALIDATION_FORM operations */
/* -------------------------- */
VALIDATION_FORM *validation_form_calloc(
			void );

VALIDATION_FORM *validation_form_new(
			char *title, 
			char *target_frame,
			LIST *primary_column_name_list,
			LIST *datatype_name_list,
			char *action_string,
			boolean table_border,
			char *data_process );

LIST *validation_form_element_list(
			LIST *primary_name_list,
			LIST *datatype_name_list );

char *validation_form_heading(
			char *table_title,
			char *action_string,
			char *target_frame,
			LIST *element_list,
			int table_border );

char *validation_form_table_heading( 
			LIST *element_list );

/* Returns heap memory or null */
/* --------------------------- */
char *validation_form_body(
			VALIDATION_FORM *validation_form );

/* Returns heap memory */
/* ------------------- */
char *validation_form_trailer(
			void );

void validation_form_row_set_datatype_data_hash_table(
			HASH_TABLE *datatype_data_hash_table,
			char *datatype_name,
			char *datatype_value,
			boolean validated );

#endif

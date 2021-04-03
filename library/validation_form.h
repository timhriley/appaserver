/* validation_form.h							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver validation_form ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* This is not in the public domain.					*/
/* -------------------------------------------------------------------- */


#ifndef VALIDATION_FORM_H
#define VALIDATION_FORM_H

#include "list.h"
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
	char *value;
	int validated;
} VALIDATION_DATATYPE;

typedef struct
{
	char *primary_key;
	HASH_TABLE *primary_data_hash_table;
	HASH_TABLE *datatype_data_hash_table;
} ROW;

typedef struct
{
	char *title;
	char *target_frame;
	char *action_string;
	LIST *row_list;
	LIST *primary_column_name_list;
	LIST *datatype_name_list;
	LIST *element_list;
	int table_border;
} VALIDATION_FORM;

/* Operations */
/* ---------- */
VALIDATION_FORM *validation_form_new( 	char *title, 
					char *target_frame,
					LIST *primary_column_name_list,
					LIST *datatype_name_list );
ROW *validation_form_row_new( 		char *primary_key );
LIST *validation_form_get_element_list( LIST *primary_column_name_list,
					LIST *datatype_name_list );
void validation_form_set_action( 	VALIDATION_FORM *validation_form, 
					char *action_string );
void validation_form_output_heading(	char *title,
					char *action_string,
					char *target_frame,
					LIST *element_list,
					int table_border );
void validation_form_output_table_heading( 
					LIST *element_list );
void validation_form_output_body( 	VALIDATION_FORM *validation_form );
void validation_form_output_trailer( 	void );
void validation_form_set_primary_data_hash_table(
					HASH_TABLE *primary_data_hash_table,
					LIST *primary_column_name_list,
					char *primary_data_comma_list );

VALIDATION_DATATYPE *validation_datatype_new(
					char *value, int validated );

LIST *validation_form_get_row_list(	LIST *primary_column_name_list,
					char *select_sys_string );
int row_compare( 			ROW *row1, ROW *row2 );
#endif

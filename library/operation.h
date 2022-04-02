/* $APPASERVER_HOME/library/operation.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef OPERATION_H
#define OPERATION_H

#include <sys/types.h>
#include <unistd.h>
#include "process.h"
#include "boolean.h"
#include "list.h"
#include "element.h"
#include "dictionary.h"

#define OPERATION_SELECT		"operation,output_yn"
#define OPERATION_TABLE			"operation"
#define OPERATION_ROW_TOTAL_LABEL	"operation_row_total"
#define OPERATION_ROW_ITERATION_LABEL	"operation_row_iteration"

#define OPERATION_DELETE_WARNING_JAVASCRIPT \
					"edit_table_delete_button_warning()"

typedef struct
{
	boolean checked;
	LIST *primary_data_list;
	DICTIONARY *single_dictionary;
} OPERATION_ROW;

/* OPERATION_ROW operations */
/* ------------------------ */
OPERATION_ROW *operation_row_calloc(
			void );

LIST *operation_row_list(
			DICTIONARY *dictionary_separate_row_dictionary,
			char *operation_name,
			LIST *primary_key_list,
			LIST *attribute_name_list,
			int operation_row_total );

OPERATION_ROW *operation_row_fetch(
			DICTIONARY *row_dictionary,
			char *operation_name,
			LIST *primary_key_list,
			LIST *attribute_name_list,
			int row_number );

LIST *operation_row_primary_data_list(
			DICTIONARY *row_dictionary,
			LIST *primary_key_list,
			int row_number );

DICTIONARY *operation_row_single_dictionary(
			DICTIONARY *row_dictionary,
			LIST *attribute_name_list,
			int row_number );

void operation_row_execute(
			char *command_line );

boolean operation_row_checked(
			DICTIONARY *row_dictionary,
			char *attribute_name,
			int row_number );

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *operation_name;
	char *appaserver_data_directory;
	pid_t parent_process_id;
	int operation_row_total;

	/* Process */
	/* ------- */
	char *filename;
	boolean group_first_time;
	int row_current;
	boolean group_last_time;
} OPERATION_SEMAPHORE;

/* OPERATION_SEMAPHORE operations */
/* ------------------------------ */
OPERATION_SEMAPHORE *operation_semaphore_calloc(
			void );

OPERATION_SEMAPHORE *operation_semaphore_fetch(
			char *operation_name,
			char *appaserver_data_directory,
			pid_t parent_process_id,
			int operation_row_total );

/* Returns heap memory */
/* ------------------- */
char *operation_semaphore_filename(
			char *operation_name,
			char *appaserver_data_directory,
			pid_t parent_process_id );

boolean operation_semaphore_group_first_time(
			char *filename );

void operation_semaphore_initialize_file(
			char *filename );

int operation_semaphore_row_current(
			char *filename );

boolean operation_semaphore_group_last_time(
			int row_current,
			int operation_row_total );

void operation_semaphore_increment(
			char *filename,
			int row_current );

void operation_semaphore_remove_file(
			char *filename );

typedef struct
{
	char *operation_name;
	boolean output_boolean;
	PROCESS *process;
	boolean delete_boolean;
	boolean detail_boolean;
	char *image_source;
	char *delete_warning_javascript;
	APPASERVER_ELEMENT *appaserver_element;
} OPERATION;

/* OPERATION operations */
/* -------------------- */

/* Usage */
/* ----- */
OPERATION *operation_fetch(
			char *operation_name,
			boolean fetch_process );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *operation_primary_where(
			char *operation_name );

/* Returns static memory */
/* --------------------- */
char *operation_system_string(
			char *operation_select,
			char *operation_table,
			char *operation_primary_where );

OPERATION *operation_parse(
			char *input,
			boolean fetch_process );

OPERATION *operation_calloc(
			void );

boolean operation_output_boolean(
			char *output_yn );

boolean operation_delete_boolean(
			char *operation_name );

boolean operation_detail_boolean(
			char *operation_name );

char *operation_image_source(
			boolean operation_delete_boolean,
			boolean operation_detail_boolean );

/* Returns delete_warning_javascript or null */
/* ----------------------------------------- */
char *operation_delete_warning_javascript(
			char *delete_warning_javascript,
			boolean operation_delete_boolean );

APPASERVER_ELEMENT *operation_element(
			char *operation_name,
			char *operation_image_source,
			char *operation_delete_warning_javascript );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *operation_html(	ELEMENT_CHECKBOX *checkbox,
			char *state,
			int row_number,
			char *background_color,
			boolean delete_mask_boolean );

int operation_row_total(
			DICTIONARY *row_dictionary,
			char *operation_name,
			int highest_index );


#endif


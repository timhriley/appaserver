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

/* Constants */
/* --------- */
#define OPERATION_TABLE			"operation"
#define ROLE_OPERATION_TABLE		"role_operation"
#define OPERATION_ROW_TOTAL_LABEL	"operation_row_total"
#define OPERATION_ROW_ITERATION_LABEL	"operation_row_iteration"

/* Structures */
/* ---------- */
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
	/* Attributes */
	/* ---------- */
	char *operation_name;
	boolean output;

	/* Process */
	/* ------- */
	PROCESS *process;
	boolean delete_name;
	boolean detail_name;
	char *image_source;
	char *delete_warning_javascript;
	APPASERVER_ELEMENT *operation_element;
} OPERATION;

/* OPERATION operations */
/* -------------------- */
OPERATION *operation_calloc(
			void );

OPERATION *operation_parse(
			char *input );

boolean operation_delete_boolean(
			char *operation_name );

boolean operation_detail_boolean(
			char *operation_name );

char *operation_image_source(
			boolean operation_delete_boolean,
			boolean operation_detail_boolean );

char *operation_delete_warning_javascript(
			boolean operation_delete_boolean );

/* Public */
/* ------ */
char *operation_html(
			APPASERVER_ELEMENT *operation_element,
			int row_number );

int operation_row_total(
			DICTIONARY *row_dictionary,
			char *operation_name,
			int highest_index );

/* Returns heap memory or null */
/* --------------------------- */
char *operation_image_source(
			boolean operation_delete_name,
			boolean operation_detail_name );

/* Returns program memory or null */
/* ------------------------------ */
char *operation_delete_warning_javascript(
			boolean operation_delete_name );

APPASERVER_ELEMENT *operation_element(
			char *operation_name,
			char *operation_image_source,
			char *operation_delete_warning_javascript );

typedef struct
{
	/* Input */
	/* ----- */
	char *folder_name;
	char *role_name;

	/* Process */
	/* ------- */
	char *where;
	char *system_string;
} OPERATION_LIST;

/* OPERATION_LIST operations */
/* ------------------------- */
OPERATION_LIST *operation_list_calloc(
			void );

OPERATION_LIST *operation_list_new(
			char *folder_name,
			char *role_name );

/* Returns static memory */
/* --------------------- */
char *operation_list_select(
			void );

/* Returns static memory */
/* --------------------- */
char *operation_list_where(
			char *folder_name,
			char *role_name );

/* Returns static memory */
/* --------------------- */
char *operation_list_system_string(
			char *operation_list_select,
			char *operation_list_where );

#endif


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
#include "dictionary.h"

/* Constants */
/* --------- */
#define OPERATION_TABLE			"operation"
#define OPERATION_ROW_TOTAL_LABEL	"operation_row_total"
#define OPERATION_ROW_ITERATION_LABEL	"operation_row_iteration"

/* Structures */
/* ---------- */
typedef struct
{
	boolean checked;
	LIST *primary_data_list;
	DICTIONARY *dictionary;
} OPERATION_ROW;

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
	char *html;
} OPERATION;

/* OPERATION operations */
/* -------------------- */
OPERATION *operation_calloc(
			void );

OPERATION *operation_new(
			char *operation_name );

int operation_row_total(
			DICTIONARY *row_dictionary,
			char *operation_name,
			int highest_index );

boolean operation_delete_name(
			char *operation_name );

boolean operation_detail_name(
			char *operation_name );

/* Always succeeds */
/* --------------- */
OPERATION *operation_fetch(
			char *operation_name );

void operation_perform(
			char *command_line );

/* OPERATION_SEMAPHORE operations */
/* ------------------------------ */
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

OPERATION_SEMAPHORE *operation_semaphore_calloc(
			void );

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

/* OPERATION_ROW operations */
/* ------------------------ */
OPERATION_ROW *operation_row_fetch(
			DICTIONARY *row_dictionary,
			char *operation_name,
			LIST *primary_key_list,
			LIST *attribute_name_list,
			int row_number );

OPERATION_ROW *operation_row_calloc(
			void );

boolean operation_row_checked(
			DICTIONARY *row_dictionary,
			char *operation_name,
			int row_number );

LIST *operation_row_primary_data_list(
			DICTIONARY *row_dictionary,
			LIST *primary_key_list,
			int row_number );

DICTIONARY *operation_single_row_dictionary(
			DICTIONARY *row_dictionary,
			LIST *attribute_name_list,
			int row_number );

/* Returns heap memory or null */
/* --------------------------- */
char *operation_image_source(
			boolean operation_delete_name,
			boolean operation_detail_name );

/* Returns program memory or null */
/* ------------------------------ */
char *operation_delete_warning_javascript(
			boolean operation_delete_name );

#endif


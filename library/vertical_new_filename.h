/* -------------------------------------------------------------------- */
/* APPASERVER_HOME/library/vertical_new_filename.h			*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new checkboxes on the prompt insert form.	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef VERTICAL_NEW_FILENAME_H
#define VERTICAL_NEW_FILENAME_H

#include "boolean.h"
#include "list.h"
#include "appaserver_link.h"

typedef struct
{
	APPASERVER_LINK *appaserver_link;
	char *output_filename;
	char *prompt_filename;
} VERTICAL_NEW_FILENAME;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
VERTICAL_NEW_FILENAME *vertical_new_filename_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		const char *filename_stem );

/* Process */
/* ------- */
VERTICAL_NEW_FILENAME *vertical_new_filename_calloc(
		void );

#endif


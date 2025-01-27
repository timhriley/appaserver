/* $APPASERVER_HOME/library/javascript.h		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef JAVASCRIPT_H
#define JAVASCRIPT_H

#define JAVASCRIPT_PROMPT_ROW_NUMBER	0
#define JAVASCRIPT_ONLOAD_ROW_NUMBER	-1
#define JAVASCRIPT_WAIT_START		"timlib_wait_start()"
#define JAVASCRIPT_WAIT_OVER		"timlib_wait_over()"


typedef struct
{
	char *script_tag;
} JAVASCRIPT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
JAVASCRIPT *javascript_new(
		char *javascript_filename,
		char *document_root,
		char *application_relative_source_directory );

/* Process */
/* ------- */
JAVASCRIPT *javascript_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *javascript_script_tag(
		char *javascript_filename,
		char *document_root,
		char *application_relative_source_directory );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *javascript_home_source(
		char *javascript_filename,
		char *document_root );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *javascript_home_script_tag(
		char *javascript_filename );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *javascript_application_relative_script_tag(
		char *javascript_filename,
		char *document_root,
		char *application_relative_source_directory );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *javascript_relative_source(
		char *javascript_filename,
		char *document_root,
		char *relative_source_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *javascript_relative_script_tag(
		char *javascript_filename,
		char *relative_source_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *javascript_include_string(
		char *application_name,
		LIST *javascript_filename_list );

/* Usage */
/* ----- */

/* Returns static memory or null */
/* ----------------------------- */
char *javascript_replace(
		char *post_change_javascript,
		char *state,
		int row_number );

/* Process */
/* ------- */
void javascript_replace_state(
		char *post_change_javascript,
		char *state );

/* Usage */
/* ----- */

/* Returns heap memory. Frees original_post_change_javascript */
/* ---------------------------------------------------------- */
char *javascript_post_change_append(
		char *original_post_change_javascript,
		char *post_change_javascript,
		boolean place_first );

/* Usage */
/* ----- */
void javascript_replace_row(
		char *post_change_javascript,
		int row_number );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *javascript_open_tag(
		void );


/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *javascript_close_tag(
		void );

#endif

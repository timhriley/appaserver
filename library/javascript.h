/* $APPASERVER_HOME/library/javascript.h		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef JAVASCRIPT_H
#define JAVASCRIPT_H

/* Structures */
/* ---------- */
typedef struct
{
	char *javascript_html;
	char *javascript_source;
} JAVASCRIPT;

/* Operations */
/* ---------- */
JAVASCRIPT *javascript_new(
			char *javascript_filename,
			char *document_root_directory,
			char *application_relative_source_directory );

JAVASCRIPT *javascript_calloc(
			void );

char *javascript_html(
			char *javascript_filename,
			char *document_root_directory,
			char *application_relative_source_directory );

char *javascript_source(
			char *javascript_html,
			char *document_root_directory );

char *javascript_replace_state(
			char *post_change_javascript,
			char *state );


char *javascript_replace_row(
			char *post_change_javascript,
			char *row );

char *javascript_post_change_append(
			char *original_post_change_javascript,
			char *post_change_javascript,
			boolean place_first );

#endif

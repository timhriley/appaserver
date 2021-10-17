/* $APPASERVER_HOME/library/frameset.h			*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef FRAMESET_H
#define FRAMESET_H

#include "boolean.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "appaserver_link.h"

/* Constants */
/* --------- */
#define FRAMESET_MENU_FRAME		"menu_frame"
#define FRAMESET_PROMPT_FRAME		"prompt_frame"
#define FRAMESET_EDIT_FRAME		"edit_frame"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *document_root_directory;
	char *session_key;
	char *frame_name;

	/* Process */
	/* ------- */
	APPASERVER_LINK *appaserver_link;
	char *output_filename;
	char *prompt_filename;
	char *html;
} FRAMESET_FRAME;

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *session_key;

	/* Process */
	/* ------- */
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FRAMESET_FRAME *frameset_frame_menu;
	FRAMESET_FRAME *frameset_frame_prompt;
	FRAMESET_FRAME *frameset_frame_edit;
	char *html;
} FRAMESET;

/* FRAMESET operations */
/* -------------------- */
FRAMESET *frameset_calloc(
			void );

FRAMESET *frameset_new(
			char *application_name,
			char *session_key,
			boolean create_menu_frame );

/* Safely returns heap memory */
/* -------------------------- */
char *frameset_html(
			char *menu_frame_html,
			char *prompt_frame_html,
			char *edit_frame_html,
			boolean create_menu_frame );

/* FRAMESET_FRAME operations */
/* ------------------------- */
FRAMESET_FRAME *frameset_frame_calloc(
			void );

/* Always succeeds */
/* --------------- */
FRAMESET_FRAME *frameset_frame_new(
			char *application_name,
			char *document_root_directory,
			char *session_key,
			char *frame_name );

/* Safely returns heap memory */
/* -------------------------- */
char *frameset_frame_html(
			char *frame_name,
			char *prompt_filename );

#endif


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
#include "appaserver_parameter.h"
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
	/* Process */
	/* ------- */
	APPASERVER_LINK *appaserver_link;
	char *output_filename;
	char *prompt_filename;
	char *html;
} FRAMESET_FRAME;

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

typedef struct
{
	/* Process */
	/* ------- */
	APPASERVER_PARAMETER *appaserver_parameter;
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
			boolean frameset_menu_horizontal );

/* Safely returns heap memory */
/* -------------------------- */
char *frameset_html(
			char *menu_frame_html,
			char *prompt_frame_html,
			char *edit_frame_html,
			boolean frameset_menu_horizontal );

boolean frameset_menu_horizontal(
			char *application_name,
			char *login_name );

#endif


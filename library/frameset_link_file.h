/* $APPASERVER_HOME/library/frameset_link_file.h	*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#ifndef FRAMESET_LINK_FILE_H
#define FRAMESET_LINK_FILE_H

/* Includes */
/* -------- */
#include "appaserver_link.h"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *document_root_directory;

	/* Process */
	/* ------- */
	char *menu_frame_prompt_filename;
	char *menu_frame_create_filename;
	char *prompt_frame_prompt_filename;
	char *prompt_frame_create_filename;
	char *edit_frame_prompt_filename;
	char *edit_frame_create_filename;
	APPASERVER_LINK *appaserver_link;
} FRAMESET_LINK_FILE;

/* Operations */
/* ---------- */
FRAMESET_LINK_FILE *frameset_link_file_new(
			char *document_root_directory );

#endif

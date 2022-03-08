/* $APPASERVER_HOME/library/prompt_insert.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROMPT_INSERT_H
#define PROMPT_INSERT_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "role.h"
#include "role_folder.h"
#include "folder.h"
#include "drillthru.h"
#include "post_dictionary.h"
#include "frameset.h"
#include "folder_menu.h"
#include "menu.h"
#include "frameset.h"
#include "dictionary_separate.h"
#include "document.h"

/* Constants */
/* --------- */
#define PROMPT_INSERT_POST_EXECUTABLE	"post_prompt_insert"
#define PROMPT_INSERT_OUTPUT_EXECUTABLE	"output_prompt_insert"

typedef struct
{
	boolean forbid;
} PROMPT_INSERT;

/* PROMPT_INSERT operations */
/* ------------------------ */

/* ---------------------------- */
/* Always succeeds		*/
/* Note: check forbid flag.	*/
/* ---------------------------- */
PROMPT_INSERT *prompt_insert_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *state,
			boolean menu_boolean,
			char *data_directory,
			POST_DICTIONARY *post_dictionary );

/* Public */
/* ------ */
char *prompt_insert_output_system_string(
			char *prompt_insert_output_executable,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename );

/* Private */
/* ------- */
PROMPT_INSERT *prompt_insert_calloc(
			void );

#endif

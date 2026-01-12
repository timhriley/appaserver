/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/drillthru.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef DRILLTHRU_H
#define DRILLTHRU_H

#include "dictionary.h"
#include "list.h"
#include "relation.h"
#include "role.h"
#include "folder.h"
#include "security_entity.h"
#include "folder_menu.h"
#include "menu.h"
#include "folder_row_level_restriction.h"
#include "form_drillthru.h"
#include "document.h"

#define DRILLTHRU_FULFILLED_KEY		"fulfilled"
#define DRILLTHRU_BASE_KEY		"base"
#define DRILLTHRU_SKIPPED_KEY		"skipped"
#define DRILLTHRU_KEY_DELIMITER		','

#define DRILLTHRU_SKIPPED_MAX_FOREIGN_LENGTH 1

typedef struct
{
	char *base_folder_name;
	boolean participating_boolean;
	boolean first_time_boolean;
	boolean skipped_boolean;
	boolean active_boolean;
} DRILLTHRU_STATUS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DRILLTHRU_STATUS *drillthru_status_new(
		DICTIONARY *drillthru_dictionary );

/* Process */
/* ------- */
DRILLTHRU_STATUS *drillthru_status_calloc(
		void );

/* Returns component of drillthru_dictionary or null */
/* ------------------------------------------------- */
char *drillthru_status_base_folder_name(
		const char *drillthru_base_key,
		DICTIONARY *drillthru_dictionary );

boolean drillthru_status_participating_boolean(
		char *drillthru_status_base_folder_name );

boolean drillthru_status_first_time_boolean(
		const char *drillthru_fulfilled_key,
		DICTIONARY *drillthru_dictionary );

boolean drillthru_status_skipped_boolean(
		const char *drillthru_skipped_key,
		DICTIONARY *drillthru_dictionary,
		boolean drillthru_status_first_time_boolean );

boolean drillthru_status_active_boolean(
	boolean drillthru_status_skipped_boolean );

typedef struct
{
	ROLE *role;
	FOLDER *folder;
	LIST *relation_mto1_list;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
	boolean application_menu_horizontal_boolean;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	FORM_DRILLTHRU *form_drillthru;
	int folder_attribute_calendar_date_name_list_length;
	char *title_string;
	char *sub_title_string;
	DOCUMENT *document;
	char *document_form_html;
} DRILLTHRU_DOCUMENT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DRILLTHRU_DOCUMENT *drillthru_document_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		DICTIONARY *drillthru_dictionary,
		char *appaserver_data_directory,
		char *drillthru_continue_base_folder_name,
		char *drillthru_current_folder_name,
		boolean first_time_boolean );

/* Process */
/* ------- */
DRILLTHRU_DOCUMENT *drillthru_document_calloc(
		void );


/* Returns static memory */
/* --------------------- */
char *drillthru_document_title_string(
		char *state,
		char *drillthru_continue_base_folder_name );

/* Returns static memory */
/* --------------------- */
char *drillthru_document_sub_title_string(
		char *drillthru_current_folder_name );

typedef struct
{
	DRILLTHRU_STATUS *drillthru_status;
	LIST *fulfilled_folder_name_list;
	LIST *folder_attribute_fetch_primary_key_list;
	LIST *relation_mto1_drillthru_list;
	boolean completed_boolean;
	char *drillthru_current_folder_name;
	char *data_list_string;
	DRILLTHRU_DOCUMENT *drillthru_document;
} DRILLTHRU_CONTINUE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DRILLTHRU_CONTINUE *drillthru_continue_new(
		DICTIONARY *drillthru_dictionary /* in/out */,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *current_folder_name,
		char *state,
		char *appaserver_data_directory );

/* Process */
/* ------- */
DRILLTHRU_CONTINUE *drillthru_continue_calloc(
		void );

/* Safely returns */
/* -------------- */
LIST *drillthru_continue_fulfilled_folder_name_list(
		const char *drillthru_fulfilled_key,
		const char drillthru_key_delimiter,
		DICTIONARY *drillthru_dictionary );

boolean drillthru_continue_completed_boolean(
		int relation_mto1_drillthru_list_length );

/* Returns heap memory */
/* ------------------- */
char *drillthru_continue_data_list_string(
		const char drillthru_key_delimiter,
		LIST *drillthru_continue_fulfilled_folder_name_list );

typedef struct
{
	LIST *folder_attribute_fetch_primary_key_list;
	LIST *relation_mto1_drillthru_list;
	boolean participating_boolean;
	DICTIONARY *drillthru_dictionary;
	char *drillthru_current_folder_name;
	DRILLTHRU_DOCUMENT *drillthru_document;
} DRILLTHRU_START;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DRILLTHRU_START *drillthru_start_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *state,
		char *appaserver_data_directory );

/* Process */
/* ------- */
DRILLTHRU_START *drillthru_start_calloc(
		void );

boolean drillthru_start_participating_boolean(
		int relation_mto1_drillthru_list_length );

/* Safely returns */
/* -------------- */
DICTIONARY *drillthru_start_dictionary(
		const char *drillthru_base_key,
		char *folder_name );

typedef struct
{
	/* stub */
} DRILLTHRU;

/* Usage */
/* ----- */

/* Returns list_tail( relation_mto1_drillthru_list )->	*/
/*			one_folder->			*/
/*			folder->			*/
/*			folder_name			*/
/* or null.						*/
/* ---------------------------------------------------- */
char *drillthru_current_folder_name(
		LIST *relation_mto1_drillthru_list );

#endif

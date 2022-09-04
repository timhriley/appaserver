/* $APPASERVER_HOME/library/lookup_before_drop_down.h	   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef LOOKUP_BEFORE_DROP_DOWN_H
#define LOOKUP_BEFORE_DROP_DOWN_H

#include "boolean.h"
#include "list.h"
#include "role.h"
#include "dictionary.h"
#include "folder.h"

/* Enumerated types */
/* ---------------- */
enum lookup_before_drop_down_state {
			lookup_not_participating,
			lookup_participating_not_root,
			lookup_skipped,
			lookup_participating_is_root_all_complete,
			lookup_unknown };

/* Constants */
/* --------- */
#define LOOKUP_BEFORE_DROP_DOWN_UNFULFILLED_FOLDER_PREFIX 	\
		"uunfulfilled_lookup"
#define LOOKUP_BEFORE_DROP_DOWN_INSERT_FOLDER_KEY		\
		"insert_folder"
#define LOOKUP_BEFORE_DROP_DOWN_BASE_FOLDER_KEY			\
		"base_folder"
#define LOOKUP_BEFORE_DROP_DOWN_FULFILLED_FOLDER_KEY		\
		"fulfilled_folder"
#define LOOKUP_BEFORE_DROP_DOWN_STATE_KEY			\
		"state"
#define LOOKUP_BEFORE_DROP_DOWN_INITIAL_STATE			\
		"initial"
#define LOOKUP_BEFORE_DROP_DOWN_NON_INITIAL_STATE		\
		"non_initial"
#define LOOKUP_BEFORE_DROP_DOWN_SKIPPED_STATE			\
		"skipped"

/* Data Structures */
/* --------------- */
typedef struct {
	char *folder_name;
	boolean fulfilled;
} LOOKUP_BEFORE_DROP_DOWN_FOLDER;

typedef struct
{
	FOLDER *base_folder;
	LIST *lookup_before_drop_down_folder_list;
	boolean first_prelookup;
	boolean omit_lookup_before_drop_down;
	enum lookup_before_drop_down_state lookup_before_drop_down_state;
	char *insert_pair_base_folder_name;
	char *insert_folder_name;
} LOOKUP_BEFORE_DROP_DOWN;

/* Operations */
/* ---------- */
enum lookup_before_drop_down_state
	lookup_before_drop_down_get_state(
		LIST *lookup_before_drop_down_folder_list,
		DICTIONARY *lookup_before_drop_down_dictionary,
		DICTIONARY *preprompt_dictionary );

char *lookup_before_drop_down_get_base_folder_name(
		DICTIONARY *lookup_before_drop_down_dictionary );

LOOKUP_BEFORE_DROP_DOWN_FOLDER *lookup_before_drop_down_folder_new(
		char *folder_name );

LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down_new(
		char *application_name,
		DICTIONARY *lookup_before_drop_down_dictionary,
		char *state );

LIST *lookup_before_drop_down_folder_list(
		boolean *omit_lookup_before_drop_down,
		LIST *mto1_lookup_before_drop_down_related_folder_list,
		char *state );

void lookup_before_drop_down_with_dictionary_set_fulfilled(
		LIST *lookup_before_drop_down_folder_list,
		DICTIONARY *lookup_before_drop_down_dictionary );

boolean lookup_before_drop_down_is_fulfilled_folder(
		LIST *lookup_before_drop_down_folder_list,
		char *folder_name );

char *lookup_before_drop_down_get_unfulfilled_folder_name(
		LIST *lookup_before_drop_down_folder_list );

void lookup_before_drop_down_set_dictionary_base_name(
		DICTIONARY *lookup_before_drop_down_dictionary,
		char *base_folder_name );

char *lookup_before_drop_down_get_base_folder_key(
		void );

void lookup_before_drop_down_append_fulfilled_folder_name(
		DICTIONARY *lookup_before_drop_down_dictionary,
		char *fulfilled_folder_name );

boolean lookup_before_drop_down_exists_unfulfilled_folder(
			LIST *lookup_before_drop_down_folder_list );

void lookup_before_drop_down_add_fulfilled_folder(
			DICTIONARY *lookup_before_drop_down_dictionary,
			char *folder_name );

char *lookup_before_drop_down_dictionary_get_base_folder_name(
			DICTIONARY *lookup_before_drop_down_dictionary );

void lookup_before_drop_down_set_fulfilled_folder_name(
			LIST *lookup_before_drop_down_folder_list,
			char *folder_name );

void lookup_before_drop_down_set_dictionary_fulfilled(
			DICTIONARY *dictionary,
			LIST *lookup_before_drop_down_folder_list );

char *lookup_before_drop_down_display(
			LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down );

void lookup_before_drop_down_append_folder_list(
			LIST *lookup_before_drop_down_folder_list,
			LOOKUP_BEFORE_DROP_DOWN_FOLDER *folder );

/*
boolean lookup_before_drop_down_prelookup_empty(
			DICTIONARY *dictionary );
*/

char *lookup_before_drop_down_get_dictionary_base_name(
			DICTIONARY *lookup_before_drop_down_dictionary );

void lookup_before_drop_down_with_folder_name_list_set_fulfilled(
			LIST *lookup_before_drop_down_folder_list,
			LIST *folder_name_list );

LIST *lookup_before_drop_down_get_unfulfilled_folder_name_list(
			LIST *lookup_before_drop_down_folder_list );

LIST *lookup_before_drop_down_get_folder_name_list(
			LIST *lookup_before_drop_down_folder_list );

char *lookup_before_drop_down_state_display(
			enum lookup_before_drop_down_state
				lookup_before_drop_down_state );

char *lookup_before_drop_down_get_state_string(
			enum lookup_before_drop_down_state
				lookup_before_drop_down_state );

boolean lookup_before_drop_down_first_prelookup(
			DICTIONARY *lookup_before_drop_down_dictionary );

char *lookup_before_drop_down_get_insert_pair_base_folder_name(
			char *application_name,
			char *base_folder_name,
			LIST *pair_1tom_related_folder_list );

void lookup_before_drop_down_set_dictionary_insert_folder_name(
			DICTIONARY *lookup_before_drop_down_dictionary,
			char *insert_folder_name );

char *lookup_before_drop_down_get_dictionary_insert_folder_name(
			DICTIONARY *lookup_before_drop_down_dictionary );

LOOKUP_BEFORE_DROP_DOWN_FOLDER *lookup_before_drop_down_folder_list_seek(
			LIST *lookup_before_drop_down_folder_list,
			char *folder_name );

LIST *lookup_before_drop_down_get_non_edit_folder_name_list(
			LIST *lookup_before_drop_down_folder_list,
			LIST *mto1_related_folder_list );

boolean lookup_before_drop_down_preprompt_dictionary_empty(
			DICTIONARY *preprompt_dictionary );

boolean lookup_before_drop_down_omit(
		LIST *mto1_lookup_before_drop_down_related_folder_list );

#endif

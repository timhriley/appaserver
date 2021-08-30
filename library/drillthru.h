/* $APPASERVER_HOME/library/drillthru.h		 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef DRILLTHRU_H
#define DRILLTHRU_H

/* Includes */
/* -------- */
#include "dictionary.h"
#include "list.h"
#include "relation.h"

/* Constants */
/* --------- */
#define DRILLTHRU_FOLDER_LIST_KEY	"drillthru_folder_list_key"
#define DRILLTHRU_BASE_FOLDER_KEY	"drillthru_base_folder_key"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *base_folder_name;
	DICTIONARY *drillthru_dictionary;

	/* Process */
	/* ------- */
	LIST *fulfilled_folder_name_list;
	LIST *relation_mto1_drillthru_list;
	char *current_folder_name;
} DRILLTHRU;

/* Operations */
/* ---------- */
DRILLTHRU *drillthru_calloc(
			void );

/* Always succeeds */
/* --------------- */
DRILLTHRU *drillthru_start(
			char *base_folder_name );

/* Always succeeds */
/* --------------- */
DRILLTHRU *drillthru_participating(
			char *base_folder_name,
			DICTIONARY *drillthru_dictionary );

LIST *drillthru_fulfilled_folder_name_list(
			char *drillthru_folder_list_key,
			DICTIONARY *drillthru_dictionary );

char *drillthru_current_folder_name(
			LIST *relation_mto1_drillthru_list,
			char *base_folder_name );

void drillthru_base_folder_name_set(
			DICTIONARY *drillthru_dictionary,
			char *drillthru_folder_key,
			char *base_folder_name );

char *drillthru_base_folder_name(
			DICTIONARY *drillthru_dictionary,
			char *drillthru_base_folder_name );

void drillthru_fulfilled_folder_name_list_set(
			DICTIONARY *drillthru_dictionary,
			char *drillthru_folder_list_key,
			LIST *fulfilled_folder_name_list );

void drillthru_fulfilled_folder_name_set(
			DICTIONARY *drillthru_dictionary,
			LIST *fulfilled_folder_name_list,
			char *current_folder_name );

boolean drillthru_middle(
			char *folder_name,
			char *current_folder_name );

boolean drillthru_finished(
			char *folder_name,
			char *current_folder_name );
#endif

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

DRILLTHRU *drillthru_start(
			char *base_folder_name );

DRILLTHRU *drillthru_fetch(
			DICTIONARY *drillthru_dictionary );

LIST *drillthru_fulfilled_folder_name_list(
			DICTIONARY *drillthru_dictionary );

char *drillthru_current_folder_name(
			LIST *relation_mto1_drillthru_list,
			char *base_folder_name );

void drillthru_base_folder_name_set(
			DICTIONARY *drillthru_dictionary,
			char *base_folder_name );

char *drillthru_base_folder_name(
			DICTIONARY *drillthru_dictionary );

void drillthru_fulfilled_current_folder_name_set(
			DICTIONARY *drillthru_dictionary,
			LIST *fulfilled_folder_name_list,
			char *current_folder_name );

void drillthru_fulfilled_folder_name_list_dictionary_set(
			DICTIONARY *drillthru_dictionary,
			LIST *fulfilled_folder_name_list );

boolean drillthru_participating(
			DICTIONARY *drillthru_dictionary,
			int relation_mto1_drillthru_list_length );

boolean drillthru_skipped(
			char *folder_name,
			char *base_folder_name,
			int drillthru_fulfilled_folder_name_list_length );

boolean drillthru_middle(
			char *folder_name,
			char *current_folder_name );

boolean drillthru_finished(
			char *folder_name,
			char *current_folder_name );

#endif

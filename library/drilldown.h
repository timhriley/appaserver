/* $APPASERVER_HOME/library/drilldown.h		 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef DRILLDOWN_H
#define DRILLDOWN_H

/* Includes */
/* -------- */
#include "dictionary.h"
#include "list.h"
#include "relation.h"

/* Constants */
/* --------- */
#define DRILLDOWN_FOLDER_LIST_KEY	"drilldown_folder_list_key"
#define DRILLDOWN_BASE_FOLDER_KEY	"drilldown_base_folder_key"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *base_folder_name;
	DICTIONARY *drilldown_dictionary;

	/* Process */
	/* ------- */
	LIST *fulfilled_folder_name_list;
	LIST *relation_mto1_drilldown_list;
	char *current_folder_name;
} DRILLDOWN;

/* Operations */
/* ---------- */
DRILLDOWN *drilldown_calloc(
			void );

DRILLDOWN *drilldown_fetch(
			char *base_folder_name,
			DICTIONARY *drilldown_dictionary );

LIST *drilldown_fulfilled_folder_name_list(
			char *drilldown_folder_list_key,
			DICTIONARY *drilldown_dictionary );

char *drilldown_current_folder_name(
			LIST *relation_mto1_drilldown_list,
			char *base_folder_name );

void drilldown_base_folder_name_set(
			DICTIONARY *drilldown_dictionary,
			char *drilldown_folder_list_key,
			char *base_folder_name );

void drilldown_fulfilled_folder_name_list_set(
			DICTIONARY *drilldown_dictionary,
			char *drilldown_folder_list_key,
			LIST *fulfilled_folder_name_list );

void drilldown_fulfilled_folder_name_set(
			DICTIONARY *drilldown_dictionary,
			LIST *fulfilled_folder_name_list,
			char *current_folder_name );

#endif

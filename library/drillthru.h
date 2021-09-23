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
#define DRILLTHRU_FULFILLED_KEY		"drillthru_fulfilled"
#define DRILLTHRU_BASE_KEY		"drillthru_base"

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
	boolean drillthru_participating;
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
			char *folder_name );

/* Always succeeds */
/* --------------- */
DRILLTHRU *drillthru_fetch(
			DICTIONARY *drillthru_dictionary );

LIST *drillthru_fulfilled_folder_name_list(
			DICTIONARY *drillthru_dictionary );

char *drillthru_base_folder_name(
			DICTIONARY *drillthru_dictionary );

char *drillthru_current_folder_name(
			LIST *relation_mto1_drillthru_list,
			char *base_folder_name );

void drillthru_base_set(
			DICTIONARY *drillthru_dictionary,
			char *folder_name );

void drillthru_fulfilled_current_set(
			DICTIONARY *drillthru_dictionary,
			LIST *fulfilled_folder_name_list,
			char *current_folder_name );

void drillthru_fulfilled_set(
			DICTIONARY *drillthru_dictionary,
			LIST *fulfilled_folder_name_list );

boolean drillthru_participating(
			char *base_folder_name );

boolean drillthru_start_participating(
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

char *drillthru_participating_folder_name(
			char *folder_name,
			DRILLTHRU *drillthru );

#endif

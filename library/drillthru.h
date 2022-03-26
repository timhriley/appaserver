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
	boolean drillthru_participating;
	char *start_current_folder_name;
	boolean skipped;
	char *base_folder_name;
	LIST *fulfilled_folder_name_list;
	LIST *relation_mto1_drillthru_list;
	char *current_folder_name;
	boolean finished;
	LIST *current_fulfilled_folder_name_list;
} DRILLTHRU;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
DRILLTHRU *drillthru_start(
			DICTIONARY *drillthru_dictionary /* out */,
			char *folder_name );

/* Process */
/* ------- */
boolean drillthru_start_participating(
			int relation_mto1_drillthru_list_length );

/* Returns list_tail( relation_mto1_drillthru_list )->	*/
/*			one_folder->			*/
/*			folder->			*/
/*			folder_name			*/
/* or null.						*/
/* ---------------------------------------------------- */
char *drillthru_start_current_folder_name(
			LIST *relation_mto1_drillthru_list );

void drillthru_base_set(
			DICTIONARY *drillthru_dictionary,
			char *drillthru_base_key,
			char *folder_name );

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
DRILLTHRU *drillthru_continue(
			DICTIONARY *drillthru_dictionary /* in/out */,
			char *folder_name );

/* Process */
/* ------- */
char *drillthru_base_folder_name(
			char *drillthru_base_key,
			DICTIONARY *drillthru_dictionary );

boolean drillthru_continue_participating(
			char *base_folder_name );

boolean drillthru_skipped(
			int drillthru_dictionary_length );

LIST *drillthru_fulfilled_folder_name_list(
			char *drillthru_fulfilled_key,
			DICTIONARY *drillthru_dictionary );

char *drillthru_current_folder_name(
			LIST *relation_mto1_drillthru_list,
			char *base_folder_name );

boolean drillthru_finished(
			char *folder_name,
			char *current_folder_name );

LIST *drillthru_current_fulfilled_folder_name_list(
			LIST *drillthru_fulfilled_folder_name_list /* out */,
			char *drillthru_current_folder_name );

void drillthru_dictionary_fulfilled_set(
			DICTIONARY *drillthru_dictionary /* out */,
			char *drillthru_fulfilled_key,
			LIST *drillthru_current_fulfilled_folder_name_list );

/* Public */
/* ------ */
DRILLTHRU *drillthru_calloc(
			void );

#endif

/* --------------------------------------------- */
/* $APPASERVER_HOME/library/relation_join.h	 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef RELATION_JOIN_H
#define RELATION_JOIN_H

#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "relation.h"
#include "folder.h"

/* Structures */
/* ---------- */
typedef struct
{
	/* Process */
	/* ------- */
} RELATION_JOIN_FOLDER;

RELATION_JOIN_FOLDER *relation_join_folder_calloc(
			void );

/* Always succeeds */
/* --------------- */
RELATION_JOIN_FOLDER *relation_join_folder_new(
			DICTIONARY *dictionary /* in only */,
			FOLDER *many_folder,
			LIST *foreign_key_list,
			LIST *primary_key_list );

typedef struct
{
	/* Process */
	/* ------- */
	RELATION_JOIN_FOLDER *relation_join_folder;
} RELATION_JOIN;

/* Operations */
/* ---------- */
RELATION_JOIN *relation_join_calloc(
			void );

RELATION_JOIN *relation_join_new(
			DICTIONARY *dictionary /* in/out */,
			LIST *relation_join_one2m_list,
			LIST *primary_key_list );


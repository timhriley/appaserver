/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/radio.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef RADIO_H
#define RADIO_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"

/* Constants */
/* --------- */
#define RADIO_CLASS		"radio_class"

#define RADIO_STATISTICS_NAME	"radio_statistics"
#define RADIO_STATISTICS_LABEL	"statistics"

#define RADIO_GROUP_COUNT_NAME	"radio_group_count"
#define RADIO_GROUP_COUNT_LABEL	"group_count"

#define RADIO_SPREADSHEET_NAME	"radio_spreadsheet"
#define RADIO_SPREADSHEET_LABEL	"spreadsheet"

#define RADIO_INSERT_NAME	"radio_insert"
#define RADIO_INSERT_LABEL	"insert"

#define RADIO_DELETE_NAME	"radio_delete"
#define RADIO_DELETE_LABEL	"delete"

#define RADIO_LOOKUP_NAME	"radio_lookup"
#define RADIO_LOOKUP_LABEL	"lookup"

typedef struct
{
	char *radio_name;
	char *radio_label;
} RADIO_PAIR;

/* RADIO_PAIR operations */
/* --------------------- */
RADIO_PAIR *radio_pair_new(
			char *radio_name,
			char *radio_label );

/* Private */
/* ------- */
RADIO_PAIR *radio_pair_calloc(
			void );

typedef struct
{
	char *html;
} RADIO_VALUE;

/* RADIO_VALUE operations */
/* ---------------------- */
RADIO_VALUE *radio_value_new(
			char *radio_list_name,
			RADIO_PAIR *radio_pair,
			char *initial_label );

/* Returns heap memory */
/* ------------------- */
char *radio_value_html(	char *radio_list_name,
			char *radio_name,
			char *radio_label,
			char *initial_label );

/* Public */
/* ------ */
void radio_value_free(	RADIO_VALUE *radio_value );

/* Private */
/* ------- */
RADIO_VALUE *radio_value_calloc(
			void );

typedef struct
{
	LIST *radio_value_list;
	char *html;
} RADIO_LIST;

/* RADIO_LIST operations */
/* --------------------- */

/* Always succeeds */
/* --------------- */
RADIO_LIST *radio_list_new(
			char *form_radio_list_name,
			LIST *radio_pair_list,
			char *initial_label );

/* Returns heap memory */
/* -------------------- */
char *radio_list_html(	LIST *radio_value_list );

/* Private */
/* ------- */
RADIO_LIST *radio_list_calloc(
			void );

#endif

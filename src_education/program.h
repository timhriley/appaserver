/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_program/program.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PROGRAM_H
#define PROGRAM_H

#include "boolean.h"
#include "list.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PROGRAM_TABLE			"program"

#define PROGRAM_ALIAS_TABLE		"program_alias"

/* Structures */
/* ---------- */
typedef struct
{
	char *program_name;
	char *alias_name;
} PROGRAM_ALIAS;

typedef struct
{
	char *program_name;
	char *revenue_account;
	double program_donation_total;
	LIST *program_alias_list;
} PROGRAM;

PROGRAM *program_new(	char *program_name );

PROGRAM *program_fetch(	char *program_name,
			boolean fetch_alias_list );

LIST *program_list_fetch(
			boolean fetch_alias_list );

PROGRAM_ALIAS *program_alias_new(
			char *program_name,
			char *alias_name );

PROGRAM *program_calloc(
			void );

LIST *program_system_list(
			char *sys_string,
			boolean fetch_alias_list );

LIST *program_alias_system_list(
			char *sys_string );

PROGRAM *program_parse(
			char *input,
			boolean fetch_alias_list );

char *program_system_string(
			char *where );

char *program_alias_system_string(
			char *where );

/* Returns static memory */
/* --------------------- */
char *program_name_escape(
			char *program_name );

/* Returns static memory */
/* --------------------- */
char *program_primary_where(
			char *program_name );

LIST *program_alias_fetch_list(
			char *program_name );

PROGRAM *program_list_seek(
			char *program_name,
			LIST *program_list );

PROGRAM_ALIAS *program_alias_list_seek(
			char *program_alias_name,
			LIST *program_alias_list );

LIST *program_name_list(
			LIST *program_list );

LIST *program_alias_name_list(
			LIST *program_list );

void program_list_fetch_update(
			LIST *program_name_list );

void program_fetch_update(
			char *program_name );

#endif


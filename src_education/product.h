/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_product/product.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PRODUCT_H
#define PRODUCT_H

#include "boolean.h"
#include "list.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PRODUCT_TABLE			"product"

/* Structures */
/* ---------- */
typedef struct
{
	char *product_name;
	char *program_name;
	char *revenue_account;
} PRODUCT;

PRODUCT *product_new(	char *product_name );

PRODUCT *product_fetch(	char *product_name );

LIST *product_list(	void );

PRODUCT *product_calloc(
			void );

LIST *product_system_list(
			char *sys_string );

PRODUCT *product_parse(
			char *input );

char *product_sys_string(
			char *where );

/* Returns static memory */
/* --------------------- */
char *product_name_escape(
			char *product_name );

char *product_primary_where(
			char *product_name );

PRODUCT *product_list_seek(
			LIST *product_list,
			char *product_name );

char *product_seek_name(
			LIST *product_list,
			char *product_name );

char *product_fetch_program_name(
			char *product_name );

#endif

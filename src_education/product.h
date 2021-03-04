/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/product.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PRODUCT_H
#define PRODUCT_H

#include "boolean.h"
#include "list.h"
#include "program.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PRODUCT_TABLE			"product"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *product_name;
	char *program_name;
	char *revenue_account;
	double retail_price;

	/* Process */
	/* ------- */
	PROGRAM *program;
	double sale_total;
	double refund_total;
} PRODUCT;

PRODUCT *product_new(	char *product_name );

PRODUCT *product_fetch(	char *product_name,
			boolean fetch_program );

LIST *product_list(	void );

PRODUCT *product_calloc(
			void );

LIST *product_system_list(
			char *sys_string,
			boolean fetch_program );

PRODUCT *product_parse(
			char *input,
			boolean fetch_program );

char *product_sys_string(
			char *where );

/* Returns static memory */
/* --------------------- */
char *product_name_escape(
			char *product_name );

char *product_primary_where(
			char *product_name );

PRODUCT *product_list_seek(
			char *product_name,
			LIST *product_list );

LIST *product_name_list(
			LIST *product_list );

void product_fetch_update(
			char *product_name );

void product_list_fetch_update(
			LIST *product_name_list );

#endif


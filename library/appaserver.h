/* library/appaserver.h 						*/
/* -------------------------------------------------------------------- */
/* This maintains a set of APPASERVER ADTs.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_H
#define APPASERVER_H

/* Includes */
/* -------- */
#include "list.h"
#include "folder.h"
#include "related_folder.h"
#include "form.h"
#include "hash_table.h"
#include "boolean.h"
#include "aggregate_statistic.h"

/* Constants */
/* --------- */
#define APPASERVER_RELATIVE_SOURCE_DIRECTORY		"src_appaserver"
#define APPASERVER_ISA_PROMPT_PREFIX			"choose_"

/* Data structures */
/* --------------- */
typedef struct
{
	char *related_folder_name;
	char *folder_name;
	char *related_attribute_name;
	boolean accounted_for;
} ISA_FOLDER;

typedef struct
{
	FOLDER *folder;
	char *session;
	char *application_name;
	LIST *choose_folder_list;
	LIST *isa_folder_list;
} APPASERVER;

/* Prototypes */
/* ---------- */
char *appaserver_get_gray_drop_downs_javascript(
				FOLDER *root_folder,
				boolean related_folder_drop_down_multi_select,
				LIST *prompt_mto1_recursive_folder_list );

LIST *appaserver_get_isa_folder_list(
					char *application_name );
char *appaserver_isa_folder_list_display(
			LIST *isa_folder_list );

ISA_FOLDER *appaserver_new_isa_folder(
			char *folder_name,
			char *related_folder_name,
			char *related_attribute_name );

LIST *appaserver_get_choose_folder_list(
			char *application_name,
			char *session,
			char *role,
			int with_count );

APPASERVER *appaserver_folder_new(
			char *application_name,
			char *session,
			char *folder_name );

char *appaserver_get_delete_display_string(
			char *folder_name,
			LIST *attribute_list,
			LIST *related_folder_list,
			char *primary_data_list_string );

LIST *appaserver_get_folder_count_list(
			char *application_name,
			char *role,
			char *permissions,
			int with_count );

LIST *appaserver_remove_attribute_name_list_from_related_folder_list(
			LIST *related_folder_list,
			LIST *exclude_attribute_name_list );

LIST *appaserver_include_attribute_name_list_in_related_folder_list(
			LIST *related_folder_list,
			LIST *attribute_name_list );

LIST *appaserver_get_exclude_permission_list(
			char *application_name,
			char *attribute_name,
			char *role_name );

boolean appaserver_exclude_permission(
			LIST *exclude_permission_list,
			char *permission );

void appaserver_append_isa_related_attribute_list(
			LIST *attribute_list,
			LIST *mto1_isa_related_folder_list );

LIST *appaserver_get_exclude_attribute_name_list(
			LIST *attribute_list,
			char *permission );

int appaserver_frameset_menu_horizontal(
			char *application_name,
			char *login_name );

LIST *appaserver_get_exclude_permission_record_list(
			char *application_name );

int appaserver_isa_folder_accounted_for(
			LIST *isa_folder_list,
			char *related_folder_name,
			char *related_attribute_name );

enum aggregate_statistic appaserver_based_on_datatype_get_aggregate_statistic(
			char *application_name,
			char *appaserver_mount_point,
			char *datatype );

char appaserver_get_aggregation_sum_yn(
			char *application,
			char *appaserver_mount_point,
			char *datatype );

/* Returns heap memory */
/* ------------------- */
char *appaserver_escape_street_address(
			char *street_address );

APPASERVER *appaserver_calloc(
			void );

#endif

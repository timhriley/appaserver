/* library/row_security_dvr.c				   */
/* ======================================================= */
/* Freely available software: see Appaserver.org	   */
/* ======================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "folder.h"
#include "related_folder.h"
#include "row_security.h"
#include "role.h"
#include "appaserver_parameter_file.h"

void row_security_dvr(
			char *application_name,
			char *role_name,
			char *folder_name,
			char *no_display_pressed_attribute_name_list_string );

int main( void )
{
	char *application_name = "benthic";

/*
	char *role_name = "dataentry";
	char *role_name = "supervisor";
*/
/*
	char *role_name = "supervisor";
	char *folder_name = "sampling_point";
	char *no_display_pressed_attribute_name_list_string =
"collection,site,tide,boat_lead_researcher,second_boat_researcher,collection_diver1,collection_diver2";
*/

	char *role_name = "supervisor";
	char *folder_name = "species_count";
	char *no_display_pressed_attribute_name_list_string = "";

	row_security_dvr(	application_name,
				role_name,
				folder_name,
				no_display_pressed_attribute_name_list_string );

	return 0;
}

void row_security_dvr(
			char *application_name,
			char *role_name,
			char *select_folder_name,
			char *no_display_pressed_attribute_name_list_string )
{
	ROW_SECURITY *row_security;
	LIST *element_list;
	ROLE *login_role;
	char *state = "update";
	char *login_name = "tim";

	login_role = role_new_role( application_name, role_name );

	row_security =
		row_security_new(
			application_name,
			login_role,
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				select_folder_name,
				role_new_role(
					application_name,
					role_name ) ),
			login_name,
			state,
			(DICTIONARY *)0 /* preprompt_dictionary */,
			(DICTIONARY *)0 /* query_dictionary */,
			(DICTIONARY *)0 /* sort_dictionary */,
			list_string2list(
				no_display_pressed_attribute_name_list_string,
				',' ) );

	row_security->row_security_element_list_structure =
		row_security_element_list_structure_new(
			application_name,
			row_security->row_security_state,
			row_security->login_name,
			row_security->state,
			row_security->login_role,
			row_security->preprompt_dictionary,
			row_security->query_dictionary,
			row_security->sort_dictionary,
			row_security->
				no_display_pressed_attribute_name_list,
			row_security->select_folder,
			row_security->attribute_not_null_folder,
			row_security->foreign_login_name_folder,
			(LIST *)0 /* where_clause_attribute_name_list */,
			(LIST *)0 /* where_clause_data_list */,
			(LIST *)0 /* non_edit_folder_name_list */,
			0 /* make_primary_keys_non_edit */,
			omit_delete_dont_care,
			0 /* omit_operation_buttons */,
			'y' /* update_yn */,
			0 /* not ajax_fill_drop_down_omit */,
			row_security->
				select_folder->
				append_isa_attribute_list );

	element_list =
		row_security->
			row_security_element_list_structure->
			regular_element_list;

	printf( "regular = %s\n",
		element_appaserver_list_display(
			element_list ) );

	element_list =
		row_security->
			row_security_element_list_structure->
			viewonly_element_list;

	printf( "viewonly = %s\n",
		element_list_appaserver_display(
			element_list ) );

} /* row_security_dvr() */

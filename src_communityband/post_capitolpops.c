/* --------------------------------------------------- 	*/
/* src_communityband/post_capitolpops.c		       	*/
/* --------------------------------------------------- 	*/
/* This gets executed from the Capitolpops website.     */
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "folder.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "environ.h"
#include "dictionary.h"
#include "post2dictionary.h"
#include "document.h"
#include "application_constants.h"
#include "capitolpops.h"
#include "musician.h"

/* Constants */
/* --------- */
#define DATABASE		"capitolpops"
#define APPLICATION		"capitolpops"
#define NULL_EMAIL_ADDRESS	"null@null"

/* Prototypes */
/* ---------- */
char *get_webmaster_email_address(	void );

char *get_section_leader_email_address(
					char *primary_instrument );

LIST *get_musician_substitute_email_list(
					char *primary_instrument );

void email_new_substitute_send(		DICTIONARY *dictionary,
					char *primary_instrument );

boolean post_cloudacus_save_contact(	char *contact_message,
					DICTIONARY *post_dictionary );

void post_cloudacus_send_contact(	DICTIONARY *post_dictionary,
					char *send_email );

void email_new_substitute(
				char *email_address,
				char *substitute_full_name,
				char *primary_instrument,
				char *home_phone,
				char *cell_phone,
				char *work_phone );

boolean musician_exists_full_name(
				char *full_name );

boolean musician_exists_email_address(
				char *email_address );

void save_substitute(
				char *substitute_full_name,
				char *email_address,
				char *home_phone,
				char *cell_phone,
				char *work_phone,
				char *instrument_1,
				char *instrument_2,
				char *instrument_3 );

void insert_substitute(		
				char *substitute_full_name,
				char *email_address,
				char *primary_instrument,
				char *home_phone,
				char *cell_phone,
				char *work_phone );

void update_substitute(		char *full_name,
				char *email_address,
				char *home_phone,
				char *cell_phone,
				char *work_phone );

void insert_secondary_instrument(
				char *substitute_full_name,
				char *instrument_1,
				char *instrument_2,
				char *instrument_3 );

void post_membership_substitution_list(
				char *member_full_name );

void post_substitute_substitution_list(
				DICTIONARY *post_dictionary );

void post_cloudacus_substitution_list(
				DICTIONARY *post_dictionary );

void post_cloudacus_musician_list(
				DICTIONARY *post_dictionary );

void contact_email_send(	char *email_address,
				char *date,
				char *time,
				char *send_email );

void get_now_date_time(		char **date, char **time );

void post_cloudacus_contact(	char *contact_message,
				DICTIONARY *post_dictionary );

void post_cloudacus_mailing_list(
				DICTIONARY *post_dictionary );

void insert_patron(
				char *email_address,
				char *full_name,
				char *home_phone );

void update_patron(		char *email_address,
				char *full_name,
				char *home_phone );

boolean patron_email_address_exists(
				char *email_address );

int main( int argc, char **argv )
{
	char *contact_message = "";
	DICTIONARY *post_dictionary;
	char *http_referer_filename;

exit( 0 );

	environ_set_environment(
		APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
		DATABASE );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( APPLICATION );

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				APPLICATION );

	post_dictionary =
		post2dictionary(stdin,
				(char *)0 /* appaserver_data_directory */,
				(char *)0 /* session */ );

	environ_set_environment(
		APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
		DATABASE );

	dictionary_get_index_data(
			&contact_message,
			post_dictionary,
			"message",
			0 );

	http_referer_filename = environ_get_http_referer_filename();

	if ( timlib_strcmp(	http_referer_filename,
				"musicians.php" ) == 0 )
	{
		post_cloudacus_musician_list(
					post_dictionary );
	}
	else
	if ( timlib_strcmp(	http_referer_filename,
				"substitution.php" ) == 0 )
	{
		post_cloudacus_substitution_list(
					post_dictionary );
	}
	else
	if ( *contact_message )
	{
		post_cloudacus_contact( contact_message,
					post_dictionary );

		post_capitolpops_redraw(
			"contact_message=succeeded" );
	}
	else
	{
		post_cloudacus_mailing_list( post_dictionary );
	}

	exit( 0 );
} /* main() */

boolean patron_email_address_exists( char *email_address )
{
	char sys_string[ 1024 ];
	char where[ 1024 ];
	int results;

	if ( !email_address || !*email_address ) return 0;

	sprintf( where, "email_address = '%s'", email_address );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=count		"
		 "			folder=patron		"
		 "			where=\"%s\"		",
		 APPLICATION,
		 where );

	results = atoi( pipe2string( sys_string ) );
	return results;
} /* patron_email_address_exists() */

void update_patron(		char *email_address,
				char *full_name,
				char *home_phone )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *key = "email_address";

	table_name = get_table_name( APPLICATION, "patron" );

	if ( full_name && *full_name )
	{
		sprintf( sys_string,
		 	"echo \"%s^full_name^%s\"			|"
		 	"update_statement.e table=%s key=%s carrot=y	|"
		 	"sql.e						 ",
		 	email_address,
		 	full_name,
		 	table_name,
			key );
		system( sys_string );
	}

	if ( home_phone && *home_phone )
	{
		sprintf( sys_string,
		 	"echo \"%s^home_phone^%s\"			|"
		 	"update_statement.e table=%s key=%s carrot=y	|"
		 	"sql.e						 ",
		 	email_address,
		 	home_phone,
		 	table_name,
			key );
		system( sys_string );
	}

} /* update_patron() */

void insert_patron(		char *email_address,
				char *full_name,
				char *home_phone )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *field_list;

	field_list = "email_address,full_name,home_phone";

	table_name = get_table_name( APPLICATION, "patron" );

	sprintf( sys_string,
		 "echo \"%s|%s|%s\"					|"
		 "insert_statement.e table=%s field=%s			|"
		 "sql.e							 ",
		 email_address,
		 (full_name) ? full_name : "",
		 (home_phone) ? home_phone : "",
		 table_name,
		 field_list );

	system( sys_string );

} /* insert_patron() */

void post_capitolpops_redraw( char *redraw_message )
{
	char *http_referer;
	char location[ 512 ];

	if ( ! ( http_referer = environ_get_http_referer() ) )
	{
		fprintf( stderr,
		 "Error in %s/%s()/%d: cannot get http_referer\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( redraw_message && *redraw_message )
	{
		sprintf( location,
			 "%s?%s",
			 http_referer,
			 redraw_message );
	}
	else
	{
		strcpy( location, http_referer );
	}

	printf(
"Content-type: text/html						\n"
"\n"
"<html>									\n"
"<script type=\"text/javascript\">					\n"
"window.location = \"%s\"						\n"
"</script>								\n"
"</html>								\n",
		location );

} /* post_capitolpops_redraw() */

void post_cloudacus_mailing_list( DICTIONARY *post_dictionary )
{
	char *email_address = "";

	dictionary_get_index_data(
			&email_address,
			post_dictionary,
			"email_address",
			0 );

	if ( !*email_address )
	{
		post_capitolpops_redraw( (char *)0 );
		return;
	}

	if ( character_count( '@', email_address ) != 1 )
	{
		post_capitolpops_redraw(
				"email_address_status=failed" );
		return;
	}

	if ( patron_email_address_exists( email_address ) )
	{
		post_capitolpops_redraw(
				"email_address_status=duplicate" );
		return;
	}

	insert_patron(		email_address,
				(char *)0 /* full_name */,
				(char *)0 /* home_phone */ );

	post_capitolpops_redraw(
			"email_address_status=succeeded" );

} /* post_cloudacus_mailing_list() */

void post_cloudacus_contact(	char *contact_message,
				DICTIONARY *post_dictionary )
{
	/* Spam gets silently rejected. */
	/* ---------------------------- */
	if ( !post_cloudacus_save_contact(
		contact_message,
		post_dictionary ) )
	{
		return;
	}

	/* Send to the director */
	/* -------------------- */
	post_cloudacus_send_contact(	post_dictionary,
					(char *)0 /* send_email */ );

	post_cloudacus_send_contact(	post_dictionary,
					get_webmaster_email_address() );

} /* post_cloudacus_contact() */

boolean post_cloudacus_save_contact(	char *contact_message,
					DICTIONARY *post_dictionary )
{
	char *email_address = "";
	char *full_name = "";
	char *home_phone = "";
	char *subject = "";
	char *date;
	char *time;
	char sys_string[ 1024 ];
	char *table_name;
	char *field_list;

	dictionary_get_index_data(
			&subject,
			post_dictionary,
			"subject",
			0 );

	if ( timlib_exists_string( subject, "website" ) )
	{
		/* Ignore spam. */
		/* ------------ */
		return 0;
	}

	if ( dictionary_get_index_data(
			&full_name,
			post_dictionary,
			"full_name",
			0 ) == -1 )
	{
		/* Full name is required. */
		/* ---------------------- */
		return 0;
	}

	if (	timlib_strcmp( full_name, "na" ) == 0
	||	timlib_strcmp( full_name, "n/a" ) == 0
	||	timlib_strncmp( full_name, "pharmacy" ) == 0 )
	{
		/* Full name is required. */
		/* ---------------------- */
		return 0;
	}

	if ( dictionary_get_index_data(
			&email_address,
			post_dictionary,
			"email_address",
			0 ) == -1 )
	{
		/* Email is required. */
		/* ------------------ */
		return 0;
	}

	if ( !isalpha( *email_address ) )
	{
		/* Email must begin with a letter. */
		/* ------------------------------- */
		return 0;
	}

	if (	timlib_strcmp( email_address, "na" ) == 0
	||	timlib_strcmp( email_address, "n/a" ) == 0
	||	isdigit( *email_address ) )
	{
		/* Email is required. */
		/* ------------------ */
		return 0;
	}

	dictionary_get_index_data(
			&home_phone,
			post_dictionary,
			"home_phone",
			0 );

	if ( patron_email_address_exists( email_address ) )
	{
		update_patron(		email_address,
					full_name,
					home_phone );
	}
	else
	{
		insert_patron(		email_address,
					full_name,
					home_phone );
	}

	get_now_date_time( &date, &time );

	field_list = "email_address,date,time,subject,message";

	table_name = get_table_name( APPLICATION, "contact" );

	sprintf( sys_string,
		 "echo \"%s|%s|%s|%s|%s\"				|"
		 "insert_statement.e table=%s field=%s			|"
		 "sql.e							 ",
		 email_address,
		 date,
		 time,
		 subject,
		 contact_message,
		 table_name,
		 field_list );

	system( sys_string );

	return 1;

} /* post_cloudacus_save_contact() */

void post_cloudacus_send_contact(	DICTIONARY *post_dictionary,
					char *send_email )
{
	char *email_address = "";
	char *date;
	char *time;

	dictionary_get_index_data(
			&email_address,
			post_dictionary,
			"email_address",
			0 );

	if ( !*email_address ) email_address = NULL_EMAIL_ADDRESS;

	get_now_date_time( &date, &time );

	contact_email_send( email_address, date, time, send_email );

} /* post_cloudacus_send_contact() */

void get_now_date_time( char **date, char **time )
{
	char *results;
	static char local_date[ 16 ] = {0};
	static char local_time[ 32 ];

	if ( *local_date )
	{
		*date = local_date;
		*time = local_time;
		return;
	}

	if ( ! ( results = pipe2string( "date.e 0 seconds" ) ) )
	{
		fprintf( stderr,
			 "ERROR In %s/%s()/%d: cannot execute date.e\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	piece( local_date, ':', results, 0 );

	strcpy( local_time, results );
	piece_inverse( local_time, ':', 0 );

	*date = local_date;
	*time = local_time;
} /* get_now_date_time() */

void contact_email_send(	char *email_address,
				char *date,
				char *time,
				char *email_send )
{
	char sys_string[ 1024 ];
	char *error_filename;

	error_filename = appaserver_error_get_filename( APPLICATION );

	if ( email_send && *email_send )
	{
		sprintf( sys_string,
		 	"send_patron_mail.sh %s %s %s %s %s 2>>%s",
		 	APPLICATION,
		 	email_address,
		 	date,
		 	time,
			email_send,
		 	error_filename );
	}
	else
	{
		sprintf( sys_string,
		 	"send_patron_mail.sh %s %s %s %s 2>>%s",
		 	APPLICATION,
		 	email_address,
		 	date,
		 	time,
		 	error_filename );
	}

	system( sys_string );

} /* contact_email_send() */

void post_cloudacus_substitution_list( DICTIONARY *post_dictionary )
{
	char *member_full_name = "";

	dictionary_get_index_data(
			&member_full_name,
			post_dictionary,
			"member_full_name",
			0 );

	if ( *member_full_name )
	{
		post_membership_substitution_list( member_full_name );
		return;
	}
	else
	{
		post_substitute_substitution_list( post_dictionary );
		return;
	}

} /* post_cloudacus_substitution_list() */

void post_substitute_substitution_list( DICTIONARY *post_dictionary )
{
	char *email_address = "";
	char *substitute_full_name = "";
	char *home_phone = "";
	char *cell_phone = "";
	char *work_phone = "";
	char *instrument_1 = "";
	char *instrument_2 = "";
	char *instrument_3 = "";

	dictionary_get_index_data(
			&substitute_full_name,
			post_dictionary,
			"substitute_full_name",
			0 );

	if ( !*substitute_full_name )
	{
		post_capitolpops_redraw(
				"substitute_message=full_name_required" );
		return;
	}

	dictionary_get_index_data(
			&email_address,
			post_dictionary,
			"email_address",
			0 );

	if ( !email_address && character_count( '@', email_address ) != 1 )
	{
		post_capitolpops_redraw(
				"substitute_message=bad_email_format" );
		return;
	}

	dictionary_get_index_data(
			&home_phone,
			post_dictionary,
			"home_phone",
			0 );

	dictionary_get_index_data(
			&cell_phone,
			post_dictionary,
			"cell_phone",
			0 );

	dictionary_get_index_data(
			&work_phone,
			post_dictionary,
			"work_phone",
			0 );

	dictionary_get_index_data(
			&instrument_1,
			post_dictionary,
			"instrument_1",
			0 );

	if ( !*instrument_1 )
	{
		post_capitolpops_redraw(
				"substitute_message=instrument_required" );
		return;
	}

	dictionary_get_index_data(
			&instrument_2,
			post_dictionary,
			"instrument_2",
			0 );

	dictionary_get_index_data(
			&instrument_3,
			post_dictionary,
			"instrument_3",
			0 );

	save_substitute(
		substitute_full_name,
		email_address,
		home_phone,
		cell_phone,
		work_phone,
		instrument_1,
		instrument_2,
		instrument_3 );

} /* post_substitute_substitution_list() */

void post_membership_substitution_list(	char *member_full_name )
{
	LIST *substitute_musician_list;
	MUSICIAN *musician;
	char *email_address = "";
	char sys_string[ 1024 ];
	FILE *output_pipe;

	if ( ! ( substitute_musician_list =
			musician_get_substitute_musician_list(
				&email_address,
				member_full_name ) ) )
	{
		post_capitolpops_redraw(
				"musician_message=no_primary_instrument" );
		return;
	}

	if ( !list_rewind( substitute_musician_list ) )
	{
		post_capitolpops_redraw(
				"member_message=no_substitutes" );
		return;
	}

	if ( !*email_address )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty email_address for (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 member_full_name );
		return;
	}

	sprintf( sys_string,
		 "send_substitution_list.sh \"%s\" \"%s\" 2>>%s",
		 member_full_name,
		 email_address,
		 appaserver_error_get_filename( APPLICATION ) );


	output_pipe = popen( sys_string, "w" );

	do {
		musician = list_get_pointer( substitute_musician_list );

		fprintf( output_pipe,
			 "%s^%s^%s^%s^%s^%s\n",
			 musician->email_address,
			 musician->full_name,
			 musician->home_phone,
			 musician->cell_phone,
			 musician->work_phone,
			 musician->substitute_concert_only_yn );

	} while( list_next( substitute_musician_list ) );

	pclose( output_pipe );

	post_capitolpops_redraw(
			"member_message=succeeded" );
} /* post_membership_substitution_list() */

void save_substitute(
		char *substitute_full_name,
		char *email_address,
		char *home_phone,
		char *cell_phone,
		char *work_phone,
		char *instrument_1,
		char *instrument_2,
		char *instrument_3 )
{
	if ( musician_exists_full_name( substitute_full_name ) )
	{
		post_capitolpops_redraw(
			"substitute_message=substitute_exists_full_name" );
		return;
	}
	else
	if ( musician_exists_email_address( email_address ) )
	{
		post_capitolpops_redraw(
			"substitute_message=substitute_exists_email_address" );
		return;
	}
	else
	{
		insert_substitute(
				substitute_full_name,
				email_address,
				instrument_1,
				home_phone,
				cell_phone,
				work_phone );

		insert_secondary_instrument(
				substitute_full_name,
				(char *)0 /* instrument_1 */,
				instrument_2,
				instrument_3 );

		email_new_substitute(
				email_address,
				substitute_full_name,
				instrument_1,
				home_phone,
				cell_phone,
				work_phone );

		post_capitolpops_redraw(
				"substitute_message=insert_succeeded" );
	}

} /* save_substitute() */

boolean musician_exists_full_name( char *full_name )
{
	char sys_string[ 1024 ];
	char where[ 1024 ];
	int results;

	sprintf( where, "full_name = '%s'", full_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=count		"
		 "			folder=musician		"
		 "			where=\"%s\"		",
		 APPLICATION,
		 where );

	results = atoi( pipe2string( sys_string ) );
	return results;
} /* musician_exists_full_name() */

boolean musician_exists_email_address( char *email_address )
{
	char sys_string[ 1024 ];
	char where[ 1024 ];
	int results;

	sprintf( where, "email_address = '%s'", email_address );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=count		"
		 "			folder=musician		"
		 "			where=\"%s\"		",
		 APPLICATION,
		 where );

	results = atoi( pipe2string( sys_string ) );
	return results;
} /* musician_exists_email_address() */

void update_substitute(		char *full_name,
				char *email_address,
				char *home_phone,
				char *cell_phone,
				char *work_phone )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *key = "full_name";

	table_name = get_table_name( APPLICATION, "musician" );

	if ( home_phone && *home_phone )
	{
		sprintf( sys_string,
		 	"echo \"%s^home_phone^%s\"			|"
		 	"update_statement.e table=%s key=%s carrot=y	|"
		 	"sql.e						 ",
		 	full_name,
		 	home_phone,
		 	table_name,
			key );
		system( sys_string );
	}

	if ( cell_phone && *cell_phone )
	{
		sprintf( sys_string,
		 	"echo \"%s^cell_phone^%s\"			|"
		 	"update_statement.e table=%s key=%s carrot=y	|"
		 	"sql.e						 ",
		 	full_name,
		 	cell_phone,
		 	table_name,
			key );
		system( sys_string );
	}

	if ( work_phone && *work_phone )
	{
		sprintf( sys_string,
		 	"echo \"%s^work_phone^%s\"			|"
		 	"update_statement.e table=%s key=%s carrot=y	|"
		 	"sql.e						 ",
		 	full_name,
		 	work_phone,
		 	table_name,
			key );
		system( sys_string );
	}

	if ( email_address && *email_address )
	{
		sprintf( sys_string,
		 	"echo \"%s^email_address^%s\"			|"
		 	"update_statement.e table=%s key=%s carrot=y	|"
		 	"sql.e						 ",
		 	full_name,
		 	email_address,
		 	table_name,
			key );
		system( sys_string );
	}

} /* update_substitute() */

void insert_secondary_instrument(
				char *substitute_full_name,
				char *instrument_1,
				char *instrument_2,
				char *instrument_3 )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *field_list;

	field_list = "full_name,instrument";

	table_name = get_table_name( APPLICATION, "secondary_instrument" );

	if ( instrument_1 && *instrument_1 )
	{
		sprintf( sys_string,
		 	"echo \"%s|%s\"					|"
		 	"insert_statement.e table=%s field=%s		|"
		 	"sql.e						 ",
		 	substitute_full_name,
		 	instrument_1,
		 	table_name,
		 	field_list );
		system( sys_string );
	}

	if ( instrument_2 && *instrument_2 )
	{
		sprintf( sys_string,
		 	"echo \"%s|%s\"					|"
		 	"insert_statement.e table=%s field=%s		|"
		 	"sql.e						 ",
		 	substitute_full_name,
		 	instrument_2,
		 	table_name,
		 	field_list );
		system( sys_string );
	}

	if ( instrument_3 && *instrument_3 )
	{
		sprintf( sys_string,
		 	"echo \"%s|%s\"					|"
		 	"insert_statement.e table=%s field=%s		|"
		 	"sql.e						 ",
		 	substitute_full_name,
		 	instrument_3,
		 	table_name,
		 	field_list );
		system( sys_string );
	}

} /* insert_secondary_instrument() */

void insert_substitute(		
				char *substitute_full_name,
				char *email_address,
				char *primary_instrument,
				char *home_phone,
				char *cell_phone,
				char *work_phone )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *field_list;

	table_name = get_table_name( APPLICATION, "musician" );
	field_list =
"full_name,email_address,primary_instrument,home_phone,cell_phone,work_phone";

	sprintf( sys_string,
		 "echo \"%s|%s|%s|%s|%s|%s\"				|"
		 "insert_statement.e table=%s field=%s			|"
		 "sql.e 2>&1						|"
		 "cat							 ",
		 substitute_full_name,
		 (email_address) ? email_address : "",
		 primary_instrument,
		 (home_phone) ? home_phone : "",
		 (cell_phone) ? cell_phone : "",
		 (work_phone) ? work_phone : "",
		 table_name,
		 field_list );

	system( sys_string );

	table_name = get_table_name( APPLICATION, "substitute" );
	field_list = "full_name";

	sprintf( sys_string,
		 "echo \"%s\"						|"
		 "insert_statement.e table=%s field=%s			|"
		 "sql.e 2>&1						|"
		 "grep -vi duplicate					 ",
		 substitute_full_name,
		 table_name,
		 field_list );

	system( sys_string );

} /* insert_substitute() */

void email_new_substitute(
				char *email_address,
				char *substitute_full_name,
				char *primary_instrument,
				char *home_phone,
				char *cell_phone,
				char *work_phone )
{
	DICTIONARY *dictionary = dictionary_new();
	char contact_message[ 256 ];
	char subject[ 128 ];
	char *phone_number = "";

	strcpy( subject, "New substitute" );

	sprintf( contact_message,
		 CONTACT_MESSAGE_TEMPLATE,
		 primary_instrument );

	if ( email_address && *email_address )
	{
		dictionary_set_pointer(	dictionary,
					"email_address",
					email_address );
	}

	dictionary_set_pointer(	dictionary,
				"full_name",
				substitute_full_name );

	if ( home_phone && *home_phone )
	{
		phone_number = home_phone;
	}
	else
	if ( cell_phone && *cell_phone )
	{
		phone_number = cell_phone;
	}
	else
	if ( work_phone && *work_phone )
	{
		phone_number = work_phone;
	}

	dictionary_set_pointer(	dictionary,
				"home_phone",
				phone_number );

	dictionary_set_pointer(	dictionary,
				"subject",
				subject );

	post_cloudacus_save_contact(	contact_message,
					dictionary );

	email_new_substitute_send(	dictionary,
					primary_instrument );

} /* email_new_substitute() */

void email_new_substitute_send(	DICTIONARY *dictionary,
				char *primary_instrument )
{
	char *send_email;
	LIST *send_email_list;

	/* Send to the director */
	/* -------------------- */
	post_cloudacus_send_contact(	dictionary,
					(char *)0 /* send_email */ );

	post_cloudacus_send_contact(	dictionary,
					get_webmaster_email_address() );

	send_email_list =
		get_musician_substitute_email_list(
			primary_instrument );

	if ( !list_rewind( send_email_list ) ) return;

	do {
		send_email = list_get_pointer( send_email_list );

		/* ---------------------------------------------- */
		/* Send to each musician for that instrument plus */
		/* the section leader, if different instrument.   */
		/* ---------------------------------------------- */
		post_cloudacus_send_contact(	dictionary,
						send_email );

	} while( list_next( send_email_list ) );

} /* email_new_substitute_send() */

LIST *get_musician_substitute_email_list(
			char *primary_instrument )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
	LIST *substitute_email_list;
	char *section_leader_email_address;

	sprintf(	where,
			"email_address is not null and		"
			"primary_instrument = '%s'		",
			primary_instrument );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=email_address	"
		 "			folder=musician		"
		 "			where=\"%s\"		",
		 APPLICATION,
		 where );

	substitute_email_list = pipe2list( sys_string );

	section_leader_email_address =
		get_section_leader_email_address(
			primary_instrument );

	if ( section_leader_email_address
	&&   *section_leader_email_address
	&&   !list_exists_string(
		section_leader_email_address,
		substitute_email_list ) )
	{
		list_append_pointer(
			substitute_email_list,
			section_leader_email_address );
	}

	return substitute_email_list;

} /* get_musician_substitute_email_list() */

void post_cloudacus_musician_list( DICTIONARY *post_dictionary )
{
	LIST *member_list;
	MUSICIAN *member;
	char *email_address = "";
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *member_full_name = "";

	dictionary_get_index_data(
			&member_full_name,
			post_dictionary,
			"member_full_name",
			0 );

	if ( ! ( member_list =
			musician_get_band_member_list(
				&email_address,
				member_full_name ) ) )
	{
		post_capitolpops_redraw(
				"membership_list_status=failed" );
		return;
	}

	if ( !list_rewind( member_list ) )
	{
		post_capitolpops_redraw(
				"membership_list_status=failed" );
		return;
	}

	if ( !*email_address )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty email_address for (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 member_full_name );
		return;
	}

	sprintf( sys_string,
		 "send_member_list.sh \"%s\" \"%s\" 2>>%s",
		 member_full_name,
		 email_address,
		 appaserver_error_get_filename( APPLICATION ) );

	output_pipe = popen( sys_string, "w" );

	do {
		member = list_get_pointer( member_list );

		if ( *member->contact_information_private_yn == 'y' )
		{
			fprintf( output_pipe,
			 	"%s^%s^%s^%s^%s^%s\n",
			 	member->full_name,
			 	member->primary_instrument,
			 	"Private",
			 	"Private",
			 	"Private",
			 	"Private" );
		}
		else
		{
			fprintf( output_pipe,
			 	"%s^%s^%s^%s^%s^%s\n",
			 	member->full_name,
			 	member->primary_instrument,
			 	member->email_address,
			 	member->home_phone,
			 	member->cell_phone,
			 	member->work_phone );
		}

	} while( list_next( member_list ) );

	pclose( output_pipe );

	post_capitolpops_redraw(
			"membership_list_status=succeeded" );

} /* post_cloudacus_musician_list() */

char *get_section_leader_email_address(
					char *primary_instrument )
{
	char sys_string[ 1024 ];
	char *where_instrument_section_join;
	char *where_section_band_member_join;
	char *where_band_member_musician_join;
	char where[ 512 ];
	char *select;
	char *from;

	where_instrument_section_join =
		"instrument.section = section.section";
	where_section_band_member_join =
		"section.section_leader = band_member.full_name";
	where_band_member_musician_join =
		"band_member.full_name = musician.full_name";

	select = "musician.email_address";
	from = "musician,instrument,section,band_member";

	sprintf( where,
		 "email_address is not null and			"
		 "instrument.instrument = '%s' and		"
		 "%s and %s and %s				",
		 primary_instrument,
		 where_instrument_section_join,
		 where_section_band_member_join,
		 where_band_member_musician_join );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e | head -1",
		 select,
		 from,
		 where );

	return pipe2string( sys_string );

} /* get_section_leader_email_address() */

char *get_webmaster_email_address( void )
{
	return pipe2string( "select_webmaster_email.sh" );

} /* get_webmaster_email_address() */


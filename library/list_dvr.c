/* list_dvr.c */
/* ---------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef struct
{
	char *last_name;
	char *first_name;
} PERSON;

PERSON *new_person( void );
void test_add_in_order();
void test_add_position();
int person_compare( PERSON *person1, PERSON *person2 );
void person_free( PERSON *person );
void person_list_display( LIST *list );
void test_list_exists( void );

int person_compare( PERSON *person1, PERSON *person2 )
{
	return strcmp( person1->last_name, person2->last_name );
}

int main()
{
	/* test_add_in_order(); */
	/* test_list_exists(); */
	test_add_position();
	return 0;
}

PERSON *new_person()
{
	return (PERSON *)calloc( 1, sizeof( PERSON ) );
}

void person_free( PERSON *person )
{
	free( person );
}

void person_list_display( LIST *list )
{
	if ( list_rewind( list ) )
	{
		PERSON *person;
		do {
			person = list_get_pointer( list );
			printf( "%s\n", person->last_name );
		} while( list_next( list ) );
	}
}

void test_add_in_order()
{
	LIST *list = list_new_list();
	PERSON *person;

	person = new_person();
	person->last_name = "zulo";
	list_add_in_order(	list, 
				(char *)person, 
				sizeof( PERSON ),
				person_compare );

	/* Free it because list_add_in_order() allocates its own memory */
	/* ------------------------------------------------------------ */
	person_free( person );

	person = new_person();
	person->last_name = "alpha";
	list_add_in_order(	list,
				(char *)person, 
				sizeof( PERSON ),
				person_compare );
	person_free( person );

	person = new_person();
	person->last_name = "bravo";
	list_add_in_order(	list,
				(char *)person, 
				sizeof( PERSON ),
				person_compare );
	person_free( person );

	person = new_person();
	person->last_name = "charley";
	list_add_in_order(	list,
				(char *)person, 
				sizeof( PERSON ),
				person_compare );
	person_free( person );

	person = new_person();
	person->last_name = "alpha2";
	list_add_in_order(	list,
				(char *)person, 
				sizeof( PERSON ),
				person_compare );
	person_free( person );

	person_list_display( list );
}

void test_list_exists()
{
	LIST *list = list_new_list();

	list_append_pointer( list, "one" );
	list_append_pointer( list, "two" );
	list_append_pointer( list, "three" );

	printf( "Got list_exists_string = %d\n",
		list_exists_string( "one", list ) );
}

void test_add_position()
{
	LIST *list = list_new_list();

	list_append_string( list, "one" );
	list_append_string( list, "two" );
	list_append_string( list, "three" );

	list_set_position_string(
			list,
			"one point five",
			1 /* position */ );
	printf( "%s\n", list_display( list ) );
}


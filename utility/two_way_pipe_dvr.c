#include <stdio.h>
#include "two_way_pipe.h"

void test1( void );
void test2( void );
void test3( void );
void test4( void );

int main( void )
{
	/* test1(); */
	test2();
	/* test3(); */
	/* test4(); */
	return 0;
}

void test1()
{
	TWO_WAY_PIPE *t = two_way_pipe_new( "cat", "/tmp" );
	char *input_line;

	two_way_pipe_set_string( t, "hello" );
	input_line = two_way_pipe_get_string( t );
	printf( "%s\n", input_line );

	two_way_pipe_set_string( t, "world" );
	input_line = two_way_pipe_get_string( t );
	printf( "%s\n", input_line );

	two_way_pipe_close( t );
}

void test2()
{
	TWO_WAY_PIPE *t = two_way_pipe_new( "table_count.e stdin", "/tmp" );
	char *input_line;

	two_way_pipe_set_string( t, "measurement" );
	input_line = two_way_pipe_get_string( t );
	printf( "%s\n", input_line );

	two_way_pipe_set_string( t, "measurement_backup" );
	input_line = two_way_pipe_get_string( t );
	printf( "%s\n", input_line );

	two_way_pipe_close( t );
}

void test3()
{
	TWO_WAY_PIPE *t =
		two_way_pipe_new( "sed 's/hello/hello world/'", "/tmp" );
	char *input_line;

	two_way_pipe_set_string( t, "hello" );
	input_line = two_way_pipe_get_string( t );
	printf( "%s\n", input_line );

	two_way_pipe_set_string( t, "hello" );
	input_line = two_way_pipe_get_string( t );
	printf( "%s\n", input_line );

	two_way_pipe_set_string( t, "hello" );
	input_line = two_way_pipe_get_string( t );
	printf( "%s\n", input_line );

	two_way_pipe_close( t );
}

void test4()
{
	TWO_WAY_PIPE *t =
		two_way_pipe_new( "piece.e ',' 1", "/tmp" );
	char *input_line;

	two_way_pipe_set_string( t, "ignored,hello,ignored" );
	input_line = two_way_pipe_get_string( t );
	printf( "%s\n", input_line );

	two_way_pipe_set_string( t, "ignored,world,ignored" );
	input_line = two_way_pipe_get_string( t );
	printf( "%s\n", input_line );

	two_way_pipe_close( t );
}


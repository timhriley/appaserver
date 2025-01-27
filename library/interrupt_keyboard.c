#include <stdio.h>
#include <termio.h>
#include <signal.h>

struct termios new_settings;
struct termios stored_settings;

void set_keypress(void) {
        tcgetattr(0,&stored_settings);
        new_settings = stored_settings;
        new_settings.c_lflag &= (~ICANON);
        new_settings.c_cc[VTIME] = 0;
        tcgetattr(0,&stored_settings);
        new_settings.c_cc[VMIN] = 1;
        tcsetattr(0,TCSANOW,&new_settings);
}

/* void process_keystroke_x(); */

void process_keystroke_x()
{
	printf( "Caught x\n" );
	signal( SIGUSR1, process_keystroke_x );
}

int main( int argc, char *argv )
{
	int c;
	int child_process_id;

	set_keypress();
	signal( SIGUSR1, process_keystroke_x );
	child_process_id = fork();

	if ( child_process_id == 0 ) {	/* If child */
		while( 1 ) {
			printf( "Waiting...\n" );
			sleep( 5 );
		}
	}

	while( 1 ) { /* If parent */
		c = getchar();
		if ( c == 'x' ) kill( child_process_id, SIGUSR1 );
	}
}


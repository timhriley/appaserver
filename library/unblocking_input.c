#include <stdio.h>
#include <termio.h>

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

void reset_keypress(void) {
                                         tcsetattr(0,TCSANOW,&stored_settings);
                                     }

int main( int argc, char *argv )
{
	char c;

	set_keypress();
	c = getchar();
	reset_keypress();
}


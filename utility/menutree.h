/* menutree.h */
/* ---------- */
/* --------------------------------------------------------
	This module supports a tree structured menu system.

	Tim Riley
----------------------------------------------------------- */

#ifndef MENU_TREE_H
#define MENU_TREE_H

#include "list.h"
#include "stack.h"
#include "queue.h"

#define MAX_STRING		4096
#define SAVE_FILE_PATTERN	"/tmp/%s_%s.dat"

typedef struct
{
	char *prompt;
	char *variable;
	char *default_value;
	char *entered_value;
	char *list_values;
} PROMPT;

typedef struct
{
	char *display_text;
	LIST *node_list;
	char *executable;
	char *directory;
	int confirm;
	LIST *prompt_list;
	QUEUE *text_queue;
	char *save_key;
	PROMPT *current_prompt;
} NODE;

typedef struct
{
	NODE root;
	NODE *current;
	STACK *node_stack;
	char error_buffer[ 256 ];

} MENU_TREE;

/* Function prototypes */
/* ------------------- */
int executable();
int list_values();
int confirm();
int directory();
int prompt();
int text();
int save();

/* Operations */
/* ---------- */
MENU_TREE *menu_tree_init( void );
int menu_tree_load_recursive( MENU_TREE *m, FILE *infile, NODE *root );
void menu_tree_display_level( MENU_TREE *m );
int menu_tree_goto_selection( MENU_TREE *m, int i );
int menu_tree_confirm( NODE *n );
void menu_tree_goto_root( MENU_TREE *m );
NODE *menu_tree_pop( MENU_TREE *m );
void menu_tree_push( MENU_TREE *m );
void menu_tree_free( MENU_TREE *m );
char *menu_tree_get_error( MENU_TREE *m );
int menu_tree_handle_error( MENU_TREE *m, char *error );
PROMPT *prompt_search( LIST *prompt_list, char *variable );
void save_default_values( LIST *prompt_list, char *save_key );
void load_default_values( LIST *prompt_list, char *save_key );
void display_prompts( LIST *prompt_list );
void get_variables_from_user( LIST *prompt_list );
void populate_sys_str( char *sys_str, LIST *prompt_list, char *executable );
char *replace_sys_str_with_prompt_list( char *s, LIST *prompt_list );
void menu_tree_display_root_text( char *display_text );
void display_text_queue( NODE *n );
int execute_it( NODE *n );
int set_other_node_info( NODE *node_ptr, char *process_string );
int search_fn_array( char *command );
int change_the_directory( NODE *n );
int menu_tree_load( MENU_TREE *m, char *filename );
void interact( MENU_TREE *m );

#endif	/* MENU_TREE_H */

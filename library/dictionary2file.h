/* library/dictionary2file.h						*/
/* -------------------------------------------------------------------- */
/* This is the dictionary2file function.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DICTIONARY2FILE_H
#define DICTIONARY2FILE_H

#include <unistd.h>
#include "dictionary.h"

/* Remove this when done. */
/* #define DICTIONARY2FILE_DIRECTORY	"data" */

DICTIONARY *dictionary2file_get_dictionary(
				pid_t process_id,
				char *appaserver_data_directory,
				char *insert_update_key,
				char *folder_name,
				char *optional_related_attribute_name,
				char dictionary_delimiter );

char *dictionary2file_get_filename(
				pid_t process_id,
				char *appaserver_mount_point,
				char *insert_update_key,
				char *folder_name,
				char *optional_related_attribute_name );

#endif

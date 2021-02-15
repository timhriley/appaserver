# src_freshwaterfish/makefile
# ---------------------------

CC=gcc
#CFLAGS=-g 
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

SOURCE_EVERGLADES=${APPASERVER_HOME}/src_hydrology

LIBRARY=${APPASERVER_HOME}/library

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/String.o 					\
		${LIBRARY}/float.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hashtbl.o

EXECUTABLES=								\
		load_throw_spreadsheet					\
		load_lab_spreadsheet					\
		output_habitat_input_spreadsheet			\
		output_lab_input_spreadsheet				\
		output_fiu_data						\
		load_fiu_data						\
		species_list						\
		calculate_checksum

EXTERNAL_LIBS=								\
		${LIBRARY}/aggregate_level.o 				\
		${LIBRARY}/aggregate_statistic.o			\
		${LIBRARY}/process_generic_output.o			\
		${LIBRARY}/decode_html_post.o				\
		${LIBRARY}/update_statement.o				\
		${LIBRARY}/fopen_path.o					\
		${LIBRARY}/name_arg.o					\
		${LIBRARY}/html_table.o					\
		${LIBRARY}/basename.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/application_constants.o			\
		${LIBRARY}/dictionary_appaserver.o			\
		${LIBRARY}/make_date_time_between_compatable.o		\
		${LIBRARY}/appaserver_library.o				\
		${LIBRARY}/prompt_recursive.o				\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/application.o				\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/element.o					\
		${LIBRARY}/form.o					\
		${LIBRARY}/query.o 					\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/validation_level.o				\
		${LIBRARY}/related_folder.o				\
		${LIBRARY}/vertical_new_button.o			\
		${LIBRARY}/relation.o					\
		${LIBRARY}/operation_list.o				\
		${LIBRARY}/operation.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/insert_database.o				\
		${LIBRARY}/update_database.o				\
		${LIBRARY}/process_parameter_list.o			\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/appaserver_parameter_file.o			\
		${LIBRARY}/process.o					\
		${LIBRARY}/validation_form.o				\
		${LIBRARY}/expected_count.o				\
		${LIBRARY}/post2dictionary.o				\
		${LIBRARY}/dictionary2file.o				\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/session.o

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	rm -f ${EXECUTABLES} *.o
	touch appaserver.err && > appaserver.err

load_throw_spreadsheet: load_throw_spreadsheet.c			\
	${EXTERNAL_LIBS} freshwaterfish_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		freshwaterfish_library.o				\
		load_throw_spreadsheet.c				\
		-o load_throw_spreadsheet

load_lab_spreadsheet: load_lab_spreadsheet.c				\
	${EXTERNAL_LIBS} freshwaterfish_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		freshwaterfish_library.o				\
		load_lab_spreadsheet.c					\
		-o load_lab_spreadsheet

output_habitat_input_spreadsheet: output_habitat_input_spreadsheet.c	\
	${EXTERNAL_LIBS} freshwaterfish_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		freshwaterfish_library.o				\
		output_habitat_input_spreadsheet.c			\
		-o output_habitat_input_spreadsheet

output_fiu_data: output_fiu_data.c					\
	${EXTERNAL_LIBS} freshwaterfish_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		freshwaterfish_library.o				\
		output_fiu_data.c					\
		-o output_fiu_data

load_fiu_data: load_fiu_data.c						\
	${EXTERNAL_LIBS} freshwaterfish_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		freshwaterfish_library.o				\
		load_fiu_data.c						\
		-o load_fiu_data

output_lab_input_spreadsheet: output_lab_input_spreadsheet.c		\
	${EXTERNAL_LIBS} freshwaterfish_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		freshwaterfish_library.o				\
		output_lab_input_spreadsheet.c				\
		-o output_lab_input_spreadsheet

species_list: species_list.c						\
	${EXTERNAL_LIBS} freshwaterfish_library.o			\
	${LIBRARY}/additional_drop_down_attribute.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		freshwaterfish_library.o				\
		${LIBRARY}/additional_drop_down_attribute.o		\
		species_list.c						\
		-o species_list

calculate_checksum: calculate_checksum.c				\
	${EXTERNAL_LIBS} freshwaterfish_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		freshwaterfish_library.o				\
		calculate_checksum.c					\
		-o calculate_checksum

freshwaterfish_library.o: freshwaterfish_library.c			\
		freshwaterfish_library.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c freshwaterfish_library.c


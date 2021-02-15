# src_benthic/makefile
# ---------------------


CC=gcc
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

LIBRARY=${APPASERVER_HOME}/library

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/list.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/String.o 					\
		${LIBRARY}/float.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hashtbl.o

EXECUTABLES=	update_cell_stage					\
		upload_photo

EXTERNAL_LIBS=								\
		${LIBRARY}/aggregate_level.o 				\
		${LIBRARY}/aggregate_statistic.o			\
		${LIBRARY}/dictionary_appaserver.o			\
		${LIBRARY}/process_generic_output.o			\
		${LIBRARY}/decode_html_post.o				\
		${LIBRARY}/update_statement.o				\
		${LIBRARY}/fopen_path.o					\
		${LIBRARY}/name_arg.o					\
		${LIBRARY}/html_table.o					\
		${LIBRARY}/prompt_recursive.o				\
		${LIBRARY}/basename.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/application_constants.o			\
		${LIBRARY}/make_date_time_between_compatable.o		\
		${LIBRARY}/appaserver_library.o				\
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
		${LIBRARY}/expected_count.o				\
		${LIBRARY}/post2dictionary.o				\
		${LIBRARY}/dictionary2file.o				\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/session.o

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f *.o ${EXECUTABLES}
	/bin/rm -f ${CGI_HOME}/post_benthic_validation_form

update_cell_stage: update_cell_stage.c					\
		${LIBRARY}/distance.o					\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		${LIBRARY}/distance.o					\
		update_cell_stage.c					\
		-o update_cell_stage -lm

upload_photo: upload_photo.c						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		upload_photo.c						\
		-o upload_photo


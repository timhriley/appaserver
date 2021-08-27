# src_sparrow/makefile
# ---------------------


CC=gcc
#CFLAGS=-g 
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

LIBRARY=${APPASERVER_HOME}/library

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/String.o 					\
		${LIBRARY}/float.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hash_table.o

EXECUTABLES=	output_bird_count					\
		bird_count_group_by					\
		output_observation_merge				\
		sparrow_validation_form					\
		post_sparrow_validation_form				\
		observation_merge					\
		output_spreadsheet					\
		upload_field_sheet

EXTERNAL_LIBRARY=							\
		${LIBRARY}/aggregate_level.o 				\
		${LIBRARY}/aggregate_statistic.o			\
		${LIBRARY}/dictionary_separate.o			\
		${LIBRARY}/process_generic_output.o			\
		${LIBRARY}/decode_html_post.o				\
		${LIBRARY}/update_statement.o				\
		${LIBRARY}/fopen_path.o					\
		${LIBRARY}/name_arg.o					\
		${LIBRARY}/html_table.o					\
		${LIBRARY}/basename.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/application_constants.o			\
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
		${LIBRARY}/security.o					\
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
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/appaserver_parameter_file.o			\
		${LIBRARY}/process.o					\
		${LIBRARY}/expected_count.o				\
		${LIBRARY}/post_dictionary.o				\
		${LIBRARY}/dictionary2file.o				\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/session.o

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	rm -f *.o ${EXECUTABLES}
	rm -f ${CGI_HOME}/post_sparrow_validation_form

output_bird_count: output_bird_count.c					\
		${STANDARD_LIBRARY}					\
		${EXTERNAL_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBRARY} 					\
		output_bird_count.c					\
		-o output_bird_count

output_observation_merge: output_observation_merge.c			\
		${STANDARD_LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		sparrow_library.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBRARY} 					\
		output_observation_merge.c				\
		sparrow_library.o					\
		-o output_observation_merge

bird_count_group_by: bird_count_group_by.c				\
		${STANDARD_LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		sparrow_library.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBRARY} 					\
		bird_count_group_by.c					\
		sparrow_library.o					\
		-o bird_count_group_by

observation_merge: observation_merge.c					\
		${STANDARD_LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		sparrow_library.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBRARY} 					\
		observation_merge.c					\
		sparrow_library.o					\
		-o observation_merge

upload_field_sheet: upload_field_sheet.c				\
		${STANDARD_LIBRARY}					\
		${EXTERNAL_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBRARY} 					\
		upload_field_sheet.c					\
		-o upload_field_sheet

output_spreadsheet: output_spreadsheet.c				\
		${STANDARD_LIBRARY}					\
		${EXTERNAL_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBRARY} 					\
		output_spreadsheet.c					\
		-o output_spreadsheet

sparrow_validation_form: sparrow_validation_form.c			\
		${STANDARD_LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		sparrow_library.o					\
		site_visit.o						\
		sparrow_observation.o					\
		vegetation_observation.o				\
		physical_observation.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBRARY} 					\
		sparrow_validation_form.c				\
		sparrow_library.o					\
		site_visit.o						\
		sparrow_observation.o					\
		physical_observation.o					\
		vegetation_observation.o				\
		-o sparrow_validation_form

post_sparrow_validation_form: post_sparrow_validation_form.c		\
		${STANDARD_LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		sparrow_library.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBRARY} 					\
		post_sparrow_validation_form.c				\
		sparrow_library.o					\
		-o post_sparrow_validation_form
	touch ${CGI_HOME}/post_sparrow_validation_form
	rm -f ${CGI_HOME}/post_sparrow_validation_form
	ln ${APPASERVER_LINK_FLAGS} 					\
	${APPASERVER_HOME}/src_sparrow/post_sparrow_validation_form 	\
	${CGI_HOME}

sparrow_library.o: sparrow_library.c
	${CC}	${CFLAGS} -I${LIBRARY} -c				\
		sparrow_library.c

site_visit.o: site_visit.c
	${CC}	${CFLAGS} -I${LIBRARY} -c				\
		site_visit.c

sparrow_observation.o: sparrow_observation.c
	${CC}	${CFLAGS} -I${LIBRARY} -c				\
		sparrow_observation.c

physical_observation.o: physical_observation.c
	${CC}	${CFLAGS} -I${LIBRARY} -c				\
		physical_observation.c

vegetation_observation.o: vegetation_observation.c
	${CC}	${CFLAGS} -I${LIBRARY} -c				\
		vegetation_observation.c


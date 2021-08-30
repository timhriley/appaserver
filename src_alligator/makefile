# src_alligator/makefile
# ----------------------

CC=gcc
#CFLAGS=-O3 -s -Wall -W
#CFLAGS=-g -O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}
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
		${LIBRARY}/hash_table.o

EXECUTABLES=	load_srf_spreadsheet					\
		load_visit_spreadsheet					\
		nest_observation_report					\
		srf_dataentry_wizard					\
		visit_dataentry_wizard					\
		post_alligator_srf_dataentry_wizard			\
		post_alligator_visit_dataentry_wizard			\
		alligator_google_map_operation				\
		output_srf_spreadsheet					\
		output_visit_spreadsheet				\
		output_group_annually					\
		google_map_basin_process				\
		eggs_report						\
		alligator_dvr						\
		locate_cell_basin					\
		assign_nest_basin					\
		update_transect_basin

EXTERNAL_LIBS=								\
		${LIBRARY}/aggregate_level.o 				\
		${LIBRARY}/aggregate_statistic.o			\
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
		${LIBRARY}/dictionary_separate.o			\
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
		${LIBRARY}/operation.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/insert_database.o				\
		${LIBRARY}/update_database.o				\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/appaserver_parameter_file.o			\
		${LIBRARY}/process.o					\
		${LIBRARY}/validation_form.o				\
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
	rm -f ${CGI_HOME}/post_alligator_srf_dataentry_wizard
	rm -f ${CGI_HOME}/post_alligator_visit_dataentry_wizard

nest_observation_report: nest_observation_report.c			\
		alligator.o						\
		${STANDARD_LIBRARY}					\
		load_spreadsheets.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		nest_observation_report.c				\
		alligator.o						\
		-o nest_observation_report

output_srf_spreadsheet: output_srf_spreadsheet.c			\
		alligator.o						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		output_srf_spreadsheet.c				\
		alligator.o						\
		-o output_srf_spreadsheet

output_visit_spreadsheet: output_visit_spreadsheet.c			\
		alligator.o						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		output_visit_spreadsheet.c				\
		alligator.o						\
		-o output_visit_spreadsheet

output_group_annually: output_group_annually.c				\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		output_group_annually.c					\
		${LIBRARY}/grace.o					\
		-o output_group_annually

post_alligator_srf_dataentry_wizard:					\
		post_alligator_srf_dataentry_wizard.c			\
		alligator.o						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		post_alligator_srf_dataentry_wizard.c			\
		alligator.o						\
		-o post_alligator_srf_dataentry_wizard
	touch ${CGI_HOME}/post_alligator_srf_dataentry_wizard
	rm -f ${CGI_HOME}/post_alligator_srf_dataentry_wizard
	ln ${APPASERVER_LINK_FLAGS} 					\
	${APPASERVER_HOME}/src_alligator/post_alligator_srf_dataentry_wizard \
	${CGI_HOME}

post_alligator_visit_dataentry_wizard:					\
		post_alligator_visit_dataentry_wizard.c			\
		alligator.o						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		post_alligator_visit_dataentry_wizard.c			\
		alligator.o						\
		-o post_alligator_visit_dataentry_wizard
	touch ${CGI_HOME}/post_alligator_visit_dataentry_wizard
	rm -f ${CGI_HOME}/post_alligator_visit_dataentry_wizard
	ln ${APPASERVER_LINK_FLAGS} 					\
	${APPASERVER_HOME}/src_alligator/post_alligator_visit_dataentry_wizard \
	${CGI_HOME}

eggs_report: eggs_report.c						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		eggs_report.c						\
		-o eggs_report

locate_cell_basin: locate_cell_basin.c					\
		alligator.o						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		alligator.o						\
		locate_cell_basin.c					\
		-o locate_cell_basin

assign_nest_basin: assign_nest_basin.c					\
		alligator.o						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		alligator.o						\
		assign_nest_basin.c					\
		-o assign_nest_basin

update_transect_basin: update_transect_basin.c				\
		alligator.o						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		alligator.o						\
		update_transect_basin.c					\
		-o update_transect_basin

alligator_dvr: alligator_dvr.c						\
		alligator.o						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		alligator.o						\
		alligator_dvr.c						\
		-o alligator_dvr

srf_dataentry_wizard: srf_dataentry_wizard.c				\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		srf_dataentry_wizard.c					\
		-o srf_dataentry_wizard

visit_dataentry_wizard: visit_dataentry_wizard.c			\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		visit_dataentry_wizard.c				\
		-o visit_dataentry_wizard

alligator_google_map_operation: alligator_google_map_operation.c	\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/google_map.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		alligator_google_map_operation.c			\
		${LIBRARY}/google_map.o					\
		-o alligator_google_map_operation

load_srf_spreadsheet: load_srf_spreadsheet.c				\
		alligator.o						\
		${STANDARD_LIBRARY}					\
		load_spreadsheets.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		load_srf_spreadsheet.c					\
		alligator.o						\
		-o load_srf_spreadsheet

load_visit_spreadsheet: load_visit_spreadsheet.c			\
		alligator.o						\
		${STANDARD_LIBRARY}					\
		load_spreadsheets.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		load_visit_spreadsheet.c				\
		alligator.o						\
		-o load_visit_spreadsheet

google_map_basin_process: 						\
		google_map_basin_process.c				\
		${LIBRARY}/google_map.o					\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/google_map.o					\
		google_map_basin_process.c				\
		-o google_map_basin_process

alligator.o: alligator.c alligator.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c alligator.c


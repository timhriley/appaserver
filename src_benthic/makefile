# src_benthic/makefile
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
		${LIBRARY}/sed.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/String.o 					\
		${LIBRARY}/float.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hashtbl.o

EXECUTABLES=	output_spss_ready_measurements				\
		output_spss_ready_counts				\
		select_spss_ready_counts				\
		output_quick_count					\
		output_wet_dry_annual					\
		load_field_sheet_scan					\
		output_scatterplot					\
		output_processing_time_sheet				\
		output_quality_control_checksum				\
		benthic_validation_form					\
		output_spss_ready_cover_abundance			\
		output_spss_ready_sampling_point_abiotic		\
		output_spss_ready_attempt_success			\
		compare_sampling_point_with_waypoint			\
		compare_sampling_point_with_waypoint_process		\
		post_benthic_validation_form				\
		generate_sampling_point_candidates			\
		benthic_species_list

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

output_spss_ready_measurements: output_spss_ready_measurements.c	\
		${STANDARD_LIBRARY}					\
		benthic_library.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		output_spss_ready_measurements.c			\
		benthic_library.o					\
		-o output_spss_ready_measurements

output_spss_ready_counts: output_spss_ready_counts.c			\
		${STANDARD_LIBRARY}					\
		benthic_species.o					\
		benthic_spss.o						\
		sampling_point.o					\
		benthic_library.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		benthic_species.o					\
		benthic_library.o					\
		sampling_point.o					\
		benthic_spss.o						\
		output_spss_ready_counts.c				\
		-o output_spss_ready_counts

select_spss_ready_counts: select_spss_ready_counts.c			\
		${STANDARD_LIBRARY}					\
		benthic_species.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		benthic_species.o					\
		select_spss_ready_counts.c				\
		-o select_spss_ready_counts

output_spss_ready_cover_abundance: output_spss_ready_cover_abundance.c	\
		${STANDARD_LIBRARY}					\
		benthic_library.o					\
		braun_blanque.o						\
		benthic_species.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		benthic_library.o					\
		braun_blanque.o						\
		benthic_species.o					\
		output_spss_ready_cover_abundance.c			\
		-o output_spss_ready_cover_abundance

output_spss_ready_sampling_point_abiotic:				\
		output_spss_ready_sampling_point_abiotic.c		\
		benthic_library.o					\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		benthic_library.o					\
		output_spss_ready_sampling_point_abiotic.c		\
		-o output_spss_ready_sampling_point_abiotic

output_spss_ready_attempt_success:					\
		output_spss_ready_attempt_success.c			\
		benthic_library.o					\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		benthic_library.o					\
		output_spss_ready_attempt_success.c			\
		-o output_spss_ready_attempt_success

compare_sampling_point_with_waypoint:					\
		compare_sampling_point_with_waypoint.c			\
		benthic_library.o					\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		benthic_library.o					\
		compare_sampling_point_with_waypoint.c			\
		-o compare_sampling_point_with_waypoint

compare_sampling_point_with_waypoint_process:				\
		compare_sampling_point_with_waypoint_process.c		\
		benthic_library.o					\
		${LIBRARY}/waypoint.o					\
		${LIBRARY}/distance.o					\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		benthic_library.o					\
		${LIBRARY}/waypoint.o					\
		${LIBRARY}/distance.o					\
		compare_sampling_point_with_waypoint_process.c		\
		-o compare_sampling_point_with_waypoint_process -lm

output_quick_count: output_quick_count.c				\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		output_quick_count.c					\
		${LIBRARY}/grace.o					\
		-o output_quick_count

output_wet_dry_annual: output_wet_dry_annual.c				\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		output_wet_dry_annual.c					\
		${LIBRARY}/grace.o					\
		-o output_wet_dry_annual

load_field_sheet_scan: load_field_sheet_scan.c				\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		load_field_sheet_scan.c					\
		-o load_field_sheet_scan

output_scatterplot: output_scatterplot.c				\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/regression.o					\
		environment.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		output_scatterplot.c					\
		environment.o						\
		${LIBRARY}/grace.o					\
		${LIBRARY}/regression.o					\
		-o output_scatterplot

output_processing_time_sheet: output_processing_time_sheet.c		\
		${STANDARD_LIBRARY}					\
		location.o						\
		site.o							\
		sampling_point.o					\
		sweep.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		output_processing_time_sheet.c				\
		location.o						\
		site.o							\
		sampling_point.o					\
		sweep.o							\
		-o output_processing_time_sheet

output_quality_control_checksum: output_quality_control_checksum.c	\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		output_quality_control_checksum.c			\
		-o output_quality_control_checksum

benthic_species_list: benthic_species_list.c				\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		benthic_species_list.c					\
		-o benthic_species_list

merge_purge_benthic_species: merge_purge_benthic_species.c		\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		merge_purge_benthic_species.c				\
		-o merge_purge_benthic_species

benthic_validation_form: benthic_validation_form.c			\
		${STANDARD_LIBRARY}					\
		sweep.o							\
		sampling_point.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		sampling_point.o					\
		sweep.o							\
		benthic_validation_form.c				\
		-o benthic_validation_form

generate_sampling_point_candidates:					\
	generate_sampling_point_candidates.c				\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/google_map.o					\
		${LIBRARY}/waypoint.o					\
		${LIBRARY}/distance.o					\
		waypoint.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		generate_sampling_point_candidates.c			\
		${LIBRARY}/google_map.o					\
		${LIBRARY}/waypoint.o					\
		${LIBRARY}/distance.o					\
		waypoint.o						\
		-o generate_sampling_point_candidates -lm

post_benthic_validation_form: post_benthic_validation_form.c		\
		${STANDARD_LIBRARY}					\
		sampling_point.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS} 					\
		post_benthic_validation_form.c				\
		sampling_point.o					\
		-o post_benthic_validation_form
	touch ${CGI_HOME}/post_benthic_validation_form
	rm -f ${CGI_HOME}/post_benthic_validation_form
	ln ${APPASERVER_LINK_FLAGS} 					\
	${APPASERVER_HOME}/src_benthic/post_benthic_validation_form 	\
	${CGI_HOME}

location.o: location.c location.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c location.c

sweep.o: sweep.c sweep.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c sweep.c

site.o: site.c site.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c site.c

environment.o: environment.c						\
		environment.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c environment.c

benthic_species.o: benthic_species.c benthic_species.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c benthic_species.c

benthic_library.o: benthic_library.c benthic_library.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c benthic_library.c

braun_blanque.o: braun_blanque.c braun_blanque.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c braun_blanque.c

benthic_spss.o: benthic_spss.c benthic_spss.h sampling_point.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c benthic_spss.c

sampling_point.o: sampling_point.c sampling_point.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c sampling_point.c

waypoint.o: waypoint.c benthic_waypoint.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c waypoint.c


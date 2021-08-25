# src_creel/makefile
# ------------------

CC=gcc
#CFLAGS=-g 
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall -Wunused-parameter

SOURCE_EVERGLADES=${APPASERVER_HOME}/src_hydrology

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

EXECUTABLES=								\
		creel_census_report					\
		creel_catches_report					\
		species_estimated_total_caught				\
		estimated_fishing_effort_hours				\
		calculate_species_cpue					\
		sport_measurement_report				\
		creel_spreadsheet_file					\
		creel_guide_spreadsheet_file				\
		species_preferred_species_caught			\
		output_measurement_spreadsheet_file			\
		load_sport_fishing_trips				\
		load_guide_fishing_trips				\
		load_trailer_count					\
		load_guide_angler_submission				\
		load_fishing_permits					\
		load_sport_measurements					\
		catches_per_species					\
		creel_catches2minutes_between_fish			\
		delete_sport_creel_census				\
		insert_sport_creel_census				\
		insert_sport_fishing_trips				\
		insert_sport_catches					\
		view_maps_figs						\
		species_list						\
		guide_compliance_mail_merge				\
		families_list						\
		permits_merge

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
		${LIBRARY}/dictionary_appaserver.o			\
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
	/bin/rm -f ${EXECUTABLES} *.o

creel_census_report: creel_census_report.c				\
	${EXTERNAL_LIBS} creel_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		creel_census_report.c					\
		-o creel_census_report

load_sport_fishing_trips: load_sport_fishing_trips.c			\
	${EXTERNAL_LIBS} creel_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		load_sport_fishing_trips.c				\
		-o load_sport_fishing_trips

load_sport_measurements: load_sport_measurements.c			\
	${EXTERNAL_LIBS} creel_library.o creel_load_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		creel_load_library.o					\
		load_sport_measurements.c				\
		-o load_sport_measurements

guide_compliance_mail_merge: guide_compliance_mail_merge.c		\
	${EXTERNAL_LIBS} creel_library.o creel_load_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		creel_load_library.o					\
		guide_compliance_mail_merge.c				\
		-o guide_compliance_mail_merge

load_guide_fishing_trips: load_guide_fishing_trips.c			\
	${EXTERNAL_LIBS}						\
	creel_library.o							\
	creel_load_library.o						\
	creel.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		creel_load_library.o					\
		creel.o							\
		load_guide_fishing_trips.c				\
		-o load_guide_fishing_trips

load_trailer_count: load_trailer_count.c				\
	${EXTERNAL_LIBS} creel_library.o creel_load_library.o		\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		creel_load_library.o					\
		load_trailer_count.c					\
		-o load_trailer_count

load_guide_angler_submission: load_guide_angler_submission.c		\
	${EXTERNAL_LIBS}						\
	creel_library.o							\
	creel_load_library.o						\
	creel.o								\
	${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		creel_load_library.o					\
		creel.o							\
		load_guide_angler_submission.c				\
		-o load_guide_angler_submission

load_fishing_permits: load_fishing_permits.c				\
	${EXTERNAL_LIBS} creel_library.o creel_load_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		creel_load_library.o					\
		load_fishing_permits.c					\
		-o load_fishing_permits

creel_catches_report: creel_catches_report.c				\
	${EXTERNAL_LIBS} creel_library.o				\
	${LIBRARY}/easycharts.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/easycharts.o					\
		creel_library.o						\
		creel_catches_report.c					\
		-o creel_catches_report

calculate_species_cpue: calculate_species_cpue.c			\
	${EXTERNAL_LIBS} creel_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		calculate_species_cpue.c				\
		-o calculate_species_cpue

species_estimated_total_caught: species_estimated_total_caught.c	\
	${STANDARD_LIBRARY} 						\
	${EXTERNAL_LIBS} total_caught.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		total_caught.o						\
		species_estimated_total_caught.c			\
		-o species_estimated_total_caught

estimated_fishing_effort_hours: estimated_fishing_effort_hours.c	\
	${STANDARD_LIBRARY} 						\
	${EXTERNAL_LIBS} total_caught.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		total_caught.o						\
		estimated_fishing_effort_hours.c			\
		-o estimated_fishing_effort_hours

sport_measurement_report: sport_measurement_report.c			\
	${EXTERNAL_LIBS} creel_library.o				\
	${LIBRARY}/easycharts.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/easycharts.o					\
		creel_library.o						\
		sport_measurement_report.c				\
		-o sport_measurement_report

creel_spreadsheet_file: creel_spreadsheet_file.c			\
	${EXTERNAL_LIBS} creel_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		creel_spreadsheet_file.c				\
		-o creel_spreadsheet_file

output_measurement_spreadsheet_file:					\
	output_measurement_spreadsheet_file.c				\
	${EXTERNAL_LIBS} creel_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		output_measurement_spreadsheet_file.c			\
		-o output_measurement_spreadsheet_file

creel_guide_spreadsheet_file: creel_guide_spreadsheet_file.c		\
	${EXTERNAL_LIBS} creel_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		creel_guide_spreadsheet_file.c				\
		-o creel_guide_spreadsheet_file

species_preferred_species_caught: species_preferred_species_caught.c	\
	${EXTERNAL_LIBS} creel_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_library.o						\
		species_preferred_species_caught.c			\
		-o species_preferred_species_caught

creel_catches_vs_salinity: creel_catches_vs_salinity.c			\
		${EXTERNAL_LIBS}					\
		${SOURCE_EVERGLADES}/hydrology_library.o		\
		${LIBRARY}/easycharts.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_EVERGLADES}/hydrology_library.o		\
		${LIBRARY}/easycharts.o					\
		creel_catches_vs_salinity.c				\
		-o creel_catches_vs_salinity

catches_per_species: catches_per_species.c				\
		${EXTERNAL_LIBS} creel_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		catches_per_species.c					\
		creel_library.o						\
		-o catches_per_species

insert_sport_creel_census:						\
	insert_sport_creel_census.c creel_load_library.o creel_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS}					\
		insert_sport_creel_census.c				\
		creel_load_library.o					\
		creel_library.o						\
		-o insert_sport_creel_census

insert_sport_fishing_trips:						\
	insert_sport_fishing_trips.c					\
		creel_load_library.o					\
		creel_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS}					\
		insert_sport_fishing_trips.c				\
		creel_load_library.o					\
		creel_library.o						\
		-o insert_sport_fishing_trips

insert_sport_catches:							\
	${EXTERNAL_LIBS} creel_library.o creel_load_library.o		\
	insert_sport_catches.c
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS}					\
		insert_sport_catches.c					\
		creel_load_library.o					\
		creel_library.o						\
		-o insert_sport_catches

delete_sport_creel_census:						\
	delete_sport_creel_census.c creel_load_library.o creel_library.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS}					\
		delete_sport_creel_census.c				\
		creel_load_library.o					\
		creel_library.o						\
		-o delete_sport_creel_census

species_list:								\
	species_list.c ${LIBRARY}/additional_drop_down_attribute.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS}					\
		species_list.c						\
		${LIBRARY}/additional_drop_down_attribute.o		\
		-o species_list

families_list:								\
	families_list.c ${LIBRARY}/additional_drop_down_attribute.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS}					\
		families_list.c						\
		${LIBRARY}/additional_drop_down_attribute.o		\
		-o families_list

permits_merge:								\
	permits_merge.c creel.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS}					\
		permits_merge.c						\
		creel.o							\
		-o permits_merge

creel_catches2minutes_between_fish:					\
		creel_catches2minutes_between_fish.c			\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_catches2minutes_between_fish.c			\
		-o creel_catches2minutes_between_fish

view_maps_figs:	view_maps_figs.c					\
		${EXTERNAL_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		view_maps_figs.c					\
		-o view_maps_figs

creel_dvr:	creel_dvr.c						\
		${EXTERNAL_LIBS} creel.o
	${CC}	${CFLAGS} -I${LIBRARY} -I${SOURCE_EVERGLADES}		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		creel_dvr.c						\
		creel.o							\
		-o creel_dvr

creel_library.o: creel_library.c creel_library.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c creel_library.c

creel_load_library.o: creel_load_library.c creel_load_library.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c creel_load_library.c

total_caught.o: total_caught.c total_caught.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c total_caught.c

creel.o: creel.c creel.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c creel.c


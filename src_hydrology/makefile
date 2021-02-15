# src_hydrology/makefile
# -----------------------

CC=gcc
#CFLAGS=-g 
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall -Wunused-parameter

SOURCE_EVERGLADES=.

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

EXECUTABLES=	hydrology_verification_process				\
		interpolation_spike_calculate				\
		measurement_null_value_insert				\
		remove_chlorophyll_light_contamination			\
		measurement_block_delete				\
		output_measurement_easycharts				\
		output_measurement_googlecharts				\
		output_measurement_data					\
		extract_measurements					\
		output_flow_daily_values				\
		output_measurement_delta				\
		output_measurement_data_sets				\
		estimation_stage_reversal				\
		estimation_linear_interpolation_spike			\
		estimation_linear_interpolation_spike_output		\
		estimation_constant_offset				\
		estimation_constant_offset_output			\
		sensor_exposed_null					\
		sensor_exposed_null_output				\
		data_collection_frequency_list				\
		measurement_data_collection_frequency_list		\
		missing_measurement_report				\
		exceedance_curve					\
		calculate_evaporation					\
		exceedance_curve_output					\
		estimation_linear_interpolation_drift			\
		estimation_linear_interpolation_drift_output		\
		estimation_linear_interpolation_malfunction		\
		estimation_linear_interpolation_malfunction_output	\
		estimation_nearest_neighbor				\
		rain_accumulate_to_delta				\
		annual_comparison					\
		google_map_station_process				\
		output_measurements_rectangle				\
		output_current_vs_historical				\
		hydrology_validation_form				\
		post_hydrology_validation_form				\
		post_hydrology_measurements_rectangle			\
		generic_measurement_load				\
		post_generic_measurement_load				\
		post_current_vs_historical				\
		exclude_conductivity_if_salinity			\
		shef_download						\
		measurement2shef					\
		shef_upload						\
		sl2_upload						\
		sl2_shef_to_comma_delimited				\
		sl3_shef_to_comma_delimited				\
		shef_to_comma_delimited					\
		station_alias_to_station				\
		station_datatype_list					\
		shef_datatype_code_dvr					\
		shef_upload_datatype_convert				\
		conductivity2salinity					\
		hydrology_statistics_report				\
		hydrology_histogram					\
		on_the_hour_measurements				\
		adjust_measurement_time					\
		delete_measurements_day					\
		measurement_insert					\
		measurement_frequency_reject				\
		prepend_century_maybe					\
		load_turkey_point					\
		LT_upload						\
		load_cr10_single					\
		load_sfwmd_single_file					\
		load_sfwmd_breakpoint_data				\
		load_biscayne_ysi_data					\
		load_ysi_data						\
		load_cr300_data						\
		cr10_parse						\
		sfwmd_spreadsheet_parse					\
		sfwmd_breakpoint_parse					\
		biscayne_spreadsheet_parse				\
		spreadsheet_parse					\
		spreadsheet_parse_header				\
		measurement_adjust_time_to_sequence			\
		measurement_date_within_range				\
		alias_datatype						\
		get_aggregation_sum_yn					\
		apply_position_translation_to_record			\
		restore_measurement_backup				\
		unvalidate_measurements					\
		google_earth_station_operation				\
		validate_midnight					\
		merge_position_translation_table			\
		station_bad_date_time_fix_update_date			\
		view_help						\
		view_maps_figs						\
		datatype_overlay					\
		custom_chart						\
		output_measurement_sets_googlechart			\
		high_average_low_chart					\
		load_realdata						\
		hydrology_quick_measurement_form			\
		output_measurement_history				\
		post_hydrology_quick_measurement_form			\
		measurement_both_null_and_zero_time			\
		datatype_overlay_output					\
		daily_moving_statistic_output				\
		merged_datasets_output					\
		output_merged_waterquality				\
		water_budget_flow					\
		output_measurement_differences				\
		measurement_value_manipulation_history			\
		output_hydroperiod					\
		output_annual_comparison				\
		calculate_hydroperiod					\
		measurement_change_order				\
		calculate_oxygen_solubility				\
		merged_datasets_remove_all_but_first_heading.e		\
		get_date_time_slot_list					\
		measurement_convert_units.e				\
		hydrology_library_dvr.e					\
		annual_periods_output					\
		generate_data_collection_frequency			\
		output_hydroperiod_annually				\
		kml_extract

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
		${LIBRARY}/element.o					\
		${LIBRARY}/form.o					\
		${LIBRARY}/query.o 					\
		${LIBRARY}/frameset.o					\
		${SOURCE_EVERGLADES}/datatype.o				\
		${LIBRARY}/validation_level.o				\
		${LIBRARY}/dictionary_appaserver.o			\
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

SOURCE_LIBS= 	${SOURCE_EVERGLADES}/measurement_update_parameter.o	\
		${SOURCE_EVERGLADES}/shef_datatype_code.o		\
		${SOURCE_EVERGLADES}/shef_alias.o			\
		${SOURCE_EVERGLADES}/station.o				\
		${SOURCE_EVERGLADES}/hydrology_library.o		\
		${SOURCE_EVERGLADES}/measurement_validation.o		\
		${SOURCE_EVERGLADES}/station_datatype.o			\
		${SOURCE_EVERGLADES}/units.o				\
		${SOURCE_EVERGLADES}/merged.o				\
		${SOURCE_EVERGLADES}/hydrology.o			\
		${SOURCE_EVERGLADES}/measurement.o			\
		${SOURCE_EVERGLADES}/measurement_backup.o		\
		${SOURCE_EVERGLADES}/measurement_spike.o		\
		${SOURCE_EVERGLADES}/station_alias.o

all: 	${SOURCE_LIBS} ${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f *.o ${EXECUTABLES}					\
	${CGI_HOME}/post_hydrology_quick_measurement_form		\
	${CGI_HOME}/post_hydrology_measurements_rectangle		\
	${CGI_HOME}/post_current_vs_historical				\
	${CGI_HOME}/post_generic_measurement_load			\
	${CGI_HOME}/post_hydrology_validation_form			\
	sl3_upload

kml_extract: kml_extract.c						\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/google_earth.o				\
		${LIBRARY}/google_map.o					\
		google_earth_station.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		${LIBRARY}/google_earth.o				\
		${LIBRARY}/google_map.o					\
		google_earth_station.o					\
		kml_extract.c						\
		-o kml_extract

delete_measurements_day: delete_measurements_day.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		delete_measurements_day.c				\
		-o delete_measurements_day

prepend_century_maybe: prepend_century_maybe.c				\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		prepend_century_maybe.c					\
		-o prepend_century_maybe

load_realdata: load_realdata.c						\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		load_realdata.c						\
		-o load_realdata

view_help: view_help.c
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		view_help.c						\
		-o view_help

view_maps_figs: view_maps_figs.c
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		view_maps_figs.c					\
		-o view_maps_figs

restore_measurement_backup: restore_measurement_backup.c		\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		restore_measurement_backup.c				\
		-o restore_measurement_backup

unvalidate_measurements: unvalidate_measurements.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		unvalidate_measurements.c				\
		-o unvalidate_measurements

google_earth_station_operation: google_earth_station_operation.c	\
	${EXTERNAL_LIBS}						\
	${SOURCE_LIBS}							\
	${LIBRARY}/google_earth.o					\
	${LIBRARY}/google_map.o						\
	google_earth_station.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}						\
		${LIBRARY}/google_earth.o				\
		${LIBRARY}/google_map.o					\
		google_earth_station.o					\
		google_earth_station_operation.c			\
		-o google_earth_station_operation

get_aggregation_sum_yn: get_aggregation_sum_yn.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		get_aggregation_sum_yn.c				\
		-o get_aggregation_sum_yn

load_turkey_point: load_turkey_point.c					\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		load_turkey_point.c					\
		-o load_turkey_point

LT_upload: LT_upload.c							\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		LT_upload.c						\
		-o LT_upload

load_cr10_single: load_cr10_single.c					\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		load_cr10_single.c					\
		-o load_cr10_single

load_sfwmd_single_file: load_sfwmd_single_file.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		load_sfwmd_single_file.c				\
		-o load_sfwmd_single_file

load_sfwmd_breakpoint_data: load_sfwmd_breakpoint_data.c		\
	${EXTERNAL_LIBS} ${SOURCE_LIBS} sfwmd_station_datatype.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		load_sfwmd_breakpoint_data.c				\
		sfwmd_station_datatype.o				\
		-o load_sfwmd_breakpoint_data

cr10_parse: cr10_parse.c						\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		cr10_parse.c						\
		${SOURCE_LIBS}						\
		-o cr10_parse

spreadsheet_parse: spreadsheet_parse.c					\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		spreadsheet_parse.c					\
		${SOURCE_LIBS}						\
		-o spreadsheet_parse

biscayne_spreadsheet_parse: biscayne_spreadsheet_parse.c		\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		biscayne_spreadsheet_parse.c				\
		${SOURCE_LIBS}						\
		-o biscayne_spreadsheet_parse

sfwmd_spreadsheet_parse: sfwmd_spreadsheet_parse.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS} sfwmd_station_datatype.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		sfwmd_spreadsheet_parse.c				\
		sfwmd_station_datatype.o				\
		${SOURCE_LIBS}						\
		-o sfwmd_spreadsheet_parse

sfwmd_breakpoint_parse: sfwmd_breakpoint_parse.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS} sfwmd_station_datatype.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		sfwmd_breakpoint_parse.c				\
		sfwmd_station_datatype.o				\
		${SOURCE_LIBS}						\
		-o sfwmd_breakpoint_parse

alias_datatype: alias_datatype.c					\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		alias_datatype.c					\
		${SOURCE_LIBS}						\
		-o alias_datatype

spreadsheet_parse_header: spreadsheet_parse_header.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}					\
		spreadsheet.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		spreadsheet_parse_header.c				\
		${SOURCE_LIBS}						\
		spreadsheet.o						\
		-o spreadsheet_parse_header

measurement_adjust_time_to_sequence:					\
	measurement_adjust_time_to_sequence.c				\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		measurement_adjust_time_to_sequence.c			\
		${SOURCE_LIBS}						\
		-o measurement_adjust_time_to_sequence

measurement_date_within_range:						\
	measurement_date_within_range.c					\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		measurement_date_within_range.c				\
		${SOURCE_LIBS}						\
		-o measurement_date_within_range

load_cr300_data: load_cr300_data.c					\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		load_cr300_data.c					\
		${SOURCE_LIBS}						\
		-o load_cr300_data

load_ysi_data: load_ysi_data.c						\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		load_ysi_data.c						\
		${SOURCE_LIBS}						\
		-o load_ysi_data

load_biscayne_ysi_data: load_biscayne_ysi_data.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}						\
		load_biscayne_ysi_data.c				\
		-o load_biscayne_ysi_data

hydrology_verification_process: hydrology_verification_process.c	\
	${EXTERNAL_LIBS} ${SOURCE_LIBS} ${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		hydrology_verification_process.c			\
		-o hydrology_verification_process

interpolation_spike_calculate: interpolation_spike_calculate.c		\
	${EXTERNAL_LIBS} ${SOURCE_LIBS} ${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		interpolation_spike_calculate.c				\
		-o interpolation_spike_calculate

measurement_null_value_insert: measurement_null_value_insert.c		\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}					\
	${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		measurement_null_value_insert.c				\
		-lm -o measurement_null_value_insert

remove_chlorophyll_light_contamination:					\
	remove_chlorophyll_light_contamination.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}					\
	${STANDARD_LIBRARY}						\
	${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		remove_chlorophyll_light_contamination.c		\
		${LIBRARY}/grace.o					\
		-o remove_chlorophyll_light_contamination

measurement_block_delete: measurement_block_delete.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}					\
	${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		measurement_block_delete.c				\
		-o measurement_block_delete

output_measurement_data: output_measurement_data.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		output_measurement_data.c				\
		-o output_measurement_data

extract_measurements: extract_measurements.c				\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		extract_measurements.c					\
		-o extract_measurements

output_flow_daily_values: output_flow_daily_values.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		output_flow_daily_values.c				\
		-o output_flow_daily_values

output_measurement_delta: output_measurement_delta.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}					\
		${LIBRARY}/google_chart.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/google_chart.o				\
		output_measurement_delta.c				\
		-o output_measurement_delta

output_measurement_easycharts: output_measurement_easycharts.c		\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}					\
		${LIBRARY}/easycharts.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/easycharts.o					\
		output_measurement_easycharts.c				\
		-o output_measurement_easycharts

output_measurement_googlecharts: output_measurement_googlecharts.c	\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}					\
		${LIBRARY}/google_chart.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/google_chart.o				\
		output_measurement_googlecharts.c			\
		-o output_measurement_googlecharts

output_measurement_data_sets: output_measurement_data_sets.c		\
	${EXTERNAL_LIBS} ${SOURCE_LIBS} ${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		output_measurement_data_sets.c				\
		-o output_measurement_data_sets

estimation_linear_interpolation_spike: 					\
		estimation_linear_interpolation_spike.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		estimation_linear_interpolation_spike.c			\
		-o estimation_linear_interpolation_spike

estimation_stage_reversal: 						\
		estimation_stage_reversal.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		estimation_stage_reversal.c				\
		-o estimation_stage_reversal

estimation_linear_interpolation_spike_output: 				\
		estimation_linear_interpolation_spike_output.c		\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		estimation_linear_interpolation_spike_output.c		\
		${LIBRARY}/grace.o					\
		-o estimation_linear_interpolation_spike_output

custom_chart: custom_chart.c						\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		custom_chart.c						\
		${LIBRARY}/grace.o					\
		-o custom_chart

high_average_low_chart: high_average_low_chart.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		high_average_low_chart.c				\
		${LIBRARY}/grace.o					\
		-o high_average_low_chart

datatype_overlay_output: datatype_overlay_output.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/scatterplot.o				\
		${LIBRARY}/regression.o					\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		datatype_overlay_output.c				\
		${LIBRARY}/grace.o					\
		${LIBRARY}/scatterplot.o				\
		${LIBRARY}/regression.o					\
		-o datatype_overlay_output

daily_moving_statistic_output: daily_moving_statistic_output.c		\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		daily_moving_statistic_output.c				\
		${LIBRARY}/grace.o					\
		-o daily_moving_statistic_output

merged_datasets_output: merged_datasets_output.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/google_chart.o				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		merged_datasets_output.c				\
		${LIBRARY}/google_chart.o				\
		${LIBRARY}/grace.o					\
		-o merged_datasets_output

output_merged_waterquality: output_merged_waterquality.c		\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/google_chart.o				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		output_merged_waterquality.c				\
		${LIBRARY}/google_chart.o				\
		${LIBRARY}/grace.o					\
		-o output_merged_waterquality

water_budget_flow: water_budget_flow.c					\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/easycharts.o					\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		water_budget_flow.c					\
		${LIBRARY}/easycharts.o					\
		${LIBRARY}/grace.o					\
		-o water_budget_flow

measurement_value_manipulation_history:					\
		measurement_value_manipulation_history.c		\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/google_chart.o				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		measurement_value_manipulation_history.c		\
		${LIBRARY}/google_chart.o				\
		${LIBRARY}/grace.o					\
		-o measurement_value_manipulation_history

output_measurement_differences: output_measurement_differences.c	\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/google_chart.o				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		output_measurement_differences.c			\
		${LIBRARY}/google_chart.o				\
		${LIBRARY}/grace.o					\
		-o output_measurement_differences

output_hydroperiod: output_hydroperiod.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/easycharts.o					\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		output_hydroperiod.c					\
		${LIBRARY}/easycharts.o					\
		${LIBRARY}/grace.o					\
		-o output_hydroperiod

output_annual_comparison: output_annual_comparison.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/easycharts.o					\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		output_annual_comparison.c				\
		${LIBRARY}/easycharts.o					\
		${LIBRARY}/grace.o					\
		-o output_annual_comparison

output_hydroperiod_annually: output_hydroperiod_annually.c		\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		annual_hydroperiod.o					\
		${LIBRARY}/easycharts.o					\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		annual_hydroperiod.o					\
		${STANDARD_LIBRARY} 					\
		output_hydroperiod_annually.c				\
		${LIBRARY}/easycharts.o					\
		${LIBRARY}/grace.o					\
		-o output_hydroperiod_annually

output_measurement_sets_googlechart:					\
		output_measurement_sets_googlechart.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/google_chart.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/google_chart.o				\
		output_measurement_sets_googlechart.c			\
		-o output_measurement_sets_googlechart

annual_periods_output: annual_periods_output.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/period_wo_date.o				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		annual_periods_output.c					\
		${LIBRARY}/period_wo_date.o				\
		${LIBRARY}/grace.o					\
		-o annual_periods_output

get_date_time_slot_list: get_date_time_slot_list.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/date_aggregate.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/date_aggregate.o				\
		get_date_time_slot_list.c				\
		-o get_date_time_slot_list

measurement_convert_units.e: measurement_convert_units.c		\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/date_aggregate.o				\
		measurement_convert_units.c				\
		-o measurement_convert_units.e

hydrology_library_dvr.e: hydrology_library_dvr.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		hydrology_library_dvr.c					\
		-o hydrology_library_dvr.e

datatype_overlay: datatype_overlay.c					\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		datatype_overlay.c					\
		-o datatype_overlay

estimation_linear_interpolation_drift: 					\
		estimation_linear_interpolation_drift.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		estimation_linear_interpolation_drift.c			\
		-o estimation_linear_interpolation_drift

rain_accumulate_to_delta: 						\
		rain_accumulate_to_delta.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		rain_accumulate_to_delta.c				\
		-o rain_accumulate_to_delta

google_map_station_process: 						\
		google_map_station_process.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/google_map.o					\
		google_map_station.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/google_map.o					\
		google_map_station.o					\
		google_map_station_process.c				\
		-o google_map_station_process
		touch ../src_waterquality/google_map_station_process
		rm -f ../src_waterquality/google_map_station_process
		ln google_map_station_process ../src_waterquality

output_current_vs_historical: 						\
		output_current_vs_historical.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/google_map.o					\
		${LIBRARY}/google_chart.o				\
		google_map_station.o					\
		current_vs_historical.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/google_map.o					\
		${LIBRARY}/google_chart.o				\
		google_map_station.o					\
		current_vs_historical.o					\
		output_current_vs_historical.c				\
		-o output_current_vs_historical

output_measurements_rectangle: 						\
		output_measurements_rectangle.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/google_map.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/google_map.o					\
		output_measurements_rectangle.c				\
		-o output_measurements_rectangle

annual_comparison: 							\
		annual_comparison.c					\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		annual_comparison.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		annual_comparison.c					\
		-o annual_comparison

google_map_station_dvr: 						\
		google_map_station_dvr.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		google_map_station.o					\
		${LIBRARY}/google_map.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		google_map_station.o					\
		${LIBRARY}/google_map.o					\
		google_map_station_dvr.c				\
		-o google_map_station_dvr

estimation_constant_offset: 						\
		estimation_constant_offset.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		estimation_constant_offset.c				\
		-o estimation_constant_offset

sensor_exposed_null:		 					\
		sensor_exposed_null.c					\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		sensor_exposed_null.c					\
		-o sensor_exposed_null

exceedance_curve:  exceedance_curve.c					\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		exceedance_curve.c					\
		-o exceedance_curve

data_collection_frequency_list:  data_collection_frequency_list.c	\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		data_collection_frequency_list.c			\
		-o data_collection_frequency_list

measurement_data_collection_frequency_list:				\
		measurement_data_collection_frequency_list.c		\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		measurement_data_collection_frequency_list.c		\
		-o measurement_data_collection_frequency_list

missing_measurement_report:						\
		missing_measurement_report.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		missing_measurement_report.c				\
		-o missing_measurement_report

exceedance_curve_kevin:  exceedance_curve_kevin.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		exceedance_curve_kevin.c				\
		-o exceedance_curve_kevin

calculate_evaporation:  calculate_evaporation.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		calculate_evaporation.c					\
		-o calculate_evaporation

exceedance_curve_output: exceedance_curve_output.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		exceedance_curve_output.c				\
		${LIBRARY}/grace.o					\
		-o exceedance_curve_output

estimation_constant_offset_output: 					\
		estimation_constant_offset_output.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		estimation_constant_offset_output.c			\
		${LIBRARY}/grace.o					\
		-o estimation_constant_offset_output

sensor_exposed_null_output: 						\
		sensor_exposed_null_output.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		sensor_exposed_null_output.c				\
		${LIBRARY}/grace.o					\
		-o sensor_exposed_null_output

estimation_linear_interpolation_drift_output: 				\
		estimation_linear_interpolation_drift_output.c		\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		estimation_linear_interpolation_drift_output.c		\
		${LIBRARY}/grace.o					\
		-o estimation_linear_interpolation_drift_output

estimation_nearest_neighbor: 						\
		estimation_nearest_neighbor.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		estimation_nearest_neighbor.c				\
		-o estimation_nearest_neighbor

estimation_linear_interpolation_malfunction: 				\
		estimation_linear_interpolation_malfunction.c		\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		estimation_linear_interpolation_malfunction.c		\
		-o estimation_linear_interpolation_malfunction

estimation_linear_interpolation_malfunction_output: 			\
		estimation_linear_interpolation_malfunction_output.c	\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}				\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		estimation_linear_interpolation_malfunction_output.c	\
		${LIBRARY}/grace.o					\
		-o estimation_linear_interpolation_malfunction_output

hydrology_validation_form:						\
		hydrology_validation_form.c				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}						\
		hydrology_validation_form.c				\
		${LIBRARY}/grace.o					\
		-o hydrology_validation_form

exclude_conductivity_if_salinity:					\
		exclude_conductivity_if_salinity.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${STANDARD_LIBRARY} 					\
		${SOURCE_LIBS}	 					\
		exclude_conductivity_if_salinity.c			\
		-o exclude_conductivity_if_salinity

post_current_vs_historical:						\
		post_current_vs_historical.c				\
	${EXTERNAL_LIBS} ${SOURCE_LIBS} current_vs_historical.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		post_current_vs_historical.c				\
		current_vs_historical.o					\
		-o post_current_vs_historical
	touch ${CGI_HOME}/post_current_vs_historical
	rm -f ${CGI_HOME}/post_current_vs_historical
	ln ${APPASERVER_LINK_FLAGS} 					\
	${APPASERVER_HOME}/src_hydrology/post_current_vs_historical	\
	${CGI_HOME}

post_hydrology_validation_form:						\
		post_hydrology_validation_form.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		post_hydrology_validation_form.c			\
		-o post_hydrology_validation_form
	touch ${CGI_HOME}/post_hydrology_validation_form
	rm -f ${CGI_HOME}/post_hydrology_validation_form
	ln ${APPASERVER_LINK_FLAGS} 					\
	${APPASERVER_HOME}/src_hydrology/post_hydrology_validation_form \
	${CGI_HOME}

post_generic_measurement_load:						\
		post_generic_measurement_load.c				\
	${EXTERNAL_LIBS} ${SOURCE_LIBS} ${STANDARD_LIBRARY}		\
		${LIBRARY}/generic_load.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		${LIBRARY}/generic_load.o				\
		post_generic_measurement_load.c				\
		-o post_generic_measurement_load
	touch ${CGI_HOME}/post_generic_measurement_load
	rm -f ${CGI_HOME}/post_generic_measurement_load
	ln ${APPASERVER_LINK_FLAGS} 					\
	${APPASERVER_HOME}/src_hydrology/post_generic_measurement_load  \
	${CGI_HOME}

generic_measurement_load: generic_measurement_load.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		generic_measurement_load.c				\
		-o generic_measurement_load

shef_download: shef_download.c shef_alias.o				\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		shef_download.c						\
		-o shef_download

shef_datatype_code_dvr: shef_datatype_code_dvr.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		shef_datatype_code_dvr.c				\
		-o shef_datatype_code_dvr

shef_upload_datatype_convert: shef_upload_datatype_convert.c		\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		shef_upload_datatype_convert.c				\
		-o shef_upload_datatype_convert

measurement2shef: measurement2shef.c					\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		measurement2shef.c					\
		-o measurement2shef

measurement_update_parameter.o: measurement_update_parameter.c
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c measurement_update_parameter.c

shef_to_comma_delimited: shef_to_comma_delimited.c			\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		shef_to_comma_delimited.c				\
		-o shef_to_comma_delimited

sl2_shef_to_comma_delimited: sl2_shef_to_comma_delimited.c		\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		sl2_shef_to_comma_delimited.c				\
		-o sl2_shef_to_comma_delimited

sl3_shef_to_comma_delimited: sl3_shef_to_comma_delimited.c		\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		sl3_shef_to_comma_delimited.c				\
		-o sl3_shef_to_comma_delimited

station_alias_to_station: station_alias_to_station.c station_alias.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		station_alias_to_station.c				\
		-o station_alias_to_station

station_datatype_list: station_datatype_list.c
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		station_datatype_list.c					\
		-o station_datatype_list

shef_upload: shef_upload.c shef_datatype_code.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		shef_upload.c						\
		-o shef_upload

sl2_upload: sl2_upload.c shef_datatype_code.o				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		sl2_upload.c						\
		-o sl2_upload
		touch sl3_upload
		rm -f sl3_upload
		ln sl2_upload sl3_upload

sl3_upload: sl3_upload.c shef_datatype_code.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		sl3_upload.c						\
		-o sl3_upload

conductivity2salinity: conductivity2salinity.c 				\
	${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		conductivity2salinity.c					\
		-lm -o conductivity2salinity

merge_position_translation_table: 					\
	merge_position_translation_table.c ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		merge_position_translation_table.c			\
		-o merge_position_translation_table

apply_position_translation_to_record: 					\
	apply_position_translation_to_record.c ${SOURCE_LIBS}		\
	station_parse.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		apply_position_translation_to_record.c			\
		station_parse.o						\
		-o apply_position_translation_to_record

station_bad_date_time_fix_update_date: 					\
	station_bad_date_time_fix_update_date.c ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		station_bad_date_time_fix_update_date.c			\
		-o station_bad_date_time_fix_update_date

validate_midnight: validate_midnight.c ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		validate_midnight.c					\
		-o validate_midnight

measurement_insert: measurement_insert.c				\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		measurement_insert.c					\
		-o measurement_insert

measurement_frequency_reject: measurement_frequency_reject.c		\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		measurement_frequency_reject.c				\
		-o measurement_frequency_reject

hydrology_statistics_report: hydrology_statistics_report.c		\
		${LIBRARY}/query_attribute_statistics_list.o		\
		${LIBRARY}/date.o					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		hydrology_statistics_report.c				\
		${LIBRARY}/query_attribute_statistics_list.o		\
		-o hydrology_statistics_report

hydrology_histogram: hydrology_histogram.c				\
		${EXTERNAL_LIBS}					\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		hydrology_histogram.c					\
		${LIBRARY}/grace.o	 				\
		-o hydrology_histogram

on_the_hour_measurements: on_the_hour_measurements.c			\
		${EXTERNAL_LIBS}					\
		${LIBRARY}/grace.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		on_the_hour_measurements.c				\
		${LIBRARY}/grace.o	 				\
		-o on_the_hour_measurements

adjust_measurement_time: adjust_measurement_time.c
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		adjust_measurement_time.c				\
		-o adjust_measurement_time

hydrology_quick_measurement_form: hydrology_quick_measurement_form.c
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		hydrology_quick_measurement_form.c			\
		-o hydrology_quick_measurement_form

output_measurement_history: output_measurement_history.c
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		output_measurement_history.c				\
		-o output_measurement_history

post_hydrology_quick_measurement_form:					\
		${EXTERNAL_LIBS}					\
		post_hydrology_quick_measurement_form.c
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		post_hydrology_quick_measurement_form.c			\
		-o post_hydrology_quick_measurement_form
	touch ${CGI_HOME}/post_hydrology_quick_measurement_form
	rm -f ${CGI_HOME}/post_hydrology_quick_measurement_form
	ln ${APPASERVER_LINK_FLAGS} 					\
	${APPASERVER_HOME}/src_hydrology/post_hydrology_quick_measurement_form ${CGI_HOME}

post_hydrology_measurements_rectangle: post_hydrology_measurements_rectangle.c \
		${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		post_hydrology_measurements_rectangle.c			\
		-o post_hydrology_measurements_rectangle
	touch ${CGI_HOME}/post_hydrology_measurements_rectangle
	rm -f ${CGI_HOME}/post_hydrology_measurements_rectangle
	ln ${APPASERVER_LINK_FLAGS} 					\
	${APPASERVER_HOME}/src_hydrology/post_hydrology_measurements_rectangle ${CGI_HOME}

measurement_both_null_and_zero_time: measurement_both_null_and_zero_time.c
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-I${SOURCE_EVERGLADES}					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		measurement_both_null_and_zero_time.c			\
		-o measurement_both_null_and_zero_time

shef_datatype_code.o: shef_datatype_code.c shef_datatype_code.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c shef_datatype_code.c

station_alias.o: station_alias.c station_alias.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c station_alias.c

station_parse.o: station_parse.c station_parse.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c station_parse.c

station.o: station.c station.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c station.c

google_map_station.o: google_map_station.c google_map_station.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c google_map_station.c

google_earth_station.o: google_earth_station.c google_earth_station.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c google_earth_station.c

alt_station.o: alt_station.c alt_station.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c alt_station.c

shef_alias.o: shef_alias.c shef_alias.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c shef_alias.c

hydrology_library.o: hydrology_library.c hydrology_library.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c hydrology_library.c

measurement.o: measurement.c measurement.h				\
	${LIBRARY}/timlib.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c measurement.c

measurement_backup.o: measurement_backup.c measurement_backup.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c measurement_backup.c

measurement_spike.o: measurement_spike.c measurement_spike.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c measurement_spike.c

measurement_validation.o: measurement_validation.c measurement_validation.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c measurement_validation.c

station_datatype.o: station_datatype.c station_datatype.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c station_datatype.c

units.o: units.c units.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c units.c

hydrology.o: hydrology.c hydrology.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c hydrology.c

datatype.o: datatype.c datatype.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c datatype.c

sfwmd_station_datatype.o: sfwmd_station_datatype.c			\
	sfwmd_station_datatype.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c sfwmd_station_datatype.c

annual_hydroperiod.o: annual_hydroperiod.c annual_hydroperiod.h		\
		alt_station.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c annual_hydroperiod.c

current_vs_historical.o: current_vs_historical.c			\
		current_vs_historical.h ${LIBRARY}/date.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c current_vs_historical.c

merged_datasets_remove_all_but_first_heading.e:				\
	merged_datasets_remove_all_but_first_heading.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		merged_datasets_remove_all_but_first_heading.c		\
		-o merged_datasets_remove_all_but_first_heading.e

generate_data_collection_frequency:					\
	generate_data_collection_frequency.c ${STANDARD_LIBRARY}	\
	${EXTERNAL_LIBS} ${SOURCE_LIBS}
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY} 					\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}	 					\
		generate_data_collection_frequency.c			\
		-o generate_data_collection_frequency

calculate_hydroperiod:  calculate_hydroperiod.c				\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		calculate_hydroperiod.c					\
		-o calculate_hydroperiod

measurement_change_order:  measurement_change_order.c			\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		measurement_change_order.c				\
		-o measurement_change_order

calculate_oxygen_solubility:  calculate_oxygen_solubility.c		\
		${EXTERNAL_LIBS} ${SOURCE_LIBS}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${EXTERNAL_LIBS}					\
		${SOURCE_LIBS}						\
		${STANDARD_LIBRARY} 					\
		calculate_oxygen_solubility.c				\
		-o calculate_oxygen_solubility -lm

merged.o: merged.c merged.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c merged.c

spreadsheet.o: spreadsheet.c spreadsheet.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c spreadsheet.c


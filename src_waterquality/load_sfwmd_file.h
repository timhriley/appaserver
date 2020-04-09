/* load_sfwmd_file.h */
/* ----------------- */

#ifndef LOAD_SFWMD_FILE_H
#define LOAD_SFWMD_FILE_H

/* Constants */
/* --------- */
#define MATRIX_ANALYTE_FREE_WATER		"analyte_free_water"
#define EQUIPMENT_BLANKS_CHECK_PIECE		5
#define EQUIPMENT_BLANKS_CODE			"EB"
#define STDERR_COUNT				1000
#define BELOW_DETECTION_LIMIT			"below_detection_limit"
#define NULL_CONCENTRATION			"null_concentration"
#define BELOW_DETECTION_LIMIT_CODE		'T'

#define PROJECT_CODE_HEADING			"Project Code"
#define STATION_HEADING				"Station ID"
#define SAMPLE_ID_HEADING			"Sample ID"
#define COLLECTION_DATE_TIME1_HEADING		"Collection_Date"
#define COLLECTION_DATE_TIME2_HEADING		"Collect Date"
#define COLLECTION_DEPTH_METERS_HEADING		"Depth"
#define COLLECTION_DEPTH_UNIT_HEADING		"Depth Unit"
#define MATRIX_CODE_HEADING			"Matrix"
#define LAB_TEST_CODE_HEADING			"Test Number"
#define PARAMETER_HEADING			"Test Name"
#define LAB_STORET_CODE_HEADING			"Storet Code"
#define EXCEPTION_CODE_HEADING			"Remark Code"
#define CONCENTRATION_HEADING			"Value"
#define MINIMUM_DETECTION_LIMIT_HEADING		"MDL"
#define UNITS_HEADING				"Units"
#define LAB_LIMS_ID_HEADING			"LIMS Number"
#define FLOW_NO_FLOW_CODE_HEADING		"Sample Type"
#define UP_DOWN_STREAM_CODE_HEADING		"Up/Down Stream"
#define SAMPLE_COMMENTS_HEADING			"Sample Comment"
#define RESULTS_COMMENTS_HEADING		"Result Comment"

#define INSERT_COLLECTION		"station,collection_date,collection_time,collection_depth_meters,up_down_stream,flow_no_flow,sample_id,lab_lims_id,sample_comments,results_comments"
#define INSERT_RESULTS 			"station,collection_date,collection_time,parameter,units,matrix,concentration,minimum_detection_limit"
#define INSERT_RESULTS_EXCEPTION	"station,collection_date,collection_time,parameter,units,exception"
#define INSERT_STATION 			"station"
#define INSERT_STATION_PARAMETER	"station,parameter,units"
#define INSERT_PARAMETER_UNIT		"parameter,units"
#define INSERT_WATER_PROJECT_STATION 	"project_name,station"

/* Prototypes */
/* ---------- */
char *get_equipment_blank_matrix(
				char *input_string );

LIST *get_parameter_list(	char *application_name );

LIST *get_units_list(		char *application_name );

LIST *get_exception_name_list(
				char *exception_code_multiple,
				FILE *error_file,
				int line_number,
				char *application_name );

boolean get_heading_piece_string(
				char *destination,
				DICTIONARY *heading_piece_dictionary,
				char *heading_string,
				char *input_buffer );

DICTIONARY *get_heading_piece_dictionary(
				char **missing_heading,
				char *heading_line );

boolean units_piece(		char *units,
				char *input_string,
				DICTIONARY *heading_piece_dictionary );

DICTIONARY *get_parameter_dictionary(
				char *application_name );

DICTIONARY *get_units_dictionary(
				char *application_name );

char *get_aliased_parameter(	char *application_name,
				char *parameter_string );

char *get_aliased_units(	char *application_name,
				char *units );

void close_pipes(
				FILE *results_insert_pipe,
				FILE *results_exception_insert_pipe,
				FILE *station_parameter_insert_pipe,
				FILE *parameter_unit_insert_pipe,
				FILE *station_insert_pipe,
				FILE *water_project_station_insert_pipe,
				FILE *collection_insert_pipe,
				FILE *table_output_pipe,
				int execute_yn );

void delete_waterquality(	char *input_filename );

int load_sfwmd_file(
				char *application_name,
				char *input_filename,
				boolean withhtml,
				char execute_yn );

char *get_flow_no_flow(		char *application_name,
				char *code );

char *get_matrix(		char *application_name,
				char *matrix_code,
				char *equipment_blank_matrix );

char *get_up_down_stream(	char *application_name,
				char *code );

char *get_exception(		char *application_name,
				char *exception_code );

void insert_results_exception(
				 FILE *results_exception_insert_pipe,
				 char *station,
				 char *collection_date_international,
				 char *collection_time_without_colon,
				 char *aliased_parameter,
				 char *units,
				 char *exception );

void insert_results_exception_code(
				 FILE *results_exception_insert_pipe,
				 FILE *error_file,
				 char *application_name,
				 char *station,
				 char *collection_date_international,
				 char *collection_time_without_colon,
				 char *aliased_parameter,
				 char *units,
				 char *exception_code,
				 int line_number );

void insert_results_exception_code_multiple(
				 FILE *results_exception_insert_pipe,
				 FILE *error_file,
				 char *application_name,
				 char *station,
				 char *collection_date_international,
				 char *collection_time_without_colon,
				 char *aliased_parameter,
				 char *units,
				 char *exception_code_multiple,
				 char *concentration,
				 int line_number );

char *subtract_colon_from_hrmi(	 char *hrmi );

#endif

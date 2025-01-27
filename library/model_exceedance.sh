:
if [ "$#" -ne 9 ]
then
	echo "Usage: $0 application login_name model version alternative_run begin_date end_date station datatype" 1>&2
	exit 0
fi

application=$1
login_name=$2
model=$3
version=$4
alternative_run=$5
begin_date=$6
end_date=$7
station=$8
datatype=$9

generic_output_exceedance $application $login_name output_model_results y "qquery_from_output_model_results_0=generic_exceedance_table&qquery_output_model_results_0=generic_exceedance_table&qquery_relation_operator_days_to_average_0=equals&qquery_login_name_0=tim&qquery_station_0=${station}&qquery_relation_operator_station_0=equals&qquery_relation_operator_units|relation_operator_units_converted_0=equals&qquery_relation_operator_units_converted_0=equals&qquery_relation_operator_units_0=equals&qquery_relation_operator_begin_date_0=equals&qquery_from_begin_date_0=${begin_date}&qquery_relation_operator_end_date_0=equals&qquery_from_end_date_0=${end_date}&qquery_begin_date_0=${begin_date}&qquery_end_date_0=${end_date}&qquery_relation_operator_aggregate_level_0=equals&qquery_version_0=${version}&qquery_relation_operator_version_0=equals&qquery_relation_operator_email_address_0=equals&qquery_model_0=${model}&qquery_relation_operator_model_0=equals&qquery_process_0=generic_exceedance_table&qquery_relation_operator_aggregate_statistic_0=equals&qquery_station|datatype_0=${station}|${datatype}&qquery_relation_operator_station|relation_operator_datatype_0=equals&qquery_datatype_0=${datatype}&qquery_relation_operator_datatype_0=equals&qquery_model|version|alternative_run_0=${model}|${version}|${alternative_run}&qquery_relation_operator_model|relation_operator_version|relation_operator_alternative_run_0=equals&qquery_alternative_run_0=${alternative_run}&qquery_relation_operator_alternative_run_0=equals&qquery_process=generic_exceedance_table"

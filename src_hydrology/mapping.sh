:
# src_hydrology/mapping.sh
# -----------------------------------
# This is a wrapper around mapping.py
# -----------------------------------

echo "$0" "$*" 1>&2

if [ "$#" -ne 7 ]
then
	echo "Usage: $0 process begin_date end_date station_list datatype_list waterquality_station_parameter_units_list omit_html_yn" 1>&2
	exit 1
fi

process=$1
begin_date=$2
end_date=$3
station_list=$4
datatype_list=$5
waterquality_station_parameter_units_list=$6
omit_html_yn=$7

process_title=`echo $process | format_initial_capital.e`

if [ "$omit_html_yn" = "y" ]
then
	tee_process="tee /dev/tty"
else
	tee_process="cat"
fi

document_root=$(get_document_root.e)
output_directory="$document_root/appaserver/$DATABASE/data"
output_file="$output_directory/mapping_${begin_date}_${end_date}"

heading="Station,Datatype,Date,Latitude,Longitude,Value"

# Hydrology
# ---------
select="station.station,datatype,min(measurement_date),lat_nad83,long_nad83,avg(measurement_value)"

group_by="station.station,datatype,lat_nad83,long_nad83"

station_datatype_where=`station_datatype_where.sh "$station_list" "$datatype_list" | sed 's/station =/station.station =/'`

# Waterquality
# ------------
waterquality_select="station.station,concat( parameter, '|', units ),min(collection_date),latitude,longitude,avg(concentration)"

waterquality_group_by="station.station,concat( parameter, '|', units ),latitude,longitude"

station_parameter_where=`waterquality_station_parameter_where.sh "$waterquality_station_parameter_units_list" | sed 's/station =/station.station =/'`

(
echo "$heading"
(
echo "	select $select
	from station, measurement
	where station.station = measurement.station
	  and $station_datatype_where
	  and measurement_date between '${begin_date}' and '${end_date}'
	GROUP BY $group_by;"						|
$tee_process								|
sql.e ',' hydrology 							|
cat;
echo "	select $waterquality_select
	from station, results
	where station.station = results.station
	  and $station_parameter_where
	  and collection_date between '${begin_date}' and '${end_date}'
	GROUP BY $waterquality_group_by;"				|
$tee_process								|
sql.e ',' waterquality 							|
cat
)									|
sort
)									|
cat > $output_file

return_file=`mapping.py $output_file`

prompt_file=$(echo "$return_file" | sed "s|`get_document_root.e`||")

if [ "$omit_html_yn" != "y" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$DATABASE/style.css></head>"
	echo "<body><h1>$process_title</h1>"
	echo "<a href=$prompt_file target=_new>Press to view file.</a>"
	echo "</body></html>"
else
	echo "Created: $return_file"
	echo "Input:"
	cat $output_file
fi

exit 0

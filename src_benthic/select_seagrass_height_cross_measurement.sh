:
echo "$0 $*" 1>&2

if [ "$#" != 7 ]
then
	echo "Usage: $0 application begin_date end_date vegetation_name family genus species" 1>&2
	exit 1
fi

application=$1
begin_date=$2
end_date=$3
vegetation_name=$4
family_name=$5
genus_name=$6
species_name=$7

quad_vegetation=$(get_table_name $application quad_vegetation)
species_measurement=$(get_table_name $application species_measurement)

select="$species_measurement.anchor_date,$species_measurement.anchor_time,vegetation_height_cm,length_millimeters"
where="$species_measurement.anchor_date between '$begin_date' and '$end_date' and vegetation_name = '$vegetation_name' and family_name = '$family_name' and genus_name = '$genus_name' and species_name = '$species_name' and $species_measurement.anchor_date = $quad_vegetation.anchor_date and $species_measurement.anchor_time = $quad_vegetation.anchor_time and vegetation_height_cm is not null"
order="vegetation_height_cm"

echo "select $select from $quad_vegetation,$species_measurement where $where order by $order;" | sql.e

exit 0

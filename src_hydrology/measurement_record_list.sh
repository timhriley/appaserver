#!/bin/sh
# src_hydrology/measurement_record_list.sh
# ------------------------------------------------------
# There is a trick to calculate the aggregate value
# towards the end of each period instead
# of the beginning: order by descending before
# aggregation, then sort ascending.
#
# Note: the begin and end dates are a single parameter
# separated by a colon to account for antiquated shells'
# 9 parameter limit. Also, aggregate_statistic and
# accumulate_yn are colon separated.
# ------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 8 ]
then
	echo "Usage: $0 application station begin_date:end_date aggregate_level validation_level aggregate_statistic:accumulate_yn append_low_high_yn units_converted [datatype]" 1>&2
	exit 1
fi

application=$1
station=$2
begin_colon_end_date=$3
aggregate_level=$4
validation_level=$5
aggregate_statistic_accumulate_yn=$6
append_low_high_yn=$7
units_converted=$8
datatype=$9

date_piece_offset=2
time_piece_offset=3
value_piece_offset=4
station_piece_offset=0
order_by_clause=""

begin_date=`echo $begin_colon_end_date | piece.e ':' 0`
end_date=`echo $begin_colon_end_date | piece.e ':' 1`

aggregate_statistic=`echo $aggregate_statistic_accumulate_yn | piece.e ':' 0`
accumulate_yn=`echo $aggregate_statistic_accumulate_yn | piece.e ':' 1`

if [ "$#" -eq 9 -a "$datatype" != "datatype" ]
then
	isolate_datatype_process="grep ^${datatype}$"

	if [ "$units_converted" != ""					\
	-a   "$units_converted" != "units_converted" ]
	then
		where="datatype = '$datatype'"
		units=`get_folder_data	application=$application	\
					folder=datatype			\
					select=units			\
					where="$where"`

		units_converted_process="measurement_convert_units.e	\
						\"$units\"		\
						\"$units_converted\"	\
						$value_piece_offset	\
						\"^\"			\
						$station_piece_offset"
	else
		units_converted_process="cat"
	fi
else
	isolate_datatype_process="cat"
	units_converted_process="cat"
fi

if [ "$validation_level" = "provisional" ]
then
	validation_where="last_validation_date is null"
elif [ "$validation_level" = "validated" ]
then
	validation_where="last_validation_date is not null"
else
	validation_where="1 = 1"
fi

if [ "$accumulate_yn" != "y" ]
then
	before_accumulate_process="cat"
	accumulate_process="cat"
else
	before_accumulate_process="sort -f"
	accumulate_process="accumulate.e		   	\
				$value_piece_offset		\
				^		   		\
				replace"
fi

measurement=`get_table_name $application measurement`

if [ "$aggregate_statistic" = "" ]
then
	aggregate_results=0
else
	aggregate_results=`strcmp.e $aggregate_statistic value`
fi

for datatype in `datatype_list.sh $application $station			  |\
		 $isolate_datatype_process`
do

	if [ $aggregate_results -eq 0 ]
	then
		aggregation_sum_yn=`get_aggregation_sum_yn		   \
					$application			   \
					$datatype`

		if [ "$aggregation_sum_yn" = "y" ]
		then
			aggregate_statistic=sum
		else
			aggregate_statistic=average
		fi
	fi

	if [ "$aggregate_level" = "real_time" ]
	then
		aggregate_process="cat"
	else
		aggregate_process="real_time2aggregate_value.e		   \
						$aggregate_statistic	   \
						$date_piece_offset	   \
						$time_piece_offset 	   \
						$value_piece_offset	   \
						^		   	   \
						$aggregate_level	   \
						$append_low_high_yn	   \
						$end_date"

		if [ "$aggregate_level" != "daily" ]
		then
			order_by_clause="order by measurement_date desc, measurement_time desc"
		fi
	fi

	echo "	select	station,				\
			datatype,				\
			measurement_date,			\
			measurement_time,			\
			measurement_value 			\
      		from $measurement 				\
      		where station = '$station'			\
        	  and datatype = '$datatype'			\
		  and measurement_date >= '$begin_date'		\
		  and measurement_date <= '$end_date'		\
		  and $validation_where				\
		$order_by_clause;"				|
	sql_quick.e '^'						|
	$units_converted_process				|
	$aggregate_process					|
	$before_accumulate_process				|
	$accumulate_process					|
	tr '^' '|'						|
	sort -f
done


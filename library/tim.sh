echo "\
select measurement_date, measurement_time, measurement_value from measurement where station = 'BA' and measurement_date between '2000-06-01' and '2000-06-30' and datatype  = 'rain' and measurement_value is not null and measurement_value > 0;" |
mysql.sh

:
days_count=3
value_piece=1
delimiter='|'
aggregate_statistic=average
begin_date=1999-06-15

moving_statistic.e $days_count $value_piece "$delimiter" $aggregate_statistic $begin_date << finished
1999-06-12|1
1999-06-13|2
1999-06-14|3
1999-06-15|4
1999-06-16|3
1999-06-17|2
1999-06-18|1
finished

# $APPASERVER_HOME/utility/makefile
# ---------------------------------

CC=gcc
#WARN=-Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
#WARN=-Wall -Wextra -Wtype-limits
WARN=-Wall -Wextra -Wunused-parameter
#CFLAGS=-g
CFLAGS=-O3 -s ${WARN}

LIBRARY=${APPASERVER_HOME}/library
PREDICTIVE=${APPASERVER_HOME}/src_predictive

CORE_LIBRARY=								\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/application.o				\
		${LIBRARY}/String.o					\
		${LIBRARY}/sed.o					\
		${LIBRARY}/piece.o					\
		${LIBRARY}/float.o					\
		${LIBRARY}/dictionary.o					\
		${LIBRARY}/column.o					\
		${LIBRARY}/hash_table.o					\
		${LIBRARY}/security.o					\
		${LIBRARY}/spool.o					\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/application_log.o				\
		${LIBRARY}/application_constant.o			\
		${LIBRARY}/appaserver_parameter.o			\
		${LIBRARY}/environ.o					\
		${LIBRARY}/date.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/julian.o					\
		${LIBRARY}/javascript.o					\
		${LIBRARY}/list.o

APPASERVER_LIBRARY=							\
		${CORE_LIBRARY}						\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/security_entity.o				\
		${LIBRARY}/entity.o					\
		${LIBRARY}/process_generic.o				\
		${LIBRARY}/output_medium.o				\
		${LIBRARY}/units.o					\
		${LIBRARY}/accumulate_boolean.o				\
		${LIBRARY}/basename.o					\
		${LIBRARY}/session.o					\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/folder_attribute.o				\
		${LIBRARY}/process_parameter.o				\
		${LIBRARY}/appaserver_link.o				\
		${LIBRARY}/widget.o					\
		${LIBRARY}/file.o					\
		${LIBRARY}/button.o					\
		${LIBRARY}/process.o					\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/role_appaserver_user.o			\
		${LIBRARY}/operation.o					\
		${LIBRARY}/role_folder.o				\
		${LIBRARY}/folder_operation.o				\
		${LIBRARY}/form.o					\
		${LIBRARY}/dictionary_separate.o			\
		${LIBRARY}/query.o					\
		${LIBRARY}/query_isa.o					\
		${LIBRARY}/insert.o					\
		${LIBRARY}/update.o					\
		${LIBRARY}/folder_row_level_restriction.o		\
		${LIBRARY}/relation.o					\
		${LIBRARY}/relation_mto1.o				\
		${LIBRARY}/relation_one2m.o				\
		${LIBRARY}/relation_join.o				\
		${LIBRARY}/relation_copy.o				\
		${LIBRARY}/aggregate_level.o				\
		${LIBRARY}/aggregate_statistic.o			\
		${LIBRARY}/choose_role.o				\
		${LIBRARY}/prompt_lookup.o				\
		${LIBRARY}/form_prompt_lookup.o				\
		${LIBRARY}/prompt_process.o				\
		${LIBRARY}/form_prompt_process.o			\
		${LIBRARY}/post_prompt_process.o			\
		${LIBRARY}/post_choose_process.o			\
		${LIBRARY}/form_choose_role.o				\
		${LIBRARY}/form_prompt_attribute.o			\
		${LIBRARY}/post_choose_role.o				\
		${LIBRARY}/recall.o					\
		${LIBRARY}/radio.o					\
		${LIBRARY}/pair_one2m.o					\
		${LIBRARY}/row_security.o				\
		${LIBRARY}/row_security_role_update.o			\
		${LIBRARY}/post_dictionary.o				\
		${LIBRARY}/folder_menu.o				\
		${LIBRARY}/menu.o					\
		${LIBRARY}/ajax.o					\
		${LIBRARY}/sql.o					\
		${LIBRARY}/execute_system_string.o			\
		${LIBRARY}/post_choose_folder.o				\
		${LIBRARY}/drillthru.o					\
		${LIBRARY}/form_drillthru.o

all: 	insert_statement.e						\
	format_sql_output.e						\
	attribute_format.e						\
	consume_socket_message.e					\
	produce_socket_message.e					\
	times_ten_to_power.e						\
	cdmenu.e							\
	piece.e								\
	nl.e								\
	nl_ff.e								\
	column.e							\
	minutes_between.e						\
	minutes_ticker.e						\
	piece_quoted.e							\
	piece_quote_comma.e						\
	merge_n_lines.e findgrep.e stat.e				\
	escape_character.e						\
	escape_security.e						\
	unescape_single_quotes.e					\
	unescape_character.e						\
	binary_count.e							\
	decimal_count.e							\
	location_convert.e						\
	html_encode_quote_and_ampersand.e				\
	date_grandfather_father_son.e					\
	trim_until_character.e						\
	column_grep.e							\
	tee_process.e							\
	group.e								\
	add_line.e							\
	update_statement.e						\
	label.e								\
	group_sum.e							\
	group_trim.e							\
	ufw_whitelist.e							\
	group_date_time.e						\
	delimiter2padded_columns.e					\
	trim.e								\
	count.e								\
	table_count.e							\
	random.e							\
	start_time.e							\
	stop_time.e							\
	basename.e							\
	trim_length.e							\
	trim_right.e							\
	string_initial_capital.e					\
	reverse.e							\
	reverse_lines.e							\
	days_passed.e format_stamp.e					\
	date_subtract.e							\
	format_stamp_undo.e						\
	menu.e								\
	queue_top_bottom_lines.e					\
	xml_separate.e							\
	join_multi_line_delimited.e					\
	pad_leading_numbers.e						\
	queue_file.e							\
	statistics.e							\
	toupper.e							\
	tolower.e							\
	extension.e							\
	statistics_weighted.e						\
	moving_statistic.e						\
	average.e							\
	complete_arrangement.e						\
	colapse_repeating.e						\
	trim_character.e 						\
	display_character.e						\
	piece_replace_date.e						\
	date_convert.e							\
	date_convert_anywhere.e						\
	date_convert_login_format.e					\
	single_quotes_around.e 						\
	double_quote_comma_delimited.e					\
	full_year.e xargs.e 						\
	column2pipe.e							\
	make_key.e 							\
	joinlines.e 							\
	join.e	 							\
	hashsearch.e 							\
	fiforead.e 							\
	fifowrite.e 							\
	remove_cr.e 							\
	append_escaped_backslash_cr.e					\
	joinfiles.e							\
	count_alpha.e							\
	isolate_line.e							\
	delete_statement.e						\
	remove_duplicate_rows.e						\
	trim_right_until_character.e					\
	append_radio_html.e						\
	merge_n_groups.e 						\
	commas_in_long.e						\
	each_week.e							\
	statistics_on_group.e						\
	range.e								\
	parse_post.e							\
	decode_html_post.e						\
	dayofyear2date.e						\
	html_wrapper.e							\
	date2dayofyear.e						\
	date2dayofweek.e						\
	suffix_extract.e						\
	ora_date_to.e							\
	piece_inverse.e							\
	oracle_date_to_mysql.e						\
	mysql_date2ora_date.e						\
	piece_insert.e							\
	piece_commas_double.e						\
	date2julian.e							\
	julian2date.e							\
	separate_children_from_parents.e				\
	prepend_zeros_to_piece.e					\
	piece_swap.e							\
	time_to_percent_of_day.e					\
	nothing.e							\
	skip_lines.e							\
	piece_shift_left.e						\
	piece_shift_right.e						\
	html_paragraph_wrapper.e					\
	piece_replace.e							\
	yesterday.e							\
	julian_time_list.e						\
	date.e								\
	piece_grep.e							\
	date_ticker.e							\
	findbig.e							\
	padspace.e							\
	insert_string.e							\
	date_add_days.e							\
	julian_add_hours.e						\
	julian_time.e							\
	findtoday.e							\
	sql_select.e							\
	unix2dos							\
	dos2unix							\
	build_date_where						\
	cal2linear.e							\
	round2five_minutes						\
	real_time2aggregate_value.e					\
	exceed_standard_deviation.e					\
	date_min_max.e commit.e round_integer.e				\
	filter_anomaly.e						\
	strncmp.e							\
	strcmp.e							\
	strlen.e							\
	character_count.e						\
	spaces2tab.e							\
	regression.e							\
	delta_values.e							\
	percent_change.e						\
	covariance.e							\
	head_sentences.e						\
	toupper_to_delimiter.e						\
	shuffle_lines.e							\
	html_table.e							\
	piece_arithmetic.e						\
	piece_sum.e							\
	cut.e								\
	restore_database.e						\
	piece_exceedance.e						\
	highest_prime_number.e						\
	format_initial_capital.e					\
	increment.e							\
	sum.e								\
	accumulate.e							\
	balance_accumulate_check.e					\
	keys_match_sum.e						\
	pad_missing_times.e						\
	time_ticker.e							\
	random_stream.e							\
	pivot_table.e							\
	search_replace_character_limited.e				\
	search_replace_string.e						\
	mysql_transform.e						\
	filename_date_older.e						\
	in_clause.e							\
	real_time2annual_period.e					\
	mail_if_output.e						\
	time_hhmm2hhcolonmm.e						\
	label_dictionary.e						\
	measurement_convert_units.e					\
	bracket_line.e							\
	sort_delimited_numbers.e					\
	mysqldump_appaserver

touch:
	/usr/bin/touch *.c

clean:
	rm -f *.[oe]							\
	build_date_where						\
	cal2linear							\
	html_table							\
	findgrep							\
	produce_socket_message						\
	consume_socket_message						\
	date2dayofyear							\
	dayofyear2date							\
	decode_html_post						\
	dos2unix							\
	html_paragraph_wrapper						\
	join_multi_line_delimited					\
	html_wrapper							\
	insert_string							\
	nothing								\
	parse_post							\
	piece								\
	piece_grep							\
	piece_replace							\
	piece_quote_comma						\
	round2five_minutes						\
	single_quotes_around						\
	double_quote_comma_delimited					\
	statistics							\
	gethostbyname							\
	queue_lines							\
	findbig								\
	findtoday							\
	menu								\
	unix2dos							\
	delete_statement						\
	update_statement						\
	insert_statement						\
	descriptive_statistics						\
	mysqldump_appaserver

fiforead.e: fiforead.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		fiforead.c 						\
		-o fiforead.e

fifowrite.e: fifowrite.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		fifowrite.c 						\
		-o fifowrite.e

google_timechart.e: google_timechart.c					\
	${LIBRARY}/google.o						\
	${LIBRARY}/google_chart.o
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		google_timechart.c 					\
		${LIBRARY}/google.o					\
		${LIBRARY}/google_chart.o				\
		-o google_timechart.e

grace_histogram.e: grace_histogram.c					\
	${LIBRARY}/grace.o ${LIBRARY}/histogram.o
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		grace_histogram.c 					\
		${LIBRARY}/grace.o					\
		${LIBRARY}/histogram.o					\
		-o grace_histogram.e

grace_scatterplot.e: grace_scatterplot.c				\
	${LIBRARY}/regression.o ${LIBRARY}/grace.o
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		grace_scatterplot.c 					\
		${LIBRARY}/regression.o					\
		${LIBRARY}/grace.o					\
		-o grace_scatterplot.e

covariance.e: covariance.c ${LIBRARY}/covariance.o
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/covariance.o					\
		covariance.c 						\
		-o covariance.e

sql_select.e: sql_select.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		sql_select.c 						\
		-o sql_select.e

cal2linear.e: cal2linear.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		cal2linear.c						\
		-o cal2linear.e
	touch cal2linear
	rm cal2linear
	ln cal2linear.e cal2linear

build_date_where: build_date_where.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		build_date_where.c					\
		-o build_date_where

unix2dos: unix2dos.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		unix2dos.c						\
		-o unix2dos

percent_change.e: percent_change.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		percent_change.c					\
		-o percent_change.e

spaces2tab.e: spaces2tab.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		spaces2tab.c						\
		-o spaces2tab.e

delta_values.e: delta_values.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		delta_values.c						\
		-o delta_values.e

regression.e: regression.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		regression.c						\
		-o regression.e -lm

times_ten_to_power.e: times_ten_to_power.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		times_ten_to_power.c					\
		-o times_ten_to_power.e

dos2unix: dos2unix.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		dos2unix.c						\
		-o dos2unix

suffix_extract.e: suffix_extract.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		suffix_extract.c 					\
		-o suffix_extract.e

prepend_zeros_to_piece.e: prepend_zeros_to_piece.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		prepend_zeros_to_piece.c				\
		-o prepend_zeros_to_piece.e

horizontal_menu_yn.e: horizontal_menu_yn.c				\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/execute_system_string.o
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		horizontal_menu_yn.c					\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/execute_system_string.o			\
		-o horizontal_menu_yn.e

separate_children_from_parents.e: separate_children_from_parents.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		separate_children_from_parents.c			\
		-o separate_children_from_parents.e

html_wrapper.e: html_wrapper.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		html_wrapper.c 						\
		-o html_wrapper.e
	touch html_wrapper
	rm html_wrapper
	ln html_wrapper.e html_wrapper

html_paragraph_wrapper.e: html_paragraph_wrapper.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		html_paragraph_wrapper.c				\
		-o html_paragraph_wrapper.e
	touch html_paragraph_wrapper
	rm html_paragraph_wrapper
	ln html_paragraph_wrapper.e html_paragraph_wrapper

dayofyear2date.e: dayofyear2date.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		dayofyear2date.c 					\
		-o dayofyear2date.e
	touch dayofyear2date
	rm dayofyear2date
	ln dayofyear2date.e dayofyear2date

database_management_system.e: database_management_system.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		database_management_system.c 				\
		-o database_management_system.e

date2dayofweek.e: date2dayofweek.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		date2dayofweek.c 					\
		-o date2dayofweek.e

date2dayofyear.e: date2dayofyear.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		date2dayofyear.c 					\
		-o date2dayofyear.e
	touch date2dayofyear
	rm date2dayofyear
	ln date2dayofyear.e date2dayofyear

decode_html_post.e: decode_html_post.c ${LIBRARY}/decode_html_post.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/decode_html_post.o				\
		decode_html_post.c 					\
		-o decode_html_post.e
	touch decode_html_post
	rm decode_html_post
	ln decode_html_post.e decode_html_post

time_to_percent_of_day.e: time_to_percent_of_day.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		time_to_percent_of_day.c 				\
		-o time_to_percent_of_day.e

parse_post.e: parse_post.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		parse_post.c	 					\
		-o parse_post.e
	touch parse_post
	rm parse_post
	ln parse_post.e parse_post

delimiter2padded_columns.e: delimiter2padded_columns.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		delimiter2padded_columns.c 				\
		-o delimiter2padded_columns.e

statistics_on_group.e: statistics_on_group.c
	$(CC)    ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		statistics_on_group.c	 				\
		-o statistics_on_group.e

range.e: range.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		range.c	 						\
		-o range.e

commas_in_long.e: commas_in_long.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		commas_in_long.c	 				\
		-o commas_in_long.e

remove_duplicate_rows.e: remove_duplicate_rows.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		remove_duplicate_rows.c	 				\
		-o remove_duplicate_rows.e

append_radio_html.e: append_radio_html.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		append_radio_html.c	 				\
		-o append_radio_html.e

isolate_line.e: isolate_line.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		isolate_line.c	 					\
		-o isolate_line.e

append_escaped_backslash_cr.e: append_escaped_backslash_cr.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		append_escaped_backslash_cr.c 				\
		-o append_escaped_backslash_cr.e

joinlines.e: joinlines.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		joinlines.c 						\
		${APPASERVER_LIBRARY}					\
		-o joinlines.e

join.e: join.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		join.c 							\
		${APPASERVER_LIBRARY}					\
		-o join.e

remove_cr.e: remove_cr.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		remove_cr.c 						\
		${APPASERVER_LIBRARY}					\
		-o remove_cr.e

joinfiles.e: joinfiles.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		joinfiles.c 						\
		${APPASERVER_LIBRARY}					\
		-o joinfiles.e

count_alpha.e: count_alpha.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		count_alpha.c 						\
		${APPASERVER_LIBRARY}					\
		-o count_alpha.e

column2pipe.e: column2pipe.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		column2pipe.c 						\
		${APPASERVER_LIBRARY}					\
		-o column2pipe.e

variable2words.e: variable2words.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		variable2words.c 					\
		${APPASERVER_LIBRARY}					\
		-o variable2words.e

xargs.e: xargs.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		xargs.c 						\
		${APPASERVER_LIBRARY}					\
		-o xargs.e

full_year.e: full_year.c calculate_full_year.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		full_year.c 						\
		calculate_full_year.o 					\
		${APPASERVER_LIBRARY}					\
		-o full_year.e

calculate_full_year.o: calculate_full_year.c calculate_full_year.h
	$(CC)	${CFLAGS} -c calculate_full_year.c			\
		${APPASERVER_ERROR_FILE_FLAG}

observing_view.e: observing_view.c
	$(CC)	${CFLAGS} observing_view.c -o observing_view.e		\
		${APPASERVER_ERROR_FILE_FLAG}

display_character.e: display_character.c
	$(CC)	${CFLAGS} display_character.c -o display_character.e	\
		${APPASERVER_ERROR_FILE_FLAG}

trim_character.e: trim_character.c
	$(CC)	${CFLAGS} trim_character.c -o trim_character.e		\
		${APPASERVER_ERROR_FILE_FLAG}

extension.e: extension.c
	$(CC)	${CFLAGS} extension.c -o extension.e			\
		${APPASERVER_ERROR_FILE_FLAG}

single_quotes_around.e: single_quotes_around.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		single_quotes_around.c 					\
		${APPASERVER_LIBRARY}					\
		-o single_quotes_around.e
	touch single_quotes_around
	rm single_quotes_around
	ln single_quotes_around.e single_quotes_around

double_quote_comma_delimited.e: double_quote_comma_delimited.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		double_quote_comma_delimited.c 				\
		${APPASERVER_LIBRARY}					\
		-o double_quote_comma_delimited.e
	touch double_quote_comma_delimited
	rm double_quote_comma_delimited
	ln double_quote_comma_delimited.e double_quote_comma_delimited

piece_ora_date.e: piece_ora_date.c ${LIBRARY}/piece.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		piece_ora_date.c 					\
		${APPASERVER_LIBRARY}					\
		-o piece_ora_date.e

piece_replace_date.e: piece_replace_date.c ${LIBRARY}/piece.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		piece_replace_date.c 					\
		${APPASERVER_LIBRARY}					\
		-o piece_replace_date.e

date_convert_login_format.e: date_convert_login_format.c		\
		${APPASERVER_LIBRARY}
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		date_convert_login_format.c				\
		${APPASERVER_LIBRARY}					\
		-o date_convert_login_format.e

date_convert.e: date_convert.c						\
		${APPASERVER_LIBRARY}
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		date_convert.c 						\
		${APPASERVER_LIBRARY}					\
		-o date_convert.e

date_convert_anywhere.e: date_convert_anywhere.c			\
		${APPASERVER_LIBRARY}
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		date_convert_anywhere.c 				\
		${APPASERVER_LIBRARY}					\
		-o date_convert_anywhere.e

piece_set_character.e: piece_set_character.c ${LIBRARY}/piece.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		piece_set_character.c 					\
		${APPASERVER_LIBRARY}					\
		-o piece_set_character.e

piece_inverse.e: piece_inverse.c ${LIBRARY}/piece.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		piece_inverse.c 					\
		${APPASERVER_LIBRARY}					\
		-o piece_inverse.e

piece_insert.e: piece_insert.c ${LIBRARY}/piece.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		piece_insert.c 						\
		${APPASERVER_LIBRARY}					\
		-o piece_insert.e

piece_swap.e: piece_swap.c ${LIBRARY}/piece.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		piece_swap.c 						\
		${APPASERVER_LIBRARY}					\
		-o piece_swap.e

piece_shift_left.e: piece_shift_left.c ${LIBRARY}/piece.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		piece_shift_left.c 					\
		${APPASERVER_LIBRARY}					\
		-o piece_shift_left.e

piece_shift_right.e: piece_shift_right.c ${LIBRARY}/piece.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		piece_shift_right.c 					\
		${APPASERVER_LIBRARY}					\
		-o piece_shift_right.e

ora_date_to.e: ora_date_to.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		ora_date_to.c 						\
		${APPASERVER_LIBRARY}					\
		-o ora_date_to.e

oracle_date_to_mysql.e: oracle_date_to_mysql.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		oracle_date_to_mysql.c 					\
		${APPASERVER_LIBRARY}					\
		-o oracle_date_to_mysql.e

mysql_date2ora_date.e: mysql_date2ora_date.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		mysql_date2ora_date.c 					\
		${APPASERVER_LIBRARY}					\
		-o mysql_date2ora_date.e

date2julian.e: date2julian.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		date2julian.c 						\
		${APPASERVER_LIBRARY}					\
		-o date2julian.e

julian2date.e: julian2date.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		julian2date.c 						\
		${APPASERVER_LIBRARY}					\
		-o julian2date.e

tolower.e: tolower.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		tolower.c 						\
		${APPASERVER_LIBRARY}					\
		-o tolower.e

make_key.e: make_key.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		make_key.c 						\
		${APPASERVER_LIBRARY}					\
		-o make_key.e

toupper.e: toupper.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		toupper.c 						\
		${APPASERVER_LIBRARY}					\
		-o toupper.e

toupper_to_delimiter.e: toupper_to_delimiter.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		toupper_to_delimiter.c					\
		${APPASERVER_LIBRARY}					\
		-o toupper_to_delimiter.e

statistics.e: statistics.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		statistics.c						\
		-o statistics.e -lm
	touch statistics
	rm statistics
	ln statistics.e statistics

statistics_weighted.e: statistics_weighted.c				\
		${LIBRARY}/statistic.o					\
		${LIBRARY}/html.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		statistics_weighted.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/statistic.o					\
		${LIBRARY}/html.o					\
		-o statistics_weighted.e -lm
	touch descriptive_statistics
	rm descriptive_statistics
	ln statistics_weighted.e descriptive_statistics

moving_statistic.e: moving_statistic.c					\
		${LIBRARY}/moving_statistic.o				\
		${LIBRARY}/statistic.o					\
		${LIBRARY}/html.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		moving_statistic.c					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/moving_statistic.o 				\
		${LIBRARY}/statistic.o 					\
		${LIBRARY}/html.o					\
		-o moving_statistic.e -lm

average.e: average.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		average.c						\
		${APPASERVER_LIBRARY}					\
		-o average.e

complete_arrangement.e: complete_arrangement.c				\
		${APPASERVER_LIBRARY}
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		complete_arrangement.c					\
		${APPASERVER_LIBRARY}					\
		-o complete_arrangement.e

colapse_repeating.e: colapse_repeating.c				\
		${APPASERVER_LIBRARY}
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		colapse_repeating.c					\
		${APPASERVER_LIBRARY}					\
		-o colapse_repeating.e

hashsearch.e: hashsearch.c						\
		${APPASERVER_LIBRARY}
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		hashsearch.c						\
		${APPASERVER_LIBRARY}					\
		-o hashsearch.e

nothing.e: nothing.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		nothing.c 						\
		${APPASERVER_LIBRARY}					\
		-o nothing.e
	touch nothing
	rm nothing
	ln nothing.e nothing

queue_file.e: queue_file.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		queue_file.c 						\
		${APPASERVER_LIBRARY}					\
		-o queue_file.e

pad_leading_numbers.e: pad_leading_numbers.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		pad_leading_numbers.c 					\
		${APPASERVER_LIBRARY}					\
		-o pad_leading_numbers.e

mysqldump_appaserver: mysqldump_appaserver.c				\
	${LIBRARY}/fork_control.o					\
	${LIBRARY}/mysqldump.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		mysqldump_appaserver.c 					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/fork_control.o				\
		${LIBRARY}/mysqldump.o					\
		-o mysqldump_appaserver

mysqldump_fork_count_drop.e: mysqldump_fork_count_drop.c		\
		${LIBRARY}/mysqldump.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		mysqldump_fork_count_drop.c 				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/mysqldump.o					\
		-o mysqldump_fork_count_drop.e

csvdump_fork.e: csvdump_fork.c ${LIBRARY}/fork_control.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		csvdump_fork.c 						\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/fork_control.o				\
		-o csvdump_fork.e

queue_top_bottom_lines.e: queue_top_bottom_lines.c			\
		${LIBRARY}/queue.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		queue_top_bottom_lines.c 				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/queue.o					\
		-o queue_top_bottom_lines.e
	touch queue_lines
	rm queue_lines
	ln queue_top_bottom_lines.e queue_lines

join_multi_line_delimited.e: join_multi_line_delimited.c		\
		${LIBRARY}/queue.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		join_multi_line_delimited.c 				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/queue.o					\
		-o join_multi_line_delimited.e
	touch join_multi_line_delimited
	rm join_multi_line_delimited
	ln join_multi_line_delimited.e join_multi_line_delimited

select2delimiter.e: select2delimiter.c					\
		${LIBRARY}/token.o
	$(CC)   ${CFLAGS} -I${LIBRARY} 					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		select2delimiter.c 					\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/token.o 					\
		-o select2delimiter.e

format_stamp_undo.e: format_stamp_undo.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		format_stamp_undo.c 					\
		${APPASERVER_LIBRARY}					\
		-o format_stamp_undo.e

format_stamp.e: format_stamp.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		format_stamp.c	 					\
		${APPASERVER_LIBRARY}					\
		-o format_stamp.e

format_initial_capital.e: format_initial_capital.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		format_initial_capital.c	 			\
		${APPASERVER_LIBRARY}					\
		-o format_initial_capital.e

days_passed.e: days_passed.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		days_passed.c	 					\
		${APPASERVER_LIBRARY}					\
		-o days_passed.e

date_subtract.e: date_subtract.c
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		date_subtract.c	 					\
		${APPASERVER_LIBRARY}					\
		-o date_subtract.e

label.e: label.c							\
	${LIBRARY}/name_arg.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		label.c 						\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o 					\
		-o label.e

add_line.e: add_line.c							\
	${LIBRARY}/name_arg.o
	$(CC)    ${CFLAGS} -I${LIBRARY} 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		add_line.c 						\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o 					\
		-o add_line.e

each_week.e: each_week.c
	$(CC) 	${CFLAGS} -I${LIBRARY} each_week.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o each_week.e

start_time.e: start_time.c
	$(CC) 	${CFLAGS} -I${LIBRARY} start_time.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o start_time.e

stop_time.e: stop_time.c
	$(CC) 	${CFLAGS} -I${LIBRARY} stop_time.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o stop_time.e

random.e: random.c
	$(CC) 	${CFLAGS} -I${LIBRARY} random.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o random.e

random_stream.e: random_stream.c
	$(CC) 	${CFLAGS} -I${LIBRARY} random_stream.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o random_stream.e

or_sequence.e: or_sequence.c
	$(CC) 	${CFLAGS} -I${LIBRARY} or_sequence.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o or_sequence.e

in_clause.e: in_clause.c
	$(CC) 	${CFLAGS} -I${LIBRARY} in_clause.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o in_clause.e

where.e: where.c
	$(CC) 	${CFLAGS} -I${LIBRARY} where.c	 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o where.e

pivot_table.e: pivot_table.c						\
		${LIBRARY}/pivot_table.o
	$(CC) 	${CFLAGS} -I${LIBRARY} pivot_table.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/pivot_table.o				\
		-o pivot_table.e

search_replace_character_limited.e: search_replace_character_limited.c
	$(CC) 	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		search_replace_character_limited.c 			\
		${APPASERVER_LIBRARY}					\
		-o search_replace_character_limited.e

search_replace_string.e: search_replace_string.c
	$(CC) 	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		search_replace_string.c		 			\
		${APPASERVER_LIBRARY}					\
		-o search_replace_string.e

mysql_transform.e: mysql_transform.c
	$(CC) 	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		mysql_transform.c		 			\
		${APPASERVER_LIBRARY}					\
		-o mysql_transform.e

count.e: count.c
	$(CC) 	${CFLAGS} -I${LIBRARY} count.c 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o count.e

table_count.e: table_count.c						\
	${APPASERVER_LIBRARY}
	$(CC) 	${CFLAGS} -I${LIBRARY} table_count.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o table_count.e

trim.e: trim.c
	$(CC) 	${CFLAGS} -I${LIBRARY} trim.c 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o trim.e

basename.e: basename.c
	$(CC) 	${CFLAGS} -I${LIBRARY} basename.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o basename.e

trim_length.e: trim_length.c
	$(CC) 	${CFLAGS} -I${LIBRARY} trim_length.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o trim_length.e

trim_right.e: trim_right.c ${APPASERVER_LIBRARY}
	$(CC) 	${CFLAGS} -I${LIBRARY} trim_right.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o trim_right.e

string_initial_capital.e: string_initial_capital.c ${APPASERVER_LIBRARY}
	$(CC) 	${CFLAGS} -I${LIBRARY} string_initial_capital.c 	\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o string_initial_capital.e

group.e: group.c
	$(CC) 	${CFLAGS} -I${LIBRARY} group.c 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o group.e

group_trim.e: group_trim.c
	$(CC) 	${CFLAGS} -I${LIBRARY} group_trim.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o group_trim.e

group_sum.e: group_sum.c
	$(CC) 	${CFLAGS} -I${LIBRARY} group_sum.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o group_sum.e

ufw_whitelist.e: ufw_whitelist.c					\
		${LIBRARY}/fopen_path.o
	$(CC) 	${CFLAGS} -I${LIBRARY} ufw_whitelist.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/fopen_path.o					\
		-o ufw_whitelist.e
		touch ufw_blacklist.e
		rm ufw_blacklist.e
		ln ufw_whitelist.e ufw_blacklist.e

group_date_time.e: group_date_time.c					\
	${LIBRARY}/name_arg.o
	$(CC) 	${CFLAGS} -I${LIBRARY} group_date_time.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o 					\
		-o group_date_time.e

stat.e: stat.c
	$(CC) 	${CFLAGS} -I${LIBRARY} stat.c 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o stat.e
		touch statymd.e
		rm statymd.e
		ln stat.e statymd.e

cut.e: cut.c
	$(CC) 	${CFLAGS} -I${LIBRARY} cut.c 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o cut.e

sum.e: sum.c
	$(CC) 	${CFLAGS} -I${LIBRARY} sum.c 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o sum.e

accumulate.e: accumulate.c
	$(CC) 	${CFLAGS} -I${LIBRARY} accumulate.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o accumulate.e

balance_accumulate_check.e: balance_accumulate_check.c
	$(CC) 	${CFLAGS} -I${LIBRARY} balance_accumulate_check.c 	\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o balance_accumulate_check.e

keys_match_sum.e: keys_match_sum.c ${LIBRARY}/array.o			\
		${LIBRARY}/keys_match_sum.o
	$(CC) 	${CFLAGS} -I${LIBRARY} keys_match_sum.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/array.o					\
		${LIBRARY}/keys_match_sum.o				\
		-o keys_match_sum.e -lm

nl.e: nl.c
	$(CC) 	${CFLAGS} -I${LIBRARY} nl.c 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o nl.e

nl_ff.e: nl_ff.c
	$(CC) 	${CFLAGS} -I${LIBRARY} nl_ff.c 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o nl_ff.e

piece_quoted.e: piece_quoted.c ${LIBRARY}/piece.o
	$(CC) 	${CFLAGS} -I${LIBRARY} piece_quoted.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o piece_quoted.e

piece_quote_comma.e: piece_quote_comma.c ${LIBRARY}/piece.o
	$(CC) 	${CFLAGS} -I${LIBRARY} piece_quote_comma.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o piece_quote_comma.e
		touch piece_quote_comma
		rm piece_quote_comma
		ln piece_quote_comma.e piece_quote_comma

piece.e: piece.c ${LIBRARY}/piece.o
	$(CC) 	${CFLAGS} -I${LIBRARY} piece.c 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o piece.e
	touch piece
	rm piece
	ln piece.e piece

insert_string.e: insert_string.c
	$(CC) 	${CFLAGS} -I${LIBRARY} insert_string.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o insert_string.e
	touch insert_string
	rm insert_string
	ln insert_string.e insert_string

piece_replace.e: piece_replace.c ${LIBRARY}/piece.o
	$(CC) 	${CFLAGS} -I${LIBRARY} piece_replace.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o piece_replace.e
	touch piece_replace
	rm piece_replace
	ln piece_replace.e piece_replace

piece_grep.e: piece_grep.c ${LIBRARY}/piece.o
	$(CC) 	${CFLAGS} -I${LIBRARY} piece_grep.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o piece_grep.e
	touch piece_grep
	rm piece_grep
	ln piece_grep.e piece_grep

produce_socket_message.e: produce_socket_message.c
	$(CC) 	${CFLAGS} produce_socket_message.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-o produce_socket_message.e
	touch produce_socket_message
	rm produce_socket_message
	ln produce_socket_message.e produce_socket_message

consume_socket_message.e: consume_socket_message.c
	$(CC) 	${CFLAGS} consume_socket_message.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-o consume_socket_message.e
	touch consume_socket_message
	rm consume_socket_message
	ln consume_socket_message.e consume_socket_message

menu.e: menu.c menutree.o
	$(CC)	${CFLAGS} -I${LIBRARY} menu.c				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		menutree.o						\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/stack.o					\
		${LIBRARY}/queue.o					\
		-o menu.e
	touch menu
	rm menu
	ln menu.e menu

menutree.o: menutree.c menutree.h
	$(CC) ${CFLAGS} -I${LIBRARY} -c menutree.c			\
		${APPASERVER_ERROR_FILE_FLAG}

cdmenu.e: cdmenu.c
	$(CC) -I${LIBRARY} cdmenu.c 					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o cdmenu.e

delete_statement.e: delete_statement.c					\
	${LIBRARY}/name_arg.o
	$(CC) 	${CFLAGS} -I${LIBRARY} delete_statement.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o 					\
		-o delete_statement.e
	touch delete_statement
	rm delete_statement
	ln delete_statement.e delete_statement

insert_statement.e: insert_statement.c					\
	${LIBRARY}/name_arg.o
	$(CC) 	${CFLAGS} -I${LIBRARY} insert_statement.c 		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o 					\
		-o insert_statement.e
	touch insert_statement
	rm insert_statement
	ln insert_statement.e insert_statement

pad_missing_times.e: pad_missing_times.c
	$(CC) 	${CFLAGS} -I${LIBRARY} pad_missing_times.c 		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o pad_missing_times.e

time_ticker.e: time_ticker.c ${LIBRARY}/expected_count.o
	$(CC) 	${CFLAGS} -I${LIBRARY} time_ticker.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/expected_count.o				\
		-o time_ticker.e

update_statement.e: update_statement.c					\
	${LIBRARY}/name_arg.o
	$(CC) 	${CFLAGS} -I${LIBRARY} update_statement.c 		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o					\
		-o update_statement.e
	touch update_statement
	rm update_statement
	ln update_statement.e update_statement

format_sql_output.e: format_sql_output.c
	$(CC) 	${CFLAGS} -I${LIBRARY} format_sql_output.c 		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o format_sql_output.e

attribute_format.e: attribute_format.c
	$(CC) ${CFLAGS}	-I${LIBRARY} attribute_format.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o attribute_format.e

column.e: column.c ${APPASERVER_HOME}/library/column.o
	$(CC) 	${CFLAGS} -I${LIBRARY} column.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o column.e

column_grep.e: column_grep.c
	$(CC) 	${CFLAGS} -I${LIBRARY} column_grep.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o column_grep.e

binary_count.e: binary_count.c
	$(CC) 	${CFLAGS} -I${LIBRARY} binary_count.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o binary_count.e

decimal_count.e: decimal_count.c
	$(CC) 	${CFLAGS} -I${LIBRARY} decimal_count.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o decimal_count.e

location_convert.e: location_convert.c
	$(CC) 	${CFLAGS} -I${LIBRARY} location_convert.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o location_convert.e

merge_n_groups.e: merge_n_groups.c					\
	${LIBRARY}/name_arg.o
	$(CC) 	-I${LIBRARY} merge_n_groups.c 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o					\
		-o merge_n_groups.e

merge_n_lines.e: merge_n_lines.c					\
	${LIBRARY}/name_arg.o
	$(CC) 	-I${LIBRARY} merge_n_lines.c 				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o					\
		-o merge_n_lines.e

unescape_single_quotes.e: unescape_single_quotes.c
	$(CC) 	-I${LIBRARY} unescape_single_quotes.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o unescape_single_quotes.e

unescape_character.e: unescape_character.c				\
		${APPASERVER_LIBRARY}
	$(CC) 	-I${LIBRARY} unescape_character.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o unescape_character.e

escape_character.e: escape_character.c					\
		${APPASERVER_LIBRARY}
	$(CC) 	-I${LIBRARY} escape_character.c				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o escape_character.e

escape_security.e: escape_security.c
	$(CC) 	-I${LIBRARY} escape_security.c				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o escape_security.e

html_encode_quote_and_ampersand.e: html_encode_quote_and_ampersand.c	\
	${LIBRARY}/name_arg.o
	$(CC) 	-I${LIBRARY} html_encode_quote_and_ampersand.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o					\
		-o html_encode_quote_and_ampersand.e

date_grandfather_father_son.e: date_grandfather_father_son.c
	$(CC) 	-I${LIBRARY} date_grandfather_father_son.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o date_grandfather_father_son.e

findgrep.e: findgrep.c							\
	${LIBRARY}/name_arg.o
	$(CC)	${CFLAGS} -I${LIBRARY} findgrep.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o					\
		-o findgrep.e
	touch findgrep
	rm findgrep
	ln findgrep.e findgrep

sedmv.e: sedmv.c							\
	${LIBRARY}/name_arg.o
	$(CC)	-I${LIBRARY} sedmv.c					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o					\
		-o sedmv.e

yesterday.e: yesterday.c
	$(CC)	${CFLAGS} -I${LIBRARY} yesterday.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o yesterday.e

date.e: date.c ${LIBRARY}/date.o
	$(CC) ${CFLAGS}	-I${LIBRARY} date.c				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o date.e

date_ticker.e: date_ticker.c ${LIBRARY}/date.o
	$(CC) ${CFLAGS}	-I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		date_ticker.c						\
		-o date_ticker.e

real_time2aggregate_value.e: real_time2aggregate_value.c		\
		${LIBRARY}/statistic.o					\
		${LIBRARY}/html.o
	$(CC) ${CFLAGS}	-I${LIBRARY} real_time2aggregate_value.c	\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/statistic.o					\
		${LIBRARY}/html.o					\
		-o real_time2aggregate_value.e -lm

exceed_standard_deviation.e: exceed_standard_deviation.c		\
		${LIBRARY}/statistic.o					\
		${LIBRARY}/html.o
	$(CC) ${CFLAGS}	-I${LIBRARY} exceed_standard_deviation.c	\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/statistic.o					\
		${LIBRARY}/html.o					\
		-o exceed_standard_deviation.e -lm

real_time2annual_period.e: real_time2annual_period.c			\
		${LIBRARY}/statistic.o					\
		${LIBRARY}/html.o
	$(CC) ${CFLAGS}	-I${LIBRARY} real_time2annual_period.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/statistic.o					\
		${LIBRARY}/html.o					\
		-o real_time2annual_period.e -lm

date_add_days.e: date_add_days.c
	$(CC) ${CFLAGS}	-I${LIBRARY} date_add_days.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o date_add_days.e

julian_time.e: julian_time.c
	$(CC) ${CFLAGS}	-I${LIBRARY} julian_time.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o julian_time.e

julian_add_hours.e: julian_add_hours.c
	$(CC) ${CFLAGS}	-I${LIBRARY} julian_add_hours.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o julian_add_hours.e

round2five_minutes: round2five_minutes.c ${LIBRARY}/date.o
	$(CC)	${CFLAGS} -I${LIBRARY} round2five_minutes.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o round2five_minutes

date_min_max.e: date_min_max.c
	$(CC)	${CFLAGS} -I${LIBRARY} date_min_max.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o date_min_max.e

julian_time_list.e: julian_time_list.c					\
		${LIBRARY}/julian_time_double_list.o
	$(CC)	${CFLAGS} -I${LIBRARY} julian_time_list.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/julian_time_double_list.o			\
		-o julian_time_list.e

padspace.e: padspace.c
	$(CC)	${CFLAGS} -I${LIBRARY} padspace.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o padspace.e

findbig.e: findbig.c
	$(CC)	${CFLAGS} -I${LIBRARY} findbig.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o findbig.e
		touch findbig
		rm findbig
		ln findbig.e findbig

findtoday.e: findtoday.c
	$(CC)	${CFLAGS} -I${LIBRARY} findtoday.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o findtoday.e
		touch findtoday
		rm findtoday
		ln findtoday.e findtoday

round_integer.e: round_integer.c
	$(CC)	${CFLAGS} -I${LIBRARY} round_integer.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o round_integer.e

tee_process.e: tee_process.c
	$(CC)	${CFLAGS} -I${LIBRARY} tee_process.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o tee_process.e

trim_until_character.e: trim_until_character.c
	$(CC)	${CFLAGS} -I${LIBRARY} trim_until_character.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o trim_until_character.e

restore_database.e: restore_database.c restore_database.h
	$(CC)	${CFLAGS} -I${LIBRARY} restore_database.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o restore_database.e

piece_exceedance.e: piece_exceedance.c ${LIBRARY}/piece.o
	$(CC)	${CFLAGS} -I${LIBRARY} piece_exceedance.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o piece_exceedance.e

shuffle_lines.e: shuffle_lines.c
	$(CC)	${CFLAGS} -I${LIBRARY} shuffle_lines.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o shuffle_lines.e

html_table.e: html_table.c ${LIBRARY}/html.o
	$(CC)	${CFLAGS} -I${LIBRARY} html_table.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/html.o	 				\
		-o html_table.e
		touch html_table
		rm html_table
		ln html_table.e html_table

piece_arithmetic.e: piece_arithmetic.c ${LIBRARY}/piece.o
	$(CC)	${CFLAGS} -I${LIBRARY} piece_arithmetic.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o piece_arithmetic.e

piece_sum.e: piece_sum.c ${LIBRARY}/piece.o
	$(CC)	${CFLAGS} -I${LIBRARY} piece_sum.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o piece_sum.e

ascii.e: ascii.c
	$(CC) ascii.c -o ascii						\
		${APPASERVER_ERROR_FILE_FLAG}

head_sentences.e: head_sentences.c
	$(CC)	-I${LIBRARY} head_sentences.c				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-o head_sentences.e

trim_right_until_character.e: trim_right_until_character.c
	$(CC)	${CFLAGS} -I${LIBRARY} trim_right_until_character.c	\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o trim_right_until_character.e

skip_lines.e: skip_lines.c						\
		${APPASERVER_LIBRARY}
	$(CC)	${CFLAGS} -I${LIBRARY} skip_lines.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o skip_lines.e

commit.e: commit.c							\
		${APPASERVER_LIBRARY}
	$(CC)	${CFLAGS} -I${LIBRARY} commit.c				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o commit.e

filter_anomaly.e: filter_anomaly.c					\
		${APPASERVER_LIBRARY}
	$(CC)	${CFLAGS} -I${LIBRARY} filter_anomaly.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o filter_anomaly.e

strncmp.e: strncmp.c							\
		${APPASERVER_LIBRARY}
	$(CC)	${CFLAGS} -I${LIBRARY} strncmp.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o strncmp.e

strcmp.e: strcmp.c							\
		${APPASERVER_LIBRARY}
	$(CC)	${CFLAGS} -I${LIBRARY} strcmp.c				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o strcmp.e

strlen.e: strlen.c							\
		${APPASERVER_LIBRARY}
	$(CC)	${CFLAGS} -I${LIBRARY} strlen.c				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o strlen.e

character_count.e: character_count.c					\
		${APPASERVER_LIBRARY}
	$(CC)	${CFLAGS} -I${LIBRARY} character_count.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o character_count.e

highest_prime_number.e: highest_prime_number.c
	$(CC)	${CFLAGS} highest_prime_number.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-o highest_prime_number.e -lm

increment.e: increment.c
	$(CC)	${CFLAGS} increment.c					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-o increment.e

reverse.e: reverse.c
	$(CC)	${CFLAGS} -I${LIBRARY} reverse.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o reverse.e

reverse_lines.e: reverse_lines.c
	$(CC)	${CFLAGS} -I${LIBRARY} reverse_lines.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		-o reverse_lines.e

gethostbyname: gethostbyname.c
	$(CC)	gethostbyname.c						\
		-o gethostbyname

piece_commas_double.e: piece_commas_double.c
	$(CC)	${CFLAGS} -I${LIBRARY} piece_commas_double.c		\
		${APPASERVER_LIBRARY}					\
		-o piece_commas_double.e

minutes_between.e: minutes_between.c
	$(CC)	${CFLAGS} -I${LIBRARY} minutes_between.c		\
		${APPASERVER_LIBRARY}					\
		-o minutes_between.e

minutes_ticker.e: minutes_ticker.c
	$(CC)	${CFLAGS} -I${LIBRARY} minutes_ticker.c			\
		${APPASERVER_LIBRARY}					\
		-o minutes_ticker.e

filename_date_older.e: filename_date_older.c
	$(CC)	${CFLAGS} -I${LIBRARY} filename_date_older.c		\
		${APPASERVER_LIBRARY}					\
		-o filename_date_older.e

mail_if_output.e: mail_if_output.c
	$(CC)	${CFLAGS} -I${LIBRARY} mail_if_output.c			\
		${APPASERVER_LIBRARY}					\
		-o mail_if_output.e

time_hhmm2hhcolonmm.e: time_hhmm2hhcolonmm.c
	$(CC)	${CFLAGS} -I${LIBRARY} time_hhmm2hhcolonmm.c		\
		${APPASERVER_LIBRARY}					\
		-o time_hhmm2hhcolonmm.e

subtract_list.e: subtract_list.c
	$(CC)	${CFLAGS} -I${LIBRARY} subtract_list.c			\
		${APPASERVER_LIBRARY}					\
		-o subtract_list.e

label_dictionary.e: label_dictionary.c
	$(CC)	${CFLAGS} -I${LIBRARY} label_dictionary.c		\
		${APPASERVER_LIBRARY}					\
		-o label_dictionary.e

measurement_convert_units.e: measurement_convert_units.c		\
		${APPASERVER_LIBRARY}
	$(CC)	${CFLAGS} -I${LIBRARY} measurement_convert_units.c	\
		${APPASERVER_LIBRARY}					\
		-o measurement_convert_units.e

payroll_period.e: payroll_period.c					\
		${PREDICTIVE}/employee.h				\
		${LIBRARY}/name_arg.o
	$(CC)	${CFLAGS} -I${LIBRARY} -I${PREDICTIVE} payroll_period.c	\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/name_arg.o					\
		-o payroll_period.e

bracket_line.e: bracket_line.c
	$(CC)	${CFLAGS} bracket_line.c -o bracket_line.e

xml_separate.e: xml_separate.c						\
		${LIBRARY}/token.o
	$(CC)   ${CFLAGS} -I${LIBRARY} 					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		xml_separate.c 						\
		${APPASERVER_LIBRARY}					\
		${LIBRARY}/token.o 					\
		-o xml_separate.e

sort_delimited_numbers.e: sort_delimited_numbers.c
	$(CC)	${CFLAGS} -I${LIBRARY} sort_delimited_numbers.c		\
		${APPASERVER_LIBRARY}					\
		-o sort_delimited_numbers.e


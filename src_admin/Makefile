# $APPASERVER_HOME/src_admin/makefile
# -----------------------------------

CC=gcc
WARN=-Wall -Wextra -Wunused-parameter
CFLAGS=-O3 -s ${WARN}

LIBRARY=${APPASERVER_HOME}/library

CORE_LIBRARY=								\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/application.o				\
		${LIBRARY}/String.o					\
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

STANDARD_LIBRARY=							\
		${CORE_LIBRARY}						\
		${LIBRARY}/basename.o					\
		${LIBRARY}/aggregate_level.o				\
		${LIBRARY}/aggregate_statistic.o

APPASERVER_LIBRARY=							\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/date_convert.o				\
		${LIBRARY}/folder_attribute.o				\
		${LIBRARY}/insert.o					\
		${LIBRARY}/widget.o					\
		${LIBRARY}/sql.o					\
		${LIBRARY}/button.o					\
		${LIBRARY}/query.o					\
		${LIBRARY}/security_entity.o				\
		${LIBRARY}/entity.o					\
		${LIBRARY}/query_isa.o					\
		${LIBRARY}/appaserver_link.o				\
		${LIBRARY}/folder.o					\
		${LIBRARY}/relation.o					\
		${LIBRARY}/relation_mto1.o				\
		${LIBRARY}/relation_one2m.o				\
		${LIBRARY}/relation_join.o				\
		${LIBRARY}/process.o					\
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/role_folder.o				\
		${LIBRARY}/folder_operation.o				\
		${LIBRARY}/operation.o

ADMIN_LIBRARY=								\
		form_field.o						\
		post.o							\
		post_signup_submit.o					\
		post_signup_receive.o					\
		post_contact_submit.o					\
		post_contact_receive.o

EXECUTABLES=								\
		post_contact_submit_execute				\
		post_signup_submit_execute				\
		post_contact_receive_execute				\
		post_signup_receive_execute

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	rm -f *.o ${EXECUTABLES}

post_contact_submit_execute: post_contact_submit_execute.c		\
	${ADMIN_LIBRARY}						\
	${STANDARD_LIBRARY}						\
	${APPASERVER_LIBRARY}
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		${ADMIN_LIBRARY}					\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		post_contact_submit_execute.c				\
		-o post_contact_submit_execute

post_signup_submit_execute: post_signup_submit_execute.c		\
	${ADMIN_LIBRARY}						\
	${STANDARD_LIBRARY}						\
	${APPASERVER_LIBRARY}
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		${ADMIN_LIBRARY}					\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		post_sigunup_submit_execute.c				\
		-o post_signup_submit_execute

post_contact_receive_execute: post_contact_receive_execute.c		\
	${ADMIN_LIBRARY}						\
	${STANDARD_LIBRARY}						\
	${APPASERVER_LIBRARY}
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		${ADMIN_LIBRARY}					\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		post_contact_receive_execute.c				\
		-o post_contact_receive_execute

post_signup_receive_execute: post_signup_receive_execute.c		\
	${ADMIN_LIBRARY}						\
	${STANDARD_LIBRARY}						\
	${APPASERVER_LIBRARY}
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		${ADMIN_LIBRARY}					\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		post_sigunup_receive_execute.c				\
		-o post_signup_receive_execute

form_field.o: form_field.c form_field.h
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		-c form_field.c

post_signup_receive.o: post_signup_receive.c post_signup_receive.h
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		-c post_signup_receive.c

post_contact_receive.o: post_contact_receive.c post_contact_receive.h
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		-c post_contact_receive.c

post_contact_submit.o: post_contact_submit.c post_contact_submit.h
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		-c post_contact_submit.c

post_signup_submit.o: post_signup_submit.c post_signup_submit.h
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		-c post_signup_submit.c

post.o: post.c post.h
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		-c post.c


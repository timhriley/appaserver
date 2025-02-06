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
		${LIBRARY}/post_dictionary.o				\
		${LIBRARY}/session.o					\
		${LIBRARY}/timlib.o					\
		${LIBRARY}/upload_source.o				\
		${LIBRARY}/post_login.o					\
		${LIBRARY}/execute_system_string.o			\
		${LIBRARY}/bot_generated.o				\
		${LIBRARY}/operation.o

ADMIN_LIBRARY=								\
		email_address.o						\
		post_signup.o						\
		post_contact.o						\
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
	rm -f *.o ${EXECUTABLES}					\
	${CGI_HOME}/post_signup_submit_execute				\
	${CGI_HOME}/post_signup_receive_execute				\
	${CGI_HOME}/post_contact_submit_execute				\
	${CGI_HOME}/post_contact_receive_execute

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
	touch ${CGI_HOME}/post_contact_submit_execute
	/bin/rm -f ${CGI_HOME}/post_contact_submit_execute
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_admin/post_contact_submit_execute	\
	${CGI_HOME}

post_signup_submit_execute: post_signup_submit_execute.c		\
	${ADMIN_LIBRARY}						\
	${STANDARD_LIBRARY}						\
	${APPASERVER_LIBRARY}
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		${ADMIN_LIBRARY}					\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		post_signup_submit_execute.c				\
		-o post_signup_submit_execute
	touch ${CGI_HOME}/post_signup_submit_execute
	/bin/rm -f ${CGI_HOME}/post_signup_submit_execute
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_admin/post_signup_submit_execute		\
	${CGI_HOME}

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
	touch ${CGI_HOME}/post_contact_receive_execute
	/bin/rm -f ${CGI_HOME}/post_contact_receive_execute
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_admin/post_contact_receive_execute	\
	${CGI_HOME}

post_signup_receive_execute: post_signup_receive_execute.c		\
	${ADMIN_LIBRARY}						\
	${STANDARD_LIBRARY}						\
	${APPASERVER_LIBRARY}
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		${ADMIN_LIBRARY}					\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		post_signup_receive_execute.c				\
		-o post_signup_receive_execute
	touch ${CGI_HOME}/post_signup_receive_execute
	/bin/rm -f ${CGI_HOME}/post_signup_receive_execute
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_admin/post_signup_receive_execute	\
	${CGI_HOME}

email_address.o: email_address.c email_address.h
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		-c email_address.c

post_signup.o: post_signup.c post_signup.h
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		-c post_signup.c

post_contact.o: post_contact.c post_contact.h
	$(CC)   ${CFLAGS} -I${LIBRARY}					\
		-c post_contact.c

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


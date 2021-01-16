# src_education/makefile
# ----------------------

CC=gcc
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall -Wunused-parameter

LIBRARY=${APPASERVER_HOME}/library
PREDICTIVEBOOKS_LIBRARY=${APPASERVER_HOME}/src_predictive

EDUCATION_CORE=								\
		offering.o						\
		paypal_sweep.o						\
		registration.o						\
		enrollment.o						\
		course.o						\
		tuition_payment.o					\
		tuition_refund.o					\
		program_donation.o					\
		product_sale.o						\
		product_refund.o					\
		ticket_sale.o						\
		ticket_refund.o						\
		education.o						\
		paypal_deposit.o					\
		semester.o						\
		student.o						\
		program.o						\
		product.o						\
		venue.o							\
		event.o

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/sed.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/float.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/String.o	 				\
		${LIBRARY}/folder_menu.o 				\
		${LIBRARY}/hashtbl.o

EXTERNAL_LIBRARY=							\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/date_aggregate.o 				\
		${LIBRARY}/aggregate_level.o 				\
		${LIBRARY}/aggregate_statistic.o 			\
		${LIBRARY}/html_table.o 				\
		${LIBRARY}/prompt_recursive.o 				\
		${LIBRARY}/decode_html_post.o				\
		${LIBRARY}/appaserver_parameter_file.o			\
		${LIBRARY}/appaserver_library.o				\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/application_constants.o			\
		${LIBRARY}/process_generic_output.o			\
		${LIBRARY}/name_arg.o					\
		${LIBRARY}/basename.o					\
		${LIBRARY}/semaphore.o					\
		${LIBRARY}/fopen_path.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/application.o				\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/process_parameter_list.o			\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/related_folder.o				\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/dictionary_appaserver.o			\
		${LIBRARY}/query.o					\
		${LIBRARY}/latex.o					\
		${LIBRARY}/process.o					\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/form.o					\
		${LIBRARY}/element.o					\
		${LIBRARY}/operation.o					\
		${LIBRARY}/role_folder.o				\
		${LIBRARY}/session.o					\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/insert_database.o				\
		${LIBRARY}/update_database.o				\
		${LIBRARY}/operation_list.o				\
		${LIBRARY}/validation_form.o				\
		${LIBRARY}/post2dictionary.o				\
		${LIBRARY}/dictionary2file.o

PREDICTIVEBOOKS_CORE= 							\
		${PREDICTIVEBOOKS_LIBRARY}/transaction.o		\
		${PREDICTIVEBOOKS_LIBRARY}/journal.o			\
		${PREDICTIVEBOOKS_LIBRARY}/element.o			\
		${PREDICTIVEBOOKS_LIBRARY}/entity.o			\
		${PREDICTIVEBOOKS_LIBRARY}/entity_self.o		\
		${PREDICTIVEBOOKS_LIBRARY}/subclassification.o		\
		${PREDICTIVEBOOKS_LIBRARY}/predictive.o			\
		${PREDICTIVEBOOKS_LIBRARY}/account.o			\
		${PREDICTIVEBOOKS_LIBRARY}/spreadsheet_upload_event.o	\
		${PREDICTIVEBOOKS_LIBRARY}/spreadsheet_column.o		\
		${PREDICTIVEBOOKS_LIBRARY}/spreadsheet.o		\
		${PREDICTIVEBOOKS_LIBRARY}/paypal.o			\
		${PREDICTIVEBOOKS_LIBRARY}/paypal_item.o		\
		${PREDICTIVEBOOKS_LIBRARY}/paypal_dataset.o

EXECUTABLES=	offering_trigger					\
		registration_trigger					\
		enrollment_trigger					\
		tuition_payment_trigger					\
		tuition_refund_trigger					\
		program_donation_trigger				\
		product_sale_trigger					\
		product_refund_trigger					\
		paypal_sweep_trigger					\
		ticket_sale_trigger					\
		ticket_refund_trigger					\
		paypal_upload						\
		generate_invoice					\
		product_trigger						\
		program_trigger

all: 	${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f *.o ${EXECUTABLES}

generate_invoice: generate_invoice.c					\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}						\
	${LIBRARY}/latex_invoice.o					\
	${LIBRARY}/email.o						\
	${LIBRARY}/table.o
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		generate_invoice.c					\
		${LIBRARY}/latex_invoice.o				\
		${LIBRARY}/email.o					\
		${LIBRARY}/table.o					\
		-o generate_invoice

offering_trigger: offering_trigger.c					\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		offering_trigger.c					\
		-o offering_trigger

product_trigger: product_trigger.c					\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		product_trigger.c					\
		-o product_trigger

program_trigger: program_trigger.c					\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		program_trigger.c					\
		-o program_trigger

enrollment_trigger: enrollment_trigger.c				\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		enrollment_trigger.c					\
		-o enrollment_trigger

tuition_payment_trigger: tuition_payment_trigger.c			\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		tuition_payment_trigger.c				\
		-o tuition_payment_trigger

tuition_refund_trigger: tuition_refund_trigger.c			\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		tuition_refund_trigger.c				\
		-o tuition_refund_trigger

program_donation_trigger: program_donation_trigger.c			\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		program_donation_trigger.c				\
		-o program_donation_trigger

product_sale_trigger: product_sale_trigger.c				\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		product_sale_trigger.c					\
		-o product_sale_trigger

ticket_sale_trigger: ticket_sale_trigger.c				\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		ticket_sale_trigger.c					\
		-o ticket_sale_trigger

product_refund_trigger: product_refund_trigger.c			\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		product_refund_trigger.c				\
		-o product_refund_trigger

paypal_sweep_trigger: paypal_sweep_trigger.c				\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		paypal_sweep_trigger.c					\
		-o paypal_sweep_trigger

ticket_refund_trigger: ticket_refund_trigger.c				\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		ticket_refund_trigger.c					\
		-o ticket_refund_trigger

registration_trigger: registration_trigger.c				\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		registration_trigger.c					\
		-o registration_trigger

paypal_upload: paypal_upload.c						\
	${EXTERNAL_LIBRARY} 						\
	${PREDICTIVEBOOKS_CORE}						\
	${EDUCATION_CORE}
	${CC}	${CFLAGS}						\
		-I${LIBRARY}						\
		-I${PREDICTIVEBOOKS_LIBRARY}				\
		${EXTERNAL_LIBRARY}					\
		${PREDICTIVEBOOKS_CORE}					\
		${EDUCATION_CORE}					\
		paypal_upload.c						\
		-o paypal_upload

# Education core
# -------------- 
offering.o: offering.c offering.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c offering.c

registration.o: registration.c registration.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c registration.c

enrollment.o: enrollment.c enrollment.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c enrollment.c

course.o: course.c course.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c course.c

program.o: program.c program.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c program.c

product.o: product.c product.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c product.c

venue.o: venue.c venue.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c venue.c

event.o: event.c event.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c event.c

tuition_payment.o: tuition_payment.c tuition_payment.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c tuition_payment.c

tuition_refund.o: tuition_refund.c tuition_refund.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c tuition_refund.c

program_donation.o: program_donation.c program_donation.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c program_donation.c

product_sale.o: product_sale.c product_sale.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c product_sale.c

product_refund.o: product_refund.c product_refund.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c product_refund.c

ticket_sale.o: ticket_sale.c ticket_sale.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c ticket_sale.c

ticket_refund.o: ticket_refund.c ticket_refund.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c ticket_refund.c

student.o: student.c student.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c student.c

semester.o: semester.c semester.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c semester.c

education.o: education.c education.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c education.c

paypal_deposit.o: paypal_deposit.c paypal_deposit.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c paypal_deposit.c

paypal_sweep.o: paypal_sweep.c paypal_sweep.h
	${CC}	${CFLAGS}	-I${LIBRARY}				\
				-I${PREDICTIVEBOOKS_LIBRARY}		\
		-c paypal_sweep.c


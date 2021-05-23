# src_predictive/makefile
# -----------------------

CC=gcc
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall -Wunused-parameter

LIBRARY=${APPASERVER_HOME}/library

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
		${LIBRARY}/String.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hash_table.o

EXECUTABLES=	trial_balance						\
		tax_form_report						\
		transaction_balance_report				\
		transaction_date_time					\
		income_statement					\
		balance_sheet						\
		program_profit						\
		bank_upload_transaction_insert				\
		ledger_unbalance					\
		ledger_debit_credit_audit				\
		close_nominal_accounts					\
		load_bank_spreadsheet					\
		feeder_upload_missing					\
		post_change_bank_upload					\
		post_reoccurring_transaction				\
		post_cash_expense_transaction				\
		post_change_self					\
		make_vendor_payment					\
		generate_invoice					\
		post_change_vendor_payment				\
		pay_liabilities_process					\
		populate_print_checks_entity				\
		depreciate_fixed_assets					\
		post_change_account_balance				\
		ledger_propagate					\
		feeder_upload_missing					\
		journal_ledger_trigger

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
		${LIBRARY}/vertical_new_button.o			\
		${LIBRARY}/relation.o					\
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
		${LIBRARY}/foreign_attribute.o				\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/insert_database.o				\
		${LIBRARY}/update_database.o				\
		${LIBRARY}/operation_list.o				\
		${LIBRARY}/validation_form.o				\
		${LIBRARY}/post2dictionary.o				\
		${LIBRARY}/dictionary2file.o

SOURCE_LIBRARY= 							\
		program.o						\
		subclassification.o					\
		depreciation.o						\
		entity_self.o						\
		entity.o						\
		purchase.o						\
		inventory_purchase.o					\
		specific_inventory.o					\
		specific_inventory_purchase.o				\
		fixed_asset.o						\
		fixed_asset_purchase.o					\
		vendor_payment.o					\
		sale.o							\
		inventory_sale.o					\
		fixed_service_sale.o					\
		hourly_service_sale.o					\
		customer_payment.o					\
		work.o							\
		predictive.o						\
		transaction.o						\
		account.o						\
		element.o						\
		journal.o						\
		statement.o						\
		supply.o						\
		supply_purchase.o					\
		prepaid_asset.o						\
		prepaid_asset_purchase.o

SPREADSHEET_LIBRARY= 							\
		spreadsheet.o						\
		spreadsheet_column.o					\
		spreadsheet_upload_event.o				\
		paypal.o						\
		paypal_dataset.o					\
		paypal_item.o

all: 	${SPREADSHEET_LIBRARY} ${SOURCE_LIBRARY} ${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f ${SOURCE_LIBRARY} ${EXECUTABLES}			\
		financial_position					\
		statement_of_activities					\
		generate_workorder

tax_dvr: tax_dvr.c 							\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		tax_dvr.c						\
		-o tax_dvr

post_reoccurring_transaction: post_reoccurring_transaction.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o						\
		accrual.o						\
		${LIBRARY}/folder_menu.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_reoccurring_transaction.c				\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o						\
		accrual.o						\
		${LIBRARY}/folder_menu.o				\
		-o post_reoccurring_transaction

post_cash_expense_transaction: post_cash_expense_transaction.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o						\
		${LIBRARY}/folder_menu.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_cash_expense_transaction.c				\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o						\
		${LIBRARY}/folder_menu.o				\
		-o post_cash_expense_transaction

post_payroll: post_payroll.c						\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_payroll.c						\
		-o post_payroll

shortcut_service_work: shortcut_service_work.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		shortcut_service_work.c					\
		-o shortcut_service_work

bank_upload_transaction_insert: bank_upload_transaction_insert.c	\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		bank_upload_transaction_insert.c			\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o						\
		-o bank_upload_transaction_insert

ledger_propagate: ledger_propagate.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		ledger_propagate.c					\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o						\
		-o ledger_propagate

load_receipt_scan: load_receipt_scan.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		load_receipt_scan.c					\
		-o load_receipt_scan

ledger_unbalance: ledger_unbalance.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		ledger_unbalance.c					\
		-o ledger_unbalance

ledger_debit_credit_audit: ledger_debit_credit_audit.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		ledger_debit_credit_audit.c				\
		-o ledger_debit_credit_audit

depreciate_fixed_assets: depreciate_fixed_assets.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		depreciate_fixed_assets.c				\
		-o depreciate_fixed_assets

tax_recover_fixed_assets: tax_recover_fixed_assets.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		tax_recover_fixed_assets.c				\
		-o tax_recover_fixed_assets

trial_balance: trial_balance.c						\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		trial_balance.c						\
		-o trial_balance

tax_form_report: tax_form_report.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY} tax.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		tax_form_report.c					\
		tax.o							\
		-o tax_form_report

transaction_balance_report: transaction_balance_report.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		transaction_balance.o					\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		transaction_balance_report.c				\
		transaction_balance.o					\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o						\
		-o transaction_balance_report

transaction_date_time: transaction_date_time.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		transaction_balance.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		transaction_date_time.c					\
		transaction_balance.o					\
		-o transaction_date_time

program_profit: program_profit.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		program_profit.c					\
		-o program_profit

income_statement: income_statement.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		income_statement.c					\
		-o income_statement
	touch statement_of_activities
	/bin/rm -f statement_of_activities
	ln income_statement statement_of_activities

balance_sheet: balance_sheet.c						\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		balance_sheet.c						\
		-o balance_sheet
	touch financial_position
	/bin/rm -f financial_position
	ln balance_sheet financial_position

close_nominal_accounts: close_nominal_accounts.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		close_nominal_accounts.c				\
		-o close_nominal_accounts

journal_ledger_trigger: journal_ledger_trigger.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		journal_ledger_trigger.c				\
		-o journal_ledger_trigger

pay_liabilities_process: pay_liabilities_process.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY} liability.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		pay_liabilities_process.c				\
		liability.o						\
		-o pay_liabilities_process

populate_print_checks_entity: populate_print_checks_entity.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY} liability.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		populate_print_checks_entity.c				\
		liability.o						\
		-o populate_print_checks_entity

load_bank_spreadsheet: load_bank_spreadsheet.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		load_bank_spreadsheet.c					\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o						\
		-o load_bank_spreadsheet

feeder_upload_missing: feeder_upload_missing.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		feeder_upload_missing.c					\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o						\
		-o feeder_upload_missing

post_change_prepaid_asset_accrual: post_change_prepaid_asset_accrual.c	\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_prepaid_asset_accrual.c			\
		-o post_change_prepaid_asset_accrual

post_change_bank_upload: post_change_bank_upload.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_bank_upload.c				\
		bank_upload.o						\
		feeder_upload.o						\
		reoccurring.o						\
		-o post_change_bank_upload

post_change_inventory_purchase: post_change_inventory_purchase.c	\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_inventory_purchase.c			\
		-o post_change_inventory_purchase

post_change_inventory_purchase_return:					\
	post_change_inventory_purchase_return.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_inventory_purchase_return.c			\
		-o post_change_inventory_purchase_return

post_change_inventory_sale_return:					\
	post_change_inventory_sale_return.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_inventory_sale_return.c			\
		-o post_change_inventory_sale_return


post_change_specific_inventory_purchase:				\
	post_change_specific_inventory_purchase.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_specific_inventory_purchase.c		\
		-o post_change_specific_inventory_purchase

post_change_supply_purchase: post_change_supply_purchase.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_supply_purchase.c				\
		-o post_change_supply_purchase

post_change_fixed_service_sale:						\
	post_change_fixed_service_sale.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_fixed_service_sale.c			\
		-o post_change_fixed_service_sale

post_change_hourly_service_sale:					\
	post_change_hourly_service_sale.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_hourly_service_sale.c			\
		-o post_change_hourly_service_sale

post_change_fixed_service_work:						\
	post_change_fixed_service_work.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_fixed_service_work.c			\
		-o post_change_fixed_service_work

post_change_activity_work:						\
	post_change_activity_work.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_activity_work.c				\
		-o post_change_activity_work

post_change_hourly_service_work:					\
	post_change_hourly_service_work.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_hourly_service_work.c			\
		-o post_change_hourly_service_work

post_change_service_purchase: post_change_service_purchase.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_service_purchase.c				\
		-o post_change_service_purchase

post_change_fixed_asset_purchase:					\
		post_change_fixed_asset_purchase.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_fixed_asset_purchase.c			\
		-o post_change_fixed_asset_purchase

post_change_prepaid_asset_purchase:					\
		post_change_prepaid_asset_purchase.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_prepaid_asset_purchase.c			\
		-o post_change_prepaid_asset_purchase

post_change_fixed_asset:						\
		post_change_fixed_asset.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_fixed_asset.c				\
		-o post_change_fixed_asset

propagate_inventory_purchase_layers:					\
	propagate_inventory_purchase_layers.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		propagate_inventory_purchase_layers.c			\
		-o propagate_inventory_purchase_layers

propagate_inventory_sale_layers:					\
	propagate_inventory_sale_layers.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		propagate_inventory_sale_layers.c			\
		-o propagate_inventory_sale_layers

inventory_balance_detail:						\
	inventory_balance_detail.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		inventory_balance_detail.c				\
		-o inventory_balance_detail

post_change_inventory_sale: post_change_inventory_sale.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_inventory_sale.c				\
		-o post_change_inventory_sale

post_change_specific_inventory_sale:					\
	post_change_specific_inventory_sale.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_specific_inventory_sale.c			\
		-o post_change_specific_inventory_sale

post_change_purchase_order: post_change_purchase_order.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_purchase_order.c				\
		-o post_change_purchase_order

post_change_vendor_payment: post_change_vendor_payment.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_vendor_payment.c				\
		-o post_change_vendor_payment

post_change_customer_payment: post_change_customer_payment.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY} subsidiary_transaction.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		subsidiary_transaction.o				\
		post_change_customer_payment.c				\
		-o post_change_customer_payment

post_change_prior_fixed_asset: post_change_prior_fixed_asset.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY} subsidiary_transaction.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		subsidiary_transaction.o				\
		post_change_prior_fixed_asset.c				\
		-o post_change_prior_fixed_asset

post_change_account_balance: post_change_account_balance.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		investment_account.o					\
		account_balance.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		investment_account.o					\
		account_balance.o					\
		post_change_account_balance.c				\
		-o post_change_account_balance

post_change_customer_sale: post_change_customer_sale.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_customer_sale.c				\
		-o post_change_customer_sale

synchronize_customer_sale_transaction:					\
	synchronize_customer_sale_transaction.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		synchronize_customer_sale_transaction.c			\
		-o synchronize_customer_sale_transaction

update_sale_completed: update_sale_completed.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		update_sale_completed.c					\
		-o update_sale_completed
	touch update_sale_not_completed
	/bin/rm -f update_sale_not_completed
	ln update_sale_completed update_sale_not_completed

collect_customer_payment: collect_customer_payment.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		collect_customer_payment.c				\
		-o collect_customer_payment

make_vendor_payment: make_vendor_payment.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		make_vendor_payment.c					\
		-o make_vendor_payment

post_change_self: post_change_self.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_self.c					\
		-o post_change_self

post_change_employee_work_day: post_change_employee_work_day.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_employee_work_day.c				\
		-o post_change_employee_work_day

inventory_cost_method_propagate: inventory_cost_method_propagate.c	\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		inventory_cost_method_propagate.c			\
		-o inventory_cost_method_propagate

generate_invoice: generate_invoice.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}				\
		${LIBRARY}/latex_invoice.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		generate_invoice.c					\
		${LIBRARY}/latex_invoice.o				\
		-o generate_invoice
	touch generate_workorder
	rm generate_workorder
	ln generate_invoice generate_workorder

inventory.o: inventory.c inventory.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c inventory.c

inventory_sale_return.o: inventory_sale_return.c			\
		inventory_sale_return.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c inventory_sale_return.c

inventory_purchase_return.o: inventory_purchase_return.c		\
		inventory_purchase_return.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c inventory_purchase_return.c

sale.o: sale.c sale.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c sale.c

depreciation.o:								\
	depreciation.c							\
	depreciation.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c depreciation.c

accrual.o: accrual.c accrual.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c accrual.c

entity_self.o: entity_self.c entity_self.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c entity_self.c

entity.o: entity.c entity.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c entity.c

employee.o: employee.c employee.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c employee.c

liability.o: liability.c liability.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c liability.c

tax.o: tax.c tax.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c tax.c

subsidiary_transaction.o: subsidiary_transaction.c			\
		subsidiary_transaction.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c subsidiary_transaction.c

investment.o: investment.c						\
		investment.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c investment.c

bank_upload.o: bank_upload.c reoccurring.h				\
		bank_upload.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c bank_upload.c

reoccurring.o: reoccurring.c						\
		reoccurring.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c reoccurring.c

fixed_asset.o: fixed_asset.c						\
		fixed_asset_purchase.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c fixed_asset.c

fixed_asset_purchase.o: fixed_asset_purchase.c				\
		fixed_asset_purchase.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c fixed_asset_purchase.c

fixed_service_sale.o: fixed_service_sale.c				\
		fixed_service_sale.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c fixed_service_sale.c

hourly_service_sale.o: hourly_service_sale.c				\
		hourly_service_sale.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c hourly_service_sale.c

vendor_payment.o: vendor_payment.c					\
		vendor_payment.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c vendor_payment.c

feeder_upload.o: feeder_upload.c					\
		feeder_upload.h reoccurring.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c feeder_upload.c

tax_recovery.o: tax_recovery.c						\
		tax_recovery.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c tax_recovery.c

transaction_balance.o: transaction_balance.c				\
		transaction_balance.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c transaction_balance.c

transaction.o: transaction.c transaction.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c transaction.c

journal.o: journal.c journal.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c journal.c

account.o: account.c account.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c account.c

element.o: element.c element.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c element.c

program.o: program.c program.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c program.c

customer_payment.o: customer_payment.c customer_payment.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c customer_payment.c

inventory_sale.o: inventory_sale.c inventory_sale.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c inventory_sale.c

subclassification.o: subclassification.c subclassification.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c subclassification.c

purchase.o: purchase.c purchase.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c purchase.c

work.o: work.c work.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c work.c

predictive.o: predictive.c predictive.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c predictive.c

investment_account.o: investment_account.c investment_account.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c investment_account.c

account_balance.o: account_balance.c account_balance.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c account_balance.c

spreadsheet.o: spreadsheet.c spreadsheet.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c spreadsheet.c

statement.o: statement.c statement.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c statement.c

paypal.o: paypal.c paypal.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c paypal.c

paypal_dataset.o: paypal_dataset.c paypal_dataset.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c paypal_dataset.c

paypal_item.o: paypal_item.c paypal_item.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c paypal_item.c

spreadsheet_column.o: spreadsheet_column.c spreadsheet_column.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c spreadsheet_column.c

spreadsheet_upload_event.o:						\
		spreadsheet_upload_event.c				\
		spreadsheet_upload_event.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c spreadsheet_upload_event.c

inventory_purchase.o: inventory_purchase.c inventory_purchase.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c inventory_purchase.c

supply_purchase.o: supply_purchase.c supply_purchase.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c supply_purchase.c

supply.o: supply.c supply.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c supply.c

specific_inventory.o: specific_inventory.c specific_inventory.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c specific_inventory.c

specific_inventory_purchase.o:						\
		specific_inventory_purchase.c				\
		specific_inventory_purchase.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c specific_inventory_purchase.c

prepaid_asset.o: prepaid_asset.c prepaid_asset.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c prepaid_asset.c

prepaid_asset_purchase.o:	prepaid_asset_purchase.c		\
				prepaid_asset_purchase.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c prepaid_asset_purchase.c


# $APPASERVER_HOME/src_predictive/makefile
# ----------------------------------------

CC=gcc
WARN=-Wall -Wextra -Wunused-parameter
CFLAGS=-O3 -s ${WARN}

LIBRARY=${APPASERVER_HOME}/library

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

SOURCE_LIBRARY= 							\
		${LIBRARY}/html.o					\
		${LIBRARY}/latex.o					\
		${LIBRARY}/semaphore.o					\
		${LIBRARY}/entity_self.o				\
		${LIBRARY}/dollar.o					\
		predictive.o						\
		subclassification.o					\
		transaction.o						\
		transaction_date.o					\
		account.o						\
		statement.o						\
		element.o						\
		journal.o						\
		journal_propagate.o

PAYPAL_LIBRARY= 							\
		paypal.o						\
		paypal_dataset.o					\
		paypal_item.o						\
		paypal_spreadsheet.o					\
		paypal_spreadsheet_column.o				\
		paypal_spreadsheet_load_event.o

SALE_LIBRARY=								\
		sale.o							\
		sale_fetch.o						\
		customer.o						\
		self_tax.o						\
		inventory_sale.o					\
		specific_inventory_sale.o				\
		fixed_service_sale.o					\
		fixed_service_work.o					\
		hourly_service_sale.o					\
		hourly_service_work.o					\
		customer_payment.o					\
		subsidiary_transaction.o				\
		subsidiary_transaction_state.o				\
		customer_payment_transaction.o				\
		sale_transaction.o					\
		sale_loss_transaction.o
CLOSE_LIBRARY= 								\
		close_nominal.o						\
		close_equity.o						\
		equity_journal.o					\
		close_account.o						\
		close_transaction.o					\
		reverse_nominal.o					\
		reverse_transaction.o

EXECUTABLES=	trial_balance_output					\
		income_statement_output					\
		balance_sheet_output					\
		budget_report						\
		ledger_debit_credit_audit				\
		ledger_propagate					\
		feeder_upload						\
		feeder_upload_csv					\
		feeder_init_execute					\
		feeder_init_csv_execute					\
		journal_trigger						\
		tax_form_report						\
		close_nominal_accounts					\
		reverse_nominal_accounts				\
		populate_print_checks_entity				\
		pay_liabilities_process					\
		depreciate_fixed_assets					\
		post_accrual						\
		transaction_html_display				\
		post_change_self					\
		post_change_account_balance				\
		prior_fixed_asset_trigger				\
		investment_transaction_driver				\
		generate_invoice					\
		import_predictbooks					\
		post_change_customer_sale				\
		hourly_service_sale_trigger				\
		post_change_customer_payment				\
		post_change_hourly_service_work				\
		post_change_fixed_service_sale				\
		post_change_fixed_service_work
#		post_change_vendor_payment				\
#		cost_recover_fixed_assets				\
#		recovery_statutory_accelerated_half_year		\
#		liability_prepaid					\
#		update_sale_completed					\
#		collect_customer_payment				\
#		inventory_balance_detail				\
#		inventory_cost_method_propagate				\
#		load_receipt_scan					\
#		post_change_activity_work				\
#		post_change_employee_work_day				\
#		post_change_fixed_asset					\
#		post_change_inventory_purchase				\
#		post_change_inventory_sale				\
#		post_change_inventory_purchase_return			\
#		post_change_inventory_sale_return			\
#		post_change_prepaid_asset_accrual			\
#		post_change_prepaid_asset_purchase			\
#		post_change_prior_fixed_asset				\
#		post_change_purchase_order				\
#		post_change_service_purchase				\
#		post_change_supply_purchase				\
#		propagate_customer_sale_accounts			\
#		propagate_inventory_purchase_layers			\
#		propagate_inventory_sale_layers				\
#		propagate_purchase_order_accounts			\
#		shortcut_service_work

#all: 	${SOURCE_LIBRARY} ${PAYPAL_LIBRARY} ${EXECUTABLES}
all: 	${SOURCE_LIBRARY} ${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f *.o ${EXECUTABLES}					\
		financial_position					\
		statement_of_activities					\
		generate_workorder					\
		update_sale_not_completed

post_accrual: post_accrual.c						\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	accrual.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_accrual.c						\
		accrual.o						\
		-o post_accrual

transaction_html_display: transaction_html_display.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		transaction_html_display.c				\
		-o transaction_html_display

post_payroll: post_payroll.c						\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_payroll.c						\
		-o post_payroll

shortcut_service_work: shortcut_service_work.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		shortcut_service_work.c					\
		-o shortcut_service_work

load_receipt_scan: load_receipt_scan.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		load_receipt_scan.c					\
		-o load_receipt_scan

ledger_debit_credit_audit: ledger_debit_credit_audit.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		ledger_debit_credit_audit.c				\
		-o ledger_debit_credit_audit

depreciate_fixed_assets: depreciate_fixed_assets.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	depreciation.o							\
	fixed_asset_purchase.o						\
	recovery.o							\
	fixed_asset.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		depreciate_fixed_assets.c				\
		depreciation.o						\
		fixed_asset_purchase.o					\
		recovery.o						\
		fixed_asset.o						\
		-o depreciate_fixed_assets

cost_recover_fixed_assets: cost_recover_fixed_assets.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		cost_recover_fixed_assets.c				\
		-o cost_recover_fixed_assets

trial_balance_output: trial_balance_output.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	trial_balance.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		trial_balance_output.c					\
		trial_balance.o						\
		-o trial_balance_output

tax_form_report: tax_form_report.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	tax_form.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		tax_form_report.c					\
		tax_form.o						\
		-o tax_form_report

program_profit: program_profit.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		program_profit.c					\
		-o program_profit

budget_report: budget_report.c						\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	budget.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		budget_report.c						\
		budget.o						\
		-o budget_report

income_statement_output: income_statement_output.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	income_statement.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		income_statement_output.c				\
		income_statement.o					\
		-o income_statement_output
	touch statement_of_activities
	/bin/rm -f statement_of_activities
	ln income_statement_output statement_of_activities

balance_sheet_output: balance_sheet_output.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	balance_sheet.o							\
	income_statement.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		balance_sheet_output.c					\
		balance_sheet.o						\
		income_statement.o					\
		-o balance_sheet_output
	touch financial_position
	/bin/rm -f financial_position
	ln balance_sheet_output financial_position

close_nominal_accounts: close_nominal_accounts.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	${CLOSE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		${CLOSE_LIBRARY}					\
		close_nominal_accounts.c				\
		-o close_nominal_accounts

reverse_nominal_accounts: reverse_nominal_accounts.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	${CLOSE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		${CLOSE_LIBRARY}					\
		reverse_nominal_accounts.c				\
		-o reverse_nominal_accounts

import_predictbooks: import_predictbooks.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	${LIBRARY}/export_table.o					\
	${LIBRARY}/shell_script.o					\
	${LIBRARY}/export_process.o					\
	import_predict.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		import_predictbooks.c					\
		${LIBRARY}/export_table.o				\
		${LIBRARY}/shell_script.o				\
		${LIBRARY}/export_process.o				\
		import_predict.o					\
		-o import_predictbooks

populate_print_checks_entity: populate_print_checks_entity.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	liability.o							\
	receivable.o							\
	check.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		populate_print_checks_entity.c				\
		liability.o						\
		receivable.o						\
		check.o							\
		-o populate_print_checks_entity

journal_trigger: journal_trigger.c					\
	${APPASERVER_LIBRRAY} ${SOURCE_LIBRARY}				\
	${LIBRARY}/preupdate_change.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		journal_trigger.c					\
		${LIBRARY}/preupdate_change.o				\
		-o journal_trigger

pay_liabilities_process: pay_liabilities_process.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	liability.o							\
	receivable.o							\
	check.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		pay_liabilities_process.c				\
		liability.o						\
		receivable.o						\
		check.o							\
		-o pay_liabilities_process

liability_prepaid: liability_prepaid.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		liability_prepaid.c					\
		-o liability_prepaid

ledger_propagate: ledger_propagate.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		ledger_propagate.c					\
		-o ledger_propagate

feeder_upload: feeder_upload.c						\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	exchange.o							\
	feeder.o							\
	feeder_account.o						\
	feeder_load_event.o						\
	feeder_audit.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		feeder_upload.c						\
		exchange.o						\
		feeder.o						\
		feeder_account.o					\
		feeder_load_event.o					\
		feeder_audit.o						\
		-o feeder_upload

feeder_upload_csv: feeder_upload_csv.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	exchange.o							\
	exchange_csv.o							\
	feeder.o							\
	feeder_account.o						\
	feeder_load_event.o						\
	feeder_audit.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		feeder_upload_csv.c					\
		exchange.o						\
		exchange_csv.o						\
		feeder.o						\
		feeder_account.o					\
		feeder_load_event.o					\
		feeder_audit.o						\
		-o feeder_upload_csv

feeder_init_execute: feeder_init_execute.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	exchange.o							\
	feeder.o							\
	feeder_audit.o							\
	feeder_account.o						\
	feeder_load_event.o						\
	feeder_init.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		feeder_init_execute.c					\
		exchange.o						\
		feeder.o						\
		feeder_audit.o						\
		feeder_account.o					\
		feeder_load_event.o					\
		feeder_init.o						\
		-o feeder_init_execute

feeder_init_csv_execute: feeder_init_csv_execute.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	exchange.o							\
	exchange_csv.o							\
	feeder.o							\
	feeder_audit.o							\
	feeder_account.o						\
	feeder_load_event.o						\
	feeder_init.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		feeder_init_csv_execute.c				\
		exchange.o						\
		exchange_csv.o						\
		feeder.o						\
		feeder_audit.o						\
		feeder_account.o					\
		feeder_load_event.o					\
		feeder_init.o						\
		-o feeder_init_csv_execute

post_change_prepaid_asset_accrual: post_change_prepaid_asset_accrual.c	\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_prepaid_asset_accrual.c			\
		-o post_change_prepaid_asset_accrual

post_change_inventory_purchase: post_change_inventory_purchase.c	\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_inventory_purchase.c			\
		-o post_change_inventory_purchase

post_change_inventory_purchase_return:					\
	post_change_inventory_purchase_return.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_inventory_purchase_return.c			\
		-o post_change_inventory_purchase_return

post_change_inventory_sale_return:					\
	post_change_inventory_sale_return.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_inventory_sale_return.c			\
		-o post_change_inventory_sale_return

post_change_specific_inventory_purchase:				\
	post_change_specific_inventory_purchase.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_specific_inventory_purchase.c		\
		-o post_change_specific_inventory_purchase

post_change_supply_purchase: post_change_supply_purchase.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_supply_purchase.c				\
		-o post_change_supply_purchase

post_change_activity_work:						\
	post_change_activity_work.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		post_change_activity_work.c				\
		-o post_change_activity_work

post_change_service_purchase: post_change_service_purchase.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_service_purchase.c				\
		-o post_change_service_purchase

prior_fixed_asset_trigger:						\
	prior_fixed_asset_trigger.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	${SALE_LIBRARY}							\
	${LIBRARY}/preupdate_change.o					\
	prior_fixed_asset.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o				\
		prior_fixed_asset_trigger.c				\
		prior_fixed_asset.o					\
		-o prior_fixed_asset_trigger

post_change_prepaid_asset_purchase:					\
	post_change_prepaid_asset_purchase.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_prepaid_asset_purchase.c			\
		-o post_change_prepaid_asset_purchase

post_change_fixed_asset:						\
	post_change_fixed_asset.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_fixed_asset.c				\
		-o post_change_fixed_asset

propagate_inventory_purchase_layers:					\
	propagate_inventory_purchase_layers.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		propagate_inventory_purchase_layers.c			\
		-o propagate_inventory_purchase_layers

propagate_inventory_sale_layers:					\
	propagate_inventory_sale_layers.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		propagate_inventory_sale_layers.c			\
		-o propagate_inventory_sale_layers

inventory_balance_detail:						\
	inventory_balance_detail.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		inventory_balance_detail.c				\
		-o inventory_balance_detail

post_change_inventory_sale: post_change_inventory_sale.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		post_change_inventory_sale.c				\
		-o post_change_inventory_sale

post_change_specific_inventory_sale:					\
	post_change_specific_inventory_sale.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_specific_inventory_sale.c			\
		-o post_change_specific_inventory_sale

post_change_purchase_order: post_change_purchase_order.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_purchase_order.c				\
		-o post_change_purchase_order

post_change_vendor_payment: post_change_vendor_payment.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_vendor_payment.c				\
		-o post_change_vendor_payment

post_change_prior_fixed_asset: post_change_prior_fixed_asset.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	subsidiary_transaction.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		subsidiary_transaction.o				\
		post_change_prior_fixed_asset.c				\
		-o post_change_prior_fixed_asset

post_change_account_balance: post_change_account_balance.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
		investment_account.o					\
		account_balance.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		investment_account.o					\
		account_balance.o					\
		post_change_account_balance.c				\
		-o post_change_account_balance

investment_transaction_driver: investment_transaction_driver.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
		investment_transaction.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		investment_transaction.o				\
		investment_transaction_driver.c				\
		-o investment_transaction_driver

post_change_customer_sale: post_change_customer_sale.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o				\
		post_change_customer_sale.c				\
		-o post_change_customer_sale

hourly_service_sale_trigger: hourly_service_sale_trigger.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o				\
		hourly_service_sale_trigger.c				\
		-o hourly_service_sale_trigger

post_change_fixed_service_sale:						\
	post_change_fixed_service_sale.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o				\
		post_change_fixed_service_sale.c			\
		-o post_change_fixed_service_sale

post_change_hourly_service_work:					\
	post_change_hourly_service_work.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o				\
		post_change_hourly_service_work.c			\
		-o post_change_hourly_service_work

post_change_fixed_service_work:						\
	post_change_fixed_service_work.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o				\
		post_change_fixed_service_work.c			\
		-o post_change_fixed_service_work

post_change_customer_payment: post_change_customer_payment.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		${SALE_LIBRARY}						\
		${LIBRARY}/preupdate_change.o				\
		post_change_customer_payment.c				\
		-o post_change_customer_payment

synchronize_customer_sale_transaction:					\
	synchronize_customer_sale_transaction.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		synchronize_customer_sale_transaction.c			\
		-o synchronize_customer_sale_transaction

update_sale_completed: update_sale_completed.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		update_sale_completed.c					\
		-o update_sale_completed
	touch update_sale_not_completed
	/bin/rm -f update_sale_not_completed
	ln update_sale_completed update_sale_not_completed

collect_customer_payment: collect_customer_payment.c			\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		collect_customer_payment.c				\
		-o collect_customer_payment

make_vendor_payment: make_vendor_payment.c				\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		make_vendor_payment.c					\
		-o make_vendor_payment

post_change_self: post_change_self.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_self.c					\
		-o post_change_self

post_change_employee_work_day: post_change_employee_work_day.c		\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_employee_work_day.c				\
		-o post_change_employee_work_day

inventory_cost_method_propagate: inventory_cost_method_propagate.c	\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		inventory_cost_method_propagate.c			\
		-o inventory_cost_method_propagate

generate_invoice: generate_invoice.c					\
	${APPASERVER_LIBRARY} ${SOURCE_LIBRARY}				\
	invoice.o							\
	invoice_latex.o							\
	customer.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		generate_invoice.c					\
		invoice.o						\
		invoice_latex.o						\
		customer.o						\
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

sale_fetch.o: sale_fetch.c sale_fetch.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c sale_fetch.c

customer_payment_transaction.o:						\
	customer_payment_transaction.c					\
	customer_payment_transaction.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c customer_payment_transaction.c

sale_loss_transaction.o:						\
	sale_loss_transaction.c						\
	sale_loss_transaction.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c sale_loss_transaction.c

sale_transaction.o:							\
	sale_transaction.c						\
	sale_transaction.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c sale_transaction.c

depreciation.o: depreciation.c depreciation.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c depreciation.c

accrual.o: accrual.c accrual.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c accrual.c

budget.o: budget.c budget.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c budget.c

employee.o: employee.c employee.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c employee.c

liability.o: liability.c liability.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c liability.c

receivable.o: receivable.c receivable.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c receivable.c

tax_form.o: tax_form.c tax_form.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c tax_form.c

investment.o: investment.c						\
		investment.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c investment.c

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

fixed_service_work.o: fixed_service_work.c				\
		fixed_service_work.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c fixed_service_work.c

hourly_service_work.o: hourly_service_work.c				\
		hourly_service_work.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c hourly_service_work.c

vendor_payment.o: vendor_payment.c					\
		vendor_payment.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c vendor_payment.c

self_tax.o: self_tax.c self_tax.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c self_tax.c

feeder.o: feeder.c feeder.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c feeder.c

feeder_init.o: feeder_init.c feeder_init.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c feeder_init.c

feeder_account.o: feeder_account.c feeder_account.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c feeder_account.c

feeder_load_event.o: feeder_load_event.c feeder_load_event.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c feeder_load_event.c

feeder_audit.o: feeder_audit.c feeder_audit.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c feeder_audit.c

exchange.o: exchange.c exchange.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c exchange.c

exchange_csv.o: exchange_csv.c exchange_csv.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c exchange_csv.c

recovery.o: recovery.c recovery.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c recovery.c

transaction.o: transaction.c transaction.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c transaction.c

transaction_date.o: transaction_date.c transaction_date.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c transaction_date.c

journal.o: journal.c journal.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c journal.c

journal_propagate.o: journal_propagate.c journal_propagate.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c journal_propagate.c

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

specific_inventory_sale.o:						\
	specific_inventory_sale.c specific_inventory_sale.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c specific_inventory_sale.c

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

statement.o: statement.c statement.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c statement.c

trial_balance.o: trial_balance.c trial_balance.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c trial_balance.c

income_statement.o: income_statement.c income_statement.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c income_statement.c

balance_sheet.o: balance_sheet.c balance_sheet.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c balance_sheet.c

paypal.o: paypal.c paypal.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c paypal.c

paypal_dataset.o: paypal_dataset.c paypal_dataset.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c paypal_dataset.c

paypal_item.o: paypal_item.c paypal_item.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c paypal_item.c

paypal_spreadsheet.o: paypal_spreadsheet.c paypal_spreadsheet.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c paypal_spreadsheet.c

paypal_spreadsheet_column.o:						\
	paypal_spreadsheet_column.c					\
	paypal_spreadsheet_column.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c paypal_spreadsheet_column.c

paypal_spreadsheet_upload_event.o:					\
	paypal_spreadsheet_upload_event.c				\
	paypal_spreadsheet_upload_event.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c paypal_spreadsheet_upload_event.c

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

recovery_statutory_accelerated_half_year:				\
	recovery_statutory_accelerated_half_year.c
	${CC} ${CFLAGS} -I${LIBRARY}					\
		recovery_statutory_accelerated_half_year.c		\
		-o recovery_statutory_accelerated_half_year -lm

close_nominal.o: close_nominal.c close_nominal.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c close_nominal.c

reverse_nominal.o: reverse_nominal.c reverse_nominal.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c reverse_nominal.c

close_transaction.o: close_transaction.c close_transaction.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c close_transaction.c

reverse_transaction.o: reverse_transaction.c reverse_transaction.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c reverse_transaction.c

close_equity.o: close_equity.c close_equity.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c close_equity.c

equity_journal.o: equity_journal.c equity_journal.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c equity_journal.c

close_account.o: close_account.c close_account.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c close_account.c

invoice.o: invoice.c invoice.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c invoice.c

invoice_latex.o: invoice_latex.c invoice_latex.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c invoice_latex.c

customer.o: customer.c customer.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c customer.c

check.o: check.c check.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c check.c

subsidiary_transaction.o:						\
	subsidiary_transaction.c					\
	subsidiary_transaction.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c subsidiary_transaction.c

subsidiary_transaction_state.o:						\
	subsidiary_transaction_state.c					\
	subsidiary_transaction_state.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c subsidiary_transaction_state.c

prior_fixed_asset.o:							\
	prior_fixed_asset.c						\
	prior_fixed_asset.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c prior_fixed_asset.c

import_predict.o: import_predict.c import_predict.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c import_predict.c

investment_transaction.o:						\
	investment_transaction.c					\
	investment_transaction.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c investment_transaction.c


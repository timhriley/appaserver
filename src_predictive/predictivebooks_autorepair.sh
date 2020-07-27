:
# Created by ./export_predictivebooks.sh

table_name=account
echo "create table $table_name (account char (60) not null,subclassification char (35),chart_account_number integer,hard_coded_account_key char (40)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (account);" | sql.e
table_name=activity
echo "create table $table_name (activity char (30) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (activity);" | sql.e
table_name=activity_work
echo "create table $table_name (login_name char (50) not null,begin_work_date_time datetime not null,end_work_date_time datetime,activity char (30),work_hours double (6,2),full_name char (60),street_address char (40),sale_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (login_name,begin_work_date_time);" | sql.e
table_name=bank_upload
echo "create table $table_name (bank_date date not null,bank_description char (140) not null,sequence_number integer,bank_amount double (10,2),bank_running_balance double (12,2),bank_upload_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (bank_date,bank_description);" | sql.e
table_name=bank_upload_activity_work
echo "create table $table_name (login_name char (50) not null,begin_work_date_time datetime not null,end_work_date_time datetime,bank_upload_date_time datetime,role char (25),activity char (30),work_hours double (6,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (login_name,begin_work_date_time);" | sql.e
table_name=bank_upload_archive
echo "create table $table_name (bank_date date not null,bank_description char (140) not null,sequence_number integer,bank_amount double (10,2),bank_running_balance double (12,2),bank_upload_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (bank_date,bank_description);" | sql.e
table_name=bank_upload_event
echo "create table $table_name (bank_upload_date_time datetime not null,login_name char (50),completed_date_time datetime,bank_upload_filename char (80),file_sha256sum char (64),feeder_account char (40)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (bank_upload_date_time);" | sql.e
echo "create unique index ${table_name}_additional_unique on $table_name (file_sha256sum);" | sql.e
table_name=transaction
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,transaction_date_time datetime not null,memo char (60),transaction_amount double (10,2),check_number integer,lock_transaction_yn char (1)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,transaction_date_time);" | sql.e
echo "create unique index ${table_name}_additional_unique on $table_name (transaction_date_time);" | sql.e
table_name=journal_ledger
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,transaction_date_time datetime not null,account char (60) not null,transaction_count integer,previous_balance double (10,2),debit_amount double (10,2),credit_amount double (10,2),balance double (11,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,transaction_date_time,account);" | sql.e
table_name=bank_upload_transaction
echo "create table $table_name (bank_date date not null,bank_description char (140) not null,full_name char (60) not null,street_address char (40) not null,transaction_date_time datetime not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (bank_date,bank_description,full_name,street_address,transaction_date_time);" | sql.e
echo "create index ${table_name}_full_name on $table_name (full_name);" | sql.e
echo "create index ${table_name}_street_address on $table_name (street_address);" | sql.e
echo "create index ${table_name}_transaction_date_time on $table_name (transaction_date_time);" | sql.e
table_name=bank_upload_transaction_balance
echo "create view bank_upload_transaction_balance as select transaction.transaction_date_time ,bank_upload. bank_date, bank_upload.bank_description, transaction.full_name, transaction.street_address, transaction_amount, bank_amount, journal_ledger.balance cash_running_balance, bank_running_balance, sequence_number from bank_upload_transaction, transaction, bank_upload, journal_ledger, account where bank_upload_transaction.full_name = transaction.full_name and bank_upload_transaction.street_address = transaction.street_address and bank_upload_transaction.transaction_date_time = transaction.transaction_date_time and bank_upload_transaction.bank_date = bank_upload.bank_date and bank_upload_transaction.bank_description = bank_upload.bank_description and transaction.full_name = journal_ledger.full_name and transaction.street_address = journal_ledger.street_address and transaction.transaction_date_time = journal_ledger.transaction_date_time and journal_ledger.account = account.account and account.hard_coded_account_key = 'cash_key';" | sql.e
table_name=bank_upload_feeder_archive
echo "create view bank_upload_feeder_archive as select bank_date, bank_description, feeder_account, sequence_number, bank_amount, bank_running_balance, bank_upload_event.bank_upload_date_time from bank_upload_event, bank_upload_archive where bank_upload_event.bank_upload_date_time = bank_upload_archive.bank_upload_date_time;" | sql.e
table_name=contra_account
echo "create table $table_name (account char (60) not null,contra_to_account char (60)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (account);" | sql.e
table_name=customer
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,sales_tax_exempt_yn char (1)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e
table_name=customer_payment
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,sale_date_time datetime not null,payment_date_time datetime not null,payment_amount double (10,2),check_number integer,transaction_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,payment_date_time);" | sql.e
table_name=customer_sale
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,sale_date_time datetime not null,vehicle_make char (20),vehicle_model char (20),vehicle_trim char (20),vehicle_year integer,odometer_miles integer,symptom text,mechanic_full_name char (60),mechanic_street_address char (40),mechanic_notes text,shipping_revenue double (8,2),sum_extension double (10,2),sales_tax double (7,2),invoice_amount double (10,2),total_payment double (10,2),amount_due double (10,2),completed_date_time datetime,uncollectible_writeoff_date date,transaction_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time);" | sql.e
table_name=customer_vehicle
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,vehicle_make char (20) not null,vehicle_model char (20) not null,vehicle_trim char (20) not null,vehicle_year integer not null,vehicle_identification_number char (20)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,vehicle_make,vehicle_model,vehicle_trim,vehicle_year);" | sql.e
table_name=day
echo "create table $table_name (day char (9) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (day);" | sql.e
table_name=depreciation_method
echo "create table $table_name (depreciation_method char (25) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (depreciation_method);" | sql.e
table_name=element
echo "create table $table_name (element char (20) not null,accumulate_debit_yn char (1)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (element);" | sql.e
table_name=employee
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,hourly_wage double (10,2),period_salary double (10,2),commission_sum_extension_percent double (10,2),gross_pay_year_to_date double (10,2),net_pay_year_to_date double (10,2),federal_marital_status char (25),federal_withholding_allowances integer,federal_withholding_additional_period_amount integer,state_marital_status char (40),state_withholding_allowances integer,state_itemized_deduction_allowances integer,retirement_contribution_plan_employer_period_amount integer,retirement_contribution_plan_employee_period_amount integer,health_insurance_employer_period_amount integer,health_insurance_employee_period_amount integer,union_dues_period_amount integer,terminated_date date) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e
table_name=employee_work_day
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,begin_work_date_time datetime not null,end_work_date_time datetime,employee_work_hours double (6,2),overtime_work_day_yn char (1),insert_timestamp datetime,update_timestamp datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,begin_work_date_time);" | sql.e
table_name=employee_work_period
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,payroll_year integer not null,payroll_period_number integer not null,begin_work_date date,end_work_date date,regular_work_hours double (6,2),overtime_work_hours double (6,2),gross_pay double (10,2),net_pay double (10,2),payroll_tax_amount double (10,2),commission_sum_extension double (10,2),federal_tax_withholding_amount double (10,2),state_tax_withholding_amount double (10,2),social_security_employee_tax_amount double (10,2),social_security_employer_tax_amount double (10,2),medicare_employee_tax_amount double (10,2),medicare_employer_tax_amount double (10,2),retirement_contribution_plan_employee_amount integer,retirement_contribution_plan_employer_amount integer,health_insurance_employee_amount integer,health_insurance_employer_amount integer,federal_unemployment_tax_amount double (10,2),state_unemployment_tax_amount double (10,2),union_dues_amount integer,transaction_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,payroll_year,payroll_period_number);" | sql.e
table_name=entity
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,login_name char (50),city char (20),state_code char (2),zip_code char (10),phone_number char (15),cell_number char (15),email_address char (50)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e
echo "create unique index ${table_name}_additional_unique on $table_name (login_name);" | sql.e
table_name=equity_account_balance
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,account_number char (50) not null,date_time datetime not null,investment_operation char (15),transaction_date_time datetime,share_price double (12,4),share_quantity_change double (12,4),share_quantity_balance double (13,4),book_value_change double (12,4),book_value_balance double (13,4),moving_share_price double (13,5),total_cost_balance double (14,5),market_value double (13,4),unrealized_gain_balance double (12,4),unrealized_gain_change double (12,4),realized_gain double (12,4)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,account_number,date_time);" | sql.e
table_name=federal_income_tax_withholding
echo "create table $table_name (federal_marital_status char (25) not null,income_over integer not null,income_not_over integer,tax_fixed_amount double (10,2),tax_percentage_amount double (5,1)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (federal_marital_status,income_over);" | sql.e
table_name=federal_marital_status
echo "create table $table_name (federal_marital_status char (25) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (federal_marital_status);" | sql.e
table_name=feeder_account
echo "create table $table_name (feeder_account char (40) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (feeder_account);" | sql.e
table_name=financial_institution
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e
table_name=fixed_asset
echo "create table $table_name (asset_name char (60) not null,account char (60)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (asset_name);" | sql.e
table_name=fixed_asset_depreciation
echo "create table $table_name (asset_name char (60) not null,serial_number char (10) not null,depreciation_date date not null,full_name char (60),street_address char (40),depreciation_amount double (10,2),transaction_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (asset_name,serial_number,depreciation_date);" | sql.e
table_name=fixed_asset_purchase
echo "create table $table_name (asset_name char (60) not null,serial_number char (10) not null,full_name char (60),street_address char (40),purchase_date_time datetime,extension double (10,2),estimated_useful_life_years integer,estimated_useful_life_units integer,estimated_residual_value double (10,2),declining_balance_n integer,depreciation_method char (25),tax_cost_basis double (12,2),service_placement_date date,tax_recovery_period char (10),disposal_date date,finance_accumulated_depreciation double (10,2),tax_accumulated_depreciation double (10,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (asset_name,serial_number);" | sql.e
echo "create index ${table_name}_full_name on $table_name (full_name);" | sql.e
echo "create index ${table_name}_street_address on $table_name (street_address);" | sql.e
echo "create index ${table_name}_purchase_date_time on $table_name (purchase_date_time);" | sql.e
table_name=fixed_service
echo "create table $table_name (service_name char (30) not null,retail_price double (10,2),service_category char (30),credit_account char (60)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (service_name);" | sql.e
table_name=fixed_service_category
echo "create table $table_name (service_category char (30) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (service_category);" | sql.e
table_name=fixed_service_sale
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,sale_date_time datetime not null,service_name char (30) not null,retail_price double (10,2),discount_amount double (10,2),work_hours double (6,2),extension double (10,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,service_name);" | sql.e
table_name=fixed_service_work
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,sale_date_time datetime not null,service_name char (30) not null,begin_work_date_time datetime not null,end_work_date_time datetime,login_name char (50),work_hours double (6,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,service_name,begin_work_date_time);" | sql.e
table_name=hourly_service
echo "create table $table_name (service_name char (30) not null,hourly_rate double (7,2),service_category char (30),credit_account char (60)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (service_name);" | sql.e
table_name=hourly_service_category
echo "create table $table_name (service_category char (30) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (service_category);" | sql.e
table_name=hourly_service_sale
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,sale_date_time datetime not null,service_name char (30) not null,description char (60) not null,estimated_hours double (6,2),hourly_rate double (7,2),work_hours double (6,2),extension double (10,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,service_name,description);" | sql.e
table_name=hourly_service_work
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,sale_date_time datetime not null,service_name char (30) not null,description char (60) not null,begin_work_date_time datetime not null,end_work_date_time datetime,login_name char (50),work_hours double (6,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,service_name,description,begin_work_date_time);" | sql.e
table_name=inventory
echo "create table $table_name (inventory_name char (30) not null,inventory_account char (60),cost_of_goods_sold_account char (60),retail_price double (10,2),inventory_category char (30),reorder_quantity integer,quantity_on_hand integer,average_unit_cost double (12,4),total_cost_balance double (14,5)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (inventory_name);" | sql.e
table_name=inventory_category
echo "create table $table_name (inventory_category char (30) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (inventory_category);" | sql.e
table_name=inventory_cost_method
echo "create table $table_name (inventory_cost_method char (15) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (inventory_cost_method);" | sql.e
table_name=inventory_purchase
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,purchase_date_time datetime not null,inventory_name char (30) not null,ordered_quantity integer,arrived_quantity integer,missing_quantity integer,quantity_on_hand integer,capitalized_unit_cost double (10,4),unit_cost double (10,2),extension double (10,2),average_unit_cost double (12,4)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time,inventory_name);" | sql.e
table_name=inventory_sale
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,sale_date_time datetime not null,inventory_name char (30) not null,quantity integer,retail_price double (10,2),discount_amount double (10,2),extension double (10,2),cost_of_goods_sold double (10,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,inventory_name);" | sql.e
table_name=investment_account
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,account_number char (50) not null,classification char (15),investment_account char (60),fair_value_adjustment_account char (60),certificate_maturity_months integer,certificate_maturity_date date,interest_rate double (5,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,account_number);" | sql.e
table_name=investment_classification
echo "create table $table_name (classification char (15) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (classification);" | sql.e
table_name=investment_operation
echo "create table $table_name (investment_operation char (15) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (investment_operation);" | sql.e
table_name=liability_account_entity
echo "create table $table_name (account char (60) not null,full_name char (60),street_address char (40)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (account);" | sql.e
table_name=mechanic
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e
table_name=payroll_pay_period
echo "create table $table_name (payroll_pay_period char (15) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (payroll_pay_period);" | sql.e
table_name=payroll_posting
echo "create table $table_name (payroll_year integer not null,payroll_period_number integer not null,begin_work_date date,end_work_date date,regular_work_hours double (6,2),overtime_work_hours double (6,2),gross_pay double (10,2),net_pay double (10,2),payroll_tax_amount double (10,2),commission_sum_extension double (10,2),federal_tax_withholding_amount double (10,2),state_tax_withholding_amount double (10,2),social_security_employee_tax_amount double (10,2),social_security_employer_tax_amount double (10,2),medicare_employee_tax_amount double (10,2),medicare_employer_tax_amount double (10,2),retirement_contribution_plan_employee_amount integer,retirement_contribution_plan_employer_amount integer,health_insurance_employee_amount integer,health_insurance_employer_amount integer,federal_unemployment_tax_amount double (10,2),state_unemployment_tax_amount double (10,2),union_dues_amount integer) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (payroll_year,payroll_period_number);" | sql.e
table_name=prepaid_asset
echo "create table $table_name (asset_name char (60) not null,asset_account char (60),expense_account char (60)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (asset_name);" | sql.e
table_name=prepaid_asset_accrual
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,purchase_date_time datetime not null,accrual_date date not null,asset_name char (60) not null,accrual_amount double (8,2),transaction_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time,accrual_date,asset_name);" | sql.e
table_name=prepaid_asset_purchase
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,purchase_date_time datetime not null,asset_name char (60) not null,extension double (10,2),accrual_period_years double (4,2),accumulated_accrual double (8,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time,asset_name);" | sql.e
table_name=prior_fixed_asset
echo "create table $table_name (asset_name char (60) not null,serial_number char (10) not null,extension double (10,2),estimated_useful_life_years integer,estimated_useful_life_units integer,estimated_residual_value double (10,2),declining_balance_n integer,depreciation_method char (25),tax_cost_basis double (12,2),service_placement_date date,tax_recovery_period char (10),disposal_date date,full_name char (60),street_address char (40),transaction_date_time datetime,finance_accumulated_depreciation double (10,2),tax_accumulated_depreciation double (10,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (asset_name,serial_number);" | sql.e
table_name=prior_fixed_asset_depreciation
echo "create table $table_name (asset_name char (60) not null,serial_number char (10) not null,depreciation_date date not null,full_name char (60),street_address char (40),units_produced integer,depreciation_amount double (10,2),transaction_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (asset_name,serial_number,depreciation_date);" | sql.e
table_name=prior_property
echo "create table $table_name (property_street_address char (40) not null,service_placement_date date,structure_cost double (14,2),land_cost double (14,2),estimated_useful_life_years integer,estimated_residual_value double (10,2),declining_balance_n integer,depreciation_method char (25),tax_cost_basis double (12,2),tax_recovery_period char (10),disposal_date date,full_name char (60),street_address char (40),transaction_date_time datetime,finance_accumulated_depreciation double (10,2),tax_accumulated_depreciation double (10,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (property_street_address);" | sql.e
table_name=prior_property_depreciation
echo "create table $table_name (property_street_address char (40) not null,depreciation_date date not null,full_name char (60),street_address char (40),depreciation_amount double (10,2),transaction_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (property_street_address,depreciation_date);" | sql.e
table_name=property
echo "create table $table_name (property_street_address char (40) not null,city char (20),state_code char (2),zip_code char (10),account char (60)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (property_street_address);" | sql.e
table_name=property_depreciation
echo "create table $table_name (property_street_address char (40) not null,depreciation_date date not null,full_name char (60),street_address char (40),depreciation_amount double (10,2),transaction_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (property_street_address,depreciation_date);" | sql.e
table_name=property_purchase
echo "create table $table_name (property_street_address char (40) not null,full_name char (60),street_address char (40),purchase_date_time datetime,service_placement_date date,structure_cost double (14,2),land_cost double (14,2),estimated_useful_life_years integer,estimated_residual_value double (10,2),declining_balance_n integer,depreciation_method char (25),tax_cost_basis double (12,2),tax_recovery_period char (10),disposal_date date,finance_accumulated_depreciation double (10,2),tax_accumulated_depreciation double (10,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (property_street_address);" | sql.e
echo "create index ${table_name}_full_name on $table_name (full_name);" | sql.e
echo "create index ${table_name}_street_address on $table_name (street_address);" | sql.e
echo "create index ${table_name}_purchase_date_time on $table_name (purchase_date_time);" | sql.e
table_name=purchase_order
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,purchase_date_time datetime not null,amount_due double (10,2),sum_extension double (10,2),sales_tax double (7,2),freight_in double (6,2),purchase_amount double (10,2),shipped_date date,arrived_date_time datetime,transaction_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time);" | sql.e
table_name=receipt_upload
echo "create table $table_name (receipt_upload_filename char (80) not null,upload_date date,posted_yn char (1)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (receipt_upload_filename);" | sql.e
table_name=reoccurring_transaction
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,transaction_description char (20) not null,debit_account char (60),credit_account char (60),transaction_amount double (10,2),bank_upload_feeder_phrase char (50),accrued_daily_amount double (10,2),accrued_monthly_amount double (10,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,transaction_description);" | sql.e
table_name=role_activity
echo "create table $table_name (role char (25) not null,activity char (30) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (role,activity);" | sql.e
table_name=self
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,inventory_cost_method char (15),payroll_pay_period char (15),payroll_beginning_day char (9),social_security_combined_tax_rate double (10,4),social_security_payroll_ceiling integer,medicare_combined_tax_rate double (10,4),medicare_additional_withholding_rate double (5,3),medicare_additional_gross_pay_floor integer,federal_withholding_allowance_period_value double (10,2),federal_nonresident_withholding_income_premium double (7,2),state_withholding_allowance_period_value double (7,2),state_itemized_allowance_period_value double (10,2),federal_unemployment_wage_base integer,federal_unemployment_tax_standard_rate double (10,4),federal_unemployment_threshold_rate double (10,4),federal_unemployment_tax_minimum_rate double (10,4),state_unemployment_wage_base integer,state_unemployment_tax_rate double (10,4),state_sales_tax_rate double (6,4)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e
table_name=service_purchase
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,purchase_date_time datetime not null,account char (60) not null,service_description text,extension double (10,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time,account);" | sql.e
table_name=state_income_tax_withholding
echo "create table $table_name (state_marital_status char (40) not null,income_over integer not null,income_not_over integer,tax_fixed_amount double (10,2),tax_percentage_amount double (5,1)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (state_marital_status,income_over);" | sql.e
table_name=state_marital_status
echo "create table $table_name (state_marital_status char (40) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (state_marital_status);" | sql.e
table_name=state_standard_deduction_table
echo "create table $table_name (state_marital_status char (40) not null,state_withholding_allowances integer not null,state_standard_deduction_amount double (8,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (state_marital_status,state_withholding_allowances);" | sql.e
table_name=subclassification
echo "create table $table_name (subclassification char (35) not null,element char (20),display_order integer) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (subclassification);" | sql.e
table_name=subsidiary_transaction
echo "create table $table_name (folder char (35) not null,attribute char (60),debit_account char (60),debit_account_folder char (50),credit_account char (60)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (folder);" | sql.e
table_name=supply
echo "create table $table_name (supply_name char (30) not null,account char (60)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (supply_name);" | sql.e
table_name=supply_purchase
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,purchase_date_time datetime not null,supply_name char (30) not null,quantity integer,unit_cost double (10,2),extension double (10,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time,supply_name);" | sql.e
table_name=tax_fixed_asset_recovery
echo "create table $table_name (asset_name char (60) not null,serial_number char (10) not null,tax_year integer not null,recovery_percent double (6,3),recovery_amount double (6,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (asset_name,serial_number,tax_year);" | sql.e
table_name=tax_form
echo "create table $table_name (tax_form char (20) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (tax_form);" | sql.e
table_name=tax_form_line
echo "create table $table_name (tax_form char (20) not null,tax_form_line char (5) not null,tax_form_description char (35),itemize_accounts_yn char (1)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (tax_form,tax_form_line);" | sql.e
table_name=tax_form_line_account
echo "create table $table_name (tax_form char (20) not null,tax_form_line char (5) not null,account char (60) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (tax_form,tax_form_line,account);" | sql.e
table_name=tax_prior_fixed_asset_recovery
echo "create table $table_name (asset_name char (60) not null,serial_number char (10) not null,tax_year integer not null,recovery_percent double (6,3),recovery_amount double (6,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (asset_name,serial_number,tax_year);" | sql.e
table_name=tax_prior_property_recovery
echo "create table $table_name (property_street_address char (40) not null,tax_year integer not null,recovery_percent double (6,3),recovery_amount double (6,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (property_street_address,tax_year);" | sql.e
table_name=tax_property_recovery
echo "create table $table_name (property_street_address char (40) not null,tax_year integer not null,recovery_percent double (6,3),recovery_amount double (6,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (property_street_address,tax_year);" | sql.e
table_name=tax_recovery_period
echo "create table $table_name (tax_recovery_period char (10) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (tax_recovery_period);" | sql.e
table_name=tax_recovery_table
echo "create table $table_name (tax_recovery_period char (10) not null,recovery_year integer not null,recovery_percent double (6,3)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (tax_recovery_period,recovery_year);" | sql.e
table_name=title_passage_rule
echo "create table $table_name (title_passage_rule char (15) not null,title_passage_date char (15)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (title_passage_rule);" | sql.e
table_name=vehicle
echo "create table $table_name (vehicle_make char (20) not null,vehicle_model char (20) not null,vehicle_trim char (20) not null,vehicle_year integer not null,assessment text) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (vehicle_make,vehicle_model,vehicle_trim,vehicle_year);" | sql.e
table_name=vehicle_maker
echo "create table $table_name (vehicle_make char (20) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (vehicle_make);" | sql.e
table_name=vendor
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e
table_name=vendor_payment
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,purchase_date_time datetime not null,payment_date_time datetime not null,payment_amount double (10,2),check_number integer,transaction_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time,payment_date_time);" | sql.e
#!/bin/sh
if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "$ . set_database" 1>&2
	exit 1
fi
folder="folder"
relation="relation"
attribute="attribute"
folder_attribute="folder_attribute"
role_folder="role_folder"
row_security_role_update="row_security_role_update"
folder_row_level_restrictions="folder_row_level_restrictions"
subschemas="subschemas"
role_operation="role_operation"
javascript_folders="javascript_folders"
javascript_files="javascript_files"
process="process"
role="role"

(
cat << all_done
delete from $folder where folder = 'account';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,post_change_process,notepad,no_initial_capital_yn,post_change_javascript,exclude_application_export_yn,html_help_file_anchor,create_view_statement,lookup_email_output_yn,appaserver_yn,data_directory,index_directory) values ('account','prompt','5','ledger',null,'populate_account',null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'account';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('account','subclassification','null',null,null,null,null,null,null,null,null,'y',null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('contra_account','account','contra_to_account',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('contra_account','account','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service','account','credit_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service','account','credit_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory','account','cost_of_goods_sold_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory','account','inventory_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('investment_account','account','fair_value_adjustment_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('investment_account','account','investment_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('journal_ledger','account','null',null,null,null,null,'y',null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('liability_account_entity','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prepaid_asset','account','asset_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prepaid_asset','account','expense_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('property','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('reoccurring_transaction','account','credit_account',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('reoccurring_transaction','account','debit_account',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('service_purchase','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('subsidiary_transaction','account','credit_account',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('subsidiary_transaction','account','debit_account',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('supply','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('tax_form_line_account','account','null','1',null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'subclassification';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('subclassification','text','35',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'chart_account_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('chart_account_number','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'hard_coded_account_key';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('hard_coded_account_key','text','40',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'account';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('account','account','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('account','chart_account_number',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('account','hard_coded_account_key',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('account','subclassification',null,'1',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('account','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('account','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('account','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('account','supervisor','update');
delete from $javascript_folders where folder = 'account';
delete from $process where process = 'populate_account';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('populate_account','populate_account.sh \$one2m_folder subclassification',null,null,null,null,null,null,null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('account','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('account','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('account','bookkeeper','ledger_propagate');
insert into $role_operation (folder,role,operation) values ('account','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('account','supervisor','detail');
insert into $role_operation (folder,role,operation) values ('account','supervisor','ledger_propagate');
delete from $folder where folder = 'activity';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,post_change_process,notepad,no_initial_capital_yn,post_change_javascript,exclude_application_export_yn,html_help_file_anchor,create_view_statement,lookup_email_output_yn,appaserver_yn,data_directory,index_directory) values ('activity','table','5','activity',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'activity';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('activity_work','activity','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('role_activity','activity','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'activity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('activity','text','30',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'activity';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('activity','activity','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('activity','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('activity','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('activity','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('activity','supervisor','update');
delete from $javascript_folders where folder = 'activity';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('activity');
insert into $role_operation (folder,role,operation) values ('activity','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('activity','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('activity','supervisor','detail');
delete from $folder where folder = 'activity_work';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,post_change_process,notepad,no_initial_capital_yn,post_change_javascript,exclude_application_export_yn,html_help_file_anchor,create_view_statement,lookup_email_output_yn,appaserver_yn,data_directory,index_directory) values ('activity_work','prompt','5','activity',null,null,'post_change_activity_work',null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'activity_work';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('activity_work','activity','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('activity_work','appaserver_user','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('activity_work','customer_sale','null',null,null,null,null,null,null,null,'y',null,null,null);
delete from $attribute where attribute = 'login_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('login_name','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'begin_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('begin_work_date_time','current_date_time','16',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'end_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('end_work_date_time','date_time','16',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'activity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('activity','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('work_hours','float','6','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sale_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sale_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'activity_work';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('activity_work','activity',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('activity_work','begin_work_date_time','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('activity_work','end_work_date_time',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('activity_work','full_name',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('activity_work','login_name','1',null,null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('activity_work','sale_date_time',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('activity_work','street_address',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('activity_work','work_hours',null,'3',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('activity_work','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('activity_work','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('activity_work','dataentry','insert');
insert into $role_folder (folder,role,permission) values ('activity_work','dataentry','update');
insert into $role_folder (folder,role,permission) values ('activity_work','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('activity_work','supervisor','update');
delete from $javascript_folders where folder = 'activity_work';
delete from $process where process = 'post_change_activity_work';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('post_change_activity_work','post_change_activity_work ignored \$login_name begin_work_date_time \$state',null,null,null,null,null,null,null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('dataentry','n',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('activity');
insert into $role_operation (folder,role,operation) values ('activity_work','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('activity_work','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('activity_work','dataentry','delete');
insert into $role_operation (folder,role,operation) values ('activity_work','dataentry','detail');
insert into $role_operation (folder,role,operation) values ('activity_work','supervisor','detail');
delete from $folder where folder = 'bank_upload';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,post_change_process,notepad,no_initial_capital_yn,post_change_javascript,exclude_application_export_yn,html_help_file_anchor,create_view_statement,lookup_email_output_yn,appaserver_yn,data_directory,index_directory) values ('bank_upload','prompt','5','feeder_upload','y',null,'post_change_bank_upload',null,null,'post_change_bank_upload( ''\$state'', \$row )',null,'bank_upload_sort_help.html',null,null,null,null,null);
delete from $relation where folder = 'bank_upload';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_transaction','bank_upload','null',null,null,null,null,null,null,'y',null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_transaction_balance','bank_upload','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_description';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_description','text','140',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sequence_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sequence_number','integer','9',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_amount','float','10','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'bank_running_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_running_balance','float','12','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'bank_upload_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','bank_amount',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','bank_date','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','bank_description','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','bank_running_balance',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','bank_upload_date_time','0','4','n','n',null,null,'n',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','sequence_number',null,'1',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('bank_upload','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('bank_upload','supervisor','lookup');
insert into $role_folder (folder,role,permission) values ('bank_upload','supervisor','update');
delete from $javascript_folders where folder = 'bank_upload';
insert into $javascript_folders (javascript_filename,folder) values ('post_change_bank_upload.js','bank_upload');
delete from $javascript_files where javascript_filename = 'post_change_bank_upload.js';
insert into $javascript_files (javascript_filename) values ('post_change_bank_upload.js');
delete from $process where process = 'post_change_bank_upload';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('post_change_bank_upload','post_change_bank_upload ignored \$state \$dictionary',null,null,null,null,null,null,null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('feeder_upload');
insert into $role_operation (folder,role,operation) values ('bank_upload','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('bank_upload','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('bank_upload','supervisor','detail');
delete from $folder where folder = 'bank_upload_activity_work';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,post_change_process,notepad,no_initial_capital_yn,post_change_javascript,exclude_application_export_yn,html_help_file_anchor,create_view_statement,lookup_email_output_yn,appaserver_yn,data_directory,index_directory) values ('bank_upload_activity_work','prompt','5','activity',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'bank_upload_activity_work';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_activity_work','appaserver_user','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_activity_work','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_activity_work','role_activity','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'login_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('login_name','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'begin_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('begin_work_date_time','current_date_time','16',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'end_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('end_work_date_time','date_time','16',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_upload_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'role';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('role','text','25',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'activity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('activity','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('work_hours','float','6','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload_activity_work';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','activity',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','bank_upload_date_time',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','begin_work_date_time','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','end_work_date_time',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','login_name','1',null,null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','role',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','work_hours',null,'5',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload_activity_work','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('bank_upload_activity_work','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('bank_upload_activity_work','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('bank_upload_activity_work','supervisor','update');
delete from $javascript_folders where folder = 'bank_upload_activity_work';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('activity');
insert into $role_operation (folder,role,operation) values ('bank_upload_activity_work','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('bank_upload_activity_work','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('bank_upload_activity_work','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('bank_upload_activity_work','supervisor','detail');
delete from $folder where folder = 'bank_upload_archive';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,post_change_process,notepad,no_initial_capital_yn,post_change_javascript,exclude_application_export_yn,html_help_file_anchor,create_view_statement,lookup_email_output_yn,appaserver_yn,data_directory,index_directory) values ('bank_upload_archive','prompt','5','feeder_upload',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'bank_upload_archive';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_archive','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_description';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_description','text','140',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sequence_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sequence_number','integer','9',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_amount','float','10','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'bank_running_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_running_balance','float','12','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'bank_upload_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload_archive';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_amount',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_date','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_description','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_running_balance',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_upload_date_time',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','sequence_number',null,'1',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload_archive','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('bank_upload_archive','supervisor','lookup');
delete from $javascript_folders where folder = 'bank_upload_archive';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('feeder_upload');
insert into $role_operation (folder,role,operation) values ('bank_upload_archive','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('bank_upload_archive','supervisor','detail');
delete from $folder where folder = 'bank_upload_event';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,post_change_process,notepad,no_initial_capital_yn,post_change_javascript,exclude_application_export_yn,html_help_file_anchor,create_view_statement,lookup_email_output_yn,appaserver_yn,data_directory,index_directory) values ('bank_upload_event','prompt','5','feeder_upload','n',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'bank_upload_event';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_event','appaserver_user','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_event','feeder_account','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_activity_work','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_archive','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_upload_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'login_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('login_name','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'completed_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('completed_date_time','date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_upload_filename';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_filename','http_filename','80',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'file_sha256sum';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('file_sha256sum','text','64',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'feeder_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('feeder_account','text','40',null,'External accounts like credit card and Pay Pal that feed in transactions.',null,null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload_event';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','bank_upload_date_time','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','bank_upload_filename',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','completed_date_time',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','feeder_account',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','file_sha256sum',null,'4',null,null,'y',null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','login_name',null,'1',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload_event','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('bank_upload_event','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('bank_upload_event','supervisor','lookup');
insert into $role_folder (folder,role,permission) values ('bank_upload_event','supervisor','update');
delete from $javascript_folders where folder = 'bank_upload_event';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('feeder_upload');
insert into $role_operation (folder,role,operation) values ('bank_upload_event','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('bank_upload_event','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('bank_upload_event','supervisor','detail');
delete from $folder where folder = 'transaction';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,post_change_process,notepad,no_initial_capital_yn,post_change_javascript,exclude_application_export_yn,html_help_file_anchor,create_view_statement,lookup_email_output_yn,appaserver_yn,data_directory,index_directory) values ('transaction','prompt','1','ledger','y',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'transaction';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('transaction','entity','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('transaction','property','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_transaction','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('customer_payment','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('customer_sale','transaction','null',null,null,null,'n',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee_work_period','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('equity_account_balance','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_depreciation','transaction','null',null,null,null,'n',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory_purchase_return','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('journal_ledger','transaction','null','1',null,null,null,'n',null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prepaid_asset_accrual','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset_depreciation','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_property','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_property_depreciation','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('property_depreciation','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('purchase_order','transaction','null',null,null,null,'n',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('vendor_payment','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'memo';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('memo','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'check_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('check_number','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'lock_transaction_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('lock_transaction_yn','text','1',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'transaction';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','check_number',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','lock_transaction_yn',null,'4',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','memo',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','transaction_amount',null,'1',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','transaction_date_time','3',null,null,null,'y',null,null,null,null);
insert into $role_folder (folder,role,permission) values ('transaction','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('transaction','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('transaction','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('transaction','supervisor','update');
insert into $row_security_role_update (folder,attribute_not_null) values ('transaction','lock_transaction_yn');
delete from $javascript_folders where folder = 'transaction';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('transaction','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('transaction','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('transaction','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('transaction','supervisor','detail');
delete from $folder where folder = 'journal_ledger';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,post_change_process,notepad,no_initial_capital_yn,post_change_javascript,exclude_application_export_yn,html_help_file_anchor,create_view_statement,lookup_email_output_yn,appaserver_yn,data_directory,index_directory) values ('journal_ledger','prompt','10','ledger',null,null,'post_change_journal_ledger',null,null,'post_change_journal_ledger( ''\$state'' )',null,null,null,null,null,null,null);
delete from $relation where folder = 'journal_ledger';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('journal_ledger','account','null',null,null,null,null,'y',null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('journal_ledger','transaction','null','1',null,null,null,'n',null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_count';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_count','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'previous_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('previous_balance','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'debit_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('debit_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'credit_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('credit_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('balance','float','11','2',null,null,null,'y','y',null);
delete from $folder_attribute where folder = 'journal_ledger';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','account','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','balance',null,'10','y','y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','credit_amount',null,'9',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','debit_amount',null,'8',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','previous_balance',null,'7','y','y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','transaction_count',null,'1',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','transaction_date_time','3',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('journal_ledger','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('journal_ledger','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('journal_ledger','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('journal_ledger','supervisor','update');
delete from $javascript_folders where folder = 'journal_ledger';
insert into $javascript_folders (javascript_filename,folder) values ('post_change_journal_ledger.js','journal_ledger');
delete from $javascript_files where javascript_filename = 'post_change_journal_ledger.js';
insert into $javascript_files (javascript_filename) values ('post_change_journal_ledger.js');
delete from $process where process = 'post_change_journal_ledger';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('post_change_journal_ledger','post_change_journal_ledger.sh ignored full_name street_address transaction_date_time account preupdate_transaction_date_time preupdate_account',null,null,null,null,null,null,null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('journal_ledger','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('journal_ledger','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('journal_ledger','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('journal_ledger','supervisor','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate


(
cat << all_done2
insert into element (element,accumulate_debit_yn) values ('asset','y');
insert into element (element,accumulate_debit_yn) values ('equity','n');
insert into element (element,accumulate_debit_yn) values ('expense','y');
insert into element (element,accumulate_debit_yn) values ('gain','n');
insert into element (element,accumulate_debit_yn) values ('liability','n');
insert into element (element,accumulate_debit_yn) values ('loss','y');
insert into element (element,accumulate_debit_yn) values ('revenue','n');
insert into subclassification (subclassification,element,display_order) values ('cash','asset','1');
insert into subclassification (subclassification,element,display_order) values ('contributed_capital','equity','18');
insert into subclassification (subclassification,element,display_order) values ('cost_of_goods_sold','expense','12');
insert into subclassification (subclassification,element,display_order) values ('current_liability','liability','6');
insert into subclassification (subclassification,element,display_order) values ('equity','equity','16');
insert into subclassification (subclassification,element,display_order) values ('gain','gain','14');
insert into subclassification (subclassification,element,display_order) values ('inventory','asset','3');
insert into subclassification (subclassification,element,display_order) values ('long-term investment','asset','4');
insert into subclassification (subclassification,element,display_order) values ('long-term liability','liability','7');
insert into subclassification (subclassification,element,display_order) values ('loss','loss','15');
insert into subclassification (subclassification,element,display_order) values ('operating_expense','expense','9');
insert into subclassification (subclassification,element,display_order) values ('property_plant_equipment','asset','5');
insert into subclassification (subclassification,element,display_order) values ('receivable','asset','2');
insert into subclassification (subclassification,element,display_order) values ('retained_earnings','equity','17');
insert into subclassification (subclassification,element,display_order) values ('revenue','revenue','8');
insert into subclassification (subclassification,element,display_order) values ('service_expense','expense','10');
insert into subclassification (subclassification,element,display_order) values ('supply_expense','expense','11');
insert into subclassification (subclassification,element,display_order) values ('tax_expense','expense','13');
insert into account (account,subclassification,hard_coded_account_key) values ('accounting','operating_expense',null);
insert into account (account,subclassification,hard_coded_account_key) values ('account_payable','current_liability','account_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('account_receivable','receivable','account_receivable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('accumulated_depreciation','property_plant_equipment','accumulated_depreciation_key');
insert into account (account,subclassification,hard_coded_account_key) values ('advertising','operating_expense',null);
insert into account (account,subclassification,hard_coded_account_key) values ('cost_of_goods_sold','cost_of_goods_sold','cost_of_goods_sold_key');
insert into account (account,subclassification,hard_coded_account_key) values ('depreciation_expense','operating_expense','depreciation_expense_key');
insert into account (account,subclassification,hard_coded_account_key) values ('federal_unemployment_tax_payable','current_liability','federal_unemployment_tax_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('federal_withholding_payable','current_liability','federal_withholding_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('freight_in_expense','operating_expense','freight_in_key');
insert into account (account,subclassification,hard_coded_account_key) values ('land','property_plant_equipment','land_key');
insert into account (account,subclassification,hard_coded_account_key) values ('loss','loss','loss_key');
insert into account (account,subclassification,hard_coded_account_key) values ('medicare_payable','current_liability','medicare_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('payroll_payable','current_liability','payroll_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('payroll_tax','tax_expense','payroll_tax_key');
insert into account (account,subclassification,hard_coded_account_key) values ('realized_investment_gain','gain','realized_investment_gain_key');
insert into account (account,subclassification,hard_coded_account_key) values ('realized_investment_loss','loss','realized_investment_loss_key');
insert into account (account,subclassification,hard_coded_account_key) values ('repairs_maintenance_expense','operating_expense',null);
insert into account (account,subclassification,hard_coded_account_key) values ('retained_earnings','retained_earnings','closing_key');
insert into account (account,subclassification,hard_coded_account_key) values ('salary_wage_expense','operating_expense','salary_wage_expense_key');
insert into account (account,subclassification,hard_coded_account_key) values ('sales_revenue','revenue','sales_revenue_key');
insert into account (account,subclassification,hard_coded_account_key) values ('sales_tax_expense','tax_expense','sales_tax_key');
insert into account (account,subclassification,hard_coded_account_key) values ('sales_tax_payable','current_liability','sales_tax_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('service_revenue','revenue',null);
insert into account (account,subclassification,hard_coded_account_key) values ('social_security_payable','current_liability','social_security_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('specific_inventory','inventory','specific_inventory_key');
insert into account (account,subclassification,hard_coded_account_key) values ('state_unemployment_tax_payable','current_liability','state_unemployment_tax_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('state_withholding_payable','current_liability','state_withholding_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('uncleared_checks','current_liability','uncleared_checks_key');
insert into account (account,subclassification,hard_coded_account_key) values ('unrealized_investment','equity','unrealized_investment_key');
insert into depreciation_method (depreciation_method) values ('double_declining_balance');
insert into depreciation_method (depreciation_method) values ('n_declining_balance');
insert into depreciation_method (depreciation_method) values ('straight_line');
insert into depreciation_method (depreciation_method) values ('sum_of_years_digits');
insert into depreciation_method (depreciation_method) values ('units_of_production');
insert into day (day) values ('friday');
insert into day (day) values ('monday');
insert into day (day) values ('saturday');
insert into day (day) values ('sunday');
insert into day (day) values ('thursday');
insert into day (day) values ('tuesday');
insert into day (day) values ('wednesday');
insert into payroll_pay_period (payroll_pay_period) values ('biweekly');
insert into payroll_pay_period (payroll_pay_period) values ('monthly');
insert into payroll_pay_period (payroll_pay_period) values ('semimonthly');
insert into payroll_pay_period (payroll_pay_period) values ('weekly');
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','001','Gross receipts or sales',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','002','Returns and allowances',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','004','Cost of goods sold',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','008','Advertising',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','009','Car and truck expenses',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','013','Depreciation',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','015','Insurance',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','016a','Mortgage interest',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','016b','Other interest',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','017','Legal and professional services',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','018','Office expense',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','020a','Vehicle, machinery, equipment rent',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','020b','Property rent',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','021','Repairs and maintenance',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','022','Supplies',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','023','Taxes and licenses',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','024a','Travel expenses',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','024b','Meals and entertainment',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','025','Utilities',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','026','Wages',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','027','Other expenses',null);
insert into tax_form_line (	tax_form,			tax_form_line,			tax_form_description,			itemize_accounts_yn) values ('Schedule C','041','Ending inventory',null);
insert into tax_form (tax_form) values ('Schedule C');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('03-year','1','33.330');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('03-year','2','44.450');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('03-year','3','14.810');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('03-year','4','7.410');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('05-year','1','20.000');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('05-year','2','32.000');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('05-year','3','19.200');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('05-year','4','11.520');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('05-year','5','11.520');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('05-year','6','5.760');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('07-year','1','14.290');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('07-year','2','24.490');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('07-year','3','17.490');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('07-year','4','12.490');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('07-year','5','8.930');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('07-year','6','8.920');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('07-year','7','8.930');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('07-year','8','4.460');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('10-year','1','10.000');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('10-year','2','18.000');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('10-year','3','14.400');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('10-year','4','11.520');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('10-year','5','9.220');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('10-year','6','7.370');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('10-year','7','6.550');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('10-year','8','6.550');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('10-year','9','6.560');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('10-year','10','6.550');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('10-year','11','3.280');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','1','5.000');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','2','9.500');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','3','8.550');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','4','7.700');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','5','6.930');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','6','6.230');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','7','5.900');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','8','5.900');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','9','5.910');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','10','5.900');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','11','5.910');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','12','5.900');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','13','5.910');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','14','5.900');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','15','5.910');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('15-year','16','2.950');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','1','3.750');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','2','7.219');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','3','6.677');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','4','6.177');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','5','5.713');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','6','5.285');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','7','4.888');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','8','4.522');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','9','4.462');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','10','4.461');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','11','4.462');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','12','4.461');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','13','4.462');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','14','4.461');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','15','4.462');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','16','4.461');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','17','4.462');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','18','4.461');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','19','4.462');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','20','4.461');
insert into tax_recovery_table (	tax_recovery_period,				recovery_year,				recovery_percent) values ('20-year','21','2.231');
insert into tax_recovery_period (tax_recovery_period) values ('03-year');
insert into tax_recovery_period (tax_recovery_period) values ('05-year');
insert into tax_recovery_period (tax_recovery_period) values ('07-year');
insert into tax_recovery_period (tax_recovery_period) values ('10-year');
insert into tax_recovery_period (tax_recovery_period) values ('15-year');
insert into tax_recovery_period (tax_recovery_period) values ('20-year');
insert into tax_recovery_period (tax_recovery_period) values ('27.5-year');
insert into tax_recovery_period (tax_recovery_period) values ('31.5-year');
insert into tax_recovery_period (tax_recovery_period) values ('39-year');
insert into inventory_cost_method (inventory_cost_method) values ('average');
insert into inventory_cost_method (inventory_cost_method) values ('FIFO');
insert into inventory_cost_method (inventory_cost_method) values ('LIFO');
insert into federal_marital_status (federal_marital_status) values ('married');
insert into federal_marital_status (federal_marital_status) values ('married_but_single_rate');
insert into federal_marital_status (federal_marital_status) values ('single');
insert into state_marital_status (state_marital_status) values ('married_one_income');
insert into state_marital_status (state_marital_status) values ('single_or_married_with_multiple_incomes');
insert into state_marital_status (state_marital_status) values ('unmarried_head_of_household');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married','166','525','0.00','10.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married','525','1626','35.90','15.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married','1626','3111','201.05','25.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married','3111','4654','572.30','28.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married','4654','8180','1004.34','33.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married','8180','9218','2167.92','35.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married','9218','0','2531.22','39.6');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_but_single_rate','44','224','0.00','10.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_but_single_rate','224','774','18.00','15.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_but_single_rate','774','1812','100.50','25.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_but_single_rate','1812','3730','360.00','28.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_but_single_rate','3730','8058','897.04','33.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_but_single_rate','8058','8090','2325.28','35.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_but_single_rate','8090','0','2336.48','39.6');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single','44','224','0.00','10.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single','224','774','18.00','15.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single','774','1812','100.50','25.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single','1812','3730','360.00','28.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single','3730','8058','897.04','33.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single','8058','8090','2325.28','35.0');
insert into federal_income_tax_withholding (federal_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single','8090','0','2336.48','39.6');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_one_income','0','308','0.00','1.1');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_one_income','308','730','3.39','2.2');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_one_income','730','1154','12.67','4.4');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_one_income','1154','1602','31.33','6.6');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_one_income','1602','2024','60.90','8.8');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_one_income','2024','10336','98.04','10.2');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_one_income','10336','12404','948.69','11.3');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_one_income','12404','19231','1182.66','12.4');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_one_income','19231','20673','2031.26','13.5');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('married_one_income','20673','0','2226.36','14.6');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','0','154','0.00','1.1');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','154','365','1.69','2.0');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','354','577','6.33','4.4');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','577','801','15.66','6.6');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','801','1012','30.44','8.8');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','1012','5168','49.01','10.2');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','5169','6202','474.17','11.3');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','6202','10337','591.32','12.4');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','10337','19231','1105.30','13.5');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','19231',null,'2308.66','14.6');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('unmarried_head_of_household','0','308','0.00','1.1');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('unmarried_head_of_household','308','731','3.39','2.2');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('unmarried_head_of_household','731','942','12.70','4.4');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('unmarried_head_of_household','942','1166','21.98','6.6');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('unmarried_head_of_household','1166','1377','36.76','8.8');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('unmarried_head_of_household','1377','7029','55.33','10.2');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('unmarried_head_of_household','7029','8435','633.53','11.3');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('unmarried_head_of_household','8435','14058','792.83','12.4');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('unmarried_head_of_household','14058','19231','1491.77','13.5');
insert into state_income_tax_withholding (state_marital_status,			 income_over,			 income_not_over,			 tax_fixed_amount,			 tax_percentage_amount) values ('unmarried_head_of_household','19231','0','2191.68','14.6');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('married_one_income','0','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('married_one_income','1','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('married_one_income','2','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('married_one_income','3','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('married_one_income','4','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('married_one_income','5','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('married_one_income','6','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('married_one_income','7','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('married_one_income','8','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('married_one_income','9','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('married_one_income','10','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','0','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','1','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','2','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','3','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','4','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','5','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','6','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','7','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','8','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','9','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','10','79.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('unmarried_head_of_household','0','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('unmarried_head_of_household','1','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('unmarried_head_of_household','2','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('unmarried_head_of_household','3','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('unmarried_head_of_household','4','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('unmarried_head_of_household','5','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('unmarried_head_of_household','6','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('unmarried_head_of_household','7','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('unmarried_head_of_household','8','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('unmarried_head_of_household','9','159.00');
insert into state_standard_deduction_table (state_marital_status,			 state_withholding_allowances,			 state_standard_deduction_amount) values ('unmarried_head_of_household','10','159.00');
insert into fixed_service_category (service_category) values ('standard');
insert into hourly_service_category (service_category) values ('standard');
all_done2
) | sql.e 2>&1 | grep -vi duplicate


(
cat << all_done3
insert into role_operation (folder,role,operation) values ('account','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('account','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('account','bookkeeper','ledger_propagate');
insert into role_operation (folder,role,operation) values ('account','supervisor','delete');
insert into role_operation (folder,role,operation) values ('account','supervisor','detail');
insert into role_operation (folder,role,operation) values ('account','supervisor','ledger_propagate');
insert into role_operation (folder,role,operation) values ('activity','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('activity','supervisor','delete');
insert into role_operation (folder,role,operation) values ('activity','supervisor','detail');
insert into role_operation (folder,role,operation) values ('activity_work','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('activity_work','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('activity_work','dataentry','delete');
insert into role_operation (folder,role,operation) values ('activity_work','dataentry','detail');
insert into role_operation (folder,role,operation) values ('activity_work','supervisor','detail');
insert into role_operation (folder,role,operation) values ('bank_upload','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('bank_upload','supervisor','delete');
insert into role_operation (folder,role,operation) values ('bank_upload','supervisor','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_activity_work','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('bank_upload_activity_work','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_activity_work','supervisor','delete');
insert into role_operation (folder,role,operation) values ('bank_upload_activity_work','supervisor','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_archive','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_archive','supervisor','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_event','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_event','supervisor','delete');
insert into role_operation (folder,role,operation) values ('bank_upload_event','supervisor','detail');
insert into role_operation (folder,role,operation) values ('transaction','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('transaction','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('transaction','supervisor','delete');
insert into role_operation (folder,role,operation) values ('transaction','supervisor','detail');
insert into role_operation (folder,role,operation) values ('journal_ledger','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('journal_ledger','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('journal_ledger','supervisor','delete');
insert into role_operation (folder,role,operation) values ('journal_ledger','supervisor','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_transaction','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('bank_upload_transaction','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_transaction','supervisor','delete');
insert into role_operation (folder,role,operation) values ('bank_upload_transaction','supervisor','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_transaction_balance','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_transaction_balance','supervisor','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_feeder_archive','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('bank_upload_feeder_archive','supervisor','detail');
insert into role_operation (folder,role,operation) values ('contra_account','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('contra_account','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('contra_account','supervisor','delete');
insert into role_operation (folder,role,operation) values ('contra_account','supervisor','detail');
insert into role_operation (folder,role,operation) values ('customer','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('customer','bookkeeper','delete_isa_only');
insert into role_operation (folder,role,operation) values ('customer','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('customer','mechanic','detail');
insert into role_operation (folder,role,operation) values ('customer','supervisor','delete');
insert into role_operation (folder,role,operation) values ('customer','supervisor','delete_isa_only');
insert into role_operation (folder,role,operation) values ('customer','supervisor','detail');
insert into role_operation (folder,role,operation) values ('customer_payment','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('customer_payment','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('customer_payment','mechanic','detail');
insert into role_operation (folder,role,operation) values ('customer_payment','supervisor','delete');
insert into role_operation (folder,role,operation) values ('customer_payment','supervisor','detail');
insert into role_operation (folder,role,operation) values ('customer_sale','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('customer_sale','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('customer_sale','mechanic','delete');
insert into role_operation (folder,role,operation) values ('customer_sale','mechanic','detail');
insert into role_operation (folder,role,operation) values ('customer_sale','supervisor','delete');
insert into role_operation (folder,role,operation) values ('customer_sale','supervisor','detail');
insert into role_operation (folder,role,operation) values ('customer_vehicle','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('customer_vehicle','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('customer_vehicle','mechanic','detail');
insert into role_operation (folder,role,operation) values ('customer_vehicle','supervisor','delete');
insert into role_operation (folder,role,operation) values ('customer_vehicle','supervisor','detail');
insert into role_operation (folder,role,operation) values ('day','supervisor','delete');
insert into role_operation (folder,role,operation) values ('depreciation_method','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('depreciation_method','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('element','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('element','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('element','supervisor','delete');
insert into role_operation (folder,role,operation) values ('element','supervisor','detail');
insert into role_operation (folder,role,operation) values ('employee','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('employee','bookkeeper','delete_isa_only');
insert into role_operation (folder,role,operation) values ('employee','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('employee','supervisor','delete');
insert into role_operation (folder,role,operation) values ('employee','supervisor','delete_isa_only');
insert into role_operation (folder,role,operation) values ('employee','supervisor','detail');
insert into role_operation (folder,role,operation) values ('employee_work_day','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('employee_work_day','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('employee_work_day','mechanic','detail');
insert into role_operation (folder,role,operation) values ('employee_work_day','supervisor','delete');
insert into role_operation (folder,role,operation) values ('employee_work_day','supervisor','detail');
insert into role_operation (folder,role,operation) values ('employee_work_period','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('employee_work_period','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('employee_work_period','supervisor','delete');
insert into role_operation (folder,role,operation) values ('employee_work_period','supervisor','detail');
insert into role_operation (folder,role,operation) values ('entity','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('entity','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('entity','mechanic','detail');
insert into role_operation (folder,role,operation) values ('entity','supervisor','delete');
insert into role_operation (folder,role,operation) values ('entity','supervisor','detail');
insert into role_operation (folder,role,operation) values ('equity_account_balance','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('equity_account_balance','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('equity_account_balance','supervisor','delete');
insert into role_operation (folder,role,operation) values ('equity_account_balance','supervisor','detail');
insert into role_operation (folder,role,operation) values ('federal_income_tax_withholding','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('federal_income_tax_withholding','supervisor','delete');
insert into role_operation (folder,role,operation) values ('federal_marital_status','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('federal_marital_status','supervisor','delete');
insert into role_operation (folder,role,operation) values ('federal_marital_status','supervisor','detail');
insert into role_operation (folder,role,operation) values ('feeder_account','supervisor','delete');
insert into role_operation (folder,role,operation) values ('feeder_account','supervisor','detail');
insert into role_operation (folder,role,operation) values ('financial_institution','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('financial_institution','bookkeeper','delete_isa_only');
insert into role_operation (folder,role,operation) values ('financial_institution','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('financial_institution','supervisor','delete');
insert into role_operation (folder,role,operation) values ('financial_institution','supervisor','delete_isa_only');
insert into role_operation (folder,role,operation) values ('financial_institution','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_asset','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('fixed_asset','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_asset','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_asset','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_asset_depreciation','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('fixed_asset_depreciation','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_asset_depreciation','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_asset_purchase','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('fixed_asset_purchase','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_asset_purchase','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_asset_purchase','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_service','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('fixed_service','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_service','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_service','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_category','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_category','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_service_category','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_sale','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('fixed_service_sale','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_sale','mechanic','delete');
insert into role_operation (folder,role,operation) values ('fixed_service_sale','mechanic','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_sale','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_service_sale','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_work','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('fixed_service_work','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_work','mechanic','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_work','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_service_work','supervisor','detail');
insert into role_operation (folder,role,operation) values ('hourly_service','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('hourly_service','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('hourly_service','mechanic','detail');
insert into role_operation (folder,role,operation) values ('hourly_service','supervisor','delete');
insert into role_operation (folder,role,operation) values ('hourly_service','supervisor','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_category','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_category','supervisor','delete');
insert into role_operation (folder,role,operation) values ('hourly_service_category','supervisor','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_sale','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('hourly_service_sale','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_sale','supervisor','delete');
insert into role_operation (folder,role,operation) values ('hourly_service_sale','supervisor','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_work','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('hourly_service_work','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_work','mechanic','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_work','supervisor','delete');
insert into role_operation (folder,role,operation) values ('hourly_service_work','supervisor','detail');
insert into role_operation (folder,role,operation) values ('inventory','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('inventory','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('inventory','supervisor','delete');
insert into role_operation (folder,role,operation) values ('inventory','supervisor','detail');
insert into role_operation (folder,role,operation) values ('inventory_category','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('inventory_category','supervisor','delete');
insert into role_operation (folder,role,operation) values ('inventory_category','supervisor','detail');
insert into role_operation (folder,role,operation) values ('inventory_cost_method','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('inventory_cost_method','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('inventory_purchase','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('inventory_purchase','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('inventory_purchase','supervisor','delete');
insert into role_operation (folder,role,operation) values ('inventory_purchase','supervisor','detail');
insert into role_operation (folder,role,operation) values ('inventory_sale','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('inventory_sale','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('inventory_sale','mechanic','delete');
insert into role_operation (folder,role,operation) values ('inventory_sale','mechanic','detail');
insert into role_operation (folder,role,operation) values ('inventory_sale','supervisor','delete');
insert into role_operation (folder,role,operation) values ('inventory_sale','supervisor','detail');
insert into role_operation (folder,role,operation) values ('investment_account','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('investment_account','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('investment_account','supervisor','delete');
insert into role_operation (folder,role,operation) values ('investment_account','supervisor','detail');
insert into role_operation (folder,role,operation) values ('investment_classification','supervisor','delete');
insert into role_operation (folder,role,operation) values ('investment_classification','supervisor','detail');
insert into role_operation (folder,role,operation) values ('investment_operation','supervisor','delete');
insert into role_operation (folder,role,operation) values ('investment_operation','supervisor','detail');
insert into role_operation (folder,role,operation) values ('liability_account_entity','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('liability_account_entity','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('liability_account_entity','supervisor','delete');
insert into role_operation (folder,role,operation) values ('liability_account_entity','supervisor','detail');
insert into role_operation (folder,role,operation) values ('mechanic','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('mechanic','bookkeeper','delete_isa_only');
insert into role_operation (folder,role,operation) values ('mechanic','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('mechanic','supervisor','delete');
insert into role_operation (folder,role,operation) values ('mechanic','supervisor','delete_isa_only');
insert into role_operation (folder,role,operation) values ('mechanic','supervisor','detail');
insert into role_operation (folder,role,operation) values ('payroll_pay_period','supervisor','delete');
insert into role_operation (folder,role,operation) values ('payroll_posting','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('payroll_posting','supervisor','detail');
insert into role_operation (folder,role,operation) values ('prepaid_asset','accountant','delete');
insert into role_operation (folder,role,operation) values ('prepaid_asset','accountant','detail');
insert into role_operation (folder,role,operation) values ('prepaid_asset','supervisor','delete');
insert into role_operation (folder,role,operation) values ('prepaid_asset','supervisor','detail');
insert into role_operation (folder,role,operation) values ('prepaid_asset_accrual','accountant','delete');
insert into role_operation (folder,role,operation) values ('prepaid_asset_accrual','accountant','detail');
insert into role_operation (folder,role,operation) values ('prepaid_asset_accrual','supervisor','delete');
insert into role_operation (folder,role,operation) values ('prepaid_asset_accrual','supervisor','detail');
insert into role_operation (folder,role,operation) values ('prepaid_asset_purchase','accountant','delete');
insert into role_operation (folder,role,operation) values ('prepaid_asset_purchase','accountant','detail');
insert into role_operation (folder,role,operation) values ('prepaid_asset_purchase','supervisor','delete');
insert into role_operation (folder,role,operation) values ('prepaid_asset_purchase','supervisor','detail');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset','supervisor','delete');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset','supervisor','detail');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset_depreciation','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset_depreciation','supervisor','detail');
insert into role_operation (folder,role,operation) values ('prior_property','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('prior_property','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('prior_property','supervisor','delete');
insert into role_operation (folder,role,operation) values ('prior_property','supervisor','detail');
insert into role_operation (folder,role,operation) values ('prior_property_depreciation','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('prior_property_depreciation','supervisor','detail');
insert into role_operation (folder,role,operation) values ('property','supervisor','delete');
insert into role_operation (folder,role,operation) values ('property','supervisor','detail');
insert into role_operation (folder,role,operation) values ('property_depreciation','supervisor','detail');
insert into role_operation (folder,role,operation) values ('property_purchase','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('property_purchase','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('property_purchase','supervisor','delete');
insert into role_operation (folder,role,operation) values ('property_purchase','supervisor','detail');
insert into role_operation (folder,role,operation) values ('purchase_order','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('purchase_order','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('purchase_order','supervisor','delete');
insert into role_operation (folder,role,operation) values ('purchase_order','supervisor','detail');
insert into role_operation (folder,role,operation) values ('receipt_upload','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('receipt_upload','supervisor','delete');
insert into role_operation (folder,role,operation) values ('reoccurring_transaction','supervisor','delete');
insert into role_operation (folder,role,operation) values ('reoccurring_transaction','supervisor','detail');
insert into role_operation (folder,role,operation) values ('role_activity','supervisor','delete');
insert into role_operation (folder,role,operation) values ('role_activity','supervisor','detail');
insert into role_operation (folder,role,operation) values ('self','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('self','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('service_purchase','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('service_purchase','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('service_purchase','supervisor','delete');
insert into role_operation (folder,role,operation) values ('service_purchase','supervisor','detail');
insert into role_operation (folder,role,operation) values ('state_income_tax_withholding','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('state_income_tax_withholding','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('state_income_tax_withholding','supervisor','delete');
insert into role_operation (folder,role,operation) values ('state_income_tax_withholding','supervisor','detail');
insert into role_operation (folder,role,operation) values ('state_marital_status','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('state_marital_status','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('state_marital_status','supervisor','delete');
insert into role_operation (folder,role,operation) values ('state_marital_status','supervisor','detail');
insert into role_operation (folder,role,operation) values ('state_standard_deduction_table','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('state_standard_deduction_table','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('state_standard_deduction_table','supervisor','delete');
insert into role_operation (folder,role,operation) values ('state_standard_deduction_table','supervisor','detail');
insert into role_operation (folder,role,operation) values ('subclassification','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('subclassification','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('subclassification','supervisor','delete');
insert into role_operation (folder,role,operation) values ('subclassification','supervisor','detail');
insert into role_operation (folder,role,operation) values ('subsidiary_transaction','supervisor','delete');
insert into role_operation (folder,role,operation) values ('subsidiary_transaction','supervisor','detail');
insert into role_operation (folder,role,operation) values ('supply','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('supply','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('supply','supervisor','delete');
insert into role_operation (folder,role,operation) values ('supply','supervisor','detail');
insert into role_operation (folder,role,operation) values ('supply_purchase','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('supply_purchase','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('supply_purchase','supervisor','delete');
insert into role_operation (folder,role,operation) values ('supply_purchase','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_fixed_asset_recovery','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('tax_fixed_asset_recovery','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_form','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('tax_form','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('tax_form','supervisor','delete');
insert into role_operation (folder,role,operation) values ('tax_form','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_form_line','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('tax_form_line','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('tax_form_line','supervisor','delete');
insert into role_operation (folder,role,operation) values ('tax_form_line','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_form_line_account','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('tax_form_line_account','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('tax_form_line_account','supervisor','delete');
insert into role_operation (folder,role,operation) values ('tax_form_line_account','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_prior_fixed_asset_recovery','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('tax_prior_fixed_asset_recovery','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_prior_property_recovery','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('tax_prior_property_recovery','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_property_recovery','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_recovery_period','supervisor','delete');
insert into role_operation (folder,role,operation) values ('tax_recovery_period','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_recovery_table','supervisor','delete');
insert into role_operation (folder,role,operation) values ('tax_recovery_table','supervisor','detail');
insert into role_operation (folder,role,operation) values ('title_passage_rule','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('vehicle','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('vehicle','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('vehicle','mechanic','detail');
insert into role_operation (folder,role,operation) values ('vehicle','supervisor','delete');
insert into role_operation (folder,role,operation) values ('vehicle','supervisor','detail');
insert into role_operation (folder,role,operation) values ('vehicle_maker','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('vehicle_maker','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('vehicle_maker','mechanic','detail');
insert into role_operation (folder,role,operation) values ('vehicle_maker','supervisor','delete');
insert into role_operation (folder,role,operation) values ('vehicle_maker','supervisor','detail');
insert into role_operation (folder,role,operation) values ('vendor','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('vendor','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('vendor','supervisor','delete');
insert into role_operation (folder,role,operation) values ('vendor','supervisor','detail');
insert into role_operation (folder,role,operation) values ('vendor_payment','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('vendor_payment','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('vendor_payment','supervisor','delete');
insert into role_operation (folder,role,operation) values ('vendor_payment','supervisor','detail');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('account_balance','account_balance.sh ignored \$process as_of_date full_name street_address',null,null,null,null,'output',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('add_column','add_column ignored \$session \$login_name \$role folder attribute really_yn',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('alter_column_datatype','alter_column_datatype ignored \$session \$login_name \$role folder attribute really_yn',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('appaserver_info','appaserver_info.sh ignored \$login_name',null,null,null,null,'documentation',null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('attribute_list','attribute_list ignored \$dictionary',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('automatic_transaction_assign_all','automatic_transaction_assign.sh all \$process',null,null,null,null,'reconcile',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('automatic_transaction_assign_one','automatic_transaction_assign.sh one \$process',null,null,null,null,'reconcile',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('balance_sheet','balance_sheet \$process fund as_of_date subclassification_option output_medium',null,null,null,null,'output',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('change_password','maintain_user_account ignored \$session \$person \$role',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('clone_application','clone_application ignored \$session \$login_name \$role destination_application delete_yn system_folders_yn really_yn output2file_yn database_management_system export_output',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('clone_folder','clone_folder ignored n \$session \$login_name \$role destination_application folder attribute old_data new_data html delete_yn execute_yn database_management_system output2file_yn',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('close_fixed_service_work','shortcut_service_work ignored \$process \$login_name full_name street_address sale_date_time close fixed service_name description begin_work_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('close_hourly_service_work','shortcut_service_work ignored \$process \$login_name full_name street_address sale_date_time close hourly service_name description begin_work_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('close_nominal_accounts','close_nominal_accounts ignored  \$process as_of_date execute_yn',null,null,null,null,'manipulate',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('collect_customer_payment','collect_customer_payment ignored \$process full_name street_address sale_date_time payment_amount check_number paid_amount_due_yn',null,null,null,null,'shortcut',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('create_application','create_application ignored build_shell_script_yn \$login_name \$role destination_application system_folders_yn output2file_yn really_yn database_management_system',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('create_empty_application','create_empty_application ignored \$session \$login_name \$role \$process destination_application new_application_title y delete_application_yn execute_yn','This process creates an empty application. It creates a new database, the Appaserver tables, the data directories, among other application objects. Following this process, you can begin inserting the folder rows.',null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('create_table','create_table ignored n \$session \$login_name \$role destination_application folder really_yn database_management_system',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('delete','delete_folder_row \$session \$login_name \$folder \$role \$primary_data_list n',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('delete_isa_only','delete_folder_row \$session \$login_name \$folder \$role \$primary_data_list y',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('depreciate_fixed_assets','depreciate_fixed_assets \$process undo_yn execute_yn',null,null,null,null,'manipulate',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('detail','detail ignored \$session \$login_name \$folder \$role \$target_frame \$primary_data_list \$dictionary',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('direct_transaction_assign','direct_transaction_assign.sh \$process bank_date bank_description full_name street_address transaction_date_time',null,null,null,null,'reconcile',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('drop_column','drop_column ignored \$session \$login_name \$role folder attribute really_yn',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('execute_select_statement','execute_select_statement ignored \$process \$login_name \$session \$role output_medium filename select_statement_title login_name',null,null,null,null,'output',null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('export_application','clone_application ignored \$session \$login_name \$role destination_application delete_yn system_folders_yn really_yn output2file_yn database_management_system export_output',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('export_folder','clone_folder ignored y \$session \$login_name \$role destination_application folder attribute old_data new_data html y really_yn database_management_system y export_output',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('export_process','export_process ignored \$session \$login_name \$role \$process \$dictionary','For a list of processes, this process exports from the following: process, role_process, process_parameter, javascript_libraries, prompt, drop_down_prompt, and drop_down_prompt_data.',null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('export_process_set','export_process_set ignored \$session \$login_name \$role \$process \$dictionary','For a list of process sets, this process exports from the following: process_set, process_set_parameter, javascript_libraries, role_process_set, prompt, drop_down_prompt, and drop_down_prompt_data.',null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('export_subschema','export_subschema ignored \$session \$login_name \$role destination_application \$dictionary','This process exports for a list of folders from the following: folder, relation, folder_attribute, attribute, folder_operation, role_operation, and role_folder.',null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('feeder_bank_upload','load_bank_spreadsheet \$process \$login_name fund feeder_account filename 1 2 3 -1 4 transactions_only_yn execute_yn',null,null,null,null,'load',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('feeder_cash_upload','load_bank_spreadsheet \$process \$login_name fund bank_of_america_checking filename 1 2 3 -1 4 transactions_only_yn execute_yn',null,null,null,null,'reconcile',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('feeder_credit_upload','load_bank_spreadsheet \$process \$login_name fund bank_of_america_credit_card filename 1 3 5 -1 -1 transactions_only_yn execute_yn',null,null,null,null,'reconcile',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('feeder_upload_statistics','feeder_upload_statistics.sh \$process output_medium',null,null,null,null,'reconcile',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('fix_orphans','fix_orphans \$process orphans_folder delete_yn really_yn','This process traverses the many-to-one relationships for a folder. It inserts the missing primary keys that contain foreign keys in the selected folder. Warning: this process could take a long time to run.',null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('generate_invoice','generate_invoice \$process full_name street_address sale_date_time',null,null,null,null,'output',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('generate_workorder','generate_workorder \$process full_name street_address sale_date_time',null,null,null,null,'output',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('generic_load','generic_load ignored \$session \$role \$process',null,null,null,null,'load',null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('google_map','google_map_operation ignored \$process \$login_name \$role \$folder latitude longitude utm_easting utm_northing \$process_id \$process_row_count \$session \$dictionary',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('grant_select_to_user','grant_select_to_user ignored \$process login_name connect_from_host revoke_only_yn really_yn',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('graphviz_database_schema','graphviz_database_schema_process.sh \$process appaserver_yn',null,null,null,null,'documentation',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('import_predictivebooks','import_predictivebooks_process.sh \$login \$process module opening_cash_balance cash_account_name equity_account_name execute_yn','This process imports the PredictiveBooks application.',null,null,null,'manipulate',null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('income_statement','income_statement \$process fund as_of_date subclassification_option output_medium',null,null,null,null,'output',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('insert_cash_expense_transaction','insert_cash_transaction.sh \$process full_name street_address transaction_date account transaction_amount check_number memo fund',null,null,null,null,'reconcile',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('inventory_balance_detail','inventory_balance_detail ignored \$process inventory_name output_medium',null,null,null,null,'output',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('inventory_propagate','post_change_inventory_purchase ignored full_name street_address purchase_date_time inventory_name preupdate_purchase_date_time preupdate_inventory_name preupdate_arrived_quantity \$state',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('ledger_debit_credit_audit','ledger_debit_credit_audit.sh \$process begin_date',null,null,null,null,'output',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('ledger_propagate','ledger_propagate ignored transaction_date_time preupdate_transaction_date_time account preupdate_account',null,null,null,null,'manipulate',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('load_receipt_scan','load_receipt_scan ignored \$process fund filename',null,null,null,null,'load',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('merge_purge','merge_purge ignored \$session \$role \$process',null,null,null,null,'manipulate',null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('null',null,null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('open_fixed_service_work','shortcut_service_work ignored \$process \$login_name full_name street_address sale_date_time open fixed service_name description begin_work_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('open_hourly_service_work','shortcut_service_work ignored \$process \$login_name full_name street_address sale_date_time open hourly service_name description begin_work_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('orphans_process_folder_list','orphans_process_folder_list.sh \$application',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('pay_liabilities','pay_liabilities_process ignored \$process \$session fund full_name street_address starting_check_number memo payment_amount y execute_yn',null,null,'post_change_pay_liabilities( this )',null,'manipulate',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('populate_account','populate_account.sh \$one2m_folder subclassification',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('populate_bank_upload_pending','populate_bank_upload_pending.sh',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('populate_bank_upload_transaction_pending','populate_bank_upload_transaction_pending.sh',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('populate_customer_sale','populate_customer_sale.sh ignored \$one2m_folder \$where',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('populate_expense_account','populate_expense_account.sh',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('populate_inventory','populate_inventory.sh ignored inventory_category',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('populate_investment_account','populate_investment_account.sh ignored \$where',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('populate_print_checks_entity','populate_print_checks_entity ignored fund',null,null,null,null,'output',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('populate_reoccurring_transaction_accrual','populate_reoccurring_transaction_accrual.sh ignored',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('populate_reoccurring_transaction_recent','populate_reoccurring_transaction_recent.sh ignored',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('populate_tax_form_line','populate_tax_form_line.sh ignored \$where',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_cash_expense_transaction','post_cash_expense_transaction \$process \$session \$role full_name street_address bank_date bank_description account memo table execute_yn',null,null,null,null,'reconcile',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_account_balance','post_change_account_balance ignored fund full_name street_address account_number date_time \$state preupdate_full_name preupdate_street_address preupdate_account_number preupdate_date_time',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_activity_work','post_change_activity_work ignored \$login_name begin_work_date_time \$state',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_bank_upload','post_change_bank_upload ignored \$state \$dictionary',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_bank_upload_transaction','post_change_bank_upload_transaction.sh \$state bank_date bank_description full_name street_address transaction_date_time',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_customer_payment','post_change_customer_payment ignored full_name street_address sale_date_time payment_date_time \$state preupdate_payment_date_time preupdate_payment_amount',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_customer_sale','post_change_customer_sale ignored full_name street_address sale_date_time \$state preupdate_full_name preupdate_street_address preupdate_title_passage_rule preupdate_completed_date_time preupdate_shipped_date_time preupdate_arrived_date preupdate_shipping_revenue',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_employee_work_day','post_change_employee_work_day ignored full_name street_address begin_work_date_time \$state preupdate_begin_work_date_time preupdate_end_work_date_time',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_fixed_asset_purchase','post_change_fixed_asset_purchase ignored full_name street_address purchase_date_time asset_name serial_number \$state preupdate_asset_name preupdate_serial_number preupdate_extension preupdate_declining_balance_n preupdate_depreciation_method',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_fixed_service_sale','post_change_fixed_service_sale ignored full_name street_address sale_date_time service_name \$state',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_fixed_service_work','post_change_fixed_service_work ignored full_name street_address sale_date_time service_name begin_work_date_time \$state preupdate_end_work_date_time',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_hourly_service_sale','post_change_hourly_service_sale ignored full_name street_address sale_date_time service_name description \$state',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_hourly_service_work','post_change_hourly_service_work ignored full_name street_address sale_date_time service_name description begin_work_date_time \$state preupdate_end_work_date_time',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_inventory_purchase','post_change_inventory_purchase ignored full_name street_address purchase_date_time inventory_name \$state preupdate_ordered_quantity preupdate_arrived_quantity preupdate_missing_quantity preupdate_unit_cost preupdate_inventory_name',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_inventory_purchase_return','post_change_inventory_purchase_return full_name street_address purchase_date_time inventory_name return_date_time \$state preupdate_inventory_name',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_inventory_sale','post_change_inventory_sale ignored full_name street_address sale_date_time inventory_name \$state preupdate_quantity preupdate_retail_price preupdate_discount_amount preupdate_inventory_name',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_journal_ledger','post_change_journal_ledger.sh ignored full_name street_address transaction_date_time account preupdate_transaction_date_time preupdate_account',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_prepaid_asset_accrual','post_change_prepaid_asset_accrual ignored full_name street_address purchase_date_time asset_name accrual_date \$state preupdate_full_name preupdate_street_address preupdate_purchase_date_time preupdate_asset_name preupdate_accrual_date',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_prepaid_asset_purchase','post_change_prepaid_asset_purchase ignored full_name street_address purchase_date_time asset_name \$state preupdate_asset_name preupdate_extension preupdate_accrual_period_years',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_prior_fixed_asset','post_change_prior_fixed_asset ignored asset_name serial_number \$state preupdate_extension',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_purchase_order','post_change_purchase_order ignored full_name street_address purchase_date_time \$state preupdate_full_name preupdate_street_address preupdate_title_passage_rule preupdate_shipped_date preupdate_arrived_date_time preupdate_sales_tax preupdate_freight_in',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_self','post_change_self ignored preupdate_inventory_cost_method',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_service_purchase','post_change_service_purchase ignored full_name street_address purchase_date_time account \$state preupdate_account',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_supply_purchase','post_change_supply_purchase ignored full_name street_address purchase_date_time supply_name \$state preupdate_supply_name',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_change_vendor_payment','post_change_vendor_payment ignored full_name street_address purchase_date_time payment_date_time \$state preupdate_full_name preupdate_street_address preupdate_payment_date_time preupdate_payment_amount',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_payroll','post_payroll \$process payroll_year period_number delete_yn y execute_yn',null,null,null,null,'manipulate',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_reoccurring_transaction_accrual','post_reoccurring_transaction \$session \$role \$process full_name street_address transaction_description transaction_date transaction_amount memo execute_yn y',null,null,null,null,'manipulate',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('post_reoccurring_transaction_recent','post_reoccurring_transaction \$session \$role \$process full_name street_address transaction_description transaction_date transaction_amount memo execute_yn y',null,null,null,null,'manipulate',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('recalculate_cost','post_change_inventory_sale ignored full_name street_address sale_date_time inventory_name quantity retail_price discount_amount \$state',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('rename_column','rename_column ignored \$process old_attribute folder attribute really_yn',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('rename_table','rename_table ignored \$session \$login_name \$role old_folder new_folder really_yn',null,null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('report_writer','post_report_writer \$application \$session \$login_name \$role \$process folder one',null,null,null,null,'output',null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('select_closed_customer_sale','select_customer_sale.sh ignored \$state closed',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('select_customer_sale_collect_payment','select_customer_sale_collect_customer_payment.sh ignored',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('select_open_customer_sale','select_customer_sale.sh ignored \$state open',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('select_open_fixed_service_sale','select_open_fixed_service_sale.sh ignored',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('select_open_fixed_service_work','select_open_fixed_service_work.sh ignored \$login_name',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('select_open_hourly_service_sale','select_open_hourly_service_sale.sh ignored',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('select_open_hourly_service_work','select_open_hourly_service_work.sh ignored \$login_name',null,null,null,null,null,null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('table_rectification','table_rectification ignored \$session \$login_name \$role','This process compares the Appaserver attributes with the Mysql table columns. It then gives you the opportunity to drop the residual columns.',null,null,null,null,null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('tax_form_report','tax_form_report ignored \$process tax_form as_of_date output_medium',null,null,null,null,'output',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('tax_recover_fixed_assets','tax_recover_fixed_assets \$process fund undo_yn execute_yn',null,null,null,null,'manipulate',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('transaction_balance_report','transaction_balance_report \$process begin_date output_medium',null,null,null,null,'reconcile',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('trial_balance','trial_balance \$session \$login_name \$role \$process fund as_of_date aggregation output_medium subclassification_option',null,null,null,null,'output',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('update_sale_completed','update_sale_completed ignored \$process full_name street_address sale_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('update_sale_not_completed','update_sale_not_completed ignored \$process full_name street_address sale_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('upload_source_file','upload_source_file ignored \$process filename','This process allows you to upload non-executable source files, like javascript. The destination directory is likely \$APPASERVER_HOME/src_\$application.',null,null,null,'load',null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('view_appaserver_log_file','view_appaserver_error_file.sh ignored /var/log/appaserver line_count','Most error messages are logged to this file. If you get the Server Error screen, then this file is the first place to find a clue. <big> <bold>Warning:</bold></big> this process exposes the session number assigned to each user''s login.',null,null,null,'output',null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('view_documentation','view_diagrams ignored',null,null,null,null,'documentation',null,'y');
insert into process (process,		command_line,		notepad,		html_help_file_anchor,		post_change_javascript,		process_set_display,		process_group,		preprompt_help_text,		appaserver_yn) values ('view_source','view_source \$application \$session \$login_name \$role',null,null,null,null,'documentation',null,'y');
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('account_balance','financial_institution','null','null','null','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('account_balance','null','null','null','as_of_date','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('add_column','attribute','null','null','null','1',null,'y',null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('add_column','folder_attribute','null','null','null','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('add_column','null','null','null','execute_yn','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('alter_column_datatype','attribute','null','null','null','1',null,'y',null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('alter_column_datatype','folder_attribute','null','null','null','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('alter_column_datatype','null','null','null','execute_yn','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('balance_sheet','null','null','null','as_of_date','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('balance_sheet','null','null','finance_output_medium','null','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('balance_sheet','null','null','subclassification_option','null','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('clone_application','null','null','null','delete_yn','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('clone_application','null','null','null','execute_yn','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('clone_application','null','null','null','system_folders_yn','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('clone_folder','attribute','null','null','null','1',null,'y',null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('clone_folder','folder_attribute','null','null','null','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('clone_folder','null','null','null','delete_yn','7',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('clone_folder','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('clone_folder','null','null','null','new_data','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('clone_folder','null','null','null','old_data','4',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('clone_folder','null','null','null','output2file_yn','6',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('close_fixed_service_work','fixed_service_work','null','null','null','1',null,null,'select_open_fixed_service_work',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('close_hourly_service_work','hourly_service_work','null','null','null','1',null,null,'select_open_hourly_service_work',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('close_nominal_accounts','null','null','null','as_of_date','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('close_nominal_accounts','null','null','null','execute_yn','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('collect_customer_payment','customer_sale','null','null','null','10',null,null,'select_customer_sale_collect_payment',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('collect_customer_payment','null','null','null','check_number','40',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('collect_customer_payment','null','null','null','paid_amount_due_yn','20',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('collect_customer_payment','null','null','null','payment_amount','30',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('create_application','null','null','null','build_shell_script_yn','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('create_application','null','null','null','execute_yn','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('create_application','null','null','null','system_folders_yn','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('create_empty_application','null','null','null','delete_application_yn','7',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('create_empty_application','null','null','null','destination_application','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('create_empty_application','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('create_empty_application','null','null','null','new_application_title','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('create_table','folder','null','null','null','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('create_table','null','null','null','execute_yn','4',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('depreciate_fixed_assets','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('depreciate_fixed_assets','null','null','null','undo_yn','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('direct_transaction_assign','bank_upload','null','null','null','1',null,null,'populate_bank_upload_pending',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('direct_transaction_assign','transaction','null','null','null','2',null,null,'populate_bank_upload_transaction_pending',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('drop_column','folder','null','null','null','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('drop_column','null','null','null','attribute','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('drop_column','null','null','null','execute_yn','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('execute_select_statement','null','null','null','filename','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('execute_select_statement','null','null','execute_select_statement_output_medium','null','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('execute_select_statement','select_statement','null','null','null','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('export_application','null','null','export_output','null','4',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('export_application','null','null','null','system_folders_yn','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('export_folder','folder','null','null','null','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('export_folder','null','null','export_output','null','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('export_process','null','null','null','exclude_roles_yn','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('export_process','process','null','null','null','1','y',null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('export_process_set','null','null','null','exclude_roles_yn','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('export_process_set','process_set','null','null','null','1','y',null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('export_subschema','folder','null','null','null','2','y',null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('feeder_bank_upload','feeder_account','null','null','null','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('feeder_bank_upload','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('feeder_bank_upload','null','null','null','filename','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('feeder_bank_upload','null','null','null','transactions_only_yn','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('feeder_cash_upload','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('feeder_cash_upload','null','null','null','filename','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('feeder_cash_upload','null','null','null','transactions_only_yn','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('feeder_credit_upload','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('feeder_credit_upload','null','null','null','filename','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('feeder_upload_statistics','null','null','output_medium','null','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('fix_orphans','null','null','null','delete_yn','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('fix_orphans','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('fix_orphans','null','null','null','orphans_folder','1',null,null,'orphans_process_folder_list',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('generate_invoice','customer_sale','null','null','null','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('generate_workorder','customer_sale','null','null','null',null,null,null,'select_open_customer_sale',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('grant_select_to_user','appaserver_user','null','null','null','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('grant_select_to_user','null','null','null','connect_from_host','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('grant_select_to_user','null','null','null','execute_yn','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('grant_select_to_user','null','null','null','revoke_only_yn','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('graphviz_database_schema','null','null','null','appaserver_yn','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('import_predictivebooks','null','null','null','cash_account_name','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('import_predictivebooks','null','null','null','equity_account_name','4',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('import_predictivebooks','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('import_predictivebooks','null','null','predictivebooks_module','null','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('import_predictivebooks','null','null','null','opening_cash_balance','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('income_statement','null','null','null','as_of_date','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('income_statement','null','null','finance_output_medium','null','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('income_statement','null','null','subclassification_option','null','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('insert_cash_expense_transaction','account','null','null','null','2',null,null,'populate_expense_account',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('insert_cash_expense_transaction','entity','null','null','null','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('insert_cash_expense_transaction','null','null','null','check_number','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('insert_cash_expense_transaction','null','null','null','memo','6',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('insert_cash_expense_transaction','null','null','null','transaction_amount','4',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('insert_cash_expense_transaction','null','null','null','transaction_date','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('inventory_balance_detail','inventory','null','null','null','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('inventory_balance_detail','null','null','inventory_balance_output_medium','null','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('ledger_debit_credit_audit','null','null','null','begin_date','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('load_receipt_scan','null','null','null','filename','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('open_fixed_service_work','fixed_service_sale','null','null','null','1',null,null,'select_open_fixed_service_sale',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('open_hourly_service_work','hourly_service_sale','null','null','null','1',null,null,'select_open_hourly_service_sale',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('pay_liabilities','entity','null','null','null','1','y',null,'populate_print_checks_entity',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('pay_liabilities','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('pay_liabilities','null','null','null','memo','4',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('pay_liabilities','null','null','null','payment_amount','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('pay_liabilities','null','null','null','starting_check_number','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_cash_expense_transaction','account','null','null','null','3',null,null,'populate_expense_account',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_cash_expense_transaction','bank_upload','null','null','null','1',null,null,'populate_bank_upload_pending',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_cash_expense_transaction','entity','null','null','null','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_cash_expense_transaction','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_cash_expense_transaction','null','null','null','memo','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_payroll','null','null','null','delete_yn','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_payroll','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_payroll','null','null','null','payroll_year','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_payroll','null','null','null','period_number','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_reoccurring_transaction_accrual','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_reoccurring_transaction_accrual','null','null','null','transaction_date','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_reoccurring_transaction_accrual','reoccurring_transaction','null','null','null','1',null,null,'populate_reoccurring_transaction_accrual',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_reoccurring_transaction_recent','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_reoccurring_transaction_recent','null','null','null','memo','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_reoccurring_transaction_recent','null','null','null','transaction_amount','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_reoccurring_transaction_recent','null','null','null','transaction_date','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('post_reoccurring_transaction_recent','reoccurring_transaction','null','null','null','1',null,null,'populate_reoccurring_transaction_recent',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('rename_column','attribute','null','null','null',null,null,'y',null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('rename_column','folder_attribute','null','null','null','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('rename_column','null','null','null','execute_yn','4',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('rename_column','null','null','null','old_attribute','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('rename_table','folder','null','null','new_folder','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('rename_table','null','null','null','execute_yn','3',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('rename_table','null','null','null','old_folder','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('tax_form_report','null','null','null','as_of_date','2',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('tax_form_report','null','null','finance_output_medium','null','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('tax_form_report','tax_form','null','null','null','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('tax_recover_fixed_assets','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('tax_recover_fixed_assets','null','null','null','undo_yn','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('transaction_balance_report','null','null','null','begin_date','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('trial_balance','null','null','null','as_of_date','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('trial_balance','null','null','finance_output_medium','null','9',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('trial_balance','null','null','trial_balance_subclassification_option','null','5',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('update_sale_completed','customer_sale','null','null','null','1',null,null,'select_open_customer_sale',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('update_sale_not_completed','customer_sale','null','null','null','1',null,null,'select_closed_customer_sale',null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('upload_source_file','null','null','null','filename','1',null,null,null,null);
insert into process_parameter (process,		folder,		attribute,		drop_down_prompt,		prompt,		display_order,		drop_down_multi_select_yn,		preprompt_yn,		populate_drop_down_process,		populate_helper_process) values ('view_appaserver_log_file','null','null','null','line_count',null,null,null,null,null);
insert into role_process (process,role) values ('account_balance','bookkeeper');
insert into role_process (process,role) values ('appaserver_info','bookkeeper');
insert into role_process (process,role) values ('automatic_transaction_assign_all','bookkeeper');
insert into role_process (process,role) values ('automatic_transaction_assign_one','bookkeeper');
insert into role_process (process,role) values ('balance_sheet','bookkeeper');
insert into role_process (process,role) values ('change_password','bookkeeper');
insert into role_process (process,role) values ('close_fixed_service_work','bookkeeper');
insert into role_process (process,role) values ('close_hourly_service_work','bookkeeper');
insert into role_process (process,role) values ('close_nominal_accounts','bookkeeper');
insert into role_process (process,role) values ('collect_customer_payment','bookkeeper');
insert into role_process (process,role) values ('depreciate_fixed_assets','bookkeeper');
insert into role_process (process,role) values ('direct_transaction_assign','bookkeeper');
insert into role_process (process,role) values ('execute_select_statement','bookkeeper');
insert into role_process (process,role) values ('feeder_bank_upload','bookkeeper');
insert into role_process (process,role) values ('feeder_cash_upload','bookkeeper');
insert into role_process (process,role) values ('feeder_credit_upload','bookkeeper');
insert into role_process (process,role) values ('generate_invoice','bookkeeper');
insert into role_process (process,role) values ('generate_workorder','bookkeeper');
insert into role_process (process,role) values ('generic_load','bookkeeper');
insert into role_process (process,role) values ('import_predictivebooks','bookkeeper');
insert into role_process (process,role) values ('income_statement','bookkeeper');
insert into role_process (process,role) values ('inventory_balance_detail','bookkeeper');
insert into role_process (process,role) values ('ledger_debit_credit_audit','bookkeeper');
insert into role_process (process,role) values ('ledger_propagate','bookkeeper');
insert into role_process (process,role) values ('load_receipt_scan','bookkeeper');
insert into role_process (process,role) values ('merge_purge','bookkeeper');
insert into role_process (process,role) values ('open_fixed_service_work','bookkeeper');
insert into role_process (process,role) values ('open_hourly_service_work','bookkeeper');
insert into role_process (process,role) values ('pay_liabilities','bookkeeper');
insert into role_process (process,role) values ('post_cash_expense_transaction','bookkeeper');
insert into role_process (process,role) values ('post_payroll','bookkeeper');
insert into role_process (process,role) values ('post_reoccurring_transaction_accrual','bookkeeper');
insert into role_process (process,role) values ('post_reoccurring_transaction_recent','bookkeeper');
insert into role_process (process,role) values ('tax_form_report','bookkeeper');
insert into role_process (process,role) values ('tax_recover_fixed_assets','bookkeeper');
insert into role_process (process,role) values ('transaction_balance_report','bookkeeper');
insert into role_process (process,role) values ('trial_balance','bookkeeper');
insert into role_process (process,role) values ('update_sale_completed','bookkeeper');
insert into role_process (process,role) values ('update_sale_not_completed','bookkeeper');
insert into role_process (process,role) values ('view_documentation','bookkeeper');
insert into role_process (process,role) values ('view_source','bookkeeper');
insert into role_process (process,role) values ('ledger_debit_credit_audit','dataentry');
insert into role_process (process,role) values ('post_reoccurring_transaction_accrual','dataentry');
insert into role_process (process,role) values ('appaserver_info','mechanic');
insert into role_process (process,role) values ('change_password','mechanic');
insert into role_process (process,role) values ('close_fixed_service_work','mechanic');
insert into role_process (process,role) values ('close_hourly_service_work','mechanic');
insert into role_process (process,role) values ('collect_customer_payment','mechanic');
insert into role_process (process,role) values ('execute_select_statement','mechanic');
insert into role_process (process,role) values ('generate_invoice','mechanic');
insert into role_process (process,role) values ('generate_workorder','mechanic');
insert into role_process (process,role) values ('open_fixed_service_work','mechanic');
insert into role_process (process,role) values ('open_hourly_service_work','mechanic');
insert into role_process (process,role) values ('update_sale_completed','mechanic');
insert into role_process (process,role) values ('update_sale_not_completed','mechanic');
insert into role_process (process,role) values ('view_documentation','mechanic');
insert into role_process (process,role) values ('view_source','mechanic');
insert into role_process (process,role) values ('account_balance','supervisor');
insert into role_process (process,role) values ('appaserver_info','supervisor');
insert into role_process (process,role) values ('automatic_transaction_assign_all','supervisor');
insert into role_process (process,role) values ('automatic_transaction_assign_one','supervisor');
insert into role_process (process,role) values ('balance_sheet','supervisor');
insert into role_process (process,role) values ('change_password','supervisor');
insert into role_process (process,role) values ('close_fixed_service_work','supervisor');
insert into role_process (process,role) values ('close_hourly_service_work','supervisor');
insert into role_process (process,role) values ('close_nominal_accounts','supervisor');
insert into role_process (process,role) values ('collect_customer_payment','supervisor');
insert into role_process (process,role) values ('depreciate_fixed_assets','supervisor');
insert into role_process (process,role) values ('direct_transaction_assign','supervisor');
insert into role_process (process,role) values ('execute_select_statement','supervisor');
insert into role_process (process,role) values ('feeder_bank_upload','supervisor');
insert into role_process (process,role) values ('feeder_cash_upload','supervisor');
insert into role_process (process,role) values ('feeder_credit_upload','supervisor');
insert into role_process (process,role) values ('feeder_upload_statistics','supervisor');
insert into role_process (process,role) values ('generate_invoice','supervisor');
insert into role_process (process,role) values ('generate_workorder','supervisor');
insert into role_process (process,role) values ('generic_load','supervisor');
insert into role_process (process,role) values ('graphviz_database_schema','supervisor');
insert into role_process (process,role) values ('import_predictivebooks','supervisor');
insert into role_process (process,role) values ('income_statement','supervisor');
insert into role_process (process,role) values ('insert_cash_expense_transaction','supervisor');
insert into role_process (process,role) values ('inventory_balance_detail','supervisor');
insert into role_process (process,role) values ('ledger_debit_credit_audit','supervisor');
insert into role_process (process,role) values ('ledger_propagate','supervisor');
insert into role_process (process,role) values ('load_receipt_scan','supervisor');
insert into role_process (process,role) values ('merge_purge','supervisor');
insert into role_process (process,role) values ('open_fixed_service_work','supervisor');
insert into role_process (process,role) values ('open_hourly_service_work','supervisor');
insert into role_process (process,role) values ('pay_liabilities','supervisor');
insert into role_process (process,role) values ('post_cash_expense_transaction','supervisor');
insert into role_process (process,role) values ('post_payroll','supervisor');
insert into role_process (process,role) values ('post_reoccurring_transaction_accrual','supervisor');
insert into role_process (process,role) values ('post_reoccurring_transaction_recent','supervisor');
insert into role_process (process,role) values ('report_writer','supervisor');
insert into role_process (process,role) values ('tax_form_report','supervisor');
insert into role_process (process,role) values ('tax_recover_fixed_assets','supervisor');
insert into role_process (process,role) values ('transaction_balance_report','supervisor');
insert into role_process (process,role) values ('trial_balance','supervisor');
insert into role_process (process,role) values ('update_sale_completed','supervisor');
insert into role_process (process,role) values ('update_sale_not_completed','supervisor');
insert into role_process (process,role) values ('view_documentation','supervisor');
insert into role_process (process,role) values ('view_source','supervisor');
insert into role_process (process,role) values ('add_column','system');
insert into role_process (process,role) values ('alter_column_datatype','system');
insert into role_process (process,role) values ('appaserver_info','system');
insert into role_process (process,role) values ('change_password','system');
insert into role_process (process,role) values ('clone_application','system');
insert into role_process (process,role) values ('clone_folder','system');
insert into role_process (process,role) values ('create_application','system');
insert into role_process (process,role) values ('create_empty_application','system');
insert into role_process (process,role) values ('create_table','system');
insert into role_process (process,role) values ('drop_column','system');
insert into role_process (process,role) values ('export_application','system');
insert into role_process (process,role) values ('export_folder','system');
insert into role_process (process,role) values ('export_process','system');
insert into role_process (process,role) values ('export_process_set','system');
insert into role_process (process,role) values ('export_subschema','system');
insert into role_process (process,role) values ('fix_orphans','system');
insert into role_process (process,role) values ('generic_load','system');
insert into role_process (process,role) values ('grant_select_to_user','system');
insert into role_process (process,role) values ('graphviz_database_schema','system');
insert into role_process (process,role) values ('merge_purge','system');
insert into role_process (process,role) values ('rename_column','system');
insert into role_process (process,role) values ('rename_table','system');
insert into role_process (process,role) values ('report_writer','system');
insert into role_process (process,role) values ('table_rectification','system');
insert into role_process (process,role) values ('upload_source_file','system');
insert into role_process (process,role) values ('view_appaserver_log_file','system');
insert into role_process (process,role) values ('view_documentation','system');
insert into role_process (process,role) values ('view_source','system');
insert into javascript_processes (process,javascript_filename) values ('clone_folder','clone_folder.js');
insert into javascript_processes (process,javascript_filename) values ('pay_liabilities','post_change_pay_liabilities.js');
insert into javascript_files (javascript_filename) values ('clone_folder.js');
insert into javascript_files (javascript_filename) values ('post_change_attribute.js');
insert into javascript_files (javascript_filename) values ('post_change_bank_upload.js');
insert into javascript_files (javascript_filename) values ('post_change_employee.js');
insert into javascript_files (javascript_filename) values ('post_change_equity_account_balance.js');
insert into javascript_files (javascript_filename) values ('post_change_folder_attribute.js');
insert into javascript_files (javascript_filename) values ('post_change_journal_ledger.js');
insert into javascript_files (javascript_filename) values ('post_change_pay_liabilities.js');
insert into javascript_files (javascript_filename) values ('post_change_reoccurring_transaction.js');
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('accumulate_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('appaserver_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('as_of_date','10',null,null,'y');
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('attribute','50',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('begin_date','10',null,null,'y');
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('begin_discovery_date','10',null,null,'y');
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('begin_time','4','Format HHMM',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('begin_work_date','10',null,null,'y');
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('begin_year','4',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('build_shell_script_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('cash_account_name','60',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('chart_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('check_number','10',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('compare_station',null,'Used to compare all of the datatypes for a station',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('compare_station|compare_datatype',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('connect_from_host','60',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('containing_date','10','Aggregate the day or week containing this date.',null,'y');
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('create_database_yn','1','Should the application be stored in a new database or the current database?',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('daily_values_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('date_offset_comma_list','100','Dates to load: 1=Jan 1, 365=Dec 31 or',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('date_or_yesterday','10',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('days_to_average','3','Defaults to 30',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('delete_application_yn','1','Delete the existing application?',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('delete_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('destination_application','30',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('destination_datatype','0',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('destination_directory','100',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('destination_station','0',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('destination_station|destination_datatype',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('detail_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('discovery_date|primary_researcher',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('display_count_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('dynamic_ending_date','10',null,null,'y');
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('elevation_offset','5','How many feet above or below the station''s ground surface elevation?',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('email_address','50','Optionally email the results',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('email_address_required','50',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('end_date','10',null,null,'y');
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('end_time','4','Format HHMM',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('end_year','4',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('equity_account_name','60',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('exceedance_format_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('exclude_folder_comma_list','255',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('exclude_roles_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('execute_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('filename','100',null,'y',null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('first_value','5',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('flow_units_converted',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('gridlines_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('hours_to_bump','5',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('ignore_closing_entry_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('inflow_station|inflow_datatype',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('last_value','5',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('line_count','4','How many lines from the end to include? Defaults to 50.',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('measurements_per_day','3',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('memo','60',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('minimum_spike','10',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('minutes_offset','2','minutes from midnight to start (0-59)',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('multiplier','5',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('multiply_first_then_add_yn','1','Default is to add first, then multiply.',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('negative_station|negative_datatype',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('nest_number',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('new_application_title','30',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('new_attribute','50',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('new_data','50',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('new_folder','50',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('new_station_datatype_group','30','Add these datatypes to a group with this name',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('notes','30',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('null','0',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('offset','5',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('old_attribute','50',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('old_data','50',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('old_folder','50',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('omit_output_if_any_missing_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('opening_cash_balance','14',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('optional_folder',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('orphans_folder',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('outflow_station|outflow_datatype',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('output2file_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('output_filename','100','Optional',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('override_destination_validation_requirement_yn',null,'Normally, if any of the destination measurements are validated, then this process will not be performed. Setting this flag to Yes will force the process to be performed even if some destination measurements are validated.',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('override_source_validation_requirement_yn',null,'Normally, if any of the source measurements are not validated, then this process will not be performed. Setting this flag to Yes will force the process to be performed even if some source measurements are not validated.',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('paid_amount_due_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('payment_amount','10',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('payroll_year','4',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('period_number','2',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('plot_for_station_check_yn','1','Create a plot to check station''s performance',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('positive_station|positive_datatype',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('really_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('revoke_only_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('shef_filename','100','For server files, use backslash for directories','y',null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('source_directory','100',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('source_station|source_datatype',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('stage_units_converted',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('starting_check_number','5',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('station|datatype',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('summary_only_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('symbols_yn',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('system_folders_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('transactions_only_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('transaction_amount','10','Optionally override the default.',null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('transaction_date','10',null,null,'y');
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('transmit_validation_info_yn',null,null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('undo_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('utm_easting','6',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('utm_northing','7',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('where_clause','100',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('with_file_trim_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('with_stub_yn','1',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('x_tick_major','6',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('x_tick_minor','6',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('ymax','6',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('ymin','6',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('y_tick_major','6',null,null,null);
insert into prompt (prompt,		input_width,		hint_message,		upload_filename_yn,		date_yn) values ('y_tick_minor','6',null,null,null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('aggregation',null,null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('execute_select_statement_output_medium',null,'output_medium');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('export_output',null,null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('finance_output_medium',null,'output_medium');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('group_annually_output_medium',null,'output_medium');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('inventory_balance_output_medium',null,'output_medium');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('null','Necessary for outter joins to work',null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('output_medium',null,null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('predictivebooks_module',null,'module');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('subclassification_option',null,null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('trial_balance_subclassification_option',null,'subclassification_option');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregation','consolidated','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregation','sequential','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('execute_select_statement_output_medium','spreadsheet','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('execute_select_statement_output_medium','table','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('export_output','gzip_file','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('export_output','shell_script','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','PDF','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','table','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('group_annually_output_medium','chart','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('group_annually_output_medium','table','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('group_annually_output_medium','text_file','3');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('inventory_balance_output_medium','PDF','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('inventory_balance_output_medium','table','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','spreadsheet','3');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','table','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','text_file','3');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('predictivebooks_module','nonprofit','3');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('predictivebooks_module','personal','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('predictivebooks_module','professional','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','aggregate','3');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','display','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('subclassification_option','omit','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('trial_balance_subclassification_option','display','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('trial_balance_subclassification_option','omit','2');
insert into operation (operation,output_yn) values ('delete','n');
insert into operation (operation,output_yn) values ('delete_isa_only',null);
insert into operation (operation,output_yn) values ('detail','y');
insert into operation (operation,output_yn) values ('google_map','y');
insert into operation (operation,output_yn) values ('ledger_propagate','n');
insert into operation (operation,output_yn) values ('null','y');
insert into process_groups (process_group) values ('documentation');
insert into process_groups (process_group) values ('load');
insert into process_groups (process_group) values ('manipulate');
insert into process_groups (process_group) values ('output');
insert into process_groups (process_group) values ('reconcile');
insert into process_groups (process_group) values ('shortcut');
all_done3
) | sql.e 2>&1 | grep -vi duplicate


(
cat << all_done4
insert into self (full_name,			street_address,			inventory_cost_method,			payroll_pay_period,			payroll_beginning_day,			social_security_combined_tax_rate,			social_security_payroll_ceiling,			medicare_combined_tax_rate,			medicare_additional_withholding_rate,			medicare_additional_gross_pay_floor,			federal_withholding_allowance_period_value,			federal_nonresident_withholding_income_premium,			state_withholding_allowance_period_value,			state_itemized_allowance_period_value,			federal_unemployment_wage_base,			federal_unemployment_tax_standard_rate,			federal_unemployment_threshold_rate,			federal_unemployment_tax_minimum_rate,			state_unemployment_wage_base,			state_unemployment_tax_rate,			state_sales_tax_rate) values ('changeme','1234 Main St.','average','weekly','friday','0.1240','127200','0.0290',null,null,'77.90',null,null,'19.50','7000','0.0600','0.0540','0.0060','7000','0.0340','0.0850');
insert into entity (full_name,street_address) values ('changeme','1234 Main St.');
insert into role_appaserver_user (login_name,role) values ( '$login_name','mechanic');
all_done4
) | sql.e 2>&1 | grep -vi duplicate


(
cat << all_done11
insert into subsidiary_transaction (folder,attribute,debit_account,debit_account_folder,credit_account) values ('customer_payment','payment_amount','checking',null,'account_receivable');
insert into subsidiary_transaction (folder,attribute,debit_account,debit_account_folder,credit_account) values ('prior_fixed_asset','extension',null,'fixed_asset','equity');
all_done11
) | sql.e 2>&1 | grep -vi duplicate


(
cat << all_done12
insert into activity (activity) values ('bookkeeping');
insert into activity (activity) values ('buying');
insert into activity (activity) values ('cleaning');
insert into activity (activity) values ('delivering');
insert into activity (activity) values ('designing');
insert into activity (activity) values ('fixing');
insert into activity (activity) values ('idling');
insert into activity (activity) values ('improving');
insert into activity (activity) values ('inspecting');
insert into activity (activity) values ('operating');
insert into activity (activity) values ('packaging');
insert into activity (activity) values ('picking');
insert into activity (activity) values ('programming');
insert into activity (activity) values ('receiving');
insert into activity (activity) values ('reconciling');
insert into activity (activity) values ('recording');
insert into activity (activity) values ('reporting');
insert into activity (activity) values ('researching');
insert into activity (activity) values ('resting');
insert into activity (activity) values ('reworking');
insert into activity (activity) values ('setting up');
insert into activity (activity) values ('storing');
insert into activity (activity) values ('supervising');
insert into activity (activity) values ('supporting');
insert into activity (activity) values ('testing');
all_done12
) | sql.e 2>&1 | grep -vi duplicate


(
cat << all_done13
insert into account ( account, subclassification, hard_coded_account_key ) values ( '$cash_account_name', 'cash', 'cash_key' );
insert into account ( account, subclassification, hard_coded_account_key ) values ( '$equity_account_name', 'contributed_capital', 'closing_key,contributed_capital_key' );
insert into transaction ( full_name, street_address, transaction_date_time, transaction_amount, memo ) values ( 'changeme', '1234 Main St.', '2020-07-27 11:16:54', '$cash_opening_balance', 'Opening entry' );
insert into journal_ledger ( full_name, street_address, transaction_date_time, account, debit_amount ) values ( 'changeme', '1234 Main St.', '2020-07-27 11:16:54', '$cash_account_name', '$cash_opening_balance' );
insert into journal_ledger ( full_name, street_address, transaction_date_time, account, credit_amount ) values ( 'changeme', '1234 Main St.', '2020-07-27 11:16:54', '$equity_account_name', '$cash_opening_balance' );
insert into bank_upload ( bank_date, bank_description, sequence_number, bank_running_balance ) values ( '2020-07-27', 'Opening balance', '1', '$cash_opening_balance' );
insert into bank_upload_transaction ( bank_date, bank_description, full_name, street_address, transaction_date_time ) values ( '2020-07-27', 'Opening balance', 'changeme', '1234 Main St.', '2020-07-27 11:16:54' );
all_done13
) | sql.e 2>&1 | grep -vi duplicate

automatic_transaction_assign.sh all process_name fund >/dev/null
ledger_propagate '' '' ''


(
cat << all_done6
insert into foreign_attribute (folder,related_folder,related_attribute,foreign_attribute,foreign_key_index) values ('customer_sale','mechanic','null','mechanic_full_name','1');
insert into foreign_attribute (folder,related_folder,related_attribute,foreign_attribute,foreign_key_index) values ('customer_sale','mechanic','null','mechanic_street_address','2');
all_done6
) | sql.e 2>&1 | grep -vi duplicate


(
cat << all_done7
insert into vehicle_maker (vehicle_make) values ('Ford');
insert into vehicle (vehicle_make,vehicle_model,vehicle_trim,vehicle_year,assessment) values ('Ford','Mustang','convertible','1966','V-8; stick shift;power top. Great car.');
all_done7
) | sql.e 2>&1 | grep -vi duplicate


(
cat << all_done8
insert into inventory_category (inventory_category) values ('standard');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('Mobil1 Oil Filter','oil_inventory','cost_of_goods_sold','standard','16.99','20','0','0.0000','0.00000');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('Plain oil','oil_inventory','cost_of_goods_sold','standard','10.00','50','0','0.0000','0.00000');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('PZL Platium SAE 5W20','oil_inventory','cost_of_goods_sold','standard','10.00','100','0','0.0000','0.00000');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('PZL Platium SAE 5W30','oil_inventory','cost_of_goods_sold','standard','10.00','100','0','0.0000','0.00000');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('Radiator Cap','coolant_inventory','cost_of_goods_sold','standard','12.99','20','0','0.0000','0.00000');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('Trinity Multi Vehicle Coolant','coolant_inventory','cost_of_goods_sold','standard','3.00','20','0','0.0000','0.00000');
all_done8
) | sql.e 2>&1 | grep -vi duplicate

table=`get_table_name ignored application`
results=`echo "select relative_source_directory from $table;" | sql.e`
relative_source_directory=src_autorepair:${results}
echo "update $table set relative_source_directory = '$relative_source_directory';" | sql.e

(
cat << all_done8
insert into inventory_category (inventory_category) values ('standard');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('Mobil1 Oil Filter','oil_inventory','cost_of_goods_sold','standard','16.99','20','0','0.0000','0.00000');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('Plain oil','oil_inventory','cost_of_goods_sold','standard','10.00','50','0','0.0000','0.00000');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('PZL Platium SAE 5W20','oil_inventory','cost_of_goods_sold','standard','10.00','100','0','0.0000','0.00000');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('PZL Platium SAE 5W30','oil_inventory','cost_of_goods_sold','standard','10.00','100','0','0.0000','0.00000');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('Radiator Cap','coolant_inventory','cost_of_goods_sold','standard','12.99','20','0','0.0000','0.00000');
insert into inventory (inventory_name,inventory_account,cost_of_goods_sold_account,inventory_category,retail_price,reorder_quantity,quantity_on_hand,average_unit_cost,total_cost_balance) values ('Trinity Multi Vehicle Coolant','coolant_inventory','cost_of_goods_sold','standard','3.00','20','0','0.0000','0.00000');
all_done8
) | sql.e 2>&1 | grep -vi duplicate


(
cat << all_done9
insert into fixed_asset (asset_name,account) values ('air_compressor','fixed_asset');
insert into prior_fixed_asset (asset_name,serial_number,service_placement_date,extension,estimated_useful_life_years,estimated_residual_value,depreciation_method,tax_cost_basis,tax_recovery_period) values ('air_compressor','12345',null,'5000.00','10','0.00','straight_line',null,null);
all_done9
) | sql.e 2>&1 | grep -vi duplicate


(
cat << all_done10
insert into hourly_service (service_name,service_category,hourly_rate,credit_account) values ('engine_work','standard','60.00','service_revenue');
insert into fixed_service (service_name,service_category,retail_price,credit_account) values ('Oil change labor','standard','36.99','service_revenue');
insert into fixed_service (service_name,service_category,retail_price,credit_account) values ('Radiator coolant labor','standard','124.00','service_revenue');
all_done10
) | sql.e 2>&1 | grep -vi duplicate

exit 0

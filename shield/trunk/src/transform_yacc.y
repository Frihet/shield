/** \file shield_yacc.yy 

   The mysql parser definition. 
   
   Currently roughly 250 of 1300 language rules are correctly handled,
   enough to handle most select, insert, update, drop table, delete,
   and create table queries. This seems to be almost everything needed
   to run Joomla.
   
   'replace' and 'update ... on duplicate key' queries are non-trivial
   to implement, luckily Joomla does not seem to use them.
   
   @remark package: shield (Originally from the MySQL database server)
   @remark Copyright: MySQL AB, FreeCode AS
   @author MySQL AB, Axel Liljencrantz
   
   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; version 3.
   
*/

%{

#include <vector>
#include <string>

#include "include/transform.hh"
#include "include/exception.hh"
#include "include/util.hh"

  /**
     This is the text contents of the last element parsed by the lexer. We need to be _very_ careful when we use this, since the parser sometimes needs to perform lookahead, in which case yytext points to the contents of the element _after_ the current one. For this reason, yytext should only ever be used on tokens which 
  */
extern char *yytext;

namespace shield
{

  namespace transform
  {

    using namespace util;

    /**
       Ugly hack - inject symbols from transform_yacc.hh into the
       shield::transform namespace. This is a plain C file, and we
       don't want to pollute the global namespace...
    */
#include "build/transform_yacc.hh"

    int yylex ();
    
    namespace
      {
	int yylex_val;
      }

%}

%union {
  chain *chain_val;
  create_table *create_table_val;
  identity *identity_val;
  insert *insert_val;
  limit *limit_val;
  printable *printable_val;
  select *select_val;
  text *text_val;
  type *type_val;
  key_type key_type_val;
  unsigned long long ull_val;
  bool bool_val;
  interval *interval_val;
  printable_alias *select_item_val;
}

%locations

%token  NOTHING
%token  ABORT_SYM
%token  ACTION
%token  ADD
%token  ADDDATE_SYM
%token  AFTER_SYM
%token  AGAINST
%token  AGGREGATE_SYM
%token  ALGORITHM_SYM
%token  ALL
%token  ALTER
%token  ANALYZE_SYM
%token  AND_AND_SYM
%token  AND_SYM
%token  ANY_SYM
%token  AS
%token  ASC
%token  ASCII_SYM
%token  ASENSITIVE_SYM
%token  ATAN
%token  AUTO_INC
%token  AVG_ROW_LENGTH
%token  AVG_SYM
%token  BACKUP_SYM
%token  BEFORE_SYM
%token  BEGIN_SYM
%token  BENCHMARK_SYM
%token  BERKELEY_DB_SYM
%token  BIGINT
%token  BINARY
%token  BINLOG_SYM
%token  BIN_NUM
%token  BIT_AND
%token  BIT_OR
%token  BIT_SYM
%token  BIT_XOR
%token  BLOB_SYM
%token  BLOCK_SYM
%token  BOOLEAN_SYM
%token  BOOL_SYM
%token  BOTH
%token  BTREE_SYM
%token  BY
%token  BYTE_SYM
%token  CACHE_SYM
%token  CALL_SYM
%token  CASCADE
%token  CASCADED
%token  CAST_SYM
%token  CHAIN_SYM
%token  CHANGE
%token  CHANGED
%token  CHARSET
%token  CHAR_SYM
%token  CHECKSUM_SYM
%token  CHECK_SYM
%token  CIPHER_SYM
%token  CLIENT_SYM
%token  CLOSE_SYM
%token  COALESCE
%token  CODE_SYM
%token  COLLATE_SYM
%token  COLLATION_SYM
%token  COLUMNS
%token  COLUMN_SYM
%token  COMMENT_SYM
%token  COMMITTED_SYM
%token  COMMIT_SYM
%token  COMPACT_SYM
%token  COMPRESSED_SYM
%token  CONCAT
%token  CONCAT_WS
%token  CONCURRENT
%token  CONDITION_SYM
%token  CONNECTION_SYM
%token  CONSISTENT_SYM
%token  CONSTRAINT
%token  CONTAINS_SYM
%token  CONTEXT_SYM
%token  CONTINUE_SYM
%token  CONVERT_SYM
%token  CONVERT_TZ_SYM
%token  COUNT_SYM
%token  CPU_SYM
%token  CREATE
%token  CROSS
%token  CUBE_SYM
%token  CURDATE
%token  CURRENT_USER
%token  CURSOR_SYM
%token  CURTIME
%token  DATABASE
%token  DATABASES
%token  DATA_SYM
%token  DATETIME
%token  DATE_ADD_INTERVAL
%token  DATE_SUB_INTERVAL
%token  DATE_SYM
%token  DAY_HOUR_SYM
%token  DAY_MICROSECOND_SYM
%token  DAY_MINUTE_SYM
%token  DAY_SECOND_SYM
%token  DAY_SYM
%token  DEALLOCATE_SYM
%token  DECIMAL_NUM
%token  DECIMAL_SYM
%token  DECLARE_SYM
%token  DECODE_SYM
%token  DEFAULT
%token  DEFINER_SYM
%token  DELAYED_SYM
%token  DELAY_KEY_WRITE_SYM
%token  DELETE_SYM
%token  DESC
%token  DESCRIBE
%token  DES_DECRYPT_SYM
%token  DES_ENCRYPT_SYM
%token  DES_KEY_FILE
%token  DETERMINISTIC_SYM
%token  DIRECTORY_SYM
%token  DISABLE_SYM
%token  DISCARD
%token  DISTINCT
%token  DIV_SYM
%token  DOUBLE_SYM
%token  DO_SYM
%token  DROP
%token  DUAL_SYM
%token  DUMPFILE
%token  DUPLICATE_SYM
%token  DYNAMIC_SYM
%token  EACH_SYM
%token  ELSEIF_SYM
%token  ELT_FUNC
%token  ENABLE_SYM
%token  ENCLOSED
%token  ENCODE_SYM
%token  ENCRYPT
%token  END
%token  ENGINES_SYM
%token  ENGINE_SYM
%token  ENUM
%token  EQ
%token  EQUAL_SYM
%token  ERRORS
%token  ESCAPED
%token  ESCAPE_SYM
%token  EVENTS_SYM
%token  EXECUTE_SYM
%token  EXISTS
%token  EXIT_SYM
%token  EXPANSION_SYM
%token  EXPORT_SET
%token  EXTENDED_SYM
%token  EXTRACT_SYM
%token  FALSE_SYM
%token  FAST_SYM
%token  FAULTS_SYM
%token  FETCH_SYM
%token  FIELD_FUNC
%token  FILE_SYM
%token  FIRST_SYM
%token  FIXED_SYM
%token  FLOAT_NUM
%token  FLOAT_SYM
%token  FLUSH_SYM
%token  FORCE_SYM
%token  FOREIGN
%token  FORMAT_SYM
%token  FOR_SYM
%token  FOUND_SYM
%token  FRAC_SECOND_SYM
%token  FROM
%token  FROM_UNIXTIME
%token  FULL
%token  FULLTEXT_SYM
%token  FUNCTION_SYM
%token  FUNC_ARG0
%token  FUNC_ARG1
%token  FUNC_ARG2
%token  FUNC_ARG3
%token  GE
%token  GEOMCOLLFROMTEXT
%token  GEOMETRYCOLLECTION
%token  GEOMETRY_SYM
%token  GEOMFROMTEXT
%token  GEOMFROMWKB
%token  GET_FORMAT
%token  GLOBAL_SYM
%token  GRANT
%token  GRANTS
%token  GREATEST_SYM
%token  GROUP
%token  GROUP_CONCAT_SYM
%token  GROUP_UNIQUE_USERS
%token  GT_SYM
%token  HANDLER_SYM
%token  HASH_SYM
%token  HAVING
%token  HELP_SYM
%token  HEX_NUM
%token  HIGH_PRIORITY
%token  HOSTS_SYM
%token  HOUR_MICROSECOND_SYM
%token  HOUR_MINUTE_SYM
%token  HOUR_SECOND_SYM
%token  HOUR_SYM
%token  IDENT
%token  IDENTIFIED_SYM
%token  IDENT_QUOTED
%token  IF
%token  IGNORE_SYM
%token  IMPORT
%token  INDEXES
%token  INDEX_SYM
%token  INFILE
%token  INNER_SYM
%token  INNOBASE_SYM
%token  INOUT_SYM
%token  INSENSITIVE_SYM
%token  INSERT
%token  INSERT_METHOD
%token  INTERVAL_SYM
%token  INTO
%token  INT_SYM
%token  INVOKER_SYM
%token  IN_SYM
%token  IO_SYM
%token  IPC_SYM
%token  IS
%token  ISOLATION
%token  ISSUER_SYM
%token  ITERATE_SYM
%token  JOIN_SYM
%token  KEYS
%token  KEY_SYM
%token  KILL_SYM
%token  LABEL_SYM
%token  LANGUAGE_SYM
%token  LAST_INSERT_ID
%token  LAST_SYM
%token  LE
%token  LEADING
%token  LEAST_SYM
%token  LEAVES
%token  LEAVE_SYM
%token  LEFT
%token  LEVEL_SYM
%token  LEX_HOSTNAME
%token  LIKE
%token  LIMIT
%token  LINEFROMTEXT
%token  LINES
%token  LINESTRING
%token  LOAD
%token  LOCAL_SYM
%token  LOCATE
%token  LOCATOR_SYM
%token  LOCKS_SYM
%token  LOCK_SYM
%token  LOGS_SYM
%token  LOG_SYM
%token  LONGBLOB
%token  LONGTEXT
%token  LONG_NUM
%token  LONG_SYM
%token  LOOP_SYM
%token  LOW_PRIORITY
%token  LT
%token  MAKE_SET_SYM
%token  MASTER_CONNECT_RETRY_SYM
%token  MASTER_HOST_SYM
%token  MASTER_LOG_FILE_SYM
%token  MASTER_LOG_POS_SYM
%token  MASTER_PASSWORD_SYM
%token  MASTER_PORT_SYM
%token  MASTER_POS_WAIT
%token  MASTER_SERVER_ID_SYM
%token  MASTER_SSL_CAPATH_SYM
%token  MASTER_SSL_CA_SYM
%token  MASTER_SSL_CERT_SYM
%token  MASTER_SSL_CIPHER_SYM
%token  MASTER_SSL_KEY_SYM
%token  MASTER_SSL_SYM
%token  MASTER_SYM
%token  MASTER_USER_SYM
%token  MATCH
%token  MAX_CONNECTIONS_PER_HOUR
%token  MAX_QUERIES_PER_HOUR
%token  MAX_ROWS
%token  MAX_SYM
%token  MAX_UPDATES_PER_HOUR
%token  MAX_USER_CONNECTIONS_SYM
%token  MEDIUMBLOB
%token  MEDIUMINT
%token  MEDIUMTEXT
%token  MEDIUM_SYM
%token  MEMORY_SYM
%token  MERGE_SYM
%token  MICROSECOND_SYM
%token  MIGRATE_SYM
%token  MINUTE_MICROSECOND_SYM
%token  MINUTE_SECOND_SYM
%token  MINUTE_SYM
%token  MIN_ROWS
%token  MIN_SYM
%token  MLINEFROMTEXT
%token  MODE_SYM
%token  MODIFIES_SYM
%token  MODIFY_SYM
%token  MOD_SYM
%token  MONTH_SYM
%token  MPOINTFROMTEXT
%token  MPOLYFROMTEXT
%token  MULTILINESTRING
%token  MULTIPOINT
%token  MULTIPOLYGON
%token  MUTEX_SYM
%token  NAMES_SYM
%token  NAME_SYM
%token  NATIONAL_SYM
%token  NATURAL
%token  NCHAR_STRING
%token  NCHAR_SYM
%token  NDBCLUSTER_SYM
%token  NE
%token  NEW_SYM
%token  NEXT_SYM
%token  NONE_SYM
%token  NOT2_SYM
%token  NOT_SYM
%token  NOW_SYM
%token  NO_SYM
%token  NO_WRITE_TO_BINLOG
%token  NULL_SYM
%token  NUM
%token  NUMERIC_SYM
%token  NVARCHAR_SYM
%token  OFFSET_SYM
%token  OJ_SYM
%token  OLD_PASSWORD
%token  ON
%token  ONE_SHOT_SYM
%token  ONE_SYM
%token  OPEN_SYM
%token  OPTIMIZE
%token  OPTION
%token  OPTIONALLY
%token  OR2_SYM
%token  ORDER_SYM
%token  OR_OR_SYM
%token  OR_SYM
%token  OUTER
%token  OUTFILE
%token  OUT_SYM
%token  PACK_KEYS_SYM
%token  PAGE_SYM
%token  PARTIAL
%token  PASSWORD
%token  PARAM_MARKER
%token  PHASE_SYM
%token  POINTFROMTEXT
%token  POINT_SYM
%token  POLYFROMTEXT
%token  POLYGON
%token  POSITION_SYM
%token  PRECISION
%token  PREPARE_SYM
%token  PREV_SYM
%token  PRIMARY_SYM
%token  PRIVILEGES
%token  PROCEDURE
%token  PROCESS
%token  PROCESSLIST_SYM
%token  PROFILE_SYM
%token  PROFILES_SYM
%token  PURGE
%token  QUARTER_SYM
%token  QUERY_SYM
%token  QUICK
%token  RAID_0_SYM
%token  RAID_CHUNKS
%token  RAID_CHUNKSIZE
%token  RAID_STRIPED_SYM
%token  RAID_TYPE
%token  RAND
%token  READS_SYM
%token  READ_SYM
%token  REAL
%token  RECOVER_SYM
%token  REDUNDANT_SYM
%token  REFERENCES
%token  REGEXP
%token  RELAY_LOG_FILE_SYM
%token  RELAY_LOG_POS_SYM
%token  RELAY_THREAD
%token  RELEASE_SYM
%token  RELOAD
%token  RENAME
%token  REPAIR
%token  REPEATABLE_SYM
%token  REPEAT_SYM
%token  REPLACE
%token  REPLICATION
%token  REQUIRE_SYM
%token  RESET_SYM
%token  RESOURCES
%token  RESTORE_SYM
%token  RESTRICT
%token  RESUME_SYM
%token  RETURNS_SYM
%token  RETURN_SYM
%token  REVOKE
%token  RIGHT
%token  ROLLBACK_SYM
%token  ROLLUP_SYM
%token  ROUND
%token  ROUTINE_SYM
%token  ROWS_SYM
%token  ROW_COUNT_SYM
%token  ROW_FORMAT_SYM
%token  ROW_SYM
%token  RTREE_SYM
%token  SAVEPOINT_SYM
%token  SECOND_MICROSECOND_SYM
%token  SECOND_SYM
%token  SECURITY_SYM
%token  SELECT_SYM
%token  SENSITIVE_SYM
%token  SEPARATOR_SYM
%token  SERIALIZABLE_SYM
%token  SERIAL_SYM
%token  SESSION_SYM
%token  SET
%token  SET_VAR
%token  SHARE_SYM
%token  SHIFT_LEFT
%token  SHIFT_RIGHT
%token  SHOW
%token  SHUTDOWN
%token  SIGNED_SYM
%token  SIMPLE_SYM
%token  SLAVE
%token  SMALLINT
%token  SNAPSHOT_SYM
%token  SOUNDS_SYM
%token  SOURCE_SYM
%token  SPATIAL_SYM
%token  SPECIFIC_SYM
%token  SQLEXCEPTION_SYM
%token  SQLSTATE_SYM
%token  SQLWARNING_SYM
%token  SQL_BIG_RESULT
%token  SQL_BUFFER_RESULT
%token  SQL_CACHE_SYM
%token  SQL_CALC_FOUND_ROWS
%token  SQL_NO_CACHE_SYM
%token  SQL_SMALL_RESULT
%token  SQL_SYM
%token  SQL_THREAD
%token  SSL_SYM
%token  STARTING
%token  START_SYM
%token  STATUS_SYM
%token  STD_SYM
%token  STDDEV_SAMP_SYM
%token  STOP_SYM
%token  STORAGE_SYM
%token  STRAIGHT_JOIN
%token  STRING_SYM
%token  SUBDATE_SYM
%token  SUBJECT_SYM
%token  SUBSTRING
%token  SUBSTRING_INDEX
%token  SUM_SYM
%token  SUPER_SYM
%token  SUSPEND_SYM
%token  SWAPS_SYM
%token  SWITCHES_SYM
%token  SYSDATE
%token  TABLES
%token  TABLESPACE
%token  TABLE_SYM
%token  TEMPORARY
%token  TEMPTABLE_SYM
%token  TERMINATED
%token  TEXT_STRING
%token  TEXT_SYM
%token  TIMESTAMP
%token  TIMESTAMP_ADD
%token  TIMESTAMP_DIFF
%token  TIME_SYM
%token  TINYBLOB
%token  TINYINT
%token  TINYTEXT
%token  TO_SYM
%token  TRAILING
%token  TRANSACTION_SYM
%token  TRIGGER_SYM
%token  TRIGGERS_SYM
%token  TRIM
%token  TRUE_SYM
%token  TRUNCATE_SYM
%token  TYPES_SYM
%token  TYPE_SYM
%token  UDF_RETURNS_SYM
%token  UDF_SONAME_SYM
%token  ULONGLONG_NUM
%token  UNCOMMITTED_SYM
%token  UNDEFINED_SYM
%token  UNDERSCORE_CHARSET
%token  UNDO_SYM
%token  UNICODE_SYM
%token  UNION_SYM
%token  UNIQUE_SYM
%token  UNIQUE_USERS
%token  UNIX_TIMESTAMP
%token  UNKNOWN_SYM
%token  UNLOCK_SYM
%token  UNSIGNED
%token  UNTIL_SYM
%token  UPDATE_SYM
%token  UPGRADE_SYM
%token  USAGE
%token  USER
%token  USE_FRM
%token  USE_SYM
%token  USING
%token  UTC_DATE_SYM
%token  UTC_TIMESTAMP_SYM
%token  UTC_TIME_SYM
%token  VAR_SAMP_SYM
%token  VALUES
%token  VALUE_SYM
%token  VARBINARY
%token  VARCHAR
%token  VARIABLES
%token  VARIANCE_SYM
%token  VARYING
%token  VIEW_SYM
%token  WARNINGS
%token  WEEK_SYM
%token  WHEN_SYM
%token  WHERE
%token  WHILE_SYM
%token  WITH
%token  WORK_SYM
%token  WRITE_SYM
%token  X509_SYM
%token  XA_SYM
%token  XOR
%token  YEARWEEK
%token  YEAR_MONTH_SYM
%token  YEAR_SYM
%token  ZEROFILL

%left   JOIN_SYM INNER_SYM STRAIGHT_JOIN CROSS LEFT RIGHT
/* A dummy token to force the priority of table_ref production in a join. */
%left   TABLE_REF_PRIORITY
%left   SET_VAR
%left	OR_OR_SYM OR_SYM OR2_SYM XOR
%left	AND_SYM AND_AND_SYM
%left	BETWEEN_SYM CASE_SYM WHEN_SYM THEN_SYM ELSE
%left	EQ EQUAL_SYM GE GT_SYM LE LT NE IS LIKE REGEXP IN_SYM
%left	'|'
%left	'&'
%left	SHIFT_LEFT SHIFT_RIGHT
%left	'-' '+'
%left	'*' '/' '%' DIV_SYM MOD_SYM
%left   '^'
%left	NEG '~'
%right	NOT_SYM NOT2_SYM
%right	BINARY COLLATE_SYM

%type <printable_val> TEXT_STRING simple_ident TEXT_STRING_filesystem TEXT_STRING_literal 
%type <printable_val> text_literal literal term_operator create opt_bin_mod update_elem 
%type <printable_val> opt_primary opt_full wild_and_where show_param select_lock_type
%type <printable_val> attribute opt_create_table_options create3 create_table_options 
%type <printable_val> create_table_option default_charset char select_option
%type <printable_val> where_clause having_clause opt_order_clause order_clause procedure_clause into 
%type <printable_val> opt_check_constraint check_constraint
%type <printable_val> IDENT IDENT_QUOTED DECIMAL_NUM FLOAT_NUM NUM LONG_NUM HEX_NUM
%type <printable_val> LEX_HOSTNAME ULONGLONG_NUM ident_or_text
%type <printable_val> UNDERSCORE_CHARSET olap_opt
%type <printable_val> NCHAR_STRING opt_component key_cache_name
%type <printable_val> sp_opt_label BIN_NUM label_ident 
%type <printable_val> expr simple_expr factor bool_term bool_and_expr bool_or_expr bool_factor 
%type <printable_val>  verb_clause
%type <printable_val> table_ident_nodb references xid
%type <printable_val> opt_ident opt_db text_or_password opt_constraint constraint ident_or_empty
%type <printable_val> text_string opt_gconcat_separator
%type <printable_val>
	real_type order_dir lock_option
	udf_type opt_local opt_table_options table_options
        table_option opt_no_write_to_binlog
        delete_option opt_temporary all_or_any opt_distinct
        opt_ignore_leaves fulltext_options spatial_type union_option
        start_transaction_opts opt_chain opt_release
        union_opt select_derived_init option_type2
%type <printable_val>
	raid_types merge_insert_types
%type <printable_val>
	replace_lock_option opt_low_priority insert_lock_option load_data_lock
%type <printable_val> insert_ident order_ident default_collation
%type <printable_val> opt_expr opt_else sum_expr in_sum_expr
%type <printable_val> variable variable_aux bool_test bool_pri 
%type <printable_val> predicate bit_expr bit_term bit_factor value_expr term udf_expr
%type <printable_val> expr_or_default set_expr_or_default interval_expr
%type <printable_val> param_marker geometry_function signed_literal now_or_signed_literal opt_escape
%type <printable_val> sp_opt_default simple_ident_nospvar simple_ident_q field_or_var 
%type <printable_val> NUM_literal
%type <printable_val> when_list ident_list ident_list_arg opt_expr_list
%type <printable_val> option_type opt_var_type opt_var_ident_type
%type <printable_val> opt_unique_or_fulltext key_alg opt_btree_or_rtree
%type <printable_val> join_table key_part key_usage_list using_list
        table_factor table_ref
        select_derived 
%type <printable_val> date_time_type;
%type <printable_val> storage_engines
%type <printable_val> call sp_proc_stmts sp_proc_stmts1 sp_proc_stmt
%type <printable_val> sp_decl_idents sp_opt_inout sp_handler_type sp_hcond_list
%type <printable_val> sp_decls sp_decl sp_cursor_stmt sp_name sp_cond sp_hcond
%type <printable_val> row_types isolation_types handler_rkey_mode cast_type
%type <printable_val> udf_func_type
%type <printable_val> FUNC_ARG0 FUNC_ARG1 FUNC_ARG2 FUNC_ARG3 
%type <printable_val> user grant_user
%type <printable_val>
	opt_collate
	charset_name
	charset_name_or_default
	old_or_new_charset_name
	old_or_new_charset_name_or_default
	collation_name
	collation_name_or_default
%type <printable_val> internal_variable_name
%type <printable_val> select_init
%type <printable_val> subselect subselect_init
	get_select_lex
%type <printable_val> comp_op
%type <printable_val> query change select do drop replace 
	update delete truncate rename
	show describe load alter optimize keycache preload flush
	reset purge begin commit rollback savepoint release
	slave master_def master_defs master_file_def slave_until_opts
	repair restore backup analyze check start checksum
	field_list_item field_spec kill column_def key_def
	keycache_list assign_to_keycache preload_list preload_keys
	no_braces delete_limit_clause 
	procedure_list procedure_list2 procedure_item handler
	opt_ignore opt_column opt_restrict
	grant revoke set lock unlock string_list field_options field_option
	field_opt_list opt_binary table_lock_list table_lock
	ref_list opt_on_delete opt_on_delete_list opt_on_delete_item use
	opt_delete_options opt_delete_option varchar nchar nvarchar
	opt_outer table_name opt_option opt_place column_list column_list_id
	opt_column_list grant_privileges grant_ident grant_list grant_option
	object_privilege object_privilege_list user_list rename_list
	clear_privileges flush_options flush_option
	equal optional_braces opt_key_definition key_usage_list2
	opt_mi_check_type opt_to mi_check_types normal_join
	table_to_table_list table_to_table opt_table_list opt_as
	handler_rkey_function handler_read_or_scan
	single_multi table_wild_list table_wild_one opt_wild
	union_clause union_list precision subselect_start opt_and charset
	subselect_end select_var_list select_var_list_init help 
	opt_extended_describe prepare prepare_src execute deallocate statement sp_suid
	sp_c_chistics sp_a_chistics sp_chistic sp_c_chistic xa
        load_data opt_field_or_var_spec fields_or_vars opt_load_data_set_spec
        definer view_replace_or_algorithm view_replace view_algorithm_opt
        view_algorithm view_or_trigger_or_sp view_or_trigger_or_sp_tail
	view_suid view_tail view_list_opt view_list view_select
	view_check_option trigger_tail sp_tail
        case_stmt_specification simple_case_stmt searched_case_stmt 
%type <printable_val>
	'-' '+' '*' '/' '%' '(' ')'
	',' '!' '{' '}' '&' '|' AND_SYM OR_SYM OR_OR_SYM BETWEEN_SYM CASE_SYM
	THEN_SYM WHEN_SYM DIV_SYM MOD_SYM OR2_SYM AND_AND_SYM

%type <text_val> ident IDENT_sys keyword keyword_sp keyword_reserved ident_any select_alias 
%type <text_val> opt_table_alias TEXT_STRING_sys 

%type<identity_val> table_ident field_ident insert2 insert_table

%type <select_item_val> select_item2 select_item table_wild

%type <select_val> select_part2 select_init2 select_from select_into opt_select_from 
%type <select_val> select_derived2 select_paren

%type <key_type_val> key_type constraint_key_type opt_key_or_index

%type <chain_val> insert_values values_list udf_expr_list udf_expr_list2 udf_expr_list3 
%type <chain_val> ident_eq_list ident_eq_value opt_attribute opt_attribute_list update_list
%type <chain_val> expr_list expr_list2 key_list opt_values values select_options 
%type <chain_val> select_option_list opt_len order_list group_clause group_list
%type <chain_val> select_item_list derived_table_list join_table_list opt_insert_update 
%type <chain_val> field_list table_list fields 

%type <insert_val> insert insert_field_spec

%type <create_table_val> create2a create2 

%type <bool_val> if_exists opt_if_not_exists

%type <type_val> type 

%type <ull_val> limit_option num ulong_num ulonglong_num

%type <limit_val> limit_options limit_clause opt_limit_clause

%type <interval_val> interval interval_time_st



%%

query: 
	 verb_clause opt_end
          { 
	    
	    if ($1)
	    {
	      chain *query_list = new chain ();
	      root = new fake_query (query_list);
	      query_list->push ($1);
	    }
	    else
	    {
	      root = 0;
	    }
	    
	  }
        ;

verb_clause:
	  statement
	| begin
	;

opt_end:
	  /* Empty */ 
	| opt_end ';'
	;

/* Verb clauses, except begin */
statement:
	  alter
	| analyze
	| backup
	| call
	| change
	| check
	| checksum
	| commit
	| create
	  {
	    $$ = $1;
	  }
        | deallocate
	| delete
	| describe
	| do
	| drop
        | execute
	| flush
	| grant
	| handler
	| help
	| insert 
	    {
	      $$ = $1;
	    }
	| kill
	| load
	| lock
	| optimize
        | keycache
	| preload
        | prepare
	| purge
	| release
	| rename
	| repair
	| replace
	| reset
	| restore
	| revoke
	| rollback
	| savepoint
	| select
	    {
	      $$ = $1;
	    }
	| set
	| show
	| slave
	| start
	| truncate
	| unlock
	| update
	| use
	| xa
        ;

deallocate:
        deallocate_or_drop PREPARE_SYM ident_any
        { throw exception::unsupported (__FILE__, __LINE__); }
	;

deallocate_or_drop:
	DEALLOCATE_SYM |
	DROP
	;


prepare:
        PREPARE_SYM ident_any FROM prepare_src
        { throw exception::unsupported (__FILE__, __LINE__); }
	;

prepare_src:
        TEXT_STRING_sys
        { throw exception::unsupported (__FILE__, __LINE__); }
        | '@' ident_or_text
        { throw exception::unsupported (__FILE__, __LINE__); }
	;

execute:
        EXECUTE_SYM ident_any
        { throw exception::unsupported (__FILE__, __LINE__); }
        execute_using
        {}
        ;

execute_using:
        /* nothing */
        | USING execute_var_list
        ;

execute_var_list:
        execute_var_list ',' execute_var_ident
        | execute_var_ident
        ;

execute_var_ident: '@' ident_or_text
	{ throw exception::unsupported (__FILE__, __LINE__); }
        ;

/* help */

help:
	HELP_SYM
       	ident_or_text
       	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

/* change master */

change:
        CHANGE MASTER_SYM TO_SYM
        { throw exception::unsupported (__FILE__, __LINE__); }
        master_defs
	{}
  	;

master_defs:
        master_def
        | master_defs ',' master_def;

master_def:
        MASTER_HOST_SYM EQ TEXT_STRING_sys
       	{ throw exception::unsupported (__FILE__, __LINE__); }
        |
        MASTER_USER_SYM EQ TEXT_STRING_sys
     	{ throw exception::unsupported (__FILE__, __LINE__); }
       |
       MASTER_PASSWORD_SYM EQ TEXT_STRING_sys
     	{ throw exception::unsupported (__FILE__, __LINE__); }       
       |
       MASTER_PORT_SYM EQ ulong_num
     	{ throw exception::unsupported (__FILE__, __LINE__); }
       |
       MASTER_CONNECT_RETRY_SYM EQ ulong_num
     	{ throw exception::unsupported (__FILE__, __LINE__); }
       | MASTER_SSL_SYM EQ ulong_num
     	{ throw exception::unsupported (__FILE__, __LINE__); }
       | MASTER_SSL_CA_SYM EQ TEXT_STRING_sys
     	{ throw exception::unsupported (__FILE__, __LINE__); }
       | MASTER_SSL_CAPATH_SYM EQ TEXT_STRING_sys
     	{ throw exception::unsupported (__FILE__, __LINE__); }
       | MASTER_SSL_CERT_SYM EQ TEXT_STRING_sys
     	{ throw exception::unsupported (__FILE__, __LINE__); }
       | MASTER_SSL_CIPHER_SYM EQ TEXT_STRING_sys
     	{ throw exception::unsupported (__FILE__, __LINE__); }
       | MASTER_SSL_KEY_SYM EQ TEXT_STRING_sys
     	{ throw exception::unsupported (__FILE__, __LINE__); }
       |
         master_file_def
       ;

master_file_def:
       MASTER_LOG_FILE_SYM EQ TEXT_STRING_sys
       { throw exception::unsupported (__FILE__, __LINE__); }
       | MASTER_LOG_POS_SYM EQ ulonglong_num
         { throw exception::unsupported (__FILE__, __LINE__); }
       | RELAY_LOG_FILE_SYM EQ TEXT_STRING_sys
         { throw exception::unsupported (__FILE__, __LINE__); }
       | RELAY_LOG_POS_SYM EQ ulong_num
         { throw exception::unsupported (__FILE__, __LINE__); }
       ;

/* create a table */

create:
	CREATE opt_table_options TABLE_SYM opt_if_not_exists table_ident create2
	  {
	    $6 -> set_name ($5);
	    $6 -> set_check_existance ($4);
	    $6 -> set_table_options ($2);
	    $$ = $6;
	  }
	| CREATE opt_unique_or_fulltext INDEX_SYM ident_any key_alg ON table_ident
	   '(' key_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CREATE DATABASE opt_if_not_exists ident_any
	  opt_create_database_options
	  { 
	    $$ = 0;
	  }
	| CREATE
	  view_or_trigger_or_sp
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CREATE USER clear_privileges grant_list
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

clear_privileges:
        /* Nothing */
        { throw exception::unsupported (__FILE__, __LINE__); }
        ;

sp_name:
	  ident '.' ident_any
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

create_function_tail:
	  RETURNS_SYM udf_type UDF_SONAME_SYM TEXT_STRING_sys
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| '('
	  { throw exception::unsupported (__FILE__, __LINE__); }
          sp_fdparam_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  RETURNS_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  type
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_c_chistics
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_proc_stmt
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_a_chistics:
	  /* Empty */ {}
	| sp_a_chistics sp_chistic {}
	;

sp_c_chistics:
	  /* Empty */ {}
	| sp_c_chistics sp_c_chistic {}
	;

/* Characteristics for both create and alter */
sp_chistic:
	  COMMENT_SYM TEXT_STRING_sys
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LANGUAGE_SYM SQL_SYM
	  { /* Just parse it, we only have one language for now. */ }
	| NO_SYM SQL_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CONTAINS_SYM SQL_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| READS_SYM SQL_SYM DATA_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MODIFIES_SYM SQL_SYM DATA_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| sp_suid
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

/* Create characteristics */
sp_c_chistic:
	  sp_chistic            { }
	| DETERMINISTIC_SYM     { throw exception::unsupported (__FILE__, __LINE__); }
	| not DETERMINISTIC_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_suid:
	  SQL_SYM SECURITY_SYM DEFINER_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SQL_SYM SECURITY_SYM INVOKER_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

call:
	  CALL_SYM sp_name
	  { throw exception::unsupported (__FILE__, __LINE__); }
          opt_sp_cparam_list {}
	;

/* CALL parameters */
opt_sp_cparam_list:
	  /* Empty */
	| '(' opt_sp_cparams ')'
	;

opt_sp_cparams:
          /* Empty */
        | sp_cparams
        ;

sp_cparams:
	  sp_cparams ',' expr
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| expr
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

/* Stored FUNCTION parameter declaration list */
sp_fdparam_list:
	  /* Empty */
	| sp_fdparams
	;

sp_fdparams:
	  sp_fdparams ',' sp_fdparam
	| sp_fdparam
	;

sp_init_param:
	  /* Empty */
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_fdparam:
	  ident sp_init_param type
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

/* Stored PROCEDURE parameter declaration list */
sp_pdparam_list:
	  /* Empty */
	| sp_pdparams
	;

sp_pdparams:
	  sp_pdparams ',' sp_pdparam
	| sp_pdparam
	;

sp_pdparam:
	  sp_opt_inout sp_init_param ident type
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_opt_inout:
	  /* Empty */ { throw exception::unsupported (__FILE__, __LINE__); }
	| IN_SYM      { throw exception::unsupported (__FILE__, __LINE__); }
	| OUT_SYM     { throw exception::unsupported (__FILE__, __LINE__); }
	| INOUT_SYM   { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_proc_stmts:
	  /* Empty */ {}
	| sp_proc_stmts  sp_proc_stmt ';'
	;

sp_proc_stmts1:
	  sp_proc_stmt ';' {}
	| sp_proc_stmts1  sp_proc_stmt ';'
	;

sp_decls:
	  /* Empty */
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| sp_decls sp_decl ';'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_decl:
          DECLARE_SYM sp_decl_idents
          { throw exception::unsupported (__FILE__, __LINE__); }
          type
          sp_opt_default
          { throw exception::unsupported (__FILE__, __LINE__); }
	| DECLARE_SYM ident_any CONDITION_SYM FOR_SYM sp_cond
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DECLARE_SYM sp_handler_type HANDLER_SYM FOR_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_hcond_list sp_proc_stmt
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DECLARE_SYM ident_any CURSOR_SYM FOR_SYM sp_cursor_stmt
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_cursor_stmt:
	  statement
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_handler_type:
	  EXIT_SYM      { throw exception::unsupported (__FILE__, __LINE__); }
	| CONTINUE_SYM  { throw exception::unsupported (__FILE__, __LINE__); }
/*	| UNDO_SYM      { QQ No yet } */
	;

sp_hcond_list:
	  sp_hcond
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| sp_hcond_list ',' sp_hcond
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_cond:
	  ulong_num
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SQLSTATE_SYM opt_value TEXT_STRING_literal
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_value:
	  /* Empty */  {}
	| VALUE_SYM    {}
	;

sp_hcond:
	  sp_cond
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ident			/* CONDITION name */
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SQLWARNING_SYM	/* SQLSTATEs 01??? */
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| not FOUND_SYM		/* SQLSTATEs 02??? */
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SQLEXCEPTION_SYM	/* All other SQLSTATEs */
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_decl_idents:
	  ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| sp_decl_idents ',' ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_opt_default:
	  /* Empty */ { throw exception::unsupported (__FILE__, __LINE__); }
        | DEFAULT expr { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_proc_stmt:
	  statement
	  { throw exception::unsupported (__FILE__, __LINE__); }
          | RETURN_SYM 
          { throw exception::unsupported (__FILE__, __LINE__); }
          expr
	  { throw exception::unsupported (__FILE__, __LINE__); }
        | IF
          { throw exception::unsupported (__FILE__, __LINE__); }
          sp_if END IF
          { throw exception::unsupported (__FILE__, __LINE__); }
        | case_stmt_specification
	| sp_labeled_control
	  {}
	| { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_unlabeled_control
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LEAVE_SYM label_ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ITERATE_SYM label_ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| OPEN_SYM ident_any
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| FETCH_SYM sp_opt_fetch_noise ident INTO
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_fetch_list
	  { }
	| CLOSE_SYM ident_any
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_opt_fetch_noise:
	  /* Empty */
	| NEXT_SYM FROM
	| FROM
	;

sp_fetch_list:
	  ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	|
	  sp_fetch_list ',' ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_if:
          { throw exception::unsupported (__FILE__, __LINE__); }
          expr THEN_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_proc_stmts1
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_elseifs
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_elseifs:
	  /* Empty */
	| ELSEIF_SYM sp_if
	| ELSE sp_proc_stmts1
	;

case_stmt_specification:
          simple_case_stmt
        | searched_case_stmt
        ;

simple_case_stmt:
          CASE_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
          expr
          { throw exception::unsupported (__FILE__, __LINE__); }
          simple_when_clause_list
          else_clause_opt
          END
          CASE_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
        ;

searched_case_stmt:
          CASE_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
          searched_when_clause_list
          else_clause_opt
          END
          CASE_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
        ;

simple_when_clause_list:
          simple_when_clause
        | simple_when_clause_list simple_when_clause
        ;

searched_when_clause_list:
          searched_when_clause
        | searched_when_clause_list searched_when_clause
        ;

simple_when_clause:
          WHEN_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
          expr
          { throw exception::unsupported (__FILE__, __LINE__); }
          THEN_SYM
          sp_proc_stmts1
          { throw exception::unsupported (__FILE__, __LINE__); }
        ;

searched_when_clause:
          WHEN_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
          expr
          { throw exception::unsupported (__FILE__, __LINE__); }
          THEN_SYM
          sp_proc_stmts1
          { throw exception::unsupported (__FILE__, __LINE__); }
        ;

else_clause_opt:
          /* empty */
          { throw exception::unsupported (__FILE__, __LINE__); }
        | ELSE sp_proc_stmts1
        ;

sp_labeled_control:
	  label_ident ':'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_unlabeled_control sp_opt_label
          { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_opt_label:
        /* Empty  */    { throw exception::unsupported (__FILE__, __LINE__); }
        | label_ident   { throw exception::unsupported (__FILE__, __LINE__); }
	;

sp_unlabeled_control:
	  BEGIN_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_decls
	  sp_proc_stmts
	  END
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LOOP_SYM
	  sp_proc_stmts1 END LOOP_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
        | WHILE_SYM 
          { throw exception::unsupported (__FILE__, __LINE__); }
          expr DO_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_proc_stmts1 END WHILE_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
        | REPEAT_SYM sp_proc_stmts1 UNTIL_SYM 
          { throw exception::unsupported (__FILE__, __LINE__); }
          expr END REPEAT_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

trg_action_time:
            BEFORE_SYM 
            { throw exception::unsupported (__FILE__, __LINE__); }
          | AFTER_SYM 
            { throw exception::unsupported (__FILE__, __LINE__); }
          ;

trg_event:
            INSERT 
            { throw exception::unsupported (__FILE__, __LINE__); }
          | UPDATE_SYM
            { throw exception::unsupported (__FILE__, __LINE__); }
          | DELETE_SYM
            { throw exception::unsupported (__FILE__, __LINE__); }
          ;

create2:
'(' create2a 
	  {
	    $$ = $2;
	  }
        | opt_create_table_options create3 
	{
	  throw exception::unsupported (__FILE__, __LINE__); 
	}
        | LIKE table_ident
          {
	    $$ = new create_table ();
	    $$ -> set_like_clause ($2);
	  }
        | '(' LIKE table_ident ')'
          {
	    $$ = new create_table ();
	    $$ -> set_like_clause ($3);
	  }
        ;

create2a:
        field_list ')' opt_create_table_options create3 
	  {
	    $$ = new create_table ();
	    $$ -> set_field_list ($1);
	    $$ -> set_create_options ($3);
	    $$ -> set_initial_data ($4);
	  }
	|  create_select ')' union_opt
	{
	  throw exception::unsupported (__FILE__, __LINE__); 
	}
        ;

create3:
	/* empty */ { $$ = 0; }
	| opt_duplicate opt_as     create_select
          { throw exception::unsupported (__FILE__, __LINE__); } union_clause {}
	| opt_duplicate opt_as '(' create_select ')'
          { throw exception::unsupported (__FILE__, __LINE__); } union_opt {}
        ;

create_select:
          SELECT_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
          select_options select_item_list
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  opt_select_from
	  { throw exception::unsupported (__FILE__, __LINE__); }
        ;

opt_as:
	/* empty */ {}
	| AS	    {};

opt_create_database_options:
	/* empty */			{}
	| create_database_options	{};

create_database_options:
	create_database_option					{}
	| create_database_options create_database_option	{};

create_database_option:
	default_collation   {}
	| default_charset   {};

opt_table_options:
	/* empty */	 
          {
	    $$ = 0;
	  }
	| table_options
        ;

table_options:
	table_option	
	| table_option table_options { $$ = new chain ($1, $2) }
	;

table_option:
	TEMPORARY	{ throw exception::unsupported (__FILE__, __LINE__); };

opt_if_not_exists:
	/* empty */	 { $$ = false; }
	| IF not EXISTS	 { $$=true; }

opt_create_table_options:
	/* empty */
	  {
	    $$ = 0;
	  }
	| create_table_options;

create_table_options_space_separated:
	create_table_option
	| create_table_option create_table_options_space_separated
	;

create_table_options:
	create_table_option
	| create_table_option     create_table_options 
	  {
	    $$ = new chain ($1, $2);
	  }
	| create_table_option ',' create_table_options
	  {
	    $$ = new chain ($1, $3);
	  }
	  ;

create_table_option:
	ENGINE_SYM opt_equal storage_engines    { $$ = 0; }
	| TYPE_SYM opt_equal storage_engines    { $$ = 0; }
	| MAX_ROWS opt_equal ulonglong_num	{ throw exception::unsupported (__FILE__, __LINE__); }
	| MIN_ROWS opt_equal ulonglong_num	{ throw exception::unsupported (__FILE__, __LINE__); }
	| AVG_ROW_LENGTH opt_equal ulong_num	{ throw exception::unsupported (__FILE__, __LINE__); }
	| PASSWORD opt_equal TEXT_STRING_sys	{ throw exception::unsupported (__FILE__, __LINE__); }
	| COMMENT_SYM opt_equal TEXT_STRING_sys	{ throw exception::unsupported (__FILE__, __LINE__); }
	| AUTO_INC opt_equal ulonglong_num	
	{ 
	  if( $3 == 1 )
	    $$ = 0;
	  else
	    throw exception::unsupported (__FILE__, __LINE__); 
	}
        | PACK_KEYS_SYM opt_equal ulong_num
          { throw exception::unsupported (__FILE__, __LINE__); }
        | PACK_KEYS_SYM opt_equal DEFAULT
          { throw exception::unsupported (__FILE__, __LINE__); }
	| CHECKSUM_SYM opt_equal ulong_num	{ throw exception::unsupported (__FILE__, __LINE__); }
	| DELAY_KEY_WRITE_SYM opt_equal ulong_num { throw exception::unsupported (__FILE__, __LINE__); }
	| ROW_FORMAT_SYM opt_equal row_types	{ throw exception::unsupported (__FILE__, __LINE__); }
	| RAID_TYPE opt_equal raid_types
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| RAID_CHUNKS opt_equal ulong_num
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| RAID_CHUNKSIZE opt_equal ulong_num
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| UNION_SYM opt_equal '(' table_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| default_charset
	| default_collation
	| INSERT_METHOD opt_equal merge_insert_types   { throw exception::unsupported (__FILE__, __LINE__); }
	| DATA_SYM DIRECTORY_SYM opt_equal TEXT_STRING_sys { throw exception::unsupported (__FILE__, __LINE__); }
	| INDEX_SYM DIRECTORY_SYM opt_equal TEXT_STRING_sys { throw exception::unsupported (__FILE__, __LINE__); }
	| CONNECTION_SYM opt_equal TEXT_STRING_sys { throw exception::unsupported (__FILE__, __LINE__); }
        ;

default_charset:
        opt_default charset opt_equal charset_name_or_default
	{ $$ = 0; /* Unsupported */ }

default_collation:
        opt_default COLLATE_SYM opt_equal collation_name_or_default
	{ $$ = 0; /* unsupportedd */ };

storage_engines:
	ident_or_text
	;

row_types:
	DEFAULT		{ throw exception::unsupported (__FILE__, __LINE__); }
	| FIXED_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| DYNAMIC_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| COMPRESSED_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| REDUNDANT_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| COMPACT_SYM	{ throw exception::unsupported (__FILE__, __LINE__); };

raid_types:
	RAID_STRIPED_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| RAID_0_SYM	 { throw exception::unsupported (__FILE__, __LINE__); }
	| ulong_num	 { throw exception::unsupported (__FILE__, __LINE__); };

merge_insert_types:
       NO_SYM            { throw exception::unsupported (__FILE__, __LINE__); }
       | FIRST_SYM       { throw exception::unsupported (__FILE__, __LINE__); }
       | LAST_SYM        { throw exception::unsupported (__FILE__, __LINE__); };

opt_select_from:
	opt_limit_clause { throw exception::unsupported (__FILE__, __LINE__); }
	| select_from select_lock_type
	{
	  $$ = $1;

	  if( $2 )
	    throw exception::unsupported (__FILE__, __LINE__); 
	}
	;

udf_func_type:
	/* empty */	{ throw exception::unsupported (__FILE__, __LINE__); }
	| AGGREGATE_SYM { throw exception::unsupported (__FILE__, __LINE__); };

udf_type:
	STRING_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| REAL { throw exception::unsupported (__FILE__, __LINE__); }
        | DECIMAL_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| INT_SYM { throw exception::unsupported (__FILE__, __LINE__); };

field_list:
	  field_list_item
	  {
	    $$ = new chain ($1);
	    $$ -> set_line_break (1);
	    $$ -> set_separator (",");
	  }
	  | field_list ',' field_list_item
	  {
	    $$ ->push ($3);
	  }
	;


field_list_item:
	   column_def
         | key_def
         ;

column_def:
	  field_spec opt_check_constraint
	{
	  if ($2)
	    {
	      throw exception::unsupported (__FILE__, __LINE__); 
	    }

	  $$ = $1;
	}
	| field_spec references
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

key_def:
	key_type opt_ident key_alg '(' key_list ')'
	  {
	    create_index *key = new create_index ();
	    key -> set_key_type ($1);
	    key -> set_name ($2);
	    key -> set_key_list ($5);
	    $$ = key;
	  }
	| opt_constraint constraint_key_type opt_ident key_alg '(' key_list ')'
	  { 
	    create_index *key = new create_index ();
	    key -> set_key_type ($2);
	    key -> set_name ($3);
	    key -> set_key_list ($6);
	    $$ = key;
	  }
	| opt_constraint FOREIGN KEY_SYM opt_ident '(' key_list ')' references
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| constraint opt_check_constraint
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| opt_constraint check_constraint
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_check_constraint:
	/* empty */
	{
	  $$=0;
	}
	| check_constraint
	;

check_constraint:
	CHECK_SYM expr { $$ = $2; }
	;

opt_constraint:
	/* empty */
	{ $$ = 0; }	
	| constraint		{ throw exception::unsupported (__FILE__, __LINE__); }
	;

constraint:
	CONSTRAINT opt_ident	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

field_spec:
	field_ident type opt_attribute
	  {
	    $$ = new field_spec ($1->get_field (), $2, $3);
	  }
	;

type:
	int_type opt_len field_options	
	  { 
	      $$ = new type ( DATA_TYPE_NUMBER, $2, $3);
	  }
	| real_type field_options 
	  {
	    $$ = new type (DATA_TYPE_FLOAT, $2);
	  }
	| real_type '(' num ',' num ')' field_options 
	  {
	    $$ = new type (DATA_TYPE_FLOAT, new paran (new text ($3), new text ($5)), $7);
	  }
	| FLOAT_SYM float_options field_options { throw exception::unsupported (__FILE__, __LINE__); }
	| BIT_SYM			{ throw exception::unsupported (__FILE__, __LINE__); }
	| BIT_SYM '(' NUM ')'		{ throw exception::unsupported (__FILE__, __LINE__); }
	| BOOL_SYM			{ throw exception::unsupported (__FILE__, __LINE__); }
	| BOOLEAN_SYM			{ throw exception::unsupported (__FILE__, __LINE__); }
	| char '(' num ')' opt_binary
	  {
	    $$ = new type (DATA_TYPE_CHAR, new paran (new text ($3)), $5);
	  }
	| char opt_binary
	  { 
	    $$ = new type (DATA_TYPE_CHAR, $2);
	  }
	| nchar '(' num ')' opt_bin_mod	{ throw exception::unsupported (__FILE__, __LINE__); }
	| nchar opt_bin_mod		{ throw exception::unsupported (__FILE__, __LINE__); }
	| BINARY '(' num ')'		{ throw exception::unsupported (__FILE__, __LINE__); }
	| BINARY			{ throw exception::unsupported (__FILE__, __LINE__); }
	| varchar '(' num ')' opt_binary 
	  {
	    if( $3 < 4000 )
	      $$ = new type ( DATA_TYPE_VARCHAR, new paran ( new text (stringify ($3))), $5 );
	    else
	      {
		$$ = new type ( DATA_TYPE_CLOB, $5 );
	      }
	  }
	| nvarchar '(' num ')' opt_bin_mod
	  {
	    if( $3 < 4000 )
	      $$ = new type ( DATA_TYPE_VARCHAR, new paran ( new text (stringify ($3))), $5 );
	    else
	      {
		$$ = new type ( DATA_TYPE_CLOB, $5 );
	      }
	  }
	| VARBINARY '(' NUM ')' 	{ throw exception::unsupported (__FILE__, __LINE__); }
	| YEAR_SYM opt_len field_options { throw exception::unsupported (__FILE__, __LINE__); }
	| DATE_SYM			
	{
	  $$ = new type (DATA_TYPE_DATE);
	}
	| TIME_SYM
	{
	  $$ = new type (DATA_TYPE_DATE);
	}
	| TIMESTAMP opt_len
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| DATETIME			
	{
	  $$ = new type (DATA_TYPE_DATETIME);
	}
	| TINYBLOB			{ throw exception::unsupported (__FILE__, __LINE__); }
	| BLOB_SYM opt_len		{ throw exception::unsupported (__FILE__, __LINE__); }
	| spatial_type
          { throw exception::unsupported (__FILE__, __LINE__); }
	| MEDIUMBLOB			{ throw exception::unsupported (__FILE__, __LINE__); }
	| LONGBLOB			{ throw exception::unsupported (__FILE__, __LINE__); }
	| LONG_SYM VARBINARY		{ throw exception::unsupported (__FILE__, __LINE__); }
	| LONG_SYM varchar opt_binary	{ throw exception::unsupported (__FILE__, __LINE__); }
	| TINYTEXT opt_binary	
	  {
	    $$ = new type ( DATA_TYPE_CLOB, $2 );
	  }
	| TEXT_SYM opt_len opt_binary
	  {
	    $$ = new type ( DATA_TYPE_CLOB, $3 );
	  }
	| MEDIUMTEXT opt_binary
	  {
	    $$ = new type ( DATA_TYPE_CLOB, $2 );
	  }
	| LONGTEXT opt_binary	      
	  {
	    $$ = new type ( DATA_TYPE_CLOB, $2 );
	  }
	| DECIMAL_SYM float_options field_options
                                        { throw exception::unsupported (__FILE__, __LINE__); }
	| NUMERIC_SYM float_options field_options
                                        { throw exception::unsupported (__FILE__, __LINE__); }
	| FIXED_SYM float_options field_options
                                        { throw exception::unsupported (__FILE__, __LINE__); }
	| ENUM { throw exception::unsupported (__FILE__, __LINE__); } '(' string_list ')' opt_binary
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SET { throw exception::unsupported (__FILE__, __LINE__); } '(' string_list ')' opt_binary
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LONG_SYM opt_binary		{ throw exception::unsupported (__FILE__, __LINE__); }
	| SERIAL_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

spatial_type:
	GEOMETRY_SYM	      { throw exception::unsupported (__FILE__, __LINE__); }
	| GEOMETRYCOLLECTION  { throw exception::unsupported (__FILE__, __LINE__); }
	| POINT_SYM           { throw exception::unsupported (__FILE__, __LINE__); }
	| MULTIPOINT          { throw exception::unsupported (__FILE__, __LINE__); }
	| LINESTRING          { throw exception::unsupported (__FILE__, __LINE__); }
	| MULTILINESTRING     { throw exception::unsupported (__FILE__, __LINE__); }
	| POLYGON             { throw exception::unsupported (__FILE__, __LINE__); }
	| MULTIPOLYGON        { throw exception::unsupported (__FILE__, __LINE__); }
	;

char:
	CHAR_SYM {}
	;

nchar:
	NCHAR_SYM {}
	| NATIONAL_SYM CHAR_SYM {}
	;

varchar:
	char VARYING {}
	| VARCHAR {}
	;

nvarchar:
	NATIONAL_SYM VARCHAR {}
	| NVARCHAR_SYM {}
	| NCHAR_SYM VARCHAR {}
	| NATIONAL_SYM CHAR_SYM VARYING {}
	| NCHAR_SYM VARYING {}
	;

int_type:
	INT_SYM		
	| TINYINT	
	| SMALLINT	
	| MEDIUMINT	
	| BIGINT	

real_type:
	REAL		{ throw exception::unsupported (__FILE__, __LINE__); }
	| DOUBLE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| DOUBLE_SYM PRECISION { throw exception::unsupported (__FILE__, __LINE__); };


float_options:
        /* empty */		{ throw exception::unsupported (__FILE__, __LINE__); }
        | '(' NUM ')'		{ throw exception::unsupported (__FILE__, __LINE__); }
	| precision		{};

precision:
	'(' NUM ',' NUM ')'
	{ throw exception::unsupported (__FILE__, __LINE__); };

/*
  We ignore signed and unsigned filed options, and throw exception::an exception
  on zerofill
*/
field_options:
	/* empty */		{ $$ = 0; }
	| field_opt_list { $$ = 0; }
	;

field_opt_list:
	field_opt_list field_option 
	| field_option;

field_option:
	SIGNED_SYM	
	| UNSIGNED	
	| ZEROFILL	{ throw exception::unsupported (__FILE__, __LINE__); };

opt_len:
	/* empty */	
	  {
	    $$ = new paran (new text ("14"), new text ("0"));
	    $$->set_separator (",");
	  }
	| '(' num ')'	
	  {
	    $$ = new paran (new text (stringify ($2)), new text ("0"));
	    $$->set_separator (",");
	  }

opt_attribute:
	/* empty */ {$$ = 0;}
	| opt_attribute_list {};

opt_attribute_list:
	opt_attribute_list attribute 
	  {
	    $$ = $1;
	    $1 -> push ($2);
	  }
	| attribute
	{
	    $$ = new chain ($1);
	}
	;

attribute:
	NULL_SYM
	  {
	    $$ = new nullable (true);
	  }
	| not NULL_SYM	  
	  {
	    $$ = new nullable (false);
	  }
	| DEFAULT now_or_signed_literal
	  {
	    $$ = new default_value ($2); 
	  }
	| ON UPDATE_SYM NOW_SYM optional_braces 
          { throw exception::unsupported (__FILE__, __LINE__); }
	| AUTO_INC	  
	{
	  $$ = new auto_increment (); 
	}
	| SERIAL_SYM DEFAULT VALUE_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| opt_primary KEY_SYM 
	  {
	    if ($1) 
	      $$ = new chain ($1, new text ("key"));
	    else
	      throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| UNIQUE_SYM	  
       	  {
	    $$ = new text ("unique"); 
	  }
	| UNIQUE_SYM KEY_SYM 
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| COMMENT_SYM TEXT_STRING_sys { throw exception::unsupported (__FILE__, __LINE__); }
	| COLLATE_SYM collation_name
	  { $$ = 0; /* Unsupported - do nothing */ }
	;

now_or_signed_literal:
        NOW_SYM optional_braces { throw exception::unsupported (__FILE__, __LINE__); }
        | signed_literal 
        ;

charset:
	CHAR_SYM SET	{}
	| CHARSET	{}
	;

charset_name:
	ident_or_text
	{ $$ = 0; }
	| BINARY { throw exception::unsupported (__FILE__, __LINE__); }
	;

charset_name_or_default:
	charset_name { $$ = 0; }
	| DEFAULT    { $$ = 0; }


old_or_new_charset_name:
	ident_or_text
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| BINARY { throw exception::unsupported (__FILE__, __LINE__); }
	;

old_or_new_charset_name_or_default:
	old_or_new_charset_name { throw exception::unsupported (__FILE__, __LINE__); }
	| DEFAULT    { throw exception::unsupported (__FILE__, __LINE__); } ;

collation_name:
	ident_or_text
	{ $$ = 0; /* Unsupported - do nothing */ }
	;

opt_collate:
	/* empty */	{ throw exception::unsupported (__FILE__, __LINE__); }
	| COLLATE_SYM collation_name_or_default { throw exception::unsupported (__FILE__, __LINE__); }
	;

collation_name_or_default:
	collation_name { $$=0; /* Unsupported - do nothing */ }
	| DEFAULT    { $$=0; /* Unsupported - do nothing */ }

opt_default:
	/* empty */	{}
	| DEFAULT	{};

opt_binary:
	/* empty */			
	  {
	    $$ = 0;
	  }
	| ASCII_SYM opt_bin_mod		
	{
	  throw exception::unsupported (__FILE__, __LINE__); 
	}
	| BYTE_SYM			
	{
	  throw exception::unsupported (__FILE__, __LINE__); 
	}
	| UNICODE_SYM opt_bin_mod
	{ 
	  throw exception::unsupported (__FILE__, __LINE__); 
	}
	| charset charset_name opt_bin_mod	
	{
	  $$ = 0;
	}
        | BINARY opt_bin_charset 
	{
	  throw exception::unsupported (__FILE__, __LINE__); 
	}
	;

opt_bin_mod:
	/* empty */ { $$ = 0; }
	| BINARY { throw exception::unsupported (__FILE__, __LINE__); };

opt_bin_charset:
        /* empty */ { throw exception::unsupported (__FILE__, __LINE__); }
	| ASCII_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| UNICODE_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| charset charset_name	{ throw exception::unsupported (__FILE__, __LINE__); } ;

opt_primary:
	/* empty */
	{
	  $$ = 0;
	}
	| PRIMARY_SYM
	{
	  $$ = new text ("primary");
	}
	;

references:
	REFERENCES table_ident
	{ throw exception::unsupported (__FILE__, __LINE__); }
	opt_ref_list
	{ throw exception::unsupported (__FILE__, __LINE__); };

opt_ref_list:
	/* empty */ opt_on_delete {}
	| '(' ref_list ')' opt_on_delete {};

ref_list:
	ref_list ',' ident	{ throw exception::unsupported (__FILE__, __LINE__); }
	| ident			{ throw exception::unsupported (__FILE__, __LINE__); };


opt_on_delete:
	/* empty */ {}
	| opt_on_delete_list {};

opt_on_delete_list:
	opt_on_delete_list opt_on_delete_item {}
	| opt_on_delete_item {};

opt_on_delete_item:
	ON DELETE_SYM delete_option   { throw exception::unsupported (__FILE__, __LINE__); }
	| ON UPDATE_SYM delete_option { throw exception::unsupported (__FILE__, __LINE__); }
	| MATCH FULL	{ throw exception::unsupported (__FILE__, __LINE__); }
	| MATCH PARTIAL { throw exception::unsupported (__FILE__, __LINE__); }
	| MATCH SIMPLE_SYM { throw exception::unsupported (__FILE__, __LINE__); };

delete_option:
	RESTRICT	 { throw exception::unsupported (__FILE__, __LINE__); }
	| CASCADE	 { throw exception::unsupported (__FILE__, __LINE__); }
	| SET NULL_SYM   { throw exception::unsupported (__FILE__, __LINE__); }
	| NO_SYM ACTION  { throw exception::unsupported (__FILE__, __LINE__); }
	| SET DEFAULT    { throw exception::unsupported (__FILE__, __LINE__); };

key_type:
	key_or_index			    { $$ = KEY; }
	| FULLTEXT_SYM opt_key_or_index	    { throw exception::unsupported (__FILE__, __LINE__); }
	| SPATIAL_SYM opt_key_or_index
	  { throw exception::unsupported (__FILE__, __LINE__); };

constraint_key_type:
	PRIMARY_SYM KEY_SYM  { $$ = PRIMARY_KEY; }
	| UNIQUE_SYM opt_key_or_index { $$ = UNIQUE; }

key_or_index:
	KEY_SYM 
	| INDEX_SYM ;

opt_key_or_index:
	/* empty */ {$$ = KEY; }
	| key_or_index { $$ = KEY; }
	;

keys_or_index:
	KEYS {}
	| INDEX_SYM {}
	| INDEXES {};

opt_unique_or_fulltext:
	/* empty */	{ throw exception::unsupported (__FILE__, __LINE__); }
	| UNIQUE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| FULLTEXT_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| SPATIAL_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
        ;

key_alg:
	/* empty */		   { $$ = 0; }
	| USING opt_btree_or_rtree { $$ = 0; }
	| TYPE_SYM opt_btree_or_rtree  { $$ = 0; }

opt_btree_or_rtree:
	BTREE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| RTREE_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| HASH_SYM	{ throw exception::unsupported (__FILE__, __LINE__); };

key_list:
	key_list ',' key_part order_dir 
	{
	  $$ = $1;
	  $$ -> push ($3);
	  
	  if ($4 && $4->str () == "desc")
	    {
	      throw exception::syntax ("Descending order indices are not supported");
	    }
	}
	| key_part order_dir		
	{
	  $$ = new chain ($1); 
	  $$ -> set_separator (",");

	  if ($2 && $2->str () == "desc")
	    {
	      throw exception::syntax ("Descending order indices are not supported");
	    }

	}

key_part:
	ident_any			{ $$ = $1; }
	| ident_any '(' NUM ')'	{ $$ = $1; }
	;

opt_ident:
	/* empty */	{ $$ = 0; }
	| field_ident	{ $$ = $1; }

opt_component:
        /* empty */      { throw exception::unsupported (__FILE__, __LINE__); }
        | '.' ident_any      { throw exception::unsupported (__FILE__, __LINE__); };

string_list:
	text_string			{ throw exception::unsupported (__FILE__, __LINE__); }
	| string_list ',' text_string	{ throw exception::unsupported (__FILE__, __LINE__); };

/*
** Alter table
*/

alter:
	ALTER opt_ignore TABLE_SYM table_ident
	{ throw exception::unsupported (__FILE__, __LINE__); }
	alter_list
	{}
	| ALTER DATABASE ident_or_empty
          { throw exception::unsupported (__FILE__, __LINE__); }
          opt_create_database_options
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ALTER PROCEDURE sp_name
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_a_chistics
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ALTER FUNCTION_SYM sp_name
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  sp_a_chistics
	  { throw exception::unsupported (__FILE__, __LINE__); }
        | ALTER view_algorithm_opt definer view_suid
          VIEW_SYM table_ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  view_list_opt AS view_select view_check_option
	  {}
	;

ident_or_empty:
	/* empty */  { throw exception::unsupported (__FILE__, __LINE__); }
	| ident      { throw exception::unsupported (__FILE__, __LINE__); };

alter_list:
	| DISCARD TABLESPACE { throw exception::unsupported (__FILE__, __LINE__); }
	| IMPORT TABLESPACE { throw exception::unsupported (__FILE__, __LINE__); }
        | alter_list_item
	| alter_list ',' alter_list_item;

add_column:
	ADD opt_column
	{ throw exception::unsupported (__FILE__, __LINE__); };

alter_list_item:
	add_column column_def opt_place { }
	| ADD key_def
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| add_column '(' field_list ')'
          { throw exception::unsupported (__FILE__, __LINE__); }
	| CHANGE opt_column field_ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
          field_spec opt_place
        | MODIFY_SYM opt_column field_ident
          { throw exception::unsupported (__FILE__, __LINE__); }
          type opt_attribute
          { throw exception::unsupported (__FILE__, __LINE__); }
          opt_place
	| DROP opt_column field_ident opt_restrict
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DROP FOREIGN KEY_SYM opt_ident
          { throw exception::unsupported (__FILE__, __LINE__); }
	| DROP PRIMARY_SYM KEY_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DROP key_or_index field_ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DISABLE_SYM KEYS
          { throw exception::unsupported (__FILE__, __LINE__); }
	| ENABLE_SYM KEYS
          { throw exception::unsupported (__FILE__, __LINE__); }
	| ALTER opt_column field_ident SET DEFAULT signed_literal
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ALTER opt_column field_ident DROP DEFAULT
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| RENAME opt_to table_ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CONVERT_SYM TO_SYM charset charset_name_or_default opt_collate
	  { throw exception::unsupported (__FILE__, __LINE__); }
        | create_table_options_space_separated
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| FORCE_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| alter_order_clause
	  { throw exception::unsupported (__FILE__, __LINE__); };

opt_column:
	/* empty */	{}
	| COLUMN_SYM	{};

opt_ignore:
	/* empty */	{ $$ = 0; }
	| IGNORE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_restrict:
	/* empty */	{ $$=0; }
	| RESTRICT	{ throw exception::unsupported (__FILE__, __LINE__); }
	| CASCADE	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_place:
	/* empty */	{}
	| AFTER_SYM ident_any { throw exception::unsupported (__FILE__, __LINE__); }
	| FIRST_SYM	  { throw exception::unsupported (__FILE__, __LINE__); };

opt_to:
	/* empty */	{}
	| TO_SYM	{}
	| EQ		{}
	| AS		{};

/*
  SLAVE START and SLAVE STOP are deprecated. We keep them for compatibility.
*/

slave:
	  START_SYM SLAVE slave_thread_opts
          { throw exception::unsupported (__FILE__, __LINE__); }
          slave_until
          {}
        | STOP_SYM SLAVE slave_thread_opts
          { throw exception::unsupported (__FILE__, __LINE__); }
	| SLAVE START_SYM slave_thread_opts
         { throw exception::unsupported (__FILE__, __LINE__); }
          slave_until
          {}
	| SLAVE STOP_SYM slave_thread_opts
         { throw exception::unsupported (__FILE__, __LINE__); }
        ;


start:
	START_SYM TRANSACTION_SYM start_transaction_opts
        { throw exception::unsupported (__FILE__, __LINE__); }
	;

start_transaction_opts:
        /*empty*/ { throw exception::unsupported (__FILE__, __LINE__); }
        | WITH CONSISTENT_SYM SNAPSHOT_SYM
        { throw exception::unsupported (__FILE__, __LINE__); }
        ;

slave_thread_opts:
	{ throw exception::unsupported (__FILE__, __LINE__); }
	slave_thread_opt_list
        {}
	;

slave_thread_opt_list:
	slave_thread_opt
	| slave_thread_opt_list ',' slave_thread_opt
	;

slave_thread_opt:
	/*empty*/	{}
	| SQL_THREAD	{ throw exception::unsupported (__FILE__, __LINE__); }
	| RELAY_THREAD 	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

slave_until:
	/*empty*/	{}
	| UNTIL_SYM slave_until_opts
          { throw exception::unsupported (__FILE__, __LINE__); }
	;

slave_until_opts:
       master_file_def
       | slave_until_opts ',' master_file_def ;


restore:
	RESTORE_SYM table_or_tables
	{ throw exception::unsupported (__FILE__, __LINE__); }
	table_list FROM TEXT_STRING_sys
        { throw exception::unsupported (__FILE__, __LINE__); };

backup:
	BACKUP_SYM table_or_tables
	{ throw exception::unsupported (__FILE__, __LINE__); }
	table_list TO_SYM TEXT_STRING_sys
        { throw exception::unsupported (__FILE__, __LINE__); };

checksum:
        CHECKSUM_SYM table_or_tables
	{ throw exception::unsupported (__FILE__, __LINE__); }
	table_list opt_checksum_type
        {}
	;

opt_checksum_type:
        /* nothing */  { throw exception::unsupported (__FILE__, __LINE__); }
	| QUICK        { throw exception::unsupported (__FILE__, __LINE__); }
	| EXTENDED_SYM { throw exception::unsupported (__FILE__, __LINE__); }
        ;

repair:
	REPAIR opt_no_write_to_binlog table_or_tables
	{ throw exception::unsupported (__FILE__, __LINE__); }
	table_list opt_mi_repair_type
	{}
	;

opt_mi_repair_type:
	/* empty */ { throw exception::unsupported (__FILE__, __LINE__); }
	| mi_repair_types {};

mi_repair_types:
	mi_repair_type {}
	| mi_repair_type mi_repair_types {};

mi_repair_type:
	QUICK          { throw exception::unsupported (__FILE__, __LINE__); }
	| EXTENDED_SYM { throw exception::unsupported (__FILE__, __LINE__); }
        | USE_FRM      { throw exception::unsupported (__FILE__, __LINE__); };

analyze:
	ANALYZE_SYM opt_no_write_to_binlog table_or_tables
	{ throw exception::unsupported (__FILE__, __LINE__); }
	table_list opt_mi_check_type
	{}
	;

check:
	CHECK_SYM table_or_tables
	{ throw exception::unsupported (__FILE__, __LINE__); }
	table_list opt_mi_check_type
	{}
	;

opt_mi_check_type:
	/* empty */ { throw exception::unsupported (__FILE__, __LINE__); }
	| mi_check_types {};

mi_check_types:
	mi_check_type {}
	| mi_check_type mi_check_types {};

mi_check_type:
	QUICK      { throw exception::unsupported (__FILE__, __LINE__); }
	| FAST_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| MEDIUM_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| EXTENDED_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| CHANGED  { throw exception::unsupported (__FILE__, __LINE__); }
        | FOR_SYM UPGRADE_SYM { throw exception::unsupported (__FILE__, __LINE__); };

optimize:
	OPTIMIZE opt_no_write_to_binlog table_or_tables
	{ throw exception::unsupported (__FILE__, __LINE__); }
	table_list opt_mi_check_type
	{}
	;

opt_no_write_to_binlog:
	/* empty */        { throw exception::unsupported (__FILE__, __LINE__); }
	| NO_WRITE_TO_BINLOG  { throw exception::unsupported (__FILE__, __LINE__); }
	| LOCAL_SYM  { throw exception::unsupported (__FILE__, __LINE__); }
	;

rename:
	RENAME table_or_tables
	{ throw exception::unsupported (__FILE__, __LINE__); }
	table_to_table_list
	{}
	| RENAME USER clear_privileges rename_list
          { throw exception::unsupported (__FILE__, __LINE__); }
	;

rename_list:
        user TO_SYM user
        { throw exception::unsupported (__FILE__, __LINE__); }
        | rename_list ',' user TO_SYM user
          { throw exception::unsupported (__FILE__, __LINE__); }
        ;

table_to_table_list:
	table_to_table
	| table_to_table_list ',' table_to_table;

table_to_table:
	table_ident TO_SYM table_ident
	{ throw exception::unsupported (__FILE__, __LINE__); };

keycache:
        CACHE_SYM INDEX_SYM keycache_list IN_SYM key_cache_name
        { throw exception::unsupported (__FILE__, __LINE__); }
        ;

keycache_list:
        assign_to_keycache
        | keycache_list ',' assign_to_keycache;

assign_to_keycache:
        table_ident cache_keys_spec
        { throw exception::unsupported (__FILE__, __LINE__); }
        ;

key_cache_name:
	ident	   { throw exception::unsupported (__FILE__, __LINE__); }
	| DEFAULT  { throw exception::unsupported (__FILE__, __LINE__); }
	;

preload:
	LOAD INDEX_SYM INTO CACHE_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
	preload_list
	{}
	;

preload_list:
	preload_keys
	| preload_list ',' preload_keys;

preload_keys:
	table_ident cache_keys_spec opt_ignore_leaves
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

cache_keys_spec:
        { throw exception::unsupported (__FILE__, __LINE__); }
        cache_key_list_or_empty
        { throw exception::unsupported (__FILE__, __LINE__); }
        ;

cache_key_list_or_empty:
	/* empty */	{ throw exception::unsupported (__FILE__, __LINE__); }
	| opt_key_or_index '(' key_usage_list2 ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_ignore_leaves:
	/* empty */
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| IGNORE_SYM LEAVES { throw exception::unsupported (__FILE__, __LINE__); }
	;

/*
  Select : retrieve data from table
*/

select:
	select_init
	;

/* Need select_init2 for subselects. */
select_init:
	SELECT_SYM select_init2
        {
	  $$ = $2;
	}
	|
	'(' select_paren ')' union_opt
        {
	  throw exception::unsupported (__FILE__, __LINE__);
	}
        ;

select_paren:
	SELECT_SYM select_part2
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| '(' select_paren ')'
	  { $$ = $2; }
	;

select_init2:
	select_part2
	union_clause
	;

select_part2:
	select_options select_item_list
	select_into select_lock_type
	{ 
	  $$ = $3;
	  $$ -> set_option_clause ($1);
	  $$ -> set_item_list( $2 ); 
	  
	  if( $4 )
	    throw exception::unsupported (__FILE__, __LINE__); 

	}
        ;

select_into:
	opt_order_clause opt_limit_clause 
        {
	  $$ = new select();
	  $$ -> set_order_clause ($1);
	  $$ -> set_limit_clause ($2);
	}
        | into 
        {
	  $$ = new select();
	  $$ -> set_into_clause ($1);
	}
	| select_from
	| into select_from
        {
	  $$ -> set_into_clause ($1);
	}
	| select_from into
        {
	  $$ -> set_into_clause ($2);
	}
;
select_from:
	  FROM join_table_list where_clause group_clause having_clause
	       opt_order_clause opt_limit_clause procedure_clause
          {
	    $$ = new select ();
	    $$ -> set_table_list ($2); 
	    $$ -> set_where_clause ($3);
	    $$ -> set_group_clause ($4);
	    $$ -> set_having_clause ($5);
	    $$ -> set_order_clause ($6);
	    $$ -> set_limit_clause ($7);
	    $$ -> set_procedure_clause ($8);
	  }
        | FROM DUAL_SYM where_clause opt_limit_clause
          {
	    $$ = new select ();
	    chain * l = new chain (new text ("dual"));
	    l -> set_separator (",");
	    $$ -> set_table_list (l); 
	    $$ -> set_where_clause ($3);
	    $$ -> set_limit_clause ($4);
	  }
	;

select_options:
	/* empty*/
	{
	  $$ = 0;
	}
	| select_option_list
	;

select_option_list:
	select_option_list select_option
	{
	  $$ -> push ($2);
	}
	| select_option
	{
	  $$ = new chain ($1);
	}
	;

select_option:
	STRAIGHT_JOIN { throw exception::unsupported (__FILE__, __LINE__); }
	| HIGH_PRIORITY
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DISTINCT         
	{
	  $$ = new text ("distinct");
	}
	| SQL_SMALL_RESULT { throw exception::unsupported (__FILE__, __LINE__); }
	| SQL_BIG_RESULT { throw exception::unsupported (__FILE__, __LINE__); }
	| SQL_BUFFER_RESULT
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SQL_CALC_FOUND_ROWS
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SQL_NO_CACHE_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
	| SQL_CACHE_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ALL		    { throw exception::unsupported (__FILE__, __LINE__); }
	;

select_lock_type:
	/* empty */
	{
	  $$ = 0;
	}
	| FOR_SYM UPDATE_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LOCK_SYM IN_SYM SHARE_SYM MODE_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

select_item_list:
	  select_item_list ',' select_item
	  { 
	    $$ -> push ($3);
	  }
	  | select_item
	  { 
	    $$ = new chain ($1);
	    $$ -> set_separator (",");
	  }
	| '*'
	  { 
	    $$ = new chain (new printable_alias (new select_item_wild ()));
	    $$ -> set_separator (",");
	  };


select_item:
	  select_item2 select_alias 
	  {
	    $$ = $1;
	    if ($2)
	      {
		$$->set_alias ($2);
	      }
	    else
	      {
		text *txt=0;
		identity *id;
		printable *sub_item;
		
		/*
		  The subitem of the printable_alias should be a cast
		*/
		printable *item = $1->get_item ();
		cast *item_cast = dynamic_cast<cast *> (item);
		
		if (item_cast)
		  {
		    sub_item = item_cast->get_item ();
		    //debug << (string ("MOPED ") + $1->get_item ()->get_node_name ());
		    
		    txt = dynamic_cast<text *> (sub_item);
		    id = dynamic_cast<identity *> (sub_item);
		    
		    if (id)
		      {
			txt = id->get_field ();
		      }
		  }
		
		if (txt && ((txt->get_type () & (IDENTIFIER) | IDENTIFIER_QUOTED) ))
		  {
		    text *imp_alias;
		    imp_alias = new text (txt->unmangled_str (), IDENTIFIER);
		    $$->set_alias (imp_alias, true);
		  }
	      }
	  };


select_item2:
	table_wild	
	| expr		
	{
	  $$ = new printable_alias (new cast ($1, DATA_TYPE_SELECTABLE));
	}
	;

select_alias:
	/* empty */		{ $$ = 0; }
	| AS ident_any		{ $$ = $2; }
	| AS TEXT_STRING_sys	{ $$ = $2; }
	| ident_any            	{ $$ = $1; }
	| TEXT_STRING_sys	
	;

optional_braces:
	/* empty */ {}
	| '(' ')' {};

/* all possible expressions */
expr:	
	  bool_term 
          bool_or_expr
          { 
	    if( $2 )
	      {
		$$ = new paran ($1, $2);
	      }
	    else
	      { 
		$$ = $1;
	      }
	  }
      	;

bool_or_expr:
	/* empty */
        { $$ = 0;}
        | bool_or_expr or bool_term
          {
	    if ($1)
	      $$ = new chain ($1, new text ("\nor"), $3); 
	    else
	      $$ = new chain (new text ("\nor"), $3);
	  }
        ;

bool_term:
	bool_term XOR bool_term 
          { 
	    $$ = new paran ($1, new text ("xor"), $3); 
	  }
	| bool_factor 
          bool_and_expr
          { 
	    if( $2 )
	      {
		$$ = new paran ($1, $2);
	      }
	    else
	      { 
		$$ = $1;
	      }
	  }
	;

bool_and_expr:
        /* empty */ { $$ = 0; }
        | bool_and_expr and bool_factor
          { 
	    $$ = new chain ($1, new text ("\nand"), $3); 
	  }
        ;

bool_factor:
	NOT_SYM bool_factor	
          {
	    $$ = new chain (new text ("not"), $2);
	  }
	| bool_test ;

bool_test:
	bool_pri IS TRUE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| bool_pri IS not TRUE_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| bool_pri IS FALSE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| bool_pri IS not FALSE_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| bool_pri IS UNKNOWN_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| bool_pri IS not UNKNOWN_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| bool_pri ;

bool_pri:
	bool_pri IS NULL_SYM	
          { 
	    $$ = new null_test ($1, true);
	  }
	| bool_pri IS not NULL_SYM
          { 
	    $$ = new null_test ($1, false);
	  }
	| bool_pri EQUAL_SYM predicate
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| bool_pri comp_op predicate %prec EQ
          {
	    $$ = new comparison ($2, $1, $3);
	  }
	| bool_pri comp_op all_or_any '(' subselect ')' %prec EQ
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| predicate ;

predicate:
        bit_expr IN_SYM '(' subselect ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| bit_expr not IN_SYM '(' subselect ')'
          { throw exception::unsupported (__FILE__, __LINE__); }
        | bit_expr IN_SYM '(' ')'
          { 
	    $$ = new text( "(0 < 0)");
	  }
        | bit_expr IN_SYM '(' expr ')'
          { 
	    paran *p = new paran ($4);
	    $$ = new chain ($1, new text ("in"), p);
	  }
	| bit_expr IN_SYM '(' expr ',' expr_list ')'
	  {
	    paran *p = new paran ($4, $6);
	    p->set_separator (",");
	    $$ = new chain ($1, new text ("in"), p);
	  }
        | bit_expr not IN_SYM '(' expr ')'
          { 
	    paran *p = new paran ($5);
	    $$ = new chain ($1, new text ("not in"), p);
	  }
	| bit_expr not IN_SYM '(' expr ',' expr_list ')'
	  {
	    paran *p = new paran ($5, $7);
	    p->set_separator (",");
	    $$ = new chain ($1, new text ("not in"), p);
	  }
	| bit_expr BETWEEN_SYM bit_expr AND_SYM predicate
	  { 
	    $$ = new chain ($1, new text ("between"),
				   $3, new text ("and"),
				   $5);
	  }
	| bit_expr not BETWEEN_SYM bit_expr AND_SYM predicate
          { throw exception::unsupported (__FILE__, __LINE__); }
	| bit_expr SOUNDS_SYM LIKE bit_expr
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| bit_expr LIKE simple_expr opt_escape
          {
	    if ($4)
	      throw exception::unsupported (__FILE__, __LINE__); 

	    $$ = new chain ($1, new text ("like"), $3);
	  }
	| bit_expr not LIKE simple_expr opt_escape
          {
	    if ($5)
	      throw exception::unsupported (__FILE__, __LINE__); 

	    $$ = new chain ($1, new text ("not like"), $4);
	  }
	| bit_expr REGEXP bit_expr	
          { throw exception::unsupported (__FILE__, __LINE__); }
	| bit_expr not REGEXP bit_expr
          { throw exception::unsupported (__FILE__, __LINE__); }
	| bit_expr ;

bit_expr:
	bit_expr '|' bit_term	
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| bit_term ;

bit_term:
	bit_term '&' bit_factor	{ throw exception::unsupported (__FILE__, __LINE__); }
	| bit_factor ;

bit_factor:
	bit_factor SHIFT_LEFT value_expr
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| bit_factor SHIFT_RIGHT value_expr 
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| value_expr ;

value_expr:
	value_expr '+' term	{ 
	  $$ = new chain (new cast ($1, DATA_TYPE_NUMBER), new text("+"), new cast ($3, DATA_TYPE_NUMBER)); 
	}
	| value_expr '-' term	
	{
	  $$ = new chain (new cast ($1, DATA_TYPE_NUMBER), new text("-"), new cast ($3, DATA_TYPE_NUMBER)); 
	}
	| value_expr '+' interval_expr
	{
	  $$ = new chain (new cast ($1, DATA_TYPE_DATE | DATA_TYPE_DATETIME), new text ("+"), $3);
	}
	| value_expr '-' interval_expr
	{
	  $$ = new chain (new cast ($1, DATA_TYPE_DATE | DATA_TYPE_DATETIME), new text ("-"), $3);
	}
	| term ;

term_operator: '*' { $$ = new text( "*" ); }
        | '/' { $$ = new text( "/" ); }
        | '%' { $$ = new text( "%" ); }
        | DIV_SYM { throw exception::unsupported (__FILE__, __LINE__); }
        | MOD_SYM { throw exception::unsupported (__FILE__, __LINE__); }
        ;

term:
	term term_operator factor		
	{
	  $$ = new chain( new cast ($1, DATA_TYPE_NUMBER), $2, new cast ($3, DATA_TYPE_NUMBER)); 
	}
	| factor ;

factor:
        factor '^' simple_expr	{ $$ = new chain( $1, new text("^"), $3); }
	| simple_expr
        ;

or:	OR_SYM | OR2_SYM ;
and:	AND_SYM | AND_AND_SYM;
not:	NOT_SYM | NOT2_SYM;
not2:	'!' | NOT2_SYM;

comp_op:  EQ            { $$ = new text ("="); }
	| GE		{ $$ = new text (">="); }
	| GT_SYM	{ $$ = new text (">"); }
	| LE		{ $$ = new text ("<="); }
	| LT		{ $$ = new text ("<"); }
	| NE		{ $$ = new text ("<>"); }
	;

all_or_any: ALL     { throw exception::unsupported (__FILE__, __LINE__); }
	|   ANY_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	;

interval_expr:
          INTERVAL_SYM expr interval
	{ 
	  $3 -> set_expr ($2);
	  $$ = $3;
	}
        ;


simple_expr:
	  simple_ident
 	| simple_expr COLLATE_SYM ident_or_text %prec NEG
	  { throw exception::unsupported (__FILE__, __LINE__); }
        | literal
	| param_marker
	| variable
	| sum_expr
	| simple_expr OR_OR_SYM simple_expr
	  {
	    chain *param = new chain ($1, $3);
	    param->set_separator (",");
	    $$ = new function (new text ("shield.concat_"), DATA_TYPE_CHAR, param, false);
	  }
	| '+' simple_expr %prec NEG	
          { 
	    $$ = new chain (new text ("+"), $2);
	  }
	| '-' simple_expr %prec NEG
          { 
	    $$ = new chain (new text ("-"), $2);
	  }
	| '~' simple_expr %prec NEG	{ throw exception::unsupported (__FILE__, __LINE__); }
	| not2 simple_expr %prec NEG	{ throw exception::unsupported (__FILE__, __LINE__); }
	| '(' subselect ')'   
          { 
	    $$ = new paran ($2);
	  }
	| '(' expr ')'		
          { 
	    $$ = new paran ($2);
	  }
	| '(' expr ',' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ROW_SYM '(' expr ',' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| EXISTS '(' subselect ')' 
          { throw exception::unsupported (__FILE__, __LINE__); }
	| '{' ident expr '}'	{ throw exception::unsupported (__FILE__, __LINE__); }
        | MATCH ident_list_arg AGAINST '(' bit_expr fulltext_options ')'
          { throw exception::unsupported (__FILE__, __LINE__); }
	| ASCII_SYM '(' expr ')' { throw exception::unsupported (__FILE__, __LINE__); }
	| BINARY simple_expr %prec NEG
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CAST_SYM '(' expr AS cast_type ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CASE_SYM opt_expr when_list opt_else END
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CONVERT_SYM '(' expr ',' cast_type ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CONVERT_SYM '(' expr USING charset_name ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DEFAULT '(' simple_ident ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| VALUES '(' simple_ident_nospvar ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| FUNC_ARG0 '(' ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| FUNC_ARG1 '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| FUNC_ARG2 '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| FUNC_ARG3 '(' expr ',' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ADDDATE_SYM '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ADDDATE_SYM '(' expr ',' INTERVAL_SYM expr interval ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| REPEAT_SYM '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ATAN	'(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ATAN	'(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CHAR_SYM '(' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CHAR_SYM '(' expr_list USING charset_name ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CHARSET '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| COALESCE '(' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| COLLATION_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CONCAT '(' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CONCAT_WS '(' expr ',' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CONVERT_TZ_SYM '(' expr ',' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CURDATE optional_braces
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CURTIME optional_braces
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CURTIME '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CURRENT_USER optional_braces
          { throw exception::unsupported (__FILE__, __LINE__); }
//	| DATE_ADD_INTERVAL '(' expr ',' interval_expr interval ')'
//	  { throw exception::unsupported (__FILE__, __LINE__); }
//	| DATE_SUB_INTERVAL '(' expr ',' interval_expr interval ')'
//	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DATABASE '(' ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DATE_SYM '(' expr ')'
	  { $$ = new function (new text ("shield.date_"), DATA_TYPE_DATE, $3, false); }
	| DAY_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ELT_FUNC '(' expr ',' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MAKE_SET_SYM '(' expr ',' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ENCRYPT '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ENCRYPT '(' expr ',' expr ')'   { throw exception::unsupported (__FILE__, __LINE__); }
	| DECODE_SYM '(' expr ',' TEXT_STRING_literal ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ENCODE_SYM '(' expr ',' TEXT_STRING_literal ')'
	 { throw exception::unsupported (__FILE__, __LINE__); }
	| DES_DECRYPT_SYM '(' expr ')'
        { throw exception::unsupported (__FILE__, __LINE__); }
	| DES_DECRYPT_SYM '(' expr ',' expr ')'
        { throw exception::unsupported (__FILE__, __LINE__); }
	| DES_ENCRYPT_SYM '(' expr ')'
        { throw exception::unsupported (__FILE__, __LINE__); }
	| DES_ENCRYPT_SYM '(' expr ',' expr ')'
        { throw exception::unsupported (__FILE__, __LINE__); }
	| EXPORT_SET '(' expr ',' expr ',' expr ')'
		{ throw exception::unsupported (__FILE__, __LINE__); }
	| EXPORT_SET '(' expr ',' expr ',' expr ',' expr ')'
		{ throw exception::unsupported (__FILE__, __LINE__); }
	| EXPORT_SET '(' expr ',' expr ',' expr ',' expr ',' expr ')'
		{ throw exception::unsupported (__FILE__, __LINE__); }
	| FORMAT_SYM '(' expr ',' NUM ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| FROM_UNIXTIME '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| FROM_UNIXTIME '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| FIELD_FUNC '(' expr ',' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| geometry_function
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| GET_FORMAT '(' date_time_type  ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| HOUR_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| IF '(' expr ',' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| INSERT '(' expr ',' expr ',' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
//	| interval_expr '+' expr
//	  /* we cannot put interval before - */
//	  { throw exception::unsupported (__FILE__, __LINE__); }
//	| interval_expr 
	| LAST_INSERT_ID '(' ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LAST_INSERT_ID '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LEFT '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LOCATE '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LOCATE '(' expr ',' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| GREATEST_SYM '(' expr ',' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LEAST_SYM '(' expr ',' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LOG_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LOG_SYM '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MASTER_POS_WAIT '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MASTER_POS_WAIT '(' expr ',' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MICROSECOND_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MINUTE_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MOD_SYM '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MONTH_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| NOW_SYM optional_braces
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| NOW_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| PASSWORD '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| OLD_PASSWORD '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| POSITION_SYM '(' bit_expr IN_SYM expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| QUARTER_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| RAND '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| RAND '(' ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| REPLACE '(' expr ',' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| RIGHT '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ROUND '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ROUND '(' expr ',' expr ')' { throw exception::unsupported (__FILE__, __LINE__); }
	| ROW_COUNT_SYM '(' ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SUBDATE_SYM '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SUBDATE_SYM '(' expr ',' INTERVAL_SYM expr interval ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SECOND_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SUBSTRING '(' expr ',' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SUBSTRING '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SUBSTRING '(' expr FROM expr FOR_SYM expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SUBSTRING '(' expr FROM expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SUBSTRING_INDEX '(' expr ',' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SYSDATE optional_braces
          { throw exception::unsupported (__FILE__, __LINE__); }
	| SYSDATE '(' expr ')'
          { throw exception::unsupported (__FILE__, __LINE__); }
//	| TIME_SYM '(' expr ')'
//	  { throw exception::unsupported (__FILE__, __LINE__); }
//	| TIMESTAMP '(' expr ')'
//	  { throw exception::unsupported (__FILE__, __LINE__); }
//	| TIMESTAMP '(' expr ',' expr ')'
//	  { throw exception::unsupported (__FILE__, __LINE__); }
	| TIMESTAMP_ADD '(' interval_time_st ',' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| TIMESTAMP_DIFF '(' interval_time_st ',' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| TRIM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| TRIM '(' LEADING expr FROM expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| TRIM '(' TRAILING expr FROM expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| TRIM '(' BOTH expr FROM expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| TRIM '(' LEADING FROM expr ')'
	 { throw exception::unsupported (__FILE__, __LINE__); }
	| TRIM '(' TRAILING FROM expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| TRIM '(' BOTH FROM expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| TRIM '(' expr FROM expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| TRUNCATE_SYM '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
 	| ident '.' ident_any '(' opt_expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| IDENT_sys '(' udf_expr_list ')'
          { 
	    string func_name = to_lower( $1->str ());
	    
	    if (func_name == "concat")
	      {
		/*
		  Need to special case this one as oracle concat can
		  only take two arguments, mysql can take any number
		  of arguments
		*/
		
		if (!$3 || !$3->size ())
		  {
		    $$ = new text ("chr (1)");
		  }
		else
		  {
		    vector<printable *>::const_iterator it;
		    it=$3 -> begin (); 
		    printable *res = *it;
		    it++;
		    while (it != $3 -> end ())
		      {
			chain *param = new chain (res, *it);
			param->set_separator (",");
			res = new function (new text ("shield.concat_"), DATA_TYPE_CHAR, param, false);
			++it;
		      }
		    $$ = res;
		  }
	      }
	    else if (func_name == "concat_ws")
	      {
		/*
		  concat_ws is like concat, only the first argument is used between every other argument
		*/

		if (!$3 || ($3->size () < 2))
		  {
		    $$ = new text ("chr (1)");
		  }
		else
		  {
		    vector<printable *>::const_iterator it;
		    it=$3 -> begin (); 

		    printable *sep = *it;
		    
		    ++it;
		    printable *res = *it;
		    ++it;
		    
		    while (it < $3 -> end ())
		      {
			chain *param1;
			chain *param2;
			printable *midpoint;

			param1= new chain (res, sep);
			param1->set_separator (",");

			midpoint = new function (new text ("shield.concat_"), DATA_TYPE_CHAR, param1, false);

			param2= new chain (midpoint, *it);
			param2->set_separator (",");

			res = new function (new text ("shield.concat_"), DATA_TYPE_CHAR, param2, false);
			
			++it;
		      }
		    $$ = new paran (res);
		    $$ -> set_context (DATA_TYPE_CHAR);
		  }
	      }
	    else if (contains (func_name.c_str (), "date_add", "date_sub"))
	      {
		string op = (func_name == "date_add")?"+":"-";
		printable *date = (*$3)[0];
		printable *interval = (*$3)[1];
		printable *p = new paran (date,
					  new text (op),
					  interval );
		$$ = new chain (new text ("shield.date_char"), p);
	      }
	    else
	      {

		static map<string,triplet<string,data_type,bool> > func_translate;

		if (func_translate.size () == 0)
		  {
		    using namespace util;
		    /*
		      DATA_TYPE_UNDEFINED return type means same
		      return type as input type of first argument.
		      
		      If min or max is called on a function with no
		      parameters, an exception will be thrown whenever
		      trying to access its type.
		    */
		    func_translate["char_length"] = make_triplet ("shield.char_length_", DATA_TYPE_NUMBER,false);
		    func_translate["length"] = make_triplet ("shield.length_", DATA_TYPE_NUMBER,false);
		    func_translate["date_format"] = make_triplet ("shield.date_format", DATA_TYPE_CHAR,false);
		    func_translate["count"] = make_triplet ("count", DATA_TYPE_NUMBER,true);
		    func_translate["min"] = make_triplet ("min", DATA_TYPE_UNDEFINED,true);
		    func_translate["max"] = make_triplet ("max", DATA_TYPE_UNDEFINED,true);
		    func_translate["lower"] = make_triplet ("lower", DATA_TYPE_CHAR,false);
		    func_translate["upper"] = make_triplet ("upper", DATA_TYPE_CHAR,false);
		    func_translate["lpad"] = make_triplet ("shield.lpad_", DATA_TYPE_CHAR,false);
		    func_translate["now"] = make_triplet ("current_date", DATA_TYPE_DATETIME,false);
		    func_translate["unix_timestamp"] = make_triplet ("shield.unix_timestamp", DATA_TYPE_NUMBER,false);

		    /*
		      Functions implemented by the shield package
		    */
		    func_translate["date"] = make_triplet ("shield.date_", DATA_TYPE_DATE,false);
		    func_translate["curdate"] = make_triplet ("shield.curdate", DATA_TYPE_DATE,false);
		    func_translate["current_date"] = make_triplet ("shield.curdate", DATA_TYPE_DATE,false);
		  }

		map<string,triplet<string,data_type,bool> >::iterator i = func_translate.find (func_name);

		if (i != func_translate.end ())
		  {
		    $$ = new function (new text (i->second.first), i->second.second, $3, i->second.third); 
		  }
		else
		  {
		    throw exception::syntax (string("Unknown function '") + func_name + "'");
		  }
	      }
	  }
	| UNIQUE_USERS '(' text_literal ',' NUM ',' NUM ',' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| UNIX_TIMESTAMP '(' ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| UNIX_TIMESTAMP '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| USER '(' ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| UTC_DATE_SYM optional_braces
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| UTC_TIME_SYM optional_braces
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| UTC_TIMESTAMP_SYM optional_braces
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| WEEK_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| WEEK_SYM '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| YEAR_SYM '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| YEARWEEK '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| YEARWEEK '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| BENCHMARK_SYM '(' ulong_num ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| EXTRACT_SYM '(' interval FROM expr ')'
	{ throw exception::unsupported (__FILE__, __LINE__); };

geometry_function:
	  CONTAINS_SYM '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| GEOMFROMTEXT '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| GEOMFROMTEXT '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| GEOMFROMWKB '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| GEOMFROMWKB '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| GEOMETRYCOLLECTION '(' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LINESTRING '(' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
 	| MULTILINESTRING '(' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
 	| MLINEFROMTEXT '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MLINEFROMTEXT '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MPOINTFROMTEXT '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MPOINTFROMTEXT '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MPOLYFROMTEXT '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MPOLYFROMTEXT '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MULTIPOINT '(' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
 	| MULTIPOLYGON '(' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| POINT_SYM '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
 	| POINTFROMTEXT '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| POINTFROMTEXT '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| POLYFROMTEXT '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| POLYFROMTEXT '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| POLYGON '(' expr_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
 	| GEOMCOLLFROMTEXT '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| GEOMCOLLFROMTEXT '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
 	| LINEFROMTEXT '(' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| LINEFROMTEXT '(' expr ',' expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

fulltext_options:
        /* nothing */                   { throw exception::unsupported (__FILE__, __LINE__); }
        | WITH QUERY_SYM EXPANSION_SYM  { throw exception::unsupported (__FILE__, __LINE__); }
        | IN_SYM BOOLEAN_SYM MODE_SYM   { throw exception::unsupported (__FILE__, __LINE__); }
        ;

udf_expr_list:
	/* empty */	 { $$ = 0; }
	| udf_expr_list2 
	|
	DISTINCT udf_expr_list2
	{
	  $$ = new chain (new text ("distinct"), $2);;
	}
	| '*'
	{
	  $$ = new chain (new text ("*"));
	}
	;

udf_expr_list2:
	udf_expr_list3
	;

udf_expr_list3:
	udf_expr 
	{
	  $$ = new chain ($1);
	  $$ -> set_separator (",");
	}
	| udf_expr_list3 ',' udf_expr 
	  {
	    $$ = $1;
	    $$ -> push ($3);
	  }
	;

udf_expr:
	expr select_alias
          { 
	    if ($2)
	      $$ = new chain ($1, $2);
	    else
	      $$ = $1;
	  }
	| interval_expr select_alias
          {
	    if ($2)
	      $$ = new chain ($1, $2);
	    else
	      $$ = $1;
	  }
	;

sum_expr:
	AVG_SYM '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| AVG_SYM '(' DISTINCT in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| BIT_AND  '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| BIT_OR  '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| BIT_XOR  '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| COUNT_SYM '(' opt_all '*' ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| COUNT_SYM '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| COUNT_SYM '(' DISTINCT
	  { throw exception::unsupported (__FILE__, __LINE__); }
	   expr_list
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| GROUP_UNIQUE_USERS '(' text_literal ',' NUM ',' NUM ',' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MIN_SYM '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
/*
   According to ANSI SQL, DISTINCT is allowed and has
   no sence inside MIN and MAX grouping functions; so MIN|MAX(DISTINCT ...)
   is processed like an ordinary MIN | MAX()
 */
	| MIN_SYM '(' DISTINCT in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MAX_SYM '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| MAX_SYM '(' DISTINCT in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| STD_SYM '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| VARIANCE_SYM '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| STDDEV_SAMP_SYM '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| VAR_SAMP_SYM '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SUM_SYM '(' in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| SUM_SYM '(' DISTINCT in_sum_expr ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| GROUP_CONCAT_SYM '(' opt_distinct
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  expr_list opt_gorder_clause
	  opt_gconcat_separator
	 ')'
	  { throw exception::unsupported (__FILE__, __LINE__); };

variable:
          '@'
          { throw exception::unsupported (__FILE__, __LINE__); }
          variable_aux
          { throw exception::unsupported (__FILE__, __LINE__); }
          ;

variable_aux:
          ident_or_text SET_VAR expr
          { throw exception::unsupported (__FILE__, __LINE__); }
        | ident_or_text
          { throw exception::unsupported (__FILE__, __LINE__); }
        | '@' opt_var_ident_type ident_or_text opt_component
          { throw exception::unsupported (__FILE__, __LINE__); }
        ;

opt_distinct:
    /* empty */ { throw exception::unsupported (__FILE__, __LINE__); }
    |DISTINCT   { throw exception::unsupported (__FILE__, __LINE__); };

opt_gconcat_separator:
    /* empty */        { throw exception::unsupported (__FILE__, __LINE__); }
    |SEPARATOR_SYM text_string  { throw exception::unsupported (__FILE__, __LINE__); };


opt_gorder_clause:
	  /* empty */
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| order_clause
          { throw exception::unsupported (__FILE__, __LINE__); };


in_sum_expr:
	opt_all
	{ throw exception::unsupported (__FILE__, __LINE__); }
	expr
	{ throw exception::unsupported (__FILE__, __LINE__); };

cast_type:
        BINARY opt_len		{ throw exception::unsupported (__FILE__, __LINE__); }
        | CHAR_SYM opt_len opt_binary	{ throw exception::unsupported (__FILE__, __LINE__); }
	| NCHAR_SYM opt_len	{ throw exception::unsupported (__FILE__, __LINE__); }
        | SIGNED_SYM		{ throw exception::unsupported (__FILE__, __LINE__); }
        | SIGNED_SYM INT_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
        | UNSIGNED		{ throw exception::unsupported (__FILE__, __LINE__); }
        | UNSIGNED INT_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
        | DATE_SYM		{ throw exception::unsupported (__FILE__, __LINE__); }
        | TIME_SYM		{ throw exception::unsupported (__FILE__, __LINE__); }
        | DATETIME		{ throw exception::unsupported (__FILE__, __LINE__); }
        | DECIMAL_SYM float_options { throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_expr_list:
	/* empty */ { throw exception::unsupported (__FILE__, __LINE__); }
	| expr_list { throw exception::unsupported (__FILE__, __LINE__); }
	;

expr_list:
	expr_list2
	;

expr_list2:
	expr 
	{
	  $$ = new chain ($1);
	  $$ -> set_separator (",");
	} 
	| expr_list2 ',' expr 
	{
	  $$ -> push ($3);
	}

ident_list_arg:
          ident_list          { throw exception::unsupported (__FILE__, __LINE__); }
        | '(' ident_list ')'  { throw exception::unsupported (__FILE__, __LINE__); };

ident_list:
        { throw exception::unsupported (__FILE__, __LINE__); }
        ident_list2
        { throw exception::unsupported (__FILE__, __LINE__); };

ident_list2:
        simple_ident { throw exception::unsupported (__FILE__, __LINE__); }
        | ident_list2 ',' simple_ident { throw exception::unsupported (__FILE__, __LINE__); };

opt_expr:
	/* empty */      { throw exception::unsupported (__FILE__, __LINE__); }
	| expr           { throw exception::unsupported (__FILE__, __LINE__); };

opt_else:
	/* empty */    { throw exception::unsupported (__FILE__, __LINE__); }
	| ELSE expr    { throw exception::unsupported (__FILE__, __LINE__); };

when_list:
          WHEN_SYM expr THEN_SYM expr
          { throw exception::unsupported (__FILE__, __LINE__); }
        | when_list WHEN_SYM expr THEN_SYM expr
          { throw exception::unsupported (__FILE__, __LINE__); }
        ;

/* Warning - may return NULL in case of incomplete SELECT */
table_ref:
        table_factor            
        | join_table
        ;

join_table_list:
	derived_table_list
	;

/* Warning - may return NULL in case of incomplete SELECT */
derived_table_list:
        table_ref
        {
	  $$ = new chain ($1);
	  $$ -> set_separator (",");
	}
        | derived_table_list ',' table_ref
        { 
	  $$ -> push ($3);
	}
        ;

/*
  Notice that JOIN is a left-associative operation, and it must be parsed
  as such, that is, the parser must process first the left join operand
  then the right one. Such order of processing ensures that the parser
  produces correct join trees which is essential for semantic analysis
  and subsequent optimization phases.
*/
join_table:
/* INNER JOIN variants */
        /*
          Use %prec to evaluate production 'table_ref' before 'normal_join'
          so that [INNER | CROSS] JOIN is properly nested as other
          left-associative joins.
        */
        table_ref %prec TABLE_REF_PRIORITY normal_join table_ref
          { throw exception::unsupported (__FILE__, __LINE__); }
	| table_ref STRAIGHT_JOIN table_factor
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| table_ref normal_join table_ref
          ON
          expr
	  {
	    $$ = new chain ($1, $2, $3, new text ("on"), $5);
	  }
        | table_ref STRAIGHT_JOIN table_factor
          ON
          { throw exception::unsupported (__FILE__, __LINE__); }
          expr
          { throw exception::unsupported (__FILE__, __LINE__); }
	| table_ref normal_join table_ref
	  USING
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  '(' using_list ')'
          { throw exception::unsupported (__FILE__, __LINE__); }
	| table_ref NATURAL JOIN_SYM table_factor
	  { throw exception::unsupported (__FILE__, __LINE__); }

/* LEFT JOIN variants */
	| table_ref LEFT opt_outer JOIN_SYM table_ref
          ON
          expr
	  { 
	    $$ = new chain ($1, 
			     new text ("\nleft", EXACT, false),
			     new text ("join"),
			     $3,
			     $5,
			     new text ("on"),
			     $7);
	  }
	| table_ref LEFT opt_outer JOIN_SYM table_factor
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  USING '(' using_list ')'
          { throw exception::unsupported (__FILE__, __LINE__); }
	| table_ref NATURAL LEFT opt_outer JOIN_SYM table_factor
	  { throw exception::unsupported (__FILE__, __LINE__); }

/* RIGHT JOIN variants */
	| table_ref RIGHT opt_outer JOIN_SYM table_ref
          ON
          { throw exception::unsupported (__FILE__, __LINE__); }
          expr
          { throw exception::unsupported (__FILE__, __LINE__); }
	| table_ref RIGHT opt_outer JOIN_SYM table_factor
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  USING '(' using_list ')'
          { throw exception::unsupported (__FILE__, __LINE__); }
	| table_ref NATURAL RIGHT opt_outer JOIN_SYM table_factor
	  { throw exception::unsupported (__FILE__, __LINE__); };

normal_join:
	JOIN_SYM		{ $$ = new text ("join");}
	| INNER_SYM JOIN_SYM	{ $$ = new chain (new text ("inner"), new text ("join"));}
	| CROSS JOIN_SYM	{throw exception::unsupported (__FILE__, __LINE__);}
	;

table_factor:
        table_ident opt_table_alias opt_key_definition
	{
	  if ($3)
	    throw exception::unsupported (__FILE__, __LINE__);

	  $$ = new printable_alias ($1, $2);

	}
	| '{' ident table_ref LEFT OUTER JOIN_SYM table_ref
          ON
          { throw exception::unsupported (__FILE__, __LINE__); }
          expr '}'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| select_derived_init get_select_lex select_derived2
          {
	    if (!$3)
	      throw exception::unsupported (__FILE__, __LINE__);
	    $$ = new printable_alias ($3);
	  }	  
	| '(' get_select_lex select_derived union_opt ')' opt_table_alias
	{ 
	  if (!$3)
	    throw exception::unsupported (__FILE__, __LINE__);
	  $$ = new printable_alias (new paran ($2, $3, $4), $6);
	}
        ;

/* handle contents of parentheses in join expression */
select_derived:
	  get_select_lex
          derived_table_list
	    {
	      $$ = $2; 
	    }
 	;

select_derived2:
        select_options select_item_list
	opt_select_from
	{ 
	  select *sel = $3;
	  if (!sel)
	    sel = new select ();

	  sel->set_option_clause ($1);
	  sel->set_item_list ($2);
	  $$ = sel;
	}
        ;

get_select_lex:
	/* Empty */ { $$ = 0; }
        ;

select_derived_init:
          SELECT_SYM
        ;

opt_outer:
	/* empty */	
	{ 
	  $$ = 0; 
	}
	| OUTER
	{
	  $$ = new text ("outer"); 
	}
	;

opt_key_definition:
	/* empty */	{ $$ = 0; }
	| USE_SYM    key_usage_list
          { throw exception::unsupported (__FILE__, __LINE__); }
	| FORCE_SYM key_usage_list
          { throw exception::unsupported (__FILE__, __LINE__); }
	| IGNORE_SYM key_usage_list
	  { throw exception::unsupported (__FILE__, __LINE__); };

key_usage_list:
	key_or_index { throw exception::unsupported (__FILE__, __LINE__); }
        '(' key_list_or_empty ')'
        { throw exception::unsupported (__FILE__, __LINE__); }
	;

key_list_or_empty:
	/* empty */ 		{}
	| key_usage_list2	{}
	;

key_usage_list2:
	key_usage_list2 ',' ident
        { throw exception::unsupported (__FILE__, __LINE__); }
	| ident
        { throw exception::unsupported (__FILE__, __LINE__); }
	| PRIMARY_SYM
        { throw exception::unsupported (__FILE__, __LINE__); };

using_list:
	ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| using_list ',' ident
	  { throw exception::unsupported (__FILE__, __LINE__); };

interval:
	interval_time_st	
	| DAY_HOUR_SYM		{ throw exception::unsupported (__FILE__, __LINE__); }
	| DAY_MICROSECOND_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| DAY_MINUTE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| DAY_SECOND_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| HOUR_MICROSECOND_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| HOUR_MINUTE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| HOUR_SECOND_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| MICROSECOND_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| MINUTE_MICROSECOND_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| MINUTE_SECOND_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| SECOND_MICROSECOND_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| YEAR_MONTH_SYM	{ throw exception::unsupported (__FILE__, __LINE__); };

interval_time_st:
	DAY_SYM			
	{ 
	  $$ = new interval (INTERVAL_DAY);
	}
	| WEEK_SYM		{ throw exception::unsupported (__FILE__, __LINE__); }
	| HOUR_SYM		
	{ 
	  $$ = new interval (INTERVAL_HOUR);
	}
	| FRAC_SECOND_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| MINUTE_SYM	
	{ 
	  $$ = new interval (INTERVAL_MINUTE);
	}
	| MONTH_SYM	
	{ 
	  $$ = new interval (INTERVAL_MONTH);
	}
	| QUARTER_SYM		{ throw exception::unsupported (__FILE__, __LINE__); }
	| SECOND_SYM
	{ 
	  $$ = new interval (INTERVAL_SECOND);
	}
	| YEAR_SYM	
	{ 
	  $$ = new interval (INTERVAL_YEAR);
	}
        ;

date_time_type:
          DATE_SYM              { throw exception::unsupported (__FILE__, __LINE__); }
        | TIME_SYM              { throw exception::unsupported (__FILE__, __LINE__); }
        | DATETIME              { throw exception::unsupported (__FILE__, __LINE__); }
        | TIMESTAMP             { throw exception::unsupported (__FILE__, __LINE__); }
        ;

table_alias:
	/* empty */
	| AS
	| EQ;

opt_table_alias:
	/* empty */		{ $$ = 0; }
	| table_alias ident_any
	  { $$ = $2; };

opt_all:
	/* empty */
	| ALL
	;

where_clause:
	/* empty */  { $$ = 0; }
	| WHERE
          expr
        {
	  $$ = $2;
	}
 	;

having_clause:
        /* empty */ { $$ = 0; }; 
	| HAVING
	  expr
        { $$ = $2; }
	;

opt_escape:
	ESCAPE_SYM simple_expr 
          { throw exception::unsupported (__FILE__, __LINE__); }
	| /* empty */
          { $$ = 0; }
        ;


/*
   group by statement in select
*/

group_clause:
	  /* empty */ { $$ = 0; }
	| GROUP BY group_list olap_opt 
	{
	  $$ = $3;
	}

group_list:
	group_list ',' order_ident order_dir
	  { 
	    $$ = $1;
	    $$ -> push (new chain ($3, $4)); 
	  }
	| order_ident order_dir
	  { 
	    $$ = new chain (new chain ($1, $2)); 
	    $$->set_separator (",");
	  }

olap_opt:
	/* empty */ { $$ = 0; }
	| WITH CUBE_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
	| WITH ROLLUP_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
	;

/*
  Order by statement in ALTER TABLE
*/

alter_order_clause:
	ORDER_SYM BY alter_order_list
        ;

alter_order_list:
          alter_order_list ',' alter_order_item
        | alter_order_item
        ;

alter_order_item:
          simple_ident_nospvar order_dir
          { throw exception::unsupported (__FILE__, __LINE__); }
        ;

/*
   Order by statement in select
*/

opt_order_clause:
	  /* empty */ { $$ = 0; }
	| order_clause;

order_clause:
	ORDER_SYM BY order_list 
	{
	  $$ = $3; 
	}
	;

order_list:
	order_list ',' order_ident order_dir
	{
	  $$ = $1;
	  $$ -> push (new chain( new cast ($3,DATA_TYPE_SORTABLE), $4)); 
	}
	| order_ident order_dir
	{
	  $$ = new chain (new chain( new cast ($1,DATA_TYPE_SORTABLE), $2 )); 
	  $$ -> set_separator (",");
	}
        ;

order_dir:
	/* empty */ { $$ = 0; }
	| ASC  { $$ = new text ("asc"); }
	| DESC { $$ = new text ("desc"); };


opt_limit_clause_init:
	/* empty */
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| limit_clause 
	;

opt_limit_clause:
	/* empty */	{ $$ = 0;}
	| limit_clause	
	;

limit_clause:
	LIMIT limit_options { $$ = $2; }
	;

limit_options:
	limit_option
	  { 
	    $$ = new limit ( 0, $1);
	  }
	| limit_option ',' limit_option
	  { 
	    $$ = new limit ( $1, $3);
	  } 
	| limit_option OFFSET_SYM limit_option
	  { 
	    $$ = new limit ( $1, $3);
	  } 
	;

limit_option:
        param_marker  { throw exception::unsupported (__FILE__, __LINE__); }
        | ULONGLONG_NUM 
	{
	  $$ = strtoull(yytext, 0, 10); 
	}
        | LONG_NUM
	{
	  $$ = strtoull(yytext, 0, 10); 
	}
        | NUM
	{
	  $$ = strtoull(yytext, 0, 10); 
	}
        ;

num:
        NUM           
	{
	  $$ = strtoull(yytext, 0, 10); 
	}
	;


delete_limit_clause:
	/* empty */
	{$$=0;}
	| LIMIT limit_option
	{ throw exception::unsupported (__FILE__, __LINE__); };

ulong_num:
          NUM           { throw exception::unsupported (__FILE__, __LINE__); }
	| HEX_NUM       { throw exception::unsupported (__FILE__, __LINE__); }
	| LONG_NUM      { throw exception::unsupported (__FILE__, __LINE__); }
	| ULONGLONG_NUM { throw exception::unsupported (__FILE__, __LINE__); }
        | DECIMAL_NUM   { throw exception::unsupported (__FILE__, __LINE__); }
	| FLOAT_NUM	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

ulonglong_num:
	NUM	    
	{
	  $$ = strtoull(yytext, 0, 10); 
	}
	| ULONGLONG_NUM
	{
	  $$ = strtoull(yytext, 0, 10); 
	}
	| LONG_NUM 
	{
	  $$ = strtoull(yytext, 0, 10); 
	}
        | DECIMAL_NUM  { throw exception::unsupported (__FILE__, __LINE__); }
	| FLOAT_NUM { throw exception::unsupported (__FILE__, __LINE__); }
	;

procedure_clause:
	/* empty */ { $$ = 0; }
	| PROCEDURE ident			/* Procedure name */
	  '(' procedure_list ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  ;

procedure_list:
	/* empty */ {}
	| procedure_list2 {};

procedure_list2:
	procedure_list2 ',' procedure_item
	| procedure_item;

procedure_item:
	  expr
	  { throw exception::unsupported (__FILE__, __LINE__); }
          ;


select_var_list_init:
	   { throw exception::unsupported (__FILE__, __LINE__); }
	   select_var_list
	   {}
           ;

select_var_list:
	   select_var_list ',' select_var_ident
	   | select_var_ident {}
           ;

select_var_ident:  
	   '@' ident_or_text
           { throw exception::unsupported (__FILE__, __LINE__); }
           | ident_or_text
           { throw exception::unsupported (__FILE__, __LINE__); }
           ;

into:
        INTO
        into_destination
	{ throw exception::unsupported (__FILE__, __LINE__); }
        ;

into_destination:
        OUTFILE TEXT_STRING_filesystem
	{ throw exception::unsupported (__FILE__, __LINE__); }
	opt_field_term opt_line_term
	| DUMPFILE TEXT_STRING_filesystem
	{ throw exception::unsupported (__FILE__, __LINE__); }
        | select_var_list_init
	{ throw exception::unsupported (__FILE__, __LINE__); }
        ;

/*
  DO statement
*/

do:	DO_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
	expr_list
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

/*
  Drop : delete tables or index or user
*/

drop:
	DROP opt_temporary table_or_tables if_exists table_list opt_restrict
	{
	  chain *res = new chain ();
	  chain::const_iterator it;
	  for (it=$5 -> begin (); it != $5 -> end (); ++it )
	    {
	      res->push (new drop_table (dynamic_cast<identity *> (*it), $4));
	    }
	  $$ = new fake_query (res);
	}
	| DROP INDEX_SYM ident ON table_ident {}
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DROP DATABASE if_exists ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DROP FUNCTION_SYM if_exists sp_name
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DROP PROCEDURE if_exists sp_name
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DROP USER clear_privileges user_list
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| DROP VIEW_SYM if_exists table_list opt_restrict
	  { throw exception::unsupported (__FILE__, __LINE__); }
        | DROP TRIGGER_SYM if_exists sp_name
          { throw exception::unsupported (__FILE__, __LINE__); }
	;

table_list:
	table_name 
	{
	  $$ = new chain ($1);
	  $$ -> set_separator (",");

	}
	| table_list ',' table_name
	{
	  $$ -> push ($3);
	}
	;

table_name:
	table_ident
	;

if_exists:
	/* empty */ { $$=false; }
	| IF EXISTS { $$ = true; }
	;

opt_temporary:
	/* empty */ { $$ = 0; }
	| TEMPORARY { $$ = new text ("temporary");}
	;
/*
** Insert : add new data to table
*/

insert:
	INSERT insert_lock_option
	opt_ignore insert2
	insert_field_spec opt_insert_update
	  {
	    $$ = $5;
	    $$ -> set_ignore ($3?true:false);
	    $$ -> set_insert_update ($6);
	    $$ -> set_name ($4);
	  }
	;

replace:
	REPLACE
	{ throw exception::unsupported (__FILE__, __LINE__); }
	replace_lock_option insert2
	{ throw exception::unsupported (__FILE__, __LINE__); }
	insert_field_spec
	{}
	;

insert_lock_option:
	/* empty */
          { $$ = 0; }
	| LOW_PRIORITY	{ throw exception::unsupported (__FILE__, __LINE__); }
	| DELAYED_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| HIGH_PRIORITY { throw exception::unsupported (__FILE__, __LINE__); }
	;

replace_lock_option:
	opt_low_priority { throw exception::unsupported (__FILE__, __LINE__); }
	| DELAYED_SYM	 { throw exception::unsupported (__FILE__, __LINE__); };

insert2:
	INTO insert_table { $$ = $2; }
	| insert_table;

insert_table:
	table_name
	;

insert_field_spec:
	insert_values 
	{
	  $$ = new insert ();
	  $$ -> set_value_list ($1);	  
	}
	| '(' ')' insert_values 
	{
	  $$ = new insert ();
	  $$ -> set_value_list ($3);	  
	}
	| '(' fields ')' insert_values 
	{
	  $$ = new insert ();
	  $$ -> set_field_list ($2);
	  $$ -> set_value_list ($4);
	}
	| SET
	ident_eq_list
	{
	  $$ = new insert ();
	  $$ -> set_eq_list ($2);	  
	}
	;

fields:
	fields ',' insert_ident 
	{ 
	  $$->push ($3);
	}
	| insert_ident
	{ 
	  $$ = new chain ($1); 
	  $$->set_separator (",");
	}
	;

insert_values:
	VALUES	values_list  
	{ 
	  $$ = $2;
	}
	| VALUE_SYM values_list  
	{ 
	  $$ = $2;
	}
	|     create_select     { throw exception::unsupported (__FILE__, __LINE__); } union_clause {}
	| '(' create_select ')' { throw exception::unsupported (__FILE__, __LINE__); } union_opt {}
        ;

values_list:
	values_list ','  no_braces
	{
	  $$ -> push ($3);
	}
	| no_braces 
	{
	  $$ = new chain ($1);
	}
	;

ident_eq_list:
	ident_eq_list ',' ident_eq_value
	{
	  $$ -> push ($3);
	}
	|
	ident_eq_value 
	{
	  $$ = new chain ($1);
	}
	;

ident_eq_value:
	simple_ident_nospvar equal expr_or_default
	 { $$ = new chain ($1, $3); }

equal:	EQ		{ $$ = new text ("="); }
	| SET_VAR	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_equal:
	/* empty */	{}
	| equal		{}
	;

no_braces:
	 '('
	 opt_values ')'
	 { 
	   $$ = $2;
	 }
	;

opt_values:
	/* empty */ {}
	| values;

values:
	values ','  expr_or_default
	{ 
	  $$ -> push ($3);
	}
	| expr_or_default
	{ 
	  $$ = new chain ($1);
	  $$ -> set_separator (",");
	  $$ -> set_line_break (5);
	}
	;

expr_or_default:
	expr	  
	| DEFAULT { throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_insert_update:
        /* empty */ 
	{
	  $$ = 0;
	}
        | ON DUPLICATE_SYM	
          KEY_SYM UPDATE_SYM insert_update_list
	{ throw exception::unsupported (__FILE__, __LINE__); }
        ;

/* Update rows in a table */

update:
	UPDATE_SYM
        opt_low_priority opt_ignore join_table_list
	SET update_list
	where_clause opt_order_clause delete_limit_clause 
	{
	  update *upd = new update ();

	  if ($2)
	    throw exception::unsupported (__FILE__, __LINE__);

	  if ($3)
	    throw exception::unsupported (__FILE__, __LINE__);
	  
	  upd->set_table_list ($4);
	  upd->set_update_list ($6);
	  upd->set_where_clause ($7);
	  upd->set_order_clause ($8);
	  upd->set_delete_limit_clause ($9);
	  
	  $$ = upd;

	}
	;

update_list:
	update_list ',' update_elem
	{
	  $$->push ($3);
	}
	| update_elem
	{
	  $$ = new chain ($1);
	  $$->set_separator (",");
	}
	;

update_elem:
	simple_ident_nospvar equal expr_or_default
	{ 
	  $$ = new chain ($1, $2, $3);
	}
	;

insert_update_list:
	insert_update_list ',' insert_update_elem
	| insert_update_elem;

insert_update_elem:
	simple_ident_nospvar equal expr_or_default
	  { throw exception::unsupported (__FILE__, __LINE__); };

opt_low_priority:
	/* empty */	{ $$=0; }
	| LOW_PRIORITY	{ throw exception::unsupported (__FILE__, __LINE__); };

/* Delete rows from a table */

delete:
	DELETE_SYM
	opt_delete_options single_multi 
	{ 
	  text *del = new text ("delete", EXACT, true);
	  text *end = new text ("\n\n", EXACT, true);
	  $$ = new fake_query (new chain(del, $2, $3, end));
	}
	;

single_multi:
 	FROM table_ident
	where_clause opt_order_clause
	delete_limit_clause 
	{
	  text *from = new text ("from");
	  text *where = $3 ? new text ("where") : 0;
	  $$ = new chain(from, $2, where, $3, $4, $5);
	}
	| table_wild_list
	  { throw exception::unsupported (__FILE__, __LINE__); }
          FROM join_table_list where_clause
          { throw exception::unsupported (__FILE__, __LINE__); }
	| FROM table_wild_list
	  { throw exception::unsupported (__FILE__, __LINE__); }
	  USING join_table_list where_clause
          { throw exception::unsupported (__FILE__, __LINE__); }
	;

table_wild_list:
	  table_wild_one {}
	  | table_wild_list ',' table_wild_one {};

table_wild_one:
	ident opt_wild opt_table_alias
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| ident '.' ident_any opt_wild opt_table_alias
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_wild:
	/* empty */	{}
	| '.' '*'	{};


opt_delete_options:
	/* empty */	{ $$=0; }
	| opt_delete_option opt_delete_options {};

opt_delete_option:
	QUICK		{ throw exception::unsupported (__FILE__, __LINE__); }
	| LOW_PRIORITY	{ throw exception::unsupported (__FILE__, __LINE__); }
	| IGNORE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); };

truncate:
	TRUNCATE_SYM opt_table_sym table_name
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_table_sym:
	/* empty */
	| TABLE_SYM;

opt_profile_defs:
  /* empty */
  | profile_defs

profile_defs:
  profile_def
  | profile_defs ',' profile_def

profile_def:
  CPU_SYM
    { throw exception::unsupported (__FILE__, __LINE__); }
  | MEMORY_SYM
    { throw exception::unsupported (__FILE__, __LINE__); }
  | BLOCK_SYM IO_SYM
    { throw exception::unsupported (__FILE__, __LINE__); }
  | CONTEXT_SYM SWITCHES_SYM
    { throw exception::unsupported (__FILE__, __LINE__); }
  | PAGE_SYM FAULTS_SYM
    { throw exception::unsupported (__FILE__, __LINE__); }
  | IPC_SYM
    { throw exception::unsupported (__FILE__, __LINE__); }
  | SWAPS_SYM
    { throw exception::unsupported (__FILE__, __LINE__); }
  | SOURCE_SYM
    { throw exception::unsupported (__FILE__, __LINE__); }
  | ALL
    { throw exception::unsupported (__FILE__, __LINE__); }
  ;

opt_profile_args:
  /* empty */
    { throw exception::unsupported (__FILE__, __LINE__); }
  | FOR_SYM QUERY_SYM NUM
    { throw exception::unsupported (__FILE__, __LINE__); }
  ;

/* Show things */

show:	SHOW
	show_param
	{ $$ = $2; }
	;

show_param:
        DATABASES wild_and_where
        { throw exception::unsupported (__FILE__, __LINE__); }
        | opt_full TABLES opt_db wild_and_where
          {
	     
	    if ($1)
	      {
		throw exception::unsupported (__FILE__, __LINE__); 
	      }
	    
	    if ($4)
	      {
		throw exception::unsupported (__FILE__, __LINE__); 
	      }
	    
	    if ($3)
	      {
		$$ = new fake_query (new text ("select table_name as tables_in_mysql from user_tables\n\n", EXACT, false));
		//		$$ = new fake_query (new text ("select table_name as tables_in_mysql from user_tables where tablespace_name = '" + $3->str () + "'\n\n", EXACT, false));
	      }
	    else
	      {
		$$ = new fake_query (new text ("select table_name as tables_in_mysql from user_tables\n\n", EXACT, false));
	      }
	    
	  }
        | opt_full TRIGGERS_SYM opt_db wild_and_where
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | TABLE_SYM STATUS_SYM opt_db wild_and_where
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | OPEN_SYM TABLES opt_db wild_and_where
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| ENGINE_SYM storage_engines show_engine_param
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| opt_full COLUMNS from_or_in table_ident opt_db wild_and_where
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | NEW_SYM MASTER_SYM FOR_SYM SLAVE WITH MASTER_LOG_FILE_SYM EQ
	  TEXT_STRING_sys AND_SYM MASTER_LOG_POS_SYM EQ ulonglong_num
	  AND_SYM MASTER_SERVER_ID_SYM EQ
	  ulong_num
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | master_or_binary LOGS_SYM
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | SLAVE HOSTS_SYM
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | BINLOG_SYM EVENTS_SYM binlog_in binlog_from opt_limit_clause_init
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  } 
        | keys_or_index from_or_in table_ident opt_db where_clause
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| COLUMN_SYM TYPES_SYM
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| TABLE_SYM TYPES_SYM
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| opt_storage ENGINES_SYM
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| PRIVILEGES
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | COUNT_SYM '(' '*' ')' WARNINGS
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | COUNT_SYM '(' '*' ')' ERRORS
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | WARNINGS opt_limit_clause_init
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | ERRORS opt_limit_clause_init
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | PROFILES_SYM
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | PROFILE_SYM opt_profile_defs opt_profile_args opt_limit_clause_init
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | opt_var_type STATUS_SYM wild_and_where
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }	
        | INNOBASE_SYM STATUS_SYM
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | MUTEX_SYM STATUS_SYM
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| opt_full PROCESSLIST_SYM
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | opt_var_type  VARIABLES wild_and_where
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | charset wild_and_where
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | COLLATION_SYM wild_and_where
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| BERKELEY_DB_SYM LOGS_SYM
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| LOGS_SYM
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| GRANTS
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| GRANTS FOR_SYM user
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| CREATE DATABASE opt_if_not_exists ident
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | CREATE TABLE_SYM table_ident
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | CREATE VIEW_SYM table_ident
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | MASTER_SYM STATUS_SYM
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | SLAVE STATUS_SYM
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| CREATE PROCEDURE sp_name
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| CREATE FUNCTION_SYM sp_name
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| PROCEDURE STATUS_SYM wild_and_where
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| FUNCTION_SYM STATUS_SYM wild_and_where
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | PROCEDURE CODE_SYM sp_name
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        | FUNCTION_SYM CODE_SYM sp_name
          {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
        ;

show_engine_param:
	STATUS_SYM
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| LOGS_SYM
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	;

master_or_binary:
	MASTER_SYM
	| BINARY;

opt_storage:
	/* empty */
	| STORAGE_SYM;

opt_db:
	/* empty */ { $$ = 0; }
	| from_or_in ident_any { $$ = $2; }

opt_full:
	/* empty */ { $$ = 0; }
	| FULL	    { $$ = new text ("full"); }

from_or_in:
	FROM
	| IN_SYM;

binlog_in:
	/* empty */ { throw exception::unsupported (__FILE__, __LINE__); }
        | IN_SYM TEXT_STRING_sys { throw exception::unsupported (__FILE__, __LINE__); };

binlog_from:
	/* empty */ { throw exception::unsupported (__FILE__, __LINE__); }
        | FROM ulonglong_num { throw exception::unsupported (__FILE__, __LINE__); };

wild_and_where:
	/* empty */ {$$ = 0;}
      | LIKE TEXT_STRING_sys
	{ throw exception::unsupported (__FILE__, __LINE__); }
      | WHERE expr
        { throw exception::unsupported (__FILE__, __LINE__); }
      ;


/* A Oracle compatible synonym for show */
describe:
	describe_command table_ident
	opt_describe_column { throw exception::unsupported (__FILE__, __LINE__); }
	| describe_command opt_extended_describe
	  select
          { throw exception::unsupported (__FILE__, __LINE__); }
	;

describe_command:
	DESC
	| DESCRIBE;

opt_extended_describe:
	/* empty */ {}
	| EXTENDED_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_describe_column:
	/* empty */	{}
	| text_string	
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	| ident
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	;


/* flush things */

flush:
	FLUSH_SYM opt_no_write_to_binlog
	flush_options
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

flush_options:
	flush_options ',' flush_option
	| flush_option;

flush_option:
	table_or_tables	{ throw exception::unsupported (__FILE__, __LINE__); } opt_table_list {}
	| TABLES WITH READ_SYM LOCK_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| QUERY_SYM CACHE_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| HOSTS_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| PRIVILEGES	{ throw exception::unsupported (__FILE__, __LINE__); }
	| LOGS_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| STATUS_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
        | SLAVE         { throw exception::unsupported (__FILE__, __LINE__); }
        | MASTER_SYM    { throw exception::unsupported (__FILE__, __LINE__); }
	| DES_KEY_FILE	{ throw exception::unsupported (__FILE__, __LINE__); }
 	| RESOURCES     { throw exception::unsupported (__FILE__, __LINE__); };

opt_table_list:
	/* empty */  {;}
	| table_list {;};

reset:
	RESET_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); } reset_options
	{}
	;

reset_options:
	reset_options ',' reset_option
	| reset_option;

reset_option:
        SLAVE                 { throw exception::unsupported (__FILE__, __LINE__); }
        | MASTER_SYM          { throw exception::unsupported (__FILE__, __LINE__); }
	| QUERY_SYM CACHE_SYM { throw exception::unsupported (__FILE__, __LINE__); };

purge:
	PURGE
	{ throw exception::unsupported (__FILE__, __LINE__); } purge_options
	{}
	;

purge_options:
	master_or_binary LOGS_SYM purge_option
	;

purge_option:
        TO_SYM TEXT_STRING_sys
        { throw exception::unsupported (__FILE__, __LINE__); }
	| BEFORE_SYM expr
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

/* kill threads */

kill:
	KILL_SYM { throw exception::unsupported (__FILE__, __LINE__); } kill_option expr
	{ throw exception::unsupported (__FILE__, __LINE__); };

kill_option:
	/* empty */	 { throw exception::unsupported (__FILE__, __LINE__); }
	| CONNECTION_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| QUERY_SYM      { throw exception::unsupported (__FILE__, __LINE__); }
        ;

/* change database */

use:	USE_SYM ident
	{ throw exception::unsupported (__FILE__, __LINE__); };

/* import, export of files */

load:   LOAD DATA_SYM
        { throw exception::unsupported (__FILE__, __LINE__); }
        load_data
        {}
        |
        LOAD TABLE_SYM table_ident FROM MASTER_SYM
        { throw exception::unsupported (__FILE__, __LINE__); };

load_data:
	load_data_lock opt_local INFILE TEXT_STRING_filesystem
	{ throw exception::unsupported (__FILE__, __LINE__); }
        opt_duplicate INTO
        { throw exception::unsupported (__FILE__, __LINE__); }
        TABLE_SYM table_ident
        { throw exception::unsupported (__FILE__, __LINE__); }
        opt_field_term opt_line_term opt_ignore_lines opt_field_or_var_spec
        opt_load_data_set_spec
        {}
        |
	FROM MASTER_SYM
        { throw exception::unsupported (__FILE__, __LINE__); };

opt_local:
	/* empty */	{ throw exception::unsupported (__FILE__, __LINE__); }
	| LOCAL_SYM	{ throw exception::unsupported (__FILE__, __LINE__); };

load_data_lock:
	/* empty */	{ throw exception::unsupported (__FILE__, __LINE__); }
	| CONCURRENT
          { throw exception::unsupported (__FILE__, __LINE__); }
	| LOW_PRIORITY	{ throw exception::unsupported (__FILE__, __LINE__); };


opt_duplicate:
	/* empty */	{ throw exception::unsupported (__FILE__, __LINE__); }
	| REPLACE	{ throw exception::unsupported (__FILE__, __LINE__); }
	| IGNORE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); };

opt_field_term:
	/* empty */
	| COLUMNS field_term_list;

field_term_list:
	field_term_list field_term
	| field_term;

field_term:
	TERMINATED BY text_string 
          { throw exception::unsupported (__FILE__, __LINE__); }
	| OPTIONALLY ENCLOSED BY text_string
	  { throw exception::unsupported (__FILE__, __LINE__); }
        | ENCLOSED BY text_string
          { throw exception::unsupported (__FILE__, __LINE__); }
        | ESCAPED BY text_string
          { throw exception::unsupported (__FILE__, __LINE__); };

opt_line_term:
	/* empty */
	| LINES line_term_list;

line_term_list:
	line_term_list line_term
	| line_term;

line_term:
        TERMINATED BY text_string
          { throw exception::unsupported (__FILE__, __LINE__); }
        | STARTING BY text_string
          { throw exception::unsupported (__FILE__, __LINE__); };

opt_ignore_lines:
	/* empty */
        | IGNORE_SYM NUM LINES
          { throw exception::unsupported (__FILE__, __LINE__); };

opt_field_or_var_spec:
	/* empty */	          {}
	| '(' fields_or_vars ')'  {}
	| '(' ')'	          {};

fields_or_vars:
        fields_or_vars ',' field_or_var
          { throw exception::unsupported (__FILE__, __LINE__); }
        | field_or_var
          { throw exception::unsupported (__FILE__, __LINE__); }
        ;

field_or_var:
        simple_ident_nospvar { throw exception::unsupported (__FILE__, __LINE__); }
        | '@' ident_or_text
          { throw exception::unsupported (__FILE__, __LINE__); }
        ;

opt_load_data_set_spec:
        /* empty */           {}
        | SET insert_update_list  {};


/* Common definitions */

text_literal:
	TEXT_STRING_literal
	| NCHAR_STRING
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| UNDERSCORE_CHARSET TEXT_STRING
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| text_literal TEXT_STRING_literal
	  {
	    $$ = new chain ( $1, new text (" ||\n"), $2);
	  }
	;

text_string:
	TEXT_STRING_literal
	| HEX_NUM
	  { throw exception::unsupported (__FILE__, __LINE__); }
        | BIN_NUM
          { throw exception::unsupported (__FILE__, __LINE__); }
	;

param_marker:
        PARAM_MARKER
        { throw exception::unsupported (__FILE__, __LINE__); }
	;

signed_literal:
	literal		
	| '+' NUM_literal 
	{ 
	  $$ = new chain (new text ("+"), $2);
	  $$ -> set_context (DATA_TYPE_NUMBER);
	}
	| '-' NUM_literal
	{ 
	  $$ = new chain (new text ("-"), $2);
	  $$ -> set_context (DATA_TYPE_NUMBER);
	}
	;


literal:
	text_literal	
	| NUM_literal	
	| NULL_SYM	{ $$ = new text( "null" ); }
	| FALSE_SYM	{ $$ = new text( "'f'" ); }
	| TRUE_SYM	{ $$ = new text( "'f'" ); }
	| HEX_NUM	{ $$ = new text( yytext ); }
	| BIN_NUM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| UNDERSCORE_CHARSET HEX_NUM
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| UNDERSCORE_CHARSET BIN_NUM
          { throw exception::unsupported (__FILE__, __LINE__); }
	| DATE_SYM text_literal { throw exception::unsupported (__FILE__, __LINE__); }
	| TIME_SYM text_literal { throw exception::unsupported (__FILE__, __LINE__); }
	| TIMESTAMP text_literal { throw exception::unsupported (__FILE__, __LINE__); };

NUM_literal:
	NUM 
	{ 
	  $$ = new text (yytext);
	  $$ -> set_context (DATA_TYPE_NUMBER);
	}
	| LONG_NUM	
	{ 
	  $$ = new text (yytext);
	  $$ -> set_context (DATA_TYPE_NUMBER);
	}
	| ULONGLONG_NUM	
	{ 
	  $$ = new text (yytext);
	  $$ -> set_context (DATA_TYPE_NUMBER);
	}
        | DECIMAL_NUM
	{ 
	  $$ = new text (yytext);
	  $$ -> set_context (DATA_TYPE_NUMBER);
	}
	| FLOAT_NUM
	{ 
	  $$ = new text (yytext);
	  $$ -> set_context (DATA_TYPE_NUMBER);
	}
	;

/**********************************************************************
** Creating different items.
**********************************************************************/

insert_ident:
	simple_ident_nospvar 
	| table_wild
	;

table_wild:
	ident '.' '*'
	{
	  $$ = new printable_alias (new select_item_wild( 0, $1 ));
	}
	| ident '.' ident_any '.' '*'
	{
	  $$ = new printable_alias (new select_item_wild( $1, $3 )); 
	}
	;

order_ident:
	expr
	;

simple_ident:
	ident { $$ = $1; }
        | simple_ident_q
	;

simple_ident_nospvar:
	ident { $$ = $1; }
	| simple_ident_q
	;

simple_ident_q:
	ident '.' ident_any
        { $$ = new identity (0, $1, $3); }
	| '.' ident '.' ident_any
	{ $$ = new identity (0, $2, $4); }
	| ident '.' ident_any '.' ident_any
	{ $$ = new identity( $1, $3, $5 ); }


field_ident:
	ident			{ $$ = new identity (0, 0, $1); }
	| ident '.' ident_any '.' ident_any
                                { $$ = new identity ($1, $3, $5); }
	| ident '.' ident_any
                                { $$ = new identity (0, $1, $3); }
	| '.' ident_any		{ $$ = new identity (0, 0, $2); }

table_ident:
	ident			
	{ 
	  $$ = new identity (0, $1, 0);
	}
	| ident '.' ident_any	{ $$ = new identity (0, $1, $3); }
	| '.' ident_any		{ $$ = new identity (0, $2, 0);  }
        ;

table_ident_nodb:
	ident			{ throw exception::unsupported (__FILE__, __LINE__); }
        ;

IDENT_sys:
	IDENT 
          {
	    $$ = new text (yytext, IDENTIFIER);
	  }
	| IDENT_QUOTED
          {
	    $$ = new text (yytext, IDENTIFIER_QUOTED); 
	  }
	;

TEXT_STRING_sys:
	TEXT_STRING
	{ $$ = new text(yytext, LITERAL); }
	;

TEXT_STRING_literal:
	TEXT_STRING
	{ $$ = new text(yytext, LITERAL); }
	;


TEXT_STRING_filesystem:
	TEXT_STRING
	{
	  $$ = new text(yytext, LITERAL); 
	}
	;

ident:
	IDENT_sys	   
	| keyword
	;

ident_any:
	ident
	| keyword_reserved
	;

label_ident:
	IDENT_sys	    { throw exception::unsupported (__FILE__, __LINE__); }
	| keyword_sp
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

ident_or_text:
        ident { $$ = $1; }                   
	| TEXT_STRING_sys	
	| LEX_HOSTNAME		{ throw exception::unsupported (__FILE__, __LINE__); };

user:
	ident_or_text
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| ident_or_text '@' ident_or_text
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| CURRENT_USER optional_braces
	{ throw exception::unsupported (__FILE__, __LINE__); };

/* Keyword that we allow for identifiers (except SP labels) */
keyword:
	keyword_sp		
	| ASCII_SYM		{ $$ = new text ("ascii", IDENTIFIER); }
	| BACKUP_SYM		{ $$ = new text ("backup", IDENTIFIER); }
	| BEGIN_SYM		{ $$ = new text ("begin", IDENTIFIER); }
	| BYTE_SYM		{ $$ = new text ("byte", IDENTIFIER); }
	| CACHE_SYM		{ $$ = new text ("cache", IDENTIFIER); }
	| CHARSET		{ $$ = new text ("charset", IDENTIFIER); }
	| CHECKSUM_SYM		{ $$ = new text ("checksum", IDENTIFIER); }
	| CLOSE_SYM		{ $$ = new text ("close", IDENTIFIER); }
	| COMMENT_SYM		{ $$ = new text ("comment", IDENTIFIER); }
	| COMMIT_SYM		{ $$ = new text ("commit", IDENTIFIER); }
	| CONTAINS_SYM          { $$ = new text ("contains", IDENTIFIER); }
        | DEALLOCATE_SYM        { $$ = new text ("deallocate", IDENTIFIER); }
	| DO_SYM		{ $$ = new text ("do", IDENTIFIER); }
	| END			{ $$ = new text ("end", IDENTIFIER); }
	| EXECUTE_SYM		{ $$ = new text ("execute", IDENTIFIER); }
	| FLUSH_SYM		{ $$ = new text ("flush", IDENTIFIER); }
	| HANDLER_SYM		{ $$ = new text ("handler", IDENTIFIER); }
	| HELP_SYM		{ $$ = new text ("help", IDENTIFIER); }
	| LANGUAGE_SYM          { $$ = new text ("language", IDENTIFIER); }
	| NO_SYM		{ $$ = new text ("no", IDENTIFIER); }
	| OPEN_SYM		{ $$ = new text ("open", IDENTIFIER); }
        | PREPARE_SYM           { $$ = new text ("prepare", IDENTIFIER); }
	| REPAIR		{ $$ = new text ("repair", IDENTIFIER); }
	| RESET_SYM		{ $$ = new text ("reset", IDENTIFIER); }
	| RESTORE_SYM		{ $$ = new text ("restore", IDENTIFIER); }
	| ROLLBACK_SYM		{ $$ = new text ("rollback", IDENTIFIER); }
	| SAVEPOINT_SYM		{ $$ = new text ("savepoint", IDENTIFIER); }
	| SECURITY_SYM		{ $$ = new text ("security", IDENTIFIER); }
	| SIGNED_SYM		{ $$ = new text ("signed", IDENTIFIER); }
	| SLAVE			{ $$ = new text ("slave", IDENTIFIER); }
	| START_SYM		{ $$ = new text ("start", IDENTIFIER); }
	| STOP_SYM		{ $$ = new text ("stop", IDENTIFIER); }
	| TRUNCATE_SYM		{ $$ = new text ("truncate", IDENTIFIER); }
	| UNICODE_SYM		{ $$ = new text ("unicode", IDENTIFIER); }
        | XA_SYM                { $$ = new text ("xa", IDENTIFIER); }
        | UPGRADE_SYM           { $$ = new text ("upgrade", IDENTIFIER); }
	;

/**

Keywords that kan be used as identifiers in special situations, when
it is _known_ that the following token must be an identifier.

*/
keyword_reserved: 
	FULLTEXT_SYM		{ $$ = new text ("fulltext", IDENTIFIER); }
	| OPTION		{ $$ = new text ("option", IDENTIFIER); }
	;

/*
 * Keywords that we allow for labels in SPs.
 * Anything that's the beginning of a statement or characteristics
 * must be in keyword above, otherwise we get (harmful) shift/reduce
 * conflicts.
 */
keyword_sp:
	ACTION			{ $$ = new text ("action", IDENTIFIER); }
	| ADDDATE_SYM		{ $$ = new text ("adddate", IDENTIFIER); }
	| AFTER_SYM		{ $$ = new text ("after", IDENTIFIER); }
	| AGAINST		{ $$ = new text ("against", IDENTIFIER); }
	| AGGREGATE_SYM		{ $$ = new text ("aggregate", IDENTIFIER); }
	| ALGORITHM_SYM		{ $$ = new text ("algorithm", IDENTIFIER); }
	| ANY_SYM		{ $$ = new text ("any", IDENTIFIER); }
	| AUTO_INC		{ $$ = new text ("auto_inc", IDENTIFIER); }
	| AVG_ROW_LENGTH	{ $$ = new text ("avg_row_length", IDENTIFIER); }
	| AVG_SYM		{ $$ = new text ("avg", IDENTIFIER); }
	| BERKELEY_DB_SYM	{ $$ = new text ("berkeley_db", IDENTIFIER); }
	| BINLOG_SYM		{ $$ = new text ("binlog", IDENTIFIER); }
	| BIT_SYM		{ $$ = new text ("bit", IDENTIFIER); }
	| BLOCK_SYM             { $$ = new text ("block", IDENTIFIER); }
	| BOOL_SYM		{ $$ = new text ("bool", IDENTIFIER); }
	| BOOLEAN_SYM		{ $$ = new text ("boolean", IDENTIFIER); }
	| BTREE_SYM		{ $$ = new text ("btree", IDENTIFIER); }
	| CASCADED              { $$ = new text ("cascaded", IDENTIFIER); }
	| CHAIN_SYM		{ $$ = new text ("chain", IDENTIFIER); }
	| CHANGED		{ $$ = new text ("changed", IDENTIFIER); }
	| CIPHER_SYM		{ $$ = new text ("cipher", IDENTIFIER); }
	| CLIENT_SYM		{ $$ = new text ("client", IDENTIFIER); }
        | CODE_SYM              { $$ = new text ("code", IDENTIFIER); }
	| COLLATION_SYM		{ $$ = new text ("collation", IDENTIFIER); }
        | COLUMNS               { $$ = new text ("columns", IDENTIFIER); }
	| COMMITTED_SYM		{ $$ = new text ("committed", IDENTIFIER); }
	| COMPACT_SYM		{ $$ = new text ("compact", IDENTIFIER); }
	| COMPRESSED_SYM	{ $$ = new text ("compressed", IDENTIFIER); }
	| CONCURRENT		{ $$ = new text ("concurrent", IDENTIFIER); }
	| CONSISTENT_SYM	{ $$ = new text ("consistent", IDENTIFIER); }
	| CONTEXT_SYM           { $$ = new text ("context", IDENTIFIER); }
	| CPU_SYM               { $$ = new text ("cpu", IDENTIFIER); }
	| CUBE_SYM		{ $$ = new text ("cube", IDENTIFIER); }
	| DATA_SYM		{ $$ = new text ("data", IDENTIFIER); }
	| DATETIME		{ $$ = new text ("datetime", IDENTIFIER); }
	| DATE_SYM		{ $$ = new text ("date", IDENTIFIER); }
	| DAY_SYM		{ $$ = new text ("day", IDENTIFIER); }
	| DEFINER_SYM		{ $$ = new text ("definer", IDENTIFIER); }
	| DELAY_KEY_WRITE_SYM	{ $$ = new text ("delay_key_write", IDENTIFIER); }
	| DES_KEY_FILE		{ $$ = new text ("des_key_file", IDENTIFIER); }
	| DIRECTORY_SYM		{ $$ = new text ("directory", IDENTIFIER); }
	| DISCARD		{ $$ = new text ("discard", IDENTIFIER); }
	| DUMPFILE		{ $$ = new text ("dumpfile", IDENTIFIER); }
	| DUPLICATE_SYM		{ $$ = new text ("duplicate", IDENTIFIER); }
	| DYNAMIC_SYM		{ $$ = new text ("dynamic", IDENTIFIER); }
	| ENUM			{ $$ = new text ("enum", IDENTIFIER); }
	| ENGINE_SYM		{ $$ = new text ("engine", IDENTIFIER); }
	| ENGINES_SYM		{ $$ = new text ("engines", IDENTIFIER); }
	| ERRORS		{ $$ = new text ("errors", IDENTIFIER); }
	| ESCAPE_SYM		{ $$ = new text ("escape", IDENTIFIER); }
	| EVENTS_SYM		{ $$ = new text ("events", IDENTIFIER); }
        | EXPANSION_SYM         { $$ = new text ("expansion", IDENTIFIER); }
	| EXTENDED_SYM		{ $$ = new text ("extended", IDENTIFIER); }
	| FAST_SYM		{ $$ = new text ("fast", IDENTIFIER); }
	| FAULTS_SYM            { $$ = new text ("faults", IDENTIFIER); }
	| FOUND_SYM		{ $$ = new text ("found", IDENTIFIER); }
	| DISABLE_SYM		{ $$ = new text ("disable", IDENTIFIER); }
	| ENABLE_SYM		{ $$ = new text ("enable", IDENTIFIER); }
	| FULL			{ $$ = new text ("full", IDENTIFIER); }
	| FILE_SYM		{ $$ = new text ("file", IDENTIFIER); }
	| FIRST_SYM		{ $$ = new text ("first", IDENTIFIER); }
	| FIXED_SYM		{ $$ = new text ("fixed", IDENTIFIER); }
	| FRAC_SECOND_SYM	{ $$ = new text ("frac_second", IDENTIFIER); }
	| GEOMETRY_SYM		{ $$ = new text ("geometry", IDENTIFIER); }
	| GEOMETRYCOLLECTION	{ $$ = new text ("geometrycollection", IDENTIFIER); }
	| GET_FORMAT		{ $$ = new text ("get_format", IDENTIFIER); }
	| GRANTS		{ $$ = new text ("grants", IDENTIFIER); }
	| GLOBAL_SYM		{ $$ = new text ("global", IDENTIFIER); }
	| HASH_SYM		{ $$ = new text ("hash", IDENTIFIER); }
	| HOSTS_SYM		{ $$ = new text ("hosts", IDENTIFIER); }
	| HOUR_SYM		{ $$ = new text ("hour", IDENTIFIER); }
	| IDENTIFIED_SYM	{ $$ = new text ("identified", IDENTIFIER); }
	| INVOKER_SYM		{ $$ = new text ("invoker", IDENTIFIER); }
	| IMPORT		{ $$ = new text ("import", IDENTIFIER); }
	| INDEXES		{ $$ = new text ("indexes", IDENTIFIER); }
	| ISOLATION		{ $$ = new text ("isolation", IDENTIFIER); }
	| ISSUER_SYM		{ $$ = new text ("issuer", IDENTIFIER); }
	| INNOBASE_SYM		{ $$ = new text ("innobase", IDENTIFIER); }
	| INSERT_METHOD		{ $$ = new text ("insert_method", IDENTIFIER); }
	| IO_SYM                { $$ = new text ("io", IDENTIFIER); }
	| IPC_SYM               { $$ = new text ("ipc", IDENTIFIER); }
	| RELAY_THREAD		{ $$ = new text ("relay_thread", IDENTIFIER); }
	| LAST_SYM		{ $$ = new text ("last", IDENTIFIER); }
	| LEAVES                { $$ = new text ("leaves", IDENTIFIER); }
	| LEVEL_SYM		{ $$ = new text ("level", IDENTIFIER); }
	| LINESTRING		{ $$ = new text ("linestring", IDENTIFIER); }
	| LOCAL_SYM		{ $$ = new text ("local", IDENTIFIER); }
	| LOCKS_SYM		{ $$ = new text ("locks", IDENTIFIER); }
	| LOGS_SYM		{ $$ = new text ("logs", IDENTIFIER); }
	| MAX_ROWS		{ $$ = new text ("max_rows", IDENTIFIER); }
	| MASTER_SYM		{ $$ = new text ("master", IDENTIFIER); }
	| MASTER_HOST_SYM	{ $$ = new text ("master_host", IDENTIFIER); }
	| MASTER_PORT_SYM	{ $$ = new text ("master_port", IDENTIFIER); }
	| MASTER_LOG_FILE_SYM	{ $$ = new text ("master_log_file", IDENTIFIER); }
	| MASTER_LOG_POS_SYM	{ $$ = new text ("master_log_pos", IDENTIFIER); }
	| MASTER_USER_SYM	{ $$ = new text ("master_user", IDENTIFIER); }
	| MASTER_PASSWORD_SYM	{ $$ = new text ("master_password", IDENTIFIER); }
	| MASTER_SERVER_ID_SYM  { $$ = new text ("master_server_id", IDENTIFIER); }
	| MASTER_CONNECT_RETRY_SYM	{ $$ = new text ("master_connect_retry", IDENTIFIER); }
	| MASTER_SSL_SYM	{ $$ = new text ("master_ssl", IDENTIFIER); }
	| MASTER_SSL_CA_SYM	{ $$ = new text ("master_ssl_ca", IDENTIFIER); }
	| MASTER_SSL_CAPATH_SYM	{ $$ = new text ("master_ssl_capath", IDENTIFIER); }
	| MASTER_SSL_CERT_SYM	{ $$ = new text ("master_ssl_cert", IDENTIFIER); }
	| MASTER_SSL_CIPHER_SYM	{ $$ = new text ("master_ssl_cipher", IDENTIFIER); }
	| MASTER_SSL_KEY_SYM	{ $$ = new text ("master_ssl_key", IDENTIFIER); }
	| MAX_CONNECTIONS_PER_HOUR	 { $$ = new text ("max_connections_per_hour", IDENTIFIER); }
	| MAX_QUERIES_PER_HOUR	{ $$ = new text ("max_queries_per_hour", IDENTIFIER); }
	| MAX_UPDATES_PER_HOUR	{ $$ = new text ("max_updates_per_hour", IDENTIFIER); }
	| MAX_USER_CONNECTIONS_SYM { $$ = new text ("max_user_connections", IDENTIFIER); }
	| MEDIUM_SYM		{ $$ = new text ("medium", IDENTIFIER); }
	| MEMORY_SYM            { $$ = new text ("memory", IDENTIFIER); }
	| MERGE_SYM		{ $$ = new text ("merge", IDENTIFIER); }
	| MICROSECOND_SYM	{ $$ = new text ("microsecond", IDENTIFIER); }
        | MIGRATE_SYM           { $$ = new text ("migrate", IDENTIFIER); }
	| MINUTE_SYM		{ $$ = new text ("minute", IDENTIFIER); }
	| MIN_ROWS		{ $$ = new text ("min_rows", IDENTIFIER); }
	| MODIFY_SYM		{ $$ = new text ("modify", IDENTIFIER); }
	| MODE_SYM		{ $$ = new text ("mode", IDENTIFIER); }
	| MONTH_SYM		{ $$ = new text ("month", IDENTIFIER); }
	| MULTILINESTRING	{ $$ = new text ("multilinestring", IDENTIFIER); }
	| MULTIPOINT		{ $$ = new text ("multipoint", IDENTIFIER); }
	| MULTIPOLYGON		{ $$ = new text ("multipolygon", IDENTIFIER); }
        | MUTEX_SYM             { $$ = new text ("mutex", IDENTIFIER); }
	| NAME_SYM              { $$ = new text ("name", IDENTIFIER); }
	| NAMES_SYM		{ $$ = new text ("names", IDENTIFIER); }
	| NATIONAL_SYM		{ $$ = new text ("national", IDENTIFIER); }
	| NCHAR_SYM		{ $$ = new text ("nchar", IDENTIFIER); }
	| NDBCLUSTER_SYM	{ $$ = new text ("ndbcluster", IDENTIFIER); }
	| NEXT_SYM		{ $$ = new text ("next", IDENTIFIER); }
	| NEW_SYM		{ $$ = new text ("new", IDENTIFIER); }
	| NONE_SYM		{ $$ = new text ("none", IDENTIFIER); }
	| NVARCHAR_SYM		{ $$ = new text ("nvarchar", IDENTIFIER); }
	| OFFSET_SYM		{ $$ = new text ("offset", IDENTIFIER); }
	| OLD_PASSWORD		{ $$ = new text ("old_password", IDENTIFIER); }
	| ONE_SHOT_SYM		{ $$ = new text ("one_shot", IDENTIFIER); }
        | ONE_SYM               { $$ = new text ("one", IDENTIFIER); }
	| PACK_KEYS_SYM		{ $$ = new text ("pack_keys", IDENTIFIER); }
	| PAGE_SYM              { $$ = new text ("page", IDENTIFIER); }
	| PARTIAL		{ $$ = new text ("partial", IDENTIFIER); }
	| PASSWORD		{ $$ = new text ("password", IDENTIFIER); }
        | PHASE_SYM             { $$ = new text ("phase", IDENTIFIER); }
	| POINT_SYM		{ $$ = new text ("point", IDENTIFIER); }
	| POLYGON		{ $$ = new text ("polygon", IDENTIFIER); }
	| PREV_SYM		{ $$ = new text ("prev", IDENTIFIER); }
        | PRIVILEGES            { $$ = new text ("privileges", IDENTIFIER); }
	| PROCESS		{ $$ = new text ("process", IDENTIFIER); }
	| PROCESSLIST_SYM	{ $$ = new text ("processlist", IDENTIFIER); }
	| PROFILE_SYM           { $$ = new text ("profile", IDENTIFIER); }
	| PROFILES_SYM          { $$ = new text ("profiles", IDENTIFIER); }
	| QUARTER_SYM		{ $$ = new text ("quarter", IDENTIFIER); }
	| QUERY_SYM		{ $$ = new text ("query", IDENTIFIER); }
	| QUICK			{ $$ = new text ("quick", IDENTIFIER); }
	| RAID_0_SYM		{ $$ = new text ("raid_0_sym", IDENTIFIER); }
	| RAID_CHUNKS		{ $$ = new text ("raid_chunks", IDENTIFIER); }
	| RAID_CHUNKSIZE	{ $$ = new text ("raid_chunksize", IDENTIFIER); }
	| RAID_STRIPED_SYM	{ $$ = new text ("raid_striped", IDENTIFIER); }
	| RAID_TYPE		{ $$ = new text ("raid_type", IDENTIFIER); }
        | RECOVER_SYM           { $$ = new text ("recover", IDENTIFIER); }
        | REDUNDANT_SYM         { $$ = new text ("redundant", IDENTIFIER); }
	| RELAY_LOG_FILE_SYM	{ $$ = new text ("relay_log_file", IDENTIFIER); }
	| RELAY_LOG_POS_SYM	{ $$ = new text ("relay_log_pos", IDENTIFIER); }
	| RELOAD		{ $$ = new text ("reload", IDENTIFIER); }
	| REPEATABLE_SYM	{ $$ = new text ("repeatable", IDENTIFIER); }
	| REPLICATION		{ $$ = new text ("replication", IDENTIFIER); }
	| RESOURCES		{ $$ = new text ("resources", IDENTIFIER); }
        | RESUME_SYM            { $$ = new text ("resume", IDENTIFIER); }
	| RETURNS_SYM           { $$ = new text ("returns", IDENTIFIER); }
	| ROLLUP_SYM		{ $$ = new text ("rollup", IDENTIFIER); }
	| ROUTINE_SYM		{ $$ = new text ("routine", IDENTIFIER); }
	| ROWS_SYM		{ $$ = new text ("rows", IDENTIFIER); }
	| ROW_FORMAT_SYM	{ $$ = new text ("row_format", IDENTIFIER); }
	| ROW_SYM		{ $$ = new text ("row", IDENTIFIER); }
	| RTREE_SYM		{ $$ = new text ("rtree", IDENTIFIER); }
	| SECOND_SYM		{ $$ = new text ("second", IDENTIFIER); }
	| SERIAL_SYM		{ $$ = new text ("serial", IDENTIFIER); }
	| SERIALIZABLE_SYM	{ $$ = new text ("serializable", IDENTIFIER); }
	| SESSION_SYM		{ $$ = new text ("session", IDENTIFIER); }
	| SIMPLE_SYM		{ $$ = new text ("simple", IDENTIFIER); }
	| SHARE_SYM		{ $$ = new text ("share", IDENTIFIER); }
	| SHUTDOWN		{ $$ = new text ("shutdown", IDENTIFIER); }
	| SNAPSHOT_SYM		{ $$ = new text ("snapshot", IDENTIFIER); }
	| SOUNDS_SYM		{ $$ = new text ("sounds", IDENTIFIER); }
	| SOURCE_SYM            { $$ = new text ("source", IDENTIFIER); }
	| SQL_CACHE_SYM		{ $$ = new text ("sql_cache", IDENTIFIER); }
	| SQL_BUFFER_RESULT	{ $$ = new text ("sql_buffer_result", IDENTIFIER); }
	| SQL_NO_CACHE_SYM	{ $$ = new text ("sql_no_cache", IDENTIFIER); }
	| SQL_THREAD		{ $$ = new text ("sql_threadD", IDENTIFIER); }
	| STATUS_SYM		{ $$ = new text ("status", IDENTIFIER); }
	| STORAGE_SYM		{ $$ = new text ("storage", IDENTIFIER); }
	| STRING_SYM		{ $$ = new text ("string", IDENTIFIER); }
	| SUBDATE_SYM		{ $$ = new text ("subdate", IDENTIFIER); }
	| SUBJECT_SYM		{ $$ = new text ("subject", IDENTIFIER); }
	| SUPER_SYM		{ $$ = new text ("super", IDENTIFIER); }
        | SUSPEND_SYM           { $$ = new text ("suspend", IDENTIFIER); }
        | SWAPS_SYM             { $$ = new text ("swaps", IDENTIFIER); }
	| SWITCHES_SYM          { $$ = new text ("switches", IDENTIFIER); }
        | TABLES                { $$ = new text ("tables", IDENTIFIER); }
	| TABLESPACE		{ $$ = new text ("tablespace", IDENTIFIER); }
	| TEMPORARY		{ $$ = new text ("temporary", IDENTIFIER); }
	| TEMPTABLE_SYM		{ $$ = new text ("temptable", IDENTIFIER); }
	| TEXT_SYM		{ $$ = new text ("text", IDENTIFIER); }
	| TRANSACTION_SYM	{ $$ = new text ("transaction", IDENTIFIER); }
	| TRIGGERS_SYM		{ $$ = new text ("triggers", IDENTIFIER); }
	| TIMESTAMP		{ $$ = new text ("timestamp", IDENTIFIER); }
	| TIMESTAMP_ADD		{ $$ = new text ("timestamp_add", IDENTIFIER); }
	| TIMESTAMP_DIFF	{ $$ = new text ("timestamp_diff", IDENTIFIER); }
	| TIME_SYM		{ $$ = new text ("time", IDENTIFIER); }
	| TYPES_SYM		{ $$ = new text ("types", IDENTIFIER); }
        | TYPE_SYM              { $$ = new text ("type", IDENTIFIER); }
        | UDF_RETURNS_SYM       { $$ = new text ("udf_returns", IDENTIFIER); }
	| FUNCTION_SYM		{ $$ = new text ("function", IDENTIFIER); }
	| UNCOMMITTED_SYM	{ $$ = new text ("uncommitted", IDENTIFIER); }
	| UNDEFINED_SYM		{ $$ = new text ("undefined", IDENTIFIER); }
	| UNKNOWN_SYM		{ $$ = new text ("unknown", IDENTIFIER); }
	| UNTIL_SYM		{ $$ = new text ("until", IDENTIFIER); }
	| USER			{ $$ = new text ("user", IDENTIFIER); }
	| USE_FRM		{ $$ = new text ("use_frm", IDENTIFIER); }
	| VARIABLES		{ $$ = new text ("variables", IDENTIFIER); }
	| VIEW_SYM		{ $$ = new text ("view", IDENTIFIER); }
	| VALUE_SYM		{ $$ = new text ("value", IDENTIFIER); }
	| WARNINGS		{ $$ = new text ("warnings", IDENTIFIER); }
	| WEEK_SYM		{ $$ = new text ("week", IDENTIFIER); }
	| WORK_SYM		{ $$ = new text ("work", IDENTIFIER); }
	| X509_SYM		{ $$ = new text ("x509", IDENTIFIER); }
	| YEAR_SYM		{ $$ = new text ("year", IDENTIFIER); }
	;

/* Option functions */

set:
	SET opt_option
	{ throw exception::unsupported (__FILE__, __LINE__); }
	option_value_list
	{}
	;

opt_option:
	/* empty */ {}
	| OPTION {};

option_value_list:
	option_type_value
	| option_value_list ',' option_type_value;

option_type_value:
        { throw exception::unsupported (__FILE__, __LINE__); }
	ext_option_value
          { throw exception::unsupported (__FILE__, __LINE__); }
;

option_type:
        option_type2    {}
	| GLOBAL_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| LOCAL_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| SESSION_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

option_type2:
	/* empty */	{ throw exception::unsupported (__FILE__, __LINE__); }
	| ONE_SHOT_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_var_type:
	/* empty */	{ throw exception::unsupported (__FILE__, __LINE__); }
	| GLOBAL_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| LOCAL_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| SESSION_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_var_ident_type:
	/* empty */		{ throw exception::unsupported (__FILE__, __LINE__); }
	| GLOBAL_SYM '.'	{ throw exception::unsupported (__FILE__, __LINE__); }
	| LOCAL_SYM '.'		{ throw exception::unsupported (__FILE__, __LINE__); }
	| SESSION_SYM '.'	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

ext_option_value:
        sys_option_value
        | option_type2 option_value;

sys_option_value:
        option_type internal_variable_name equal set_expr_or_default
          { throw exception::unsupported (__FILE__, __LINE__); }
        | option_type TRANSACTION_SYM ISOLATION LEVEL_SYM isolation_types
	{ throw exception::unsupported (__FILE__, __LINE__); }
        ;

option_value:
	'@' ident_or_text equal expr
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| '@' '@' opt_var_ident_type internal_variable_name equal set_expr_or_default
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| charset old_or_new_charset_name_or_default
	{ throw exception::unsupported (__FILE__, __LINE__); }
        | NAMES_SYM equal expr
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| NAMES_SYM charset_name_or_default opt_collate
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| PASSWORD equal text_or_password
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| PASSWORD FOR_SYM user equal text_or_password
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

internal_variable_name:
	ident
        { throw exception::unsupported (__FILE__, __LINE__); }
	| DEFAULT '.' ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
        ;

isolation_types:
	READ_SYM UNCOMMITTED_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| READ_SYM COMMITTED_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| REPEATABLE_SYM READ_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| SERIALIZABLE_SYM		{ throw exception::unsupported (__FILE__, __LINE__); }
	;

text_or_password:
	TEXT_STRING { throw exception::unsupported (__FILE__, __LINE__); }
	| PASSWORD '(' TEXT_STRING ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| OLD_PASSWORD '(' TEXT_STRING ')'
	  { throw exception::unsupported (__FILE__, __LINE__); }
          ;


set_expr_or_default:
	expr      { throw exception::unsupported (__FILE__, __LINE__); }
	| DEFAULT { throw exception::unsupported (__FILE__, __LINE__); }
	| ON	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ALL	  { throw exception::unsupported (__FILE__, __LINE__); }
	| BINARY  { throw exception::unsupported (__FILE__, __LINE__); }
	;


/* Lock function */

lock:
	LOCK_SYM table_or_tables
	{ throw exception::unsupported (__FILE__, __LINE__); }
	table_lock_list
	{}
	;

table_or_tables:
	TABLE_SYM
	| TABLES;

table_lock_list:
	table_lock
	| table_lock_list ',' table_lock;

table_lock:
	table_ident opt_table_alias lock_option
	{ throw exception::unsupported (__FILE__, __LINE__); }
        ;

lock_option:
	READ_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| WRITE_SYM     { throw exception::unsupported (__FILE__, __LINE__); }
	| LOW_PRIORITY WRITE_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| READ_SYM LOCAL_SYM { throw exception::unsupported (__FILE__, __LINE__); }
        ;

unlock:
	UNLOCK_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
	table_or_tables
	{}
        ;


/*
** Handler: direct access to ISAM functions
*/

handler:
	HANDLER_SYM table_ident OPEN_SYM opt_table_alias
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| HANDLER_SYM table_ident_nodb CLOSE_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| HANDLER_SYM table_ident_nodb READ_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
        handler_read_or_scan where_clause opt_limit_clause {}
        ;

handler_read_or_scan:
	handler_scan_function         { throw exception::unsupported (__FILE__, __LINE__); }
        | ident handler_rkey_function { throw exception::unsupported (__FILE__, __LINE__); }
        ;

handler_scan_function:
	FIRST_SYM  { throw exception::unsupported (__FILE__, __LINE__); }
	| NEXT_SYM { throw exception::unsupported (__FILE__, __LINE__); }
        ;

handler_rkey_function:
	FIRST_SYM  { throw exception::unsupported (__FILE__, __LINE__); }
	| NEXT_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| PREV_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| LAST_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| handler_rkey_mode
	{ throw exception::unsupported (__FILE__, __LINE__); } '(' values ')' {}
        ;

handler_rkey_mode:
	  EQ     { throw exception::unsupported (__FILE__, __LINE__); }
	| GE     { throw exception::unsupported (__FILE__, __LINE__); }
	| LE     { throw exception::unsupported (__FILE__, __LINE__); }
	| GT_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| LT     { throw exception::unsupported (__FILE__, __LINE__); }
        ;

/* GRANT / REVOKE */

revoke:
	REVOKE clear_privileges revoke_command
	{}
        ;

revoke_command:
	grant_privileges ON opt_table grant_ident FROM grant_list
	{ throw exception::unsupported (__FILE__, __LINE__); }
        |
        grant_privileges ON FUNCTION_SYM grant_ident FROM grant_list
        { throw exception::unsupported (__FILE__, __LINE__); }
	|
        grant_privileges ON PROCEDURE grant_ident FROM grant_list
        { throw exception::unsupported (__FILE__, __LINE__); }
	|
	ALL opt_privileges ',' GRANT OPTION FROM grant_list
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

grant:
	GRANT clear_privileges grant_command
	{}
        ;

grant_command:
	grant_privileges ON opt_table grant_ident TO_SYM grant_list
	require_clause grant_options
	{ throw exception::unsupported (__FILE__, __LINE__); }
        |
	grant_privileges ON FUNCTION_SYM grant_ident TO_SYM grant_list
	require_clause grant_options
	{ throw exception::unsupported (__FILE__, __LINE__); }
        |
	grant_privileges ON PROCEDURE grant_ident TO_SYM grant_list
	require_clause grant_options
	{ throw exception::unsupported (__FILE__, __LINE__); }
        ;

opt_table:
	/* Empty */
	| TABLE_SYM ;
        
grant_privileges:
	object_privilege_list {}
	| ALL opt_privileges
        { throw exception::unsupported (__FILE__, __LINE__); }
        ;

opt_privileges:
	/* empty */
	| PRIVILEGES
	;

object_privilege_list:
	object_privilege
	| object_privilege_list ',' object_privilege;

object_privilege:
	SELECT_SYM	{ throw exception::unsupported (__FILE__, __LINE__); } opt_column_list {}
	| INSERT	{ throw exception::unsupported (__FILE__, __LINE__); } opt_column_list {}
	| UPDATE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); } opt_column_list {}
	| REFERENCES	{ throw exception::unsupported (__FILE__, __LINE__); } opt_column_list {}
	| DELETE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| USAGE		{}
	| INDEX_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| ALTER		{ throw exception::unsupported (__FILE__, __LINE__); }
	| CREATE	{ throw exception::unsupported (__FILE__, __LINE__); }
	| DROP		{ throw exception::unsupported (__FILE__, __LINE__); }
	| EXECUTE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| RELOAD	{ throw exception::unsupported (__FILE__, __LINE__); }
	| SHUTDOWN	{ throw exception::unsupported (__FILE__, __LINE__); }
	| PROCESS	{ throw exception::unsupported (__FILE__, __LINE__); }
	| FILE_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| GRANT OPTION  { throw exception::unsupported (__FILE__, __LINE__); }
	| SHOW DATABASES { throw exception::unsupported (__FILE__, __LINE__); }
	| SUPER_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| CREATE TEMPORARY TABLES { throw exception::unsupported (__FILE__, __LINE__); }
	| LOCK_SYM TABLES   { throw exception::unsupported (__FILE__, __LINE__); }
	| REPLICATION SLAVE  { throw exception::unsupported (__FILE__, __LINE__); }
	| REPLICATION CLIENT_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| CREATE VIEW_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| SHOW VIEW_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| CREATE ROUTINE_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| ALTER ROUTINE_SYM { throw exception::unsupported (__FILE__, __LINE__); }
	| CREATE USER { throw exception::unsupported (__FILE__, __LINE__); }
	;


opt_and:
	/* empty */	{}
	| AND_SYM	{}
	;

require_list:
	 require_list_element opt_and require_list
	 | require_list_element
	 ;

require_list_element:
	SUBJECT_SYM TEXT_STRING
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| ISSUER_SYM TEXT_STRING
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| CIPHER_SYM TEXT_STRING
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

grant_ident:
	'*'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| ident '.' '*'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| '*' '.' '*'
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| table_ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
          ;


user_list:
	user  { throw exception::unsupported (__FILE__, __LINE__); }
	| user_list ',' user
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;


grant_list:
	grant_user  { throw exception::unsupported (__FILE__, __LINE__); }
	| grant_list ',' grant_user
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;


grant_user:
	user IDENTIFIED_SYM BY TEXT_STRING
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| user IDENTIFIED_SYM BY PASSWORD TEXT_STRING
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| user
	  { throw exception::unsupported (__FILE__, __LINE__); }
        ;


opt_column_list:
	/* empty */
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| '(' column_list ')';

column_list:
	column_list ',' column_list_id
	| column_list_id;

column_list_id:
	ident
	{ throw exception::unsupported (__FILE__, __LINE__); }
        ;


require_clause: /* empty */
        | REQUIRE_SYM require_list
          { throw exception::unsupported (__FILE__, __LINE__); }
        | REQUIRE_SYM SSL_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
        | REQUIRE_SYM X509_SYM
          { throw exception::unsupported (__FILE__, __LINE__); }
	| REQUIRE_SYM NONE_SYM
	  { throw exception::unsupported (__FILE__, __LINE__); }
          ;

grant_options:
	/* empty */ {}
	| WITH grant_option_list;

grant_option_list:
	grant_option_list grant_option {}
	| grant_option {}
        ;

grant_option:
	GRANT OPTION { throw exception::unsupported (__FILE__, __LINE__); }
        | MAX_QUERIES_PER_HOUR ulong_num
        { throw exception::unsupported (__FILE__, __LINE__); }
        | MAX_UPDATES_PER_HOUR ulong_num
        { throw exception::unsupported (__FILE__, __LINE__); }
        | MAX_CONNECTIONS_PER_HOUR ulong_num
        { throw exception::unsupported (__FILE__, __LINE__); }
        | MAX_USER_CONNECTIONS_SYM ulong_num
        { throw exception::unsupported (__FILE__, __LINE__); }
        ;

begin:
	BEGIN_SYM  
        { throw exception::unsupported (__FILE__, __LINE__); }
        opt_work {}
	;

opt_work:
	/* empty */ {}
	| WORK_SYM  {}
        ;

opt_chain:
	/* empty */ { throw exception::unsupported (__FILE__, __LINE__); }
	| AND_SYM NO_SYM CHAIN_SYM	{ throw exception::unsupported (__FILE__, __LINE__); }
	| AND_SYM CHAIN_SYM		{ throw exception::unsupported (__FILE__, __LINE__); }
	;

opt_release:
	/* empty */ { throw exception::unsupported (__FILE__, __LINE__); }
	| RELEASE_SYM 			{ throw exception::unsupported (__FILE__, __LINE__); }
	| NO_SYM RELEASE_SYM 		{ throw exception::unsupported (__FILE__, __LINE__); }
	;
	
opt_savepoint:
	/* empty */	{}
	| SAVEPOINT_SYM {}
	;

commit:
	COMMIT_SYM opt_work opt_chain opt_release
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

rollback:
	ROLLBACK_SYM opt_work opt_chain opt_release
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| ROLLBACK_SYM opt_work
	  TO_SYM opt_savepoint ident
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

savepoint:
	SAVEPOINT_SYM ident
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

release:
	RELEASE_SYM SAVEPOINT_SYM ident
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;
  
/*
   UNIONS : glue selects together
*/


union_clause:
	/* empty */ {}
	| union_list
	;

union_list:
	UNION_SYM union_option
	{ throw exception::unsupported (__FILE__, __LINE__); }
	select_init
        { throw exception::unsupported (__FILE__, __LINE__); }
	;

union_opt:
	/* Empty */ { $$ = 0; }
	| union_list { throw exception::unsupported (__FILE__, __LINE__); }
	| union_order_or_limit { throw exception::unsupported (__FILE__, __LINE__); }
	;

union_order_or_limit:
	  { throw exception::unsupported (__FILE__, __LINE__); }
	order_or_limit
          { throw exception::unsupported (__FILE__, __LINE__); }
	;

order_or_limit:
	order_clause opt_limit_clause_init
	| limit_clause
	;

union_option:
	/* empty */ { throw exception::unsupported (__FILE__, __LINE__); }
	| DISTINCT  { throw exception::unsupported (__FILE__, __LINE__); }
	| ALL       { throw exception::unsupported (__FILE__, __LINE__); }
        ;

subselect:
        SELECT_SYM subselect_start subselect_init subselect_end
        { throw exception::unsupported (__FILE__, __LINE__); }
        | '(' subselect_start subselect ')'
          union_clause subselect_end 
	  {
	    throw exception::unsupported (__FILE__, __LINE__); 
	  }
	;

subselect_init:
  select_init2
  { throw exception::unsupported (__FILE__, __LINE__); };

subselect_start:
	{ throw exception::unsupported (__FILE__, __LINE__); };

subselect_end:
	{ throw exception::unsupported (__FILE__, __LINE__); };

/**************************************************************************

 CREATE VIEW | TRIGGER | PROCEDURE statements.

**************************************************************************/

view_or_trigger_or_sp:
	definer view_or_trigger_or_sp_tail
	{}
	| view_replace_or_algorithm definer view_tail
	{}
	;

view_or_trigger_or_sp_tail:
	view_tail
	{}
	| trigger_tail
	{}
	| sp_tail
	{}
	;

/**************************************************************************

 DEFINER clause support.

**************************************************************************/

definer:
	/* empty */
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| DEFINER_SYM EQ user
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

/**************************************************************************

 CREATE VIEW statement parts.

**************************************************************************/

view_replace_or_algorithm:
	view_replace
	{}
	| view_replace view_algorithm
	{}
	| view_algorithm
	{}
	;

view_replace:
	OR_SYM REPLACE
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

view_algorithm:
	ALGORITHM_SYM EQ UNDEFINED_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| ALGORITHM_SYM EQ MERGE_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| ALGORITHM_SYM EQ TEMPTABLE_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

view_algorithm_opt:
	/* empty */
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| view_algorithm
	{}
	;

view_suid:
	/* empty */
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| SQL_SYM SECURITY_SYM DEFINER_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| SQL_SYM SECURITY_SYM INVOKER_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

view_tail:
	view_suid VIEW_SYM table_ident
	{ throw exception::unsupported (__FILE__, __LINE__); }
	view_list_opt AS view_select view_check_option
	{}
	;

view_list_opt:
	/* empty */
	{}
	| '(' view_list ')'
	;

view_list:
	ident 
	  { throw exception::unsupported (__FILE__, __LINE__); }
	| view_list ',' ident
	  { throw exception::unsupported (__FILE__, __LINE__); }
	;

view_select:
        { throw exception::unsupported (__FILE__, __LINE__); }        
        view_select_aux
        { throw exception::unsupported (__FILE__, __LINE__); }
        ;

view_select_aux:
	SELECT_SYM select_init2
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| '(' select_paren ')' union_opt
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

view_check_option:
	/* empty */
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| WITH CHECK_SYM OPTION
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| WITH CASCADED CHECK_SYM OPTION
	{ throw exception::unsupported (__FILE__, __LINE__); }
	| WITH LOCAL_SYM CHECK_SYM OPTION
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

/**************************************************************************

 CREATE TRIGGER statement parts.

**************************************************************************/

trigger_tail:
	TRIGGER_SYM sp_name trg_action_time trg_event
	ON table_ident FOR_SYM EACH_SYM ROW_SYM
	{ throw exception::unsupported (__FILE__, __LINE__); }
	sp_proc_stmt
	{ throw exception::unsupported (__FILE__, __LINE__); }
	;

/**************************************************************************

 CREATE FUNCTION | PROCEDURE statements parts.

**************************************************************************/

sp_tail:
	udf_func_type FUNCTION_SYM sp_name
	{ throw exception::unsupported (__FILE__, __LINE__); }
	create_function_tail
	{}
	| PROCEDURE sp_name
	{ throw exception::unsupported (__FILE__, __LINE__); }
        '('
	{ throw exception::unsupported (__FILE__, __LINE__); }
	sp_pdparam_list
	')'
	{ throw exception::unsupported (__FILE__, __LINE__); }
	sp_c_chistics
	{ throw exception::unsupported (__FILE__, __LINE__); }
	sp_proc_stmt
        { throw exception::unsupported (__FILE__, __LINE__); }
	;

/*************************************************************************/

xa: XA_SYM begin_or_start xid opt_join_or_resume     { throw exception::unsupported (__FILE__, __LINE__); }
    | XA_SYM END xid opt_suspend     { throw exception::unsupported (__FILE__, __LINE__); }
    | XA_SYM PREPARE_SYM xid     { throw exception::unsupported (__FILE__, __LINE__); }
    | XA_SYM COMMIT_SYM xid opt_one_phase     { throw exception::unsupported (__FILE__, __LINE__); }
    | XA_SYM ROLLBACK_SYM xid     { throw exception::unsupported (__FILE__, __LINE__); }
    | XA_SYM RECOVER_SYM      { throw exception::unsupported (__FILE__, __LINE__); }
    ;

xid: text_string
     { throw exception::unsupported (__FILE__, __LINE__); }
     | text_string ',' text_string
     { throw exception::unsupported (__FILE__, __LINE__); }
     | text_string ',' text_string ',' ulong_num
     { throw exception::unsupported (__FILE__, __LINE__); }
     ;

begin_or_start:   BEGIN_SYM {}
    |             START_SYM {}
    ;

opt_join_or_resume:
    /* nothing */           { throw exception::unsupported (__FILE__, __LINE__); }
    | JOIN_SYM              { throw exception::unsupported (__FILE__, __LINE__); }
    | RESUME_SYM            { throw exception::unsupported (__FILE__, __LINE__); }
    ;

opt_one_phase:
    /* nothing */           { throw exception::unsupported (__FILE__, __LINE__); }
    | ONE_SYM PHASE_SYM     { throw exception::unsupported (__FILE__, __LINE__); }
    ;

opt_suspend:
    /* nothing */           { throw exception::unsupported (__FILE__, __LINE__); }
    | SUSPEND_SYM           { throw exception::unsupported (__FILE__, __LINE__); }
      opt_migrate
    ;

opt_migrate:
    /* nothing */           {}
    | FOR_SYM MIGRATE_SYM   { throw exception::unsupported (__FILE__, __LINE__); }
    ;


%%

int yylex ()
{

  int pos;
  int pos2;
  
  while (true)
    {
      pos = yypos;
      yylex_val = lex_do ();

      if (yylex_val != NOTHING)
	break;
    }
  pos2 = yypos;
  
  yylloc.first_column = pos;
  yylloc.last_column = pos2;
  
  return yylex_val;
}

}

}

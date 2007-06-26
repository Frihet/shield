/*
	Todo list for shield_package.sql:

	Most of these functions need more testing.

	date_format has unimeplemented commands.
	
*/

SET SERVEROUTPUT ON;

/**
	This type is the implementation of the shield_arb_agg_clob arbitrary aggregation function.
*/
create or replace type shield_agg_arb_clob_t as object (

	str_agg clob,

	static function ODCIAggregateInitialize (sctx in out shield_agg_arb_clob_t) 
		return number,

	member function ODCIAggregateIterate (self  in out shield_agg_arb_clob_t, 
	                                      value in     clob ) 
		return number,

	member function ODCIAggregateTerminate (self         in  shield_agg_arb_clob_t, 
	                                        return_value out clob, 
	                                        flags        in  number)               
		            return number,

	member function ODCIAggregateMerge (self in out shield_agg_arb_clob_t, 
	                                    ctx2 in     shield_agg_arb_clob_t) 
		return number
);
/

create or replace type body shield_agg_arb_clob_t is 

	static function ODCIAggregateInitialize (sctx in out shield_agg_arb_clob_t) 
		return number 
	is 
	begin
		sctx := shield_agg_arb_clob_t(null);
		return ODCIConst.Success;
	end;

	member function ODCIAggregateIterate (self  in out shield_agg_arb_clob_t,
	                                      value in     clob) 
		return number 
	is
	begin
		str_agg := value;
		return ODCIConst.Success;
	end;

	member function ODCIAggregateTerminate (self         in  shield_agg_arb_clob_t, 
	                                        return_value out clob, 
	                                        flags        in  number) 
		return number 
	is
	begin
		return_value := str_agg;
		return ODCIConst.Success;
	end;

	member function ODCIAggregateMerge (self in out shield_agg_arb_clob_t, 
	                                    ctx2 in     shield_agg_arb_clob_t)
		return number 
	is
	begin
		return ODCIConst.Success;
	end;
end;
/

/**
	This type is the implementation of the shield_arb_agg_char arbitrary aggregation function.
*/
create or replace type shield_agg_arb_char_t as object (

	char_agg varchar2 (4000),

	static function ODCIAggregateInitialize(sctx in out shield_agg_arb_char_t) 
		return number,

	member function ODCIAggregateIterate (self  in out shield_agg_arb_char_t, 
	                                      value in     varchar2) 
		return number,

	member function ODCIAggregateTerminate (self         in  shield_agg_arb_char_t, 
	                                        return_value out varchar2, 
	                                        flags        in  number)               
		return number,

	member function ODCIAggregateMerge (self in out shield_agg_arb_char_t, 
	                                    ctx2 in     shield_agg_arb_char_t) 
		return number
);
/

create or replace type body shield_agg_arb_char_t is 

	static function ODCIAggregateInitialize (sctx in out shield_agg_arb_char_t) 
		return number 
	is 
	begin
		sctx := shield_agg_arb_char_t(null);
		return ODCIConst.Success;
	end;

	member function ODCIAggregateIterate (self  in out shield_agg_arb_char_t, 
	                                      value in     varchar2) 
		return number 
	is
	begin
		char_agg := value;
		return ODCIConst.Success;
	end;

	member function ODCIAggregateTerminate (self         in  shield_agg_arb_char_t, 
	                                        return_value out varchar2, 
	                                        flags        in  number) 
		return number 
	is
	begin
		return_value := char_agg;
		return ODCIConst.Success;
	end;

	member function ODCIAggregateMerge (self in out shield_agg_arb_char_t, 
	                                    ctx2 in     shield_agg_arb_char_t) 
		return number 
	is
	begin
		return ODCIConst.Success;
	end;
end;
/


/**
	This type is the implementation of the shield_arb_agg_num arbitrary aggregation function.
*/
create or replace type shield_agg_arb_num_t as object (

	char_agg number,

	static function ODCIAggregateInitialize(sctx in out shield_agg_arb_num_t) 
		return number,

	member function ODCIAggregateIterate (self  in out shield_agg_arb_num_t, 
	                                      value in     number) 
		return number,

	member function ODCIAggregateTerminate (self         in  shield_agg_arb_num_t, 
	                                        return_value out number, 
	                                        flags        in  number)               
		return number,

	member function ODCIAggregateMerge (self in out shield_agg_arb_num_t, 
	                                    ctx2 in     shield_agg_arb_num_t) 
		return number
);
/

create or replace type body shield_agg_arb_num_t is 

	static function ODCIAggregateInitialize (sctx in out shield_agg_arb_num_t) 
		return number 
	is 
	begin
		sctx := shield_agg_arb_num_t(null);
		return ODCIConst.Success;
	end;

	member function ODCIAggregateIterate (self  in out shield_agg_arb_num_t, 
	                                      value in     number) 
		return number 
	is
	begin
		char_agg := value;
		return ODCIConst.Success;
	end;

	member function ODCIAggregateTerminate (self         in  shield_agg_arb_num_t, 
	                                        return_value out number, 
	                                        flags        in  number) 
		return number 
	is
	begin
		return_value := char_agg;
		return ODCIConst.Success;
	end;

	member function ODCIAggregateMerge (self in out shield_agg_arb_num_t, 
	                                    ctx2 in     shield_agg_arb_num_t) 
		return number 
	is
	begin
		return ODCIConst.Success;
	end;
end;
/

/**
	Aggregate function for picking an arbitrary element from a group. This is the clob version.
*/

create or replace function shield_arb_agg_clob (input clob) 
	return clob
	parallel_enable aggregate
	using shield_agg_arb_clob_t;
/

/**
	Aggregate function for picking an arbitrary element from a group. This is the char version.
*/

create or replace function shield_arb_agg_char (input varchar2) 
	return varchar2
	parallel_enable aggregate 
	using shield_agg_arb_char_t;
/

/**
	Aggregate function for picking an arbitrary element from a group. This is the numeric version.
*/

create or replace function shield_arb_agg_num (input number) 
	return number
	parallel_enable aggregate 
	using shield_agg_arb_num_t;
/

/**
	This type is used to pass around lists of table names in shield for functions that construct weird queries based on introspection data.
*/
create or replace type shield_list as varray (10) of varchar2(30);
/

create or replace package shield
is
	last_insert_id integer;

	/**
		This function returns the last auto_increment id.
	*/

	function get_last_insert_id 
		return number;

	/**
		This function takes a string containing a mysql-style date and converts it to an oracle date.
	*/
	function date_parse (str_date varchar2) 
		return date deterministic;

	/**
		This function takes a string containing a mysql-style date and converts it to a number.
	*/
	function date_numeric(str_date varchar2) 
		return number deterministic;

	/**
		This function takes a date and converts it to a string in the same format mysql uses.
	*/
	function date_char (da date) 
		return varchar2 deterministic;

	/**
		This is a workalike to the mysql curdate function.
	*/
	function curdate 
		return varchar2;

	/**
		This is a workalike to the mysql date function.
	*/
	function date_ (str_date varchar2) 
		return varchar2 deterministic;

	/**
		This is a workalike to the mysql now function.
	*/
	function now 
		return varchar2;

	/**
		This is a workalike to the mysql date_format function.
	*/
	function date_format (str_date varchar2, format varchar2) 
		return varchar2 deterministic;

	/**
		This function is a no-op. 

		The set of 'natural' functions are used to make sure that when comparing different field values, clobs are implicitly converted to chars, so that no error is thrown.
	*/
	function natural (in_val number) 
		return number deterministic;
	/**
		This function converts a clob to a char. 

		The set of 'natural' functions are used to make sure that when comparing different field values, clobs are implicitly converted to chars, so that no error is thrown.
	*/
	function natural (in_val clob) 
		return varchar2 deterministic;
	/**
		This function is a no-op. 

		The set of 'natural' functions are used to make sure that when comparing different field values, clobs are implicitly converted to chars, so that no error is thrown.
	*/
	function natural (in_val varchar2) 
		return varchar2 deterministic;

	/**
		Perform null-safe equal comaprison, i.e. mysql <=> operator
	*/
	function null_safe_equal (v1 varchar2, v2 varchar) 
		return char deterministic;

	/**
		Perform null-safe equal comaprison, i.e. mysql <=> operator
	*/
	function null_safe_equal (v1 number, v2 number) 
		return char deterministic;

	/**
		Perform null-safe equal comaprison, i.e. mysql <=> operator
	*/
	function null_safe_equal (v1 clob, v2 clob) 
		return char deterministic;

	function equal (v1 number, v2 number) 
		return char deterministic;

	function equal (v1 clob, v2 clob) 
		return char deterministic;

	function equal (v1 varchar2, v2 varchar2) 
		return char deterministic;

	function equal (v1 varchar2, v2 number) 
		return char deterministic;

	function equal (v1 number, v2 varchar2) 
		return char deterministic;

	/**
		This function creates and performs a select query using _all_ columns from the tables in the \c table_name varray, but without using a wildcard. This might seem useless (and it is) but it is needed since you can not use wildcards in subselect queries, and shield uses subselect to emulate the mysql limit clause.
	*/
	function make_select_limit_query (pre varchar2, post varchar2, table_name shield_list)
		return number;

	function make_select_group_query (pre varchar2, post varchar2, table_name shield_list, agg_name shield_list)
		return number;

end shield;
/

create or replace package body shield
is

        function get_last_insert_id
		return number
        is
	begin
		return last_insert_id;
	end;
	
	/*
		Convert a date or datetime to an oracle date
	*/
	function date_parse (str_date varchar2) 
		return date deterministic
	is 
	begin
		if length (str_date) = 10 then
			return to_date (str_date, 'yyyy-mm-dd');
		else
			return to_date (str_date, 'yyyy-mm-dd hh24:mi:ss');
		end if;
	end;

	/*
		Convert a date or datetime variable to its numeric representation
	*/
	function date_numeric (str_date varchar2) 
		return number deterministic
	is 
		da date;
		str_res varchar2 (32);
	begin
		da := date_parse (str_date);

		if length (str_date) = 10 then
			str_res := to_char (da, 'yyyymmdd');
		else
			str_res := to_char (da, 'yyyymmddhh24miss');
		end if;

		return to_number(str_res);		
	end;

	/*
		Convert an oracle date object to a datetime
	*/
	function date_char (da date) 
		return varchar2 deterministic
	is
	
	begin
		return to_char (da, 'yyyy-mm-dd hh24:mi:ss');
	end;

	/*
		Return the current date
	*/
	function curdate 
		return varchar2
	is 
	begin
		return to_char (current_date, 'yyyy-mm-dd');
	end;

	/*
		Return the date portion of a datetime variable
	*/
	function date_ (str_date varchar2) 
		return varchar2 deterministic
	is 
	begin
		return substr (str_date, 1, 10);
	end;

	function now return varchar2
	is 
		tm varchar (32);
	begin
		return date_char (current_date);
	end;

	function date_format (str_date varchar2, 
	                      format varchar2)
		return varchar2 deterministic
	is 
		pos integer;
		res varchar2 (256);
		chr varchar2 (1);
		da date;
	begin
		pos  := 1;
		res  := '';
		da := date_parse (str_date);

		while pos <= length (format) loop
			chr := substr (format,pos,1);
			if chr = '%' then
				pos := pos + 1;				
				chr := substr (format,pos,1);

				if chr = 'a' then
					res := res || substr (to_char(da, 'Day'), 1, 3);
				elsif chr = 'b' then
					res := res || substr (to_char(da, 'month'), 1, 3);
				elsif chr = 'c' then
					res := res || to_char(da, 'fmmm');
				elsif chr = 'D' then
					res := res || to_char(da, 'ddth');
				elsif chr = 'd' then
					res := res || to_char(da, 'dd');
				elsif chr = 'e' then
					res := res || to_char(da, 'fmdd');
				elsif chr = 'f' then
					res := res || '000000';
				elsif chr = 'H' then
					res := res || to_char(da, 'hh24');
				elsif chr = 'h' then
					res := res || to_char(da, 'hh12');
				elsif chr = 'I' then
					res := res || to_char(da, 'hh12');
				elsif chr = 'i' then
					res := res || to_char(da, 'mi');
				elsif chr = 'j' then
					res := res || to_char(da, '');
				elsif chr = 'k' then
					res := res || to_char(da, 'hh24');
				elsif chr = 'l' then
					res := res || to_char(da, 'hh12');
				elsif chr = 'M' then
					res := res || to_char(da, 'Month');
				elsif chr = 'm' then
					res := res || to_char(da, 'mm');
				elsif chr = 'p' then
					res := res || to_char(da, '');
				elsif chr = 'r' then
					res := res || to_char(da, '');
				elsif chr = 'S' then
					res := res || to_char(da, 'ss');
				elsif chr = 's' then
					res := res || to_char(da, 'ss');
				elsif chr = 'T' then
					res := res || to_char(da, 'hh24:mi:ss');
				elsif chr = 'U' then
					res := res || to_char(da, '');
				elsif chr = 'u' then
					res := res || to_char(da, '');
				elsif chr = 'V' then
					res := res || to_char(da, '');
				elsif chr = 'v' then
					res := res || to_char(da, '');
				elsif chr = 'W' then
					res := res || to_char(da, 'fmDay');
				elsif chr = 'w' then
					res := res || to_char(da, '');
				elsif chr = 'X' then
					res := res || to_char(da, '');
				elsif chr = 'x' then
					res := res || to_char(da, '');
				elsif chr = 'Y' then
					res := res || to_char(da, 'yyyy');
				elsif chr = 'y' then
					res := res || to_char(da, 'yy');
				elsif chr = '%' then
					res := res || '%';
				else
					res := res || chr;
				end if;

			else
				res := res || chr;
			end if;

			pos := pos + 1;
		end loop;
		return res;

	end;

	function natural (in_val number) 
		return number deterministic
	is
	begin
		return in_val;
	end;

	function natural (in_val clob) 
		return varchar2 deterministic
	is
	begin
		return to_char(in_val);
	end;

	function natural (in_val varchar2) 
		return varchar2 deterministic
	is
	begin
		return in_val;
	end;

	function null_safe_equal (v1 varchar2, v2 varchar) 
		return char deterministic
	is
	begin
		if v1 is null then 
			if v2 is null then
				return 'Y';
			end if;
			return 'N';
		else
			if v2 is null then
				return 'N';
			end if;
		end if;

		if v1 = v2 then
			return 'Y';
		end if;
		return 'N';
	end;

	function null_safe_equal (v1 clob, v2 clob) 
		return char deterministic
	is
	begin
		if v1 is null then 
			if v2 is null then
				return 'Y';
			end if;
			return 'N';
		else
			if v2 is null then
				return 'N';
			end if;
		end if;

		if v1 = v2 then
			return 'Y';
		end if;
		return 'N';
	end;

	function null_safe_equal (v1 number, v2 number) 
		return char deterministic
	is
	begin
		if v1 is null then 
			if v2 is null then
				return 'Y';
			end if;
			return 'N';
		else
			if v2 is null then
				return 'N';
			end if;
		end if;

		if v1 = v2 then
			return 'Y';
		end if;
		return 'N';
	end;


	function equal (v1 number, v2 number) 
		return char deterministic
	is
	begin
		if v1 = v2 then
			return 'Y';
		end if;
		return 'N';
	end;


	function equal (v1 varchar2, v2 varchar2) 
		return char deterministic
	is
	begin
		if v1 = v2 then
			return 'Y';
		end if;
		return 'N';
	end;

	function equal (v1 clob, v2 clob) 
		return char deterministic
	is
	begin
		if v1 = v2 then
			return 'Y';
		end if;
		return 'N';
	end;

	function equal (v1 number, v2 varchar2) 
		return char deterministic
	is
	begin
		if to_char (v1) = v2 then
			return 'Y';
		end if;
		return 'N';
	end;

	function equal (v1 varchar2, v2 number) 
		return char deterministic
	is
	begin
		if v1 = to_char (v2) then
			return 'Y';
		end if;
		return 'N';
	end;


	function make_select_limit_query (pre varchar2, post varchar2, table_name shield_list)
		return number
	is
		/*
			Declare vars for cursor traversing over all columns of affected tables
		*/
		type column_cursor_type is ref cursor return user_tab_columns%rowtype;
		column_cursor column_cursor_type;
		current_row user_tab_columns%rowtype;

		/*
			Iterator variables
		*/
		t_name varchar (30);
		i integer;

		/*
			resulting query goes here
		*/
		res varchar2 (4000);
		
		/*
			flag for detecting first lap in loop
		*/
		is_first boolean;
	begin
		res := pre;

		i := table_name.first;
		is_first := true;
		while i is not null loop
			t_name := upper (table_name (i));
			open column_cursor for select * from user_tab_columns where table_name = t_name;
			loop
				fetch column_cursor into current_row;
				exit when column_cursor%notfound;
				if is_first = true then
					is_first := false;
				else
					res := res || ',';
				end if;
				res := res || ' ' || lower (t_name) || '.' || lower (current_row.column_name);
			end loop;

			i := table_name.next (i);
		end loop;

		res := res || ' ' || post;	
		dbms_output.put_line( res);
		return 0;
	end;

	function make_select_group_query (pre varchar2, post varchar2, table_name shield_list, agg_name shield_list)
		return number
	is
		/*
			Declare vars for cursor traversing over all columns of affected tables
		*/
		type column_cursor_type is ref cursor return user_tab_columns%rowtype;
		column_cursor column_cursor_type;
		current_row user_tab_columns%rowtype;

		/*
			Iterator variables
		*/
		t_name varchar (30);
		i integer;

		/*
			resulting query goes here
		*/
		res varchar2 (4000);
		
		/*
			flag for detecting first lap in loop
		*/
		is_first boolean;

		agg_func varchar2 (128);
	begin
		res := pre;

		i := table_name.first;
		is_first := true;
		while i is not null loop
			t_name := upper (table_name (i));
			open column_cursor for select * from user_tab_columns where table_name = t_name;
			loop
				fetch column_cursor into current_row;
				exit when column_cursor%notfound;
				if is_first = true then
					is_first := false;
				else
					res := res || ',';
				end if;

				if current_row.type = 'NUMBER' or current_row.type = 'FLOAT' then
					agg_func := 'shield_arb_agg_num';
				elsif current_row.type = 'CLOB' then
					agg_func := 'arb_agg_func_clob';
				else
					agg_func := 'arb_agg_func_char';
				end if;

				res := res || agg_func || ' (' || lower (t_name) || '.' || lower (current_row.column_name) || ')';
			end loop;

			i := table_name.next (i);
		end loop;


		res := res || ' ' || post;	
		dbms_output.put_line( res);
		return 0;
	end;

end shield;
/


create or replace trigger after_logon
	after logon 
	on database
begin
	execute immediate 'alter session set nls_date_format=''yyyymmddhh24miss''';
end;
/

alter session set nls_date_format='yyyymmddhh24miss';

declare
	res number;
begin
	res := shield.make_select_query ('select', 'from test2 group by test2.id;', shield_list ('test2'));
end;
/

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

create or replace package shield
is
	last_insert_id integer;

	/**
		This function returns the last auto_increment id.
	*/

	function get_last_insert_id 
		return number;

	/**
		This function takes a date and strips the hour, 
		minute and second information from it.
	*/
	function date_ (date_in date) 
		return date deterministic;

	/**
		This is a workalike to the mysql date_format
		function. It performs string formating on a date. Not
		all formating commands are currently supported.
	*/
	function date_format (da date, format varchar2) 
		return varchar2 deterministic;

	/**
		Returns the current date with hour, minute and second information stripped.
	*/

	function curdate 
		return date;

	function to_date_ (date_in varchar2, format varchar2) 
		return date deterministic;

	function to_date_ (date_in number, format varchar2) 
		return date deterministic;

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
	
	function date_ (date_in date) 
		return date deterministic
	is 
	begin
		return to_date (to_char (date_in, 'yyyy-mm-dd'), 'yyyy-mm-dd');
	end;

	function curdate 
		return date
	is 
	begin
		return shield.date_ (current_date);
	end;

	function date_format (da date, 
	                      format varchar2)
		return varchar2 deterministic
	is 
		pos integer;
		res varchar2 (512);
		chr varchar2 (1);
	begin
		pos  := 1;
		res  := '';

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

	function to_date_ (date_in varchar2, format varchar2) 
		return date deterministic
	is 
		date_in2 varchar2 (512);
	begin
		date_in2 := date_in;
		if length(format) = 10 then
			if date_in = '0000-00-00' or date_in = '0' then
				date_in2 := '0001-01-01';
			end if;
		else
			if date_in = '0000-00-00 00:00:00' or date_in = '0' then
				date_in2 := '0001-01-01 00:00:00';
			end if;

		end if;
		
		return to_date (date_in2, format);
	end;


	function to_date_ (date_in number, format varchar2) 
		return date deterministic
	is 
		date_in2 varchar2 (512);
	begin
		date_in2 := date_in;
		if length(format) = 8 then
			if date_in = '00000000' or date_in = '0' then
				date_in2 := '00010101';
			end if;
		else
			if date_in = '00000000000000' or date_in = '0' then
				date_in2 := '00010101000000';
			end if;

		end if;
		
		return to_date (date_in2, format);
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

drop table shield_table_column;

/*

*/
create table shield_table_column
(
       	id number (14, 0),     
	table_name varchar2 (64),     
	column_name varchar2 (64),    
	column_type varchar2 (32)
);

drop sequence shield_table_column_id_s_;

create sequence shield_table_column_id_s_
start with 1
increment by 1
nomaxvalue;

create or replace trigger shield_table_column_id_t_
before insert on shield_table_column
for each row
begin
        select shield_table_column_id_s_.nextval into :new.id from dual;
end;
/


drop index shield_table_column_idx1;

create index shield_table_column_idx1
on shield_table_column (table_name);

drop index shield_table_column_idx2;

create index shield_table_column_idx2
on shield_table_column (table_name, column_name);


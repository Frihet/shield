/*
	Todo list for shield_package.sql:

	Most of these functions need more testing.

	date_format has unimeplemented commands.

*/

/*
SET SERVEROUTPUT ON;
*/

/**
	This type is the implementation of the shield_arb_agg_clob arbitrary aggregation function.
*/

create or replace type shield_agg_arb_date_t as object (

	str_agg date,

	static function ODCIAggregateInitialize (sctx in out shield_agg_arb_date_t) 
		return number,

	member function ODCIAggregateIterate (self  in out shield_agg_arb_date_t, 
	                                      value in     date ) 
		return number,

	member function ODCIAggregateTerminate (self         in  shield_agg_arb_date_t, 
	                                        return_value out date, 
	                                        flags        in  number)               
		            return number,

	member function ODCIAggregateMerge (self in out shield_agg_arb_date_t, 
	                                    ctx2 in     shield_agg_arb_date_t) 
		return number
);
/

create or replace type body shield_agg_arb_date_t is 

	static function ODCIAggregateInitialize (sctx in out shield_agg_arb_date_t) 
		return number 
	is 
	begin
		sctx := shield_agg_arb_date_t(null);
		return ODCIConst.Success;
	end;

	member function ODCIAggregateIterate (self  in out shield_agg_arb_date_t,
	                                      value in     date) 
		return number 
	is
	begin
		str_agg := value;
		return ODCIConst.Success;
	end;

	member function ODCIAggregateTerminate (self         in  shield_agg_arb_date_t, 
	                                        return_value out date, 
	                                        flags        in  number) 
		return number 
	is
	begin
		return_value := str_agg;
		return ODCIConst.Success;
	end;

	member function ODCIAggregateMerge (self in out shield_agg_arb_date_t, 
	                                    ctx2 in     shield_agg_arb_date_t)
		return number 
	is
	begin
		return ODCIConst.Success;
	end;
end;
/

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
	Aggregate function for picking an arbitrary element from a group. This is the date version.
*/

create or replace function shield_arb_agg_date (input date) 
	return date
	parallel_enable aggregate
	using shield_agg_arb_date_t;
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
		This is a workalike to the mysql length function
	*/
	function length_ (str varchar2)
		return varchar2 deterministic;

	/**
		This is a workalike to the mysql char_length function
	*/
	function char_length_ (str varchar2)
		return varchar2 deterministic;

	/**
		This is a workalike to the mysql concat function
	*/
	function concat_ (str1 varchar2, str2 varchar2)
		return varchar2 deterministic;

	/**
		This is a workalike to the mysql lpad function
	*/
	function lpad_ (str varchar2, len number, pad varchar2)
		return varchar2 deterministic;


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

	function to_date_ (date_in date, format varchar2) 
		return date deterministic;

	function to_date_ (date_in varchar2, format varchar2) 
		return date deterministic;

	function to_date_ (date_in number, format varchar2) 
		return date deterministic;

	function equal_null_safe (a varchar2, b varchar2) 
		return number deterministic;

	/**
		In MySQL, there are 'zero dates' of '0000-00-00 00:00:00'. Oracle does not have a zear zero, so Shield uses the date '0001-01-01 01:01:01' as a substitute.
	*/
	function handle_zero_date (date_in varchar2)
		return varchar2 deterministic;

	/**
		Like the regular to_number function, but handles empty strings like mysql
	*/
	function to_number_ (str varchar2) 
		return number deterministic;

	/**
		Like the regular to_number function, but handles empty strings like mysql
	*/
	function to_number_ (str clob) 
		return number deterministic;

	/**
		Numeric null test. Returns 0 or 1 depending on whether the value is null. Needed in the select field spec, since regular sql does not have a boolean type like mysql
	*/
	function is_null (v char)
		return number deterministic;

	/**
		Numeric null test. Returns 0 or 1 depending on whether the value is null. Needed in the select field spec, since regular sql does not have a boolean type like mysql
	*/
	function is_not_null (v char)
		return number deterministic;

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

	function length_ (str varchar2)
		return varchar2 deterministic
	is
	begin
		if str is null then
			return null;
		end if;

		if str = chr (1) then
			return 0;
		end if;

		return lengthb (str);
	end;

        function char_length_ (str varchar2)
                return varchar2 deterministic
        is
        begin
                if str is null then
                        return null;
                end if;

                if str = chr (1) then
                        return 0;
                end if;

                return length (str);
        end;
	

	function concat_ (str1 varchar2, str2 varchar2)
		return varchar2 deterministic
	is
	begin
                if str1 is null then
                        return null;
                end if;

                if str2 is null then
                        return null;
                end if;

                if str1 = chr (1) then
	                if str2 = chr (1) then
				return chr (1);
			end if;
			return str2;	
                end if;

                if str2 = chr (1) then
			return str1;	
		end if;

                return str1 || str2;

	end;


	function lpad_ (str varchar2, len number, pad varchar2)
		return varchar2 deterministic
	is
	begin
                if str = chr (1) then
			return substr (lpad ('a', len+1, pad),1, len);
		end if;
		
		return lpad (str, len, pad);
	end;

	function date_format (da date, 
	                      format varchar2)
		return varchar2 deterministic
	is 
		pos integer;
		res varchar2 (512);
		ch varchar2 (1);
		chn number (10,0);
	begin
		pos  := 1;
		res  := '';

		if format = chr (1) then
			return format;
		end if;

		while pos <= length (format) loop
			ch := substr (format,pos,1);
			chn := ascii (ch);
			if chn = ascii ('%') then
				pos := pos + 1;				
				ch := substr (format,pos,1);
				chn := ascii (ch);

				if chn = ascii ('a') then
					res := res || substr (to_char(da, 'Day'), 1, 3);
				elsif chn = ascii ('b') then
					res := res || substr (to_char(da, 'month'), 1, 3);
				elsif chn = ascii ('c') then
					res := res || to_char(da, 'fmmm');
				elsif chn = ascii ('D') then
					res := res || to_char(da, 'ddth');
				elsif chn = ascii ('d') then
					res := res || to_char(da, 'dd');
				elsif chn = ascii ('e') then
					res := res || to_char(da, 'fmdd');
				elsif chn = ascii ('f') then
					res := res || '000000';
				elsif chn = ascii ('H') then
					res := res || to_char(da, 'hh24');
				elsif chn = ascii ('h') then
					res := res || to_char(da, 'hh12');
				elsif chn = ascii ('I') then
					res := res || to_char(da, 'hh12');
				elsif chn = ascii ('i') then
					res := res || to_char(da, 'mi');
				elsif chn = ascii ('j') then
					res := res || to_char(da, '');
				elsif chn = ascii ('k') then
					res := res || to_char(da, 'hh24');
				elsif chn = ascii ('l') then
					res := res || to_char(da, 'hh12');
				elsif chn = ascii ('M') then
					res := res || to_char(da, 'Month');
				elsif chn = ascii ('m') then
					res := res || to_char(da, 'mm');
				elsif chn = ascii ('p') then
					res := res || to_char(da, '');
				elsif chn = ascii ('r') then
					res := res || to_char(da, '');
				elsif chn = ascii ('S') then
					res := res || to_char(da, 'ss');
				elsif chn = ascii ('s') then
					res := res || to_char(da, 'ss');
				elsif chn = ascii ('T') then
					res := res || to_char(da, 'hh24:mi:ss');
				elsif chn = ascii ('U') then
					res := res || to_char(da, '');
				elsif chn = ascii ('u') then
					res := res || to_char(da, '');
				elsif chn = ascii ('V') then
					res := res || to_char(da, '');
				elsif chn = ascii ('v') then
					res := res || to_char(da, '');
				elsif chn = ascii ('W') then
					res := res || to_char(da, 'fmDay');
				elsif chn = ascii ('w') then
					res := res || to_char(da, '');
				elsif chn = ascii ('X') then
					res := res || to_char(da, '');
				elsif chn = ascii ('x') then
					res := res || to_char(da, '');
				elsif chn = ascii ('Y') then
					res := res || to_char(da, 'yyyy');
				elsif chn = ascii ('y') then
					res := res || to_char(da, 'yy');
				elsif chn = ascii ('%') then
					res := res || '%';
				else
					res := res || ch;
				end if;

			else
				res := res || ch;
			end if;

			pos := pos + 1;
		end loop;
		return res;

	end;

	function to_date_ (date_in date, format varchar2) 
		return date deterministic
	is 
	begin
		return date_in;
	end;

	
	function to_date_ (date_in varchar2, format varchar2) 
		return date deterministic
	is 
		i number (10,0);
		date_in2 varchar2 (64);
		c varchar2 (1);
		fields number (10,0);
	begin
		if date_in is null then
			return null;
		end if;
		if date_in = '0000-00-00' or date_in like '00_00_00' or date_in = '00000000' or date_in = chr (1) or date_in like '0000_00_00_00_00_0_' or  date_in = '00000000000000' or length (date_in) < 3 then
			return to_date ('0001-01-01 01:01:01','yyyy-mm-dd hh24:mi:ss');
		end if;

		if length(date_in) = 8 then
			return to_date (date_in,'yyyymmdd');
		end if;	

		if length(date_in) < 11 then
			return to_date (date_in,'yyyy-mm-dd');
		end if;	

		if length(date_in) = 14 then
			return to_date (date_in,'yyyymmddhh24miss');
		end if;	
 
		i:=1;
		date_in2 := '';
		fields := 1;
		while i < length(date_in) loop
			c := substr (date_in, i, 1);
			if c < '0' or c > '9' then
				c := ' ';
				fields := fields + 1;
			end if; 
			date_in2 := date_in2 || c;

			i := i+1;
		end loop;
			
		if fields < 3 then
			return to_date ('0001-01-01 01:01:01','yyyy-mm-dd hh24:mi:ss');
		end if;

		return to_date (date_in2,'yyyy-mm-dd hh24:mi:ss');
	end;


	function to_date_ (date_in number, format varchar2) 
		return date deterministic
	is 
		date_in2 number;
	begin
		date_in2 := date_in;
		if length(format) = 8 then
			if date_in = 0 then
				date_in2 := '00010101';
			end if;
		else
			if date_in = 0 then
				date_in2 := '00010101010101';
			end if;

		end if;
		
		return to_date (date_in2, format);
	end;

	function equal_null_safe (a varchar2, b varchar2)
		return number deterministic
	is 
	begin
		if a is null then
                     if b is null then
                         return 1;
                     end if;
		     return 0;
		end if;
		if a = b then
			return 1;
		end if;
		return 0;
	end;

	function handle_zero_date (date_in varchar2) 
		return varchar2 deterministic
	is
	begin
		if date_in = '0001-01-01' then
			return '0000-00-00';
		end if;

		if date_in = '0001-01-01 01:01:01' then
			return '0000-00-00 00:00:00';
		end if;
		return date_in;
	end;

	function to_number_ (str varchar2) 
		return number deterministic
	is
	begin
                if str = chr (1) then
			return 0;
		end if;
		
		return to_number (str);
	end;

	function to_number_ (str clob) 
		return number deterministic
	is
	begin
                if str = to_clob (chr (1)) then
			return 0;
		end if;
		
		return to_number (str);
	end;

	function is_null (v char)
		return number deterministic
	is
	begin
		if v is null then
			return 1;
		end if;
		return 0;
	end;

	function is_not_null (v char)
		return number deterministic
	is
	begin
		if v is not null then
			return 1;
		end if;
		return 0;
	end;

	function unix_timestamp (d date)
		return number deterministic
	is
	begin
		return (d - to_date('1970-01-01','yyyy-mm-dd')) * (86400);
	end;

end shield;
/



create or replace trigger after_logon
	after logon 
	on schema
begin
	execute immediate 'alter session set nls_sort=''binary_ci''';
	execute immediate 'alter session set nls_comp=''ansi''';
end;
/

/*
	Create lookup table for name mangling and type lookup on columns
*/

drop table shield_table_column;

create table shield_table_column
(
       	id number (14, 0) not null,     
	table_name varchar2 (64) not null,     
	mangled_table_name varchar2 (32) not null,    
	column_name varchar2 (64) not null,    
	mangled_column_name varchar2 (32) not null,    
	column_type varchar2 (32) not null
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

drop index shield_table_column_idx3;

create index shield_table_column_idx3
on shield_table_column (mangled_table_name);

drop index shield_table_column_idx4;

create index shield_table_column_idx4
on shield_table_column (mangled_table_name, mangled_column_name);


/**
   @file introspection.hh

   A set of functions and classes for obtaining introspection
   information on the tables of a Oracle database.
   
   @package Shield
   @remark Copyright: FreeCode AS
   @author Axel Liljencrantz
   
   This file is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 3.

*/
#ifndef INTROSPECTION_HH
#define INTROSPECTION_HH

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "shield.hh"
#include "logger.hh"

namespace shield
{

  /**
     @namespace shield::introspection

     Shield introspection classes.
  */
  namespace introspection
  {

    using namespace std;

    /**
       Logger object for introspection warnings
    */
    extern logger::logger warning;
    /**
       Logger object for introspection debug output
    */
    extern logger::logger debug;
    
    /**
       An incomplete list of types. These are the types currently used
       by shield. No attempt has been made to cover all oracle
       types. Remember that when adding more types here, the
       column_type class also needs modification.
    */

    /**
       A class representing a data type.
    */
    class column_type
    {
    public:

      /**
	 Construct a new type. 
	 
	 \param t a string describing the type, such as 'varchar2' or 'float'
      */
      column_type (string t);

      /**
	 returns true of this type is of a character string type
      */
      bool is_char () const;

      /**
	 returns true of this type is of a number type
      */
      bool is_number () const;
      
      /**
	 returns true of this type is of a lob (large object) type
      */
      bool is_lob () const;
  
      /**
	 returns true of this type is of a date type
      */
      bool is_date () const;
  
      /**
	 Returns the data_type representing this type
      */
      data_type get_type () const;

    private:
      /**
	 The type of this column
      */
      data_type __type;
    }
    ;

    /**
       A class representing a column in a table. The name and type of
       the column can be optained from objects of this class.
    */
    class column
    {
    public:

      /**
	 select * from foo where is_parent( id, 123, "foo", "id", "parent_id")
      */
      column (string name, string type);

      /**
	 Returns the name of the column
      */
      string get_name () const;

      /**
	 Returns the type of this column
      */
      column_type get_type () const;
  
    private:
    
      /**
	 The name of this column
      */
      string __name;

      /**
	 The column type
      */
      column_type __type;
    }
    ;

    /**
       A class representing a table in the Oracle database. Currently,
       this class can be used to obtain column information. In the
       future, information about indexes will also be available.
    */
    class table
    {
    public:

      /**
	 The column iterator type.
      */
      typedef std::vector<column>::const_iterator column_const_iterator; 

      /**
	 Return the column with the specified name. Throws the
	 not_found exception if no column by tyhat name exists.
      */
      const column &get_column (string name);

      /**
	 Return an iterator of all columns
      */
      column_const_iterator column_begin ();

      /**
	 Return end iterator for all columns
      */
      column_const_iterator column_end ();

      /**
	 Check that this table exists
      */
      bool exists ();
  
    private:

      friend table &get_table (string table_name);

      /**
	 Table name
      */
      string __name;

      /**
	 This flag is true if columns have already been fetched
      */
      bool __is_col_init;

      /**
	 Vector of all columns of the table
      */
      vector<column> __col;
    
      /**
	 Table constructor. It it private, use the gat_table factory
	 function to get an actual table instance.
      */
      table (string name);
    
      /**
	 This function loads all columns of this table into the __col vector
      */
      void load_column ();
    }
    ;

    /**
       Factory function for getting a table with a specific name
    */
    table &get_table (string table_name);
    
    /**
       Factory function for telling the introspection code that the
       specified table has been changed.
    */
    void clear_table (string table_name);
    void clear_tables (void);
  
  }

}

#endif //#ifndef INTROSPECTION_HH


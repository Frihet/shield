#ifndef INTROSPECTION_HH
#define INTROSPECTION_HH

#include <string>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

namespace shield
{

  namespace introspection
  {

    /**
       An incomplete list of types. These are the types currently used
       by shield. No attempt has been made to cover all oracle
       types. Remember that when adding more types here, the
       column_type class also needs modification.
    */
    enum column_type_enum
      {
	CHAR,
	VARCHAR,
	VARCHAR2,
	NUMBER,
	FLOAT,
	CLOB
      }
    ;


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
	 Returns the column_type_enum representing this type
      */
      column_type_enum get_type () const;

    private:
      /**
	 The type of this column
      */
      column_type_enum __type;
    }
    ;

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
	 The column name
      */
      string __name;

      /**
	 The column type
      */
      column_type __type;
    }
    ;

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
  
  }

}

#endif

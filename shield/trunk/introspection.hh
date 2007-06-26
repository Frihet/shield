
#include <string>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

namespace introspection
{

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
 
    column_type (string t);

    bool is_char () const;
    bool is_number () const;
    bool is_lob () const;
  
  private:
    column_type_enum __type;
  }
    ;

  class column
  {
  public:

    string get_name () const;

    column_type get_type () const;
  
  private:
    
    string __name;
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
       Return the column with the specified name
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
  
  private:

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

#include "transform.hh"

namespace shield
{

namespace transform
{

void 
date_function::print (ostream &stream) const
{
  
  stream << " ";
  
  if (get_context () == NUMERIC)
    stream << "shield.date_numeric (";
  
  stream << *__call;


  if (get_context () == NUMERIC)
    stream << ")";  

}

}

}

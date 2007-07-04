#include "transform.hh"

namespace shield
{

namespace transform
{

void 
date_function::print (ostream &stream)
{
  
  stream << " ";
  
  if (get_context () == CONTEXT_NUMBER)
    stream << "shield.date_numeric (";
  
  stream << *__call;


  if (get_context () == CONTEXT_NUMBER)
    stream << ")";  

}

}

}

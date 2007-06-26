#include "shield.hh"

namespace shield
{

void identity::
print (ostream &stream) const
{
    bool printed = false;

    if (get_skip_space ())
      {
	if (__namespace && __namespace->length ())
	  __namespace->set_skip_space (true);
	else if (__table && __table->length ())
	  __table->set_skip_space (true);
	if (__field && __field->length ())
	  __field->set_skip_space (true);
      }

    if (__namespace && __namespace->length ())
      {
	stream << *__namespace;
	printed = true;
      }

    if (__table && __table->length ())
      {
	if (printed)
	  {
	    stream << ".";
	    __table->set_skip_space (true);
	  }
	printed = true;
	stream << *__table;	
      }
    
    if (__field && __field->length ())
      {
	if (printed)
	  {
	    stream << ".";
	    __field->set_skip_space (true);
	  }

	stream << *__field;	
      }

}

}

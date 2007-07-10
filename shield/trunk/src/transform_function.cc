#include "transform.hh"
#include "exception.hh"

namespace shield
{

  namespace transform
  {

    function::
    function (printable *name, data_type type, printable *param)
      : __type (type)
      {
	if (!name || !param)
	  {
	    throw exception::invalid_param (get_node_name () +" constructor called with null param");
	  }
	_set_child (CHILD_NAME, name);
	_set_child (CHILD_PARAM, param);
      }

    data_type function::
    get_context ()
    {
      if (__type == DATA_TYPE_UNDEFINED)
	{
	  printable *node = _get_child (CHILD_PARAM);
	  while(true)
	    {
	      chain *ch = dynamic_cast<chain *> (node);
	      if (!ch)
		break;
	      if (!ch->size ())
		{
		  return __type;
		}
	      node = (*ch)[0];
	    }
	  
	  if (node)
	    return node->get_context ();
	}

      return __type;
    }

    void function::
    _print (ostream &stream)
    {

      if (!get_skip_space ())
	stream << " ";
      
      stream << *get_name () << " (";
      
      get_param ()->set_skip_space (true);
      stream << *get_param () << ")";
    }

  }

}

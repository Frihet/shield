<?php
/**
 * @version $Id: database.php 5903 2006-12-01 02:01:55Z friesengeist $
 * @package Joomla
 * @subpackage Database
 * @copyright Copyright (C) 2005 Open Source Matters. All rights reserved.
 * @license http://www.gnu.org/copyleft/gpl.html GNU/GPL, see LICENSE.php
 * Joomla! is free software. This version may have been modified pursuant
 * to the GNU General Public License, and as distributed it includes or
 * is derivative of works licensed under the GNU General Public License or
 * other free or open source software licenses.
 * See COPYRIGHT.php for copyright notices and details.
 */

// no direct access
defined( '_VALID_MOS' ) or die( 'Restricted access' );


putenv('LD_LIBRARY_PATH=/usr/lib/oracle/xe/app/oracle/product/10.2.0/server/lib');
#putenv('ORACLE_SID=XE');
#putenv('NLS_LANG=AMERICAN_AMERICA.AL32UTF8');

global $shield_resource;
$shield_resource=array ();

global $shield_current_resource;
$shield_current_resource=0;

global $shield_query;
$shield_query=array();

global $shield_password;
global $shield_username;
global $shield_server;
global $shield_verbose;
$shield_verbose=0;

function shield_begin_verbose()
{
  global $shield_verbose;
  $shield_verbose++;
}

function shield_end_verbose()
{
  global $shield_verbose;
  $shield_verbose--;
}

function shield_debug ($msg)
{
  global $shield_verbose;
  if ($shield_verbose > 0)
    {
      echo $msg;
    }
}

function var_describe ($arg)
{
  ob_start();
  var_dump($arg);
  $my_string = ob_get_contents();
  ob_end_clean();
  return $my_string;
}

function get_stack_trace ()
{
  $st = debug_backtrace ();
  $res = "";

  $st = array_slice ($st, 1);
  

  foreach ($st as $i)
    {
      $arg_str = array ();
      foreach ($i['args'] as $arg)
	{
	  if (is_numeric ($arg))
	    {
	      $arg_str[] = "$arg";
	    }
	  else if (is_string ($arg))
	    {
	      $msg = $arg;
	      if (strlen($arg) > 20)
		{
		  $msg = substr ($msg, 0, 15). "...";
		}
	      $arg_str[] = "'".htmlentities($msg)."'";
	    }
	  else if (is_null ($arg))
	    {
	      $arg_str[] = "<null>";
	    }
	  else if (is_array($arg))
	    {
	      $arg_str [] = "<array> (" . count ($arg) . ")";
	    }
	  else if (is_object ($arg))
	    {
	      $arg_str [] = "<object>";
	    }
	}

      $string = "<b>";
      if (isset ($i['class']))
	$string .= $i['class']. "::";

      if (isset ($i['function']))
	$string .= $i['function'];
      
      $string .=" (". implode (", ", $arg_str). ")</b>: ";
      if (isset ($i['file']))
	$string .= basename ($i['file']);
      if (isset ($i['line']))
	$string .= " (".$i['line'].")";
      $string .= "<br>\n";
      $res  .= $string;
    }

  return $res;
}

function shield_get_resource ($r)
{
  global $shield_resource;
  global $shield_current_resource;
  
  if ($r!==null && is_int ($r))
    {
      $shield_current_resource = $r;
    }
  //  echo ("<br>shield_get_resource ($r) = ".($shield_resource[$shield_current_resource-1])."<br>\n");
  return $shield_resource[$shield_current_resource-1];
}

function shield_connect ($server, $username, $password)
{
  global $shield_password;
  global $shield_username;
  global $shield_server;

  global $shield_current_resource;
  global $shield_resource;

  $shield_username = $username;
  $shield_password = $password;
  $shield_server = $server;
  
  $shield_resource[] = false;
  $shield_current_resource = count ($shield_resource);
  return $shield_current_resource;
}

function shield_select_db ($db, $resource=null)
{
  global $shield_password;
  global $shield_username;
  global $shield_server;
  global $shield_resource;
  global $shield_current_resource;

  shield_get_resource ($resource);

  $res = $shield_resource[$shield_current_resource-1] = ocilogon ($shield_username, $shield_password, "$shield_server/$db");
  //  echo "ocilogon ('$shield_username', '$shield_password', '$shield_server/$db') = $res<br>\n";
  return $res;
}

function shield_escape_string ($s, $resource=0)
{
  return mysql_escape_string ($s);
}

function shield_real_escape_string ($s, $resource=0)
{
  return mysql_real_escape_string ($s);
}

function shield_translate_query ($q)
{

  //  echo "Attempting to create socket...<br>\n";

  $socket = socket_create (AF_UNIX, SOCK_STREAM, 0);

  if ($socket === false) 
    {
      echo "socket_create() failed: reason: " . socket_strerror (socket_last_error()) . "<br>\n";
      return false;
    }
  else
    {
      //      echo "OK.<br>\n";
    }
  
  //  echo "Attempting to connect to socket...";
  $result = socket_connect ($socket, "/tmp/shield.socket");
  if ($result === false) 
    {
      echo "socket_connect() failed.\nReason: ($result) " . socket_strerror (socket_last_error ($socket)) . "\n<br>";
      return false;
    } 
  else 
    {
      //  echo "OK.\n<br>";
    }
  
  //  echo "Sending information to socket...";

  $q .= "\0";
  socket_write ($socket, $q, strlen ($q));
  
  //  echo "OK.\n";


  //  echo "Reading response:\n\n";

  $res="";
  while ($out = socket_read ($socket, 2048)) 
    {
      $res .= $out;
    }  

  //echo "Response: $res";

  //echo "Closing socket...";
  socket_close($socket);
  //  echo "OK.\n\n";

  $res =  explode("\0", $res);
  
  while (count($res) && end($res)=="")
    {
      array_pop($res);
    }

  //array_pop($res);

  return $res;
  /*
  $q_escaped = escapeshellarg ($q);

  $output = array ();
  $ret_val=0;
  
  $cmd = "shield -u system -p changeme " . $q_escaped;
  
  $res = array ();
  
  $foo = shell_exec ($cmd);
  
  $res =  explode("\0", $foo);
  
  // echo $cmd . " -> " . count($res) . " queries\n\n";
  
  array_pop($res);
  array_pop($res);
  return $res;
  */

}

//$arr = shield_translate_query ("create table test3 (id int auto_increment)");
//echo "Woot '<pre>" . $arr[4] ."</pre>'<br>\n";

function shield_query ($q, $resource=null)
{
  shield_debug ("shield_query (<pre>$q</pre>, $resource);<br>\n");

  global $shield_query;
  
  $q2 = shield_translate_query ($q);
  $r = shield_get_resource ($resource);
  
  $res = array ();

  if (!count ($q2))
    {
      //      echo "Query $q resulted in null array of queries!!!<br>\n";
    }

  //  echo "bla bla into " . count($q2) . " queries\n\n";
  
  foreach ($q2 as $next_query)
    {

      $pos = strpos ($next_query, "\n/\n");
      if ($pos)
	$next_query = substr( $next_query, 0, $pos );

      
      $stmt->oci_stmt = ociparse ($r, $next_query);
      if (!$stmt->oci_stmt)
	{
	  return false;
	}

      shield_debug ("<pre>{$next_query}</pre><br>\n");

      if (!ociexecute ($stmt->oci_stmt))
	{
	  echo "in:<br>\n";
	  echo "shield_query (<pre>$q</pre>, $resource);<br>\n";
	  echo "-&gt;<br>\n";
	  echo "<pre>{$next_query}</pre><br>\n";

	  echo get_stack_trace ();

	  return false;
	}
      
      $stmt->query = $next_query;
      $stmt->original_query = $q;
      $res[] = $stmt;
      
    }
  //  echo "End of shield_query, return statement array:\n";
  //print_r ($res);
  
  return $res;
}

function shield_errno ($resource =null)
{
  return 0;
}

function shield_error ($resource = null)
{
  return ocierror (shield_get_resource ($resource));
}

function shield_affected_rows ($resource = null)
{
  return ocirowcount (shield_get_resource ($resource));
}

function shield_get_server_info ($resource = null)
{
  /*
	Return some arbitrary version to keep Joomla happy...
  */
  return "4.0.0";
}

function shield_fetch_row ($stmt_arr)
{
  $el = end ($stmt_arr);
  $res = array();
  
  $status = ocifetchinto ($el->oci_stmt, $res, OCI_NUM);

  if ($status)
    {

      $res2 = array ();
      foreach ($res as $i)
	{
	  if (is_object($i))
	    {
	      $res2[] = $i->load ();
	    }
	  else
	    {
	      $res2[] = $i;
	    }
	}

      shield_debug ("shield_fetch_row (<stmt>) = <pre>" . var_describe ($res2) . "</pre><br>\n");

      return $res2;

    }

  shield_debug ("shield_fetch_row (<stmt>)=false<br>\n");
  
  return false;
}

function shield_fetch_assoc ($stmt_arr)
{
  $stmt = end($stmt_arr);

  //  echo "Called shield_fetch_assoc\n";
  //print_r ($stmt_arr);
  
  if (!isset($stmt->arg_list))
    {
      //      echo "Parse argument list<br>";
      $q = $stmt->query;
      
      $arr = explode ("\n", $q);
      
      $arg_list = array();
      foreach ($arr as $i)
	{
	  if ($i == "/*")
	    continue;
	  else if ($i == "*/")
	    {
	      break;
	    }
	  else
	    {
	      //	      echo "Add argument $i<br>";
	      $arg_list[] = $i;
	    }
	}

      $stmt->arg_list = $arg_list;
      //      end($stmt_arr) = $stmt;
    }

  $oci_row = shield_fetch_row($stmt_arr);
  
  if (!$oci_row)
    return false;

  $res = array();
  /*
  if (count ($stmt->arg_list) != count ($oci_row))
    {
      echo "Wrong number of arguments (". count ($oci_row) . ", not " .count ($stmt->arg_list). ") returned from query <pre>";
      print_r ($stmt->original_query);
      echo "</pre>-&gt;<pre>";
      print_r ($stmt->query);
      echo "</pre>";
      return false;
    }
  */
  for ($i=0; $i < count ($stmt->arg_list); $i++)
    {
      if (count ($oci_row) > $i)
	$res[$stmt->arg_list[$i]] = $oci_row[$i];
      else
	$res[$stmt->arg_list[$i]] = null;
    }
  return $res;
}

function shield_free_result ($stmt_arr)
{
  foreach ($stmt_arr as $stmt)
    {
      ocifreecursor ($stmt->oci_stmt);
    }
}
function shield_num_rows ($stmt_arr)
{
  /*
    This is wrong... 
  */
  $el = end ($stmt_arr);

  return ocirowcount ($el->oci_stmt);  
}

function shield_fetch_object ($stmt)
{
  $arr = shield_fetch_assoc ($stmt);

  if (!$arr)
    return false;

  foreach ($arr as $key => $val)
    {
      $res->$key = $val;
    }

  return $res;
    
}

function shield_insert_id ($resource=null)
{
  $r = shield_get_resource ($resource);
  $stmt = ociparse ($r, "select shield.get_last_insert_id from dual");
  
  if (!$stmt)
    return false;

  if (!ociexecute ($stmt))
    { 
      return false;
    }

  $row = array ();
  $status = ocifetchinto ($stmt, $row, OCI_NUM);
  ocifreecursor ($stmt);
  
  if ($status && count($row))
    return $row[0];
  else
    return false;
  
}


/**
 * Database connector class
 * @subpackage Database
 * @package Joomla
 */

class database {
  /** @var string Internal variable to hold the query sql */
  var $_sql			= '';
  /** @var int Internal variable to hold the database error number */
  var $_errorNum		= 0;
  /** @var string Internal variable to hold the database error message */
  var $_errorMsg		= '';
  /** @var string Internal variable to hold the prefix used on all database tables */
  var $_table_prefix	= '';
  /** @var Internal variable to hold the connector resource */
  var $_resource		= '';
  /** @var Internal variable to hold the last query cursor */
  var $_cursor		= null;
  /** @var boolean Debug option */
  var $_debug			= 0;
  /** @var int The limit for the query */
  var $_limit			= 0;
  /** @var int The for offset for the limit */
  var $_offset		= 0;
  /** @var int A counter for the number of queries performed by the object instance */
  var $_ticker		= 0;
  /** @var array A log of queries */
  var $_log			= null;
  /** @var string The null/zero date string */
  var $_nullDate		= '0000-00-00 00:00:00';
  /** @var string Quote for named objects */
  var $_nameQuote		= '`';

  /**
   * Database object constructor
   * @param string Database host
   * @param string Database user name
   * @param string Database user password
   * @param string Database name
   * @param string Common prefix for all tables
   * @param boolean If true and there is an error, go offline
   */
  function database( $host='localhost', $user, $pass, $db='', $table_prefix='', $goOffline=true ) {
    // perform a number of fatality checks, then die gracefully
    if (!function_exists( 'shield_connect' )) {
      $mosSystemError = 1;
      if ($goOffline) {
	$basePath = dirname( __FILE__ );
	include $basePath . '/../configuration.php';
	include $basePath . '/../offline.php';
	exit();
      }
    }
    if (phpversion() < '4.2.0') {
      if (!($this->_resource = @shield_connect( $host, $user, $pass ))) {
	$mosSystemError = 2;
	if ($goOffline) {
	  $basePath = dirname( __FILE__ );
	  include $basePath . '/../configuration.php';
	  include $basePath . '/../offline.php';
	  exit();
	}
      }
    } else {
      if (!($this->_resource = @shield_connect( $host, $user, $pass, true ))) {
	$mosSystemError = 2;
	if ($goOffline) {
	  $basePath = dirname( __FILE__ );
	  include $basePath . '/../configuration.php';
	  include $basePath . '/../offline.php';
	  exit();
	}
      }
    }
    if ($db != '' && !shield_select_db( $db, $this->_resource )) {
      $mosSystemError = 3;
      if ($goOffline) {
	$basePath = dirname( __FILE__ );
	include $basePath . '/../configuration.php';
	include $basePath . '/../offline.php';
	exit();
      }
    }
    $this->_table_prefix = $table_prefix;
    //@shield_query("SET NAMES 'utf8'", $this->_resource);
    $this->_ticker = 0;
    $this->_log = array();
  }
  /**
   * @param int
   */
  function debug( $level ) {
    $this->_debug = intval( $level );
  }

  function log( $sql ) {
    $tm=explode(" ",microtime());
    $ssstr = substr($tm[0],2,4);
    $date = date( "Y-m-d_H:i:s" ).":".$ssstr;
    $axel_evil_log_variable = fopen( "/tmp/axel_evil_" . $date . ".log", "a" );
    fwrite( $axel_evil_log_variable, $sql . "\n\n" );
	
    fclose( $axel_evil_log_variable );

  }

  /**
   * @return int The error number for the most recent query
   */
  function getErrorNum() {
    return $this->_errorNum;
  }
  /**
   * @return string The error message for the most recent query
   */
  function getErrorMsg() {
    return str_replace( array( "\n", "'" ), array( '\n', "\'" ), $this->_errorMsg );
  }
  /**
   * Get a database escaped string
   * @return string
   */
  function getEscaped( $text ) {
    /*
     * Use the appropriate escape string depending upon which version of php
     * you are running
     */
    if (version_compare(phpversion(), '4.3.0', '<')) {
      $string = shield_escape_string($text);
    } else 	{
      $string = shield_real_escape_string($text, $this->_resource);
    }

    return $string;
  }
  /**
   * Get a quoted database escaped string
   * @return string
   */
  function Quote( $text ) {
    return '\'' . $this->getEscaped( $text ) . '\'';
  }
  /**
   * Quote an identifier name (field, table, etc)
   * @param string The name
   * @return string The quoted name
   */
  function NameQuote( $s ) {
    $q = $this->_nameQuote;
    if (strlen( $q ) == 1) {
      return $q . $s . $q;
    } else {
      return $q{0} . $s . $q{1};
    }
  }
  /**
   * @return string The database prefix
   */
  function getPrefix() {
    return $this->_table_prefix;
  }
  /**
   * @return string Quoted null/zero date string
   */
  function getNullDate() {
    return $this->_nullDate;
  }
  /**
   * Sets the SQL query string for later execution.
   *
   * This function replaces a string identifier <var>$prefix</var> with the
   * string held is the <var>_table_prefix</var> class variable.
   *
   * @param string The SQL query
   * @param string The offset to start selection
   * @param string The number of results to return
   * @param string The common table prefix
   */
  function setQuery( $sql, $offset = 0, $limit = 0, $prefix='#__' ) {
    $this->_sql = $this->replacePrefix( $sql, $prefix );
    $this->_limit = intval( $limit );
    $this->_offset = intval( $offset );
  }

  /**
   * This function replaces a string identifier <var>$prefix</var> with the
   * string held is the <var>_table_prefix</var> class variable.
   *
   * @param string The SQL query
   * @param string The common table prefix
   * @author thede, David McKinnis
   */
  function replacePrefix( $sql, $prefix='#__' ) {
    $sql = trim( $sql );

    $escaped = false;
    $quoteChar = '';

    $n = strlen( $sql );

    $startPos = 0;
    $literal = '';
    while ($startPos < $n) {
      $ip = strpos($sql, $prefix, $startPos);
      if ($ip === false) {
	break;
      }

      $j = strpos( $sql, "'", $startPos );
      $k = strpos( $sql, '"', $startPos );
      if (($k !== FALSE) && (($k < $j) || ($j === FALSE))) {
	$quoteChar	= '"';
	$j			= $k;
      } else {
	$quoteChar	= "'";
      }

      if ($j === false) {
	$j = $n;
      }

      $literal .= str_replace( $prefix, $this->_table_prefix, substr( $sql, $startPos, $j - $startPos ) );
      $startPos = $j;

      $j = $startPos + 1;

      if ($j >= $n) {
	break;
      }

      // quote comes first, find end of quote
      while (TRUE) {
	$k = strpos( $sql, $quoteChar, $j );
	$escaped = false;
	if ($k === false) {
	  break;
	}
	$l = $k - 1;
	while ($l >= 0 && $sql{$l} == '\\') {
	  $l--;
	  $escaped = !$escaped;
	}
	if ($escaped) {
	  $j	= $k+1;
	  continue;
	}
	break;
      }
      if ($k === FALSE) {
	// error in the query - no end quote; ignore it
	break;
      }
      $literal .= substr( $sql, $startPos, $k - $startPos + 1 );
      $startPos = $k+1;
    }
    if ($startPos < $n) {
      $literal .= substr( $sql, $startPos, $n - $startPos );
    }
    return $literal;
  }
  /**
   * @return string The current value of the internal SQL vairable
   */
  function getQuery() {
    return "<pre>" . htmlspecialchars( $this->_sql ) . "</pre>";
  }
  /**
   * Execute the query
   * @return mixed A database resource if successful, FALSE if not.
   */
  function query() {
    global $mosConfig_debug;
    if ($this->_limit > 0 && $this->_offset == 0) {
      $this->_sql .= "\nLIMIT $this->_limit";
    } else if ($this->_limit > 0 || $this->_offset > 0) {
      $this->_sql .= "\nLIMIT $this->_offset, $this->_limit";
    }
    if ($this->_debug) {
      $this->_ticker++;
      $this->_log[] = $this->_sql;
    }
    $this->_errorNum = 0;
    $this->_errorMsg = '';
    $this->log( $this->_sql );
    $this->_cursor = shield_query( $this->_sql, $this->_resource );
    if (!$this->_cursor) {
      $this->_errorNum = shield_errno( $this->_resource );
      $this->_errorMsg = shield_error( $this->_resource )." SQL=$this->_sql";
      if ($this->_debug) {
	trigger_error( shield_error( $this->_resource ), E_USER_NOTICE );
	//echo "<pre>" . $this->_sql . "</pre>\n";
	if (function_exists( 'debug_backtrace' )) {
	  foreach( debug_backtrace() as $back) {
	    if (@$back['file']) {
	      echo '<br />'.$back['file'].':'.$back['line'];
	    }
	  }
	}
      }
      return false;
    }
    return $this->_cursor;
  }

  /**
   * @return int The number of affected rows in the previous operation
   */
  function getAffectedRows() {
    return shield_affected_rows( $this->_resource );
  }

  function query_batch( $abort_on_error=true, $p_transaction_safe = false) {
    $this->_errorNum = 0;
    $this->_errorMsg = '';
    if ($p_transaction_safe) {
      $si = shield_get_server_info( $this->_resource );
      preg_match_all( "/(\d+)\.(\d+)\.(\d+)/i", $si, $m );
      if ($m[1] >= 4) {
	$this->_sql = 'START TRANSACTION;' . $this->_sql . '; COMMIT;';
      } else if ($m[2] >= 23 && $m[3] >= 19) {
	$this->_sql = 'BEGIN WORK;' . $this->_sql . '; COMMIT;';
      } else if ($m[2] >= 23 && $m[3] >= 17) {
	$this->_sql = 'BEGIN;' . $this->_sql . '; COMMIT;';
      }
    }
    $query_split = preg_split ("/[;]+/", $this->_sql);
    $error = 0;
    foreach ($query_split as $command_line) {
      $command_line = trim( $command_line );
      if ($command_line != '') {
	$this->log( $command_line );
	$this->_cursor = shield_query( $command_line, $this->_resource );
	if (!$this->_cursor) {
	  $error = 1;
	  $this->_errorNum .= shield_errno( $this->_resource ) . ' ';
	  $this->_errorMsg .= shield_error( $this->_resource )." SQL=$command_line <br />";
	  if ($abort_on_error) {
	    return $this->_cursor;
	  }
	}
      }
    }
    return $error ? false : true;
  }

  /**
   * Diagnostic function
   */
  function explain() {
    $temp = $this->_sql;
    $this->_sql = "EXPLAIN $this->_sql";
    $this->query();

    if (!($cur = $this->query())) {
      return null;
    }
    $first = true;

    $buf = "<table cellspacing=\"1\" cellpadding=\"2\" border=\"0\" bgcolor=\"#000000\" align=\"center\">";
    $buf .= $this->getQuery();
    while ($row = shield_fetch_assoc( $cur )) {
      if ($first) {
	$buf .= "<tr>";
	foreach ($row as $k=>$v) {
	  $buf .= "<th bgcolor=\"#ffffff\">$k</th>";
	}
	$buf .= "</tr>";
	$first = false;
      }
      $buf .= "<tr>";
      foreach ($row as $k=>$v) {
	$buf .= "<td bgcolor=\"#ffffff\">$v</td>";
      }
      $buf .= "</tr>";
    }
    $buf .= "</table><br />&nbsp;";
    shield_free_result( $cur );

    $this->_sql = $temp;

    return "<div style=\"background-color:#FFFFCC\" align=\"left\">$buf</div>";
  }
  /**
   * @return int The number of rows returned from the most recent query.
   */
  function getNumRows( $cur=null ) {
    return shield_num_rows( $cur ? $cur : $this->_cursor );
  }

  /**
   * This method loads the first field of the first row returned by the query.
   *
   * @return The value returned in the query or null if the query failed.
   */
  function loadResult() {
    if (!($cur = $this->query())) {
      return null;
    }
    $ret = null;
    if ($row = shield_fetch_row( $cur )) {
      $ret = $row[0];
    }
    shield_free_result( $cur );
    return $ret;
  }
  /**
   * Load an array of single field results into an array
   */
  function loadResultArray($numinarray = 0) {
    if (!($cur = $this->query())) {
      return null;
    }
    $array = array();
    while ($row = shield_fetch_row( $cur )) {
      $array[] = $row[$numinarray];
    }
    shield_free_result( $cur );
    return $array;
  }
  /**
   * Load a assoc list of database rows
   * @param string The field name of a primary key
   * @return array If <var>key</var> is empty as sequential list of returned records.
   */
  function loadAssocList( $key='' ) {
    if (!($cur = $this->query())) {
      return null;
    }
    $array = array();
    while ($row = shield_fetch_assoc( $cur )) {
      if ($key) {
	$array[$row[$key]] = $row;
      } else {
	$array[] = $row;
      }
    }
    shield_free_result( $cur );
    return $array;
  }
  /**
   * This global function loads the first row of a query into an object
   *
   * If an object is passed to this function, the returned row is bound to the existing elements of <var>object</var>.
   * If <var>object</var> has a value of null, then all of the returned query fields returned in the object.
   * @param string The SQL query
   * @param object The address of variable
   */
  function loadObject( &$object ) {
    if ($object != null) {
      if (!($cur = $this->query())) {
	return false;
      }
      if ($array = shield_fetch_assoc( $cur )) {
	shield_free_result( $cur );
	mosBindArrayToObject( $array, $object, null, null, false );
	return true;
      } else {
	return false;
      }
    } else {
      if ($cur = $this->query()) {
	if ($object = shield_fetch_object( $cur )) {
	  shield_free_result( $cur );
	  return true;
	} else {
	  $object = null;
	  return false;
	}
      } else {
	return false;
      }
    }
  }
  /**
   * Load a list of database objects
   * @param string The field name of a primary key
   * @return array If <var>key</var> is empty as sequential list of returned records.
   * If <var>key</var> is not empty then the returned array is indexed by the value
   * the database key.  Returns <var>null</var> if the query fails.
   */
  function loadObjectList( $key='' ) {
    if (!($cur = $this->query())) {
      return null;
    }
    $array = array();
    while ($row = shield_fetch_object( $cur )) {
      if ($key) {
	$array[$row->$key] = $row;
      } else {
	$array[] = $row;
      }
    }
    shield_free_result( $cur );
    return $array;
  }
  /**
   * @return The first row of the query.
   */
  function loadRow() {
    if (!($cur = $this->query())) {
      return null;
    }
    $ret = null;
    if ($row = shield_fetch_row( $cur )) {
      $ret = $row;
    }
    shield_free_result( $cur );
    return $ret;
  }
  /**
   * Load a list of database rows (numeric column indexing)
   * @param int Value of the primary key
   * @return array If <var>key</var> is empty as sequential list of returned records.
   * If <var>key</var> is not empty then the returned array is indexed by the value
   * the database key.  Returns <var>null</var> if the query fails.
   */
  function loadRowList( $key=null ) {
    if (!($cur = $this->query())) {
      return null;
    }
    $array = array();
    while ($row = shield_fetch_row( $cur )) {
      if ( !is_null( $key ) ) {
	$array[$row[$key]] = $row;
      } else {
	$array[] = $row;
      }
    }
    shield_free_result( $cur );
    return $array;
  }
  /**
   * Document::db_insertObject()
   *
   * { Description }
   *
   * @param string $table This is expected to be a valid (and safe!) table name
   * @param [type] $keyName
   * @param [type] $verbose
   */
  function insertObject( $table, &$object, $keyName = NULL, $verbose=false ) {
    $fmtsql = "INSERT INTO $table ( %s ) VALUES ( %s ) ";
    $fields = array();
    foreach (get_object_vars( $object ) as $k => $v) {
      if (is_array($v) or is_object($v) or $v === NULL) {
	continue;
      }
      if ($k[0] == '_') { // internal field
	continue;
      }
      $fields[] = $this->NameQuote( $k );
      $values[] = $this->Quote( $v );
    }
    $this->setQuery( sprintf( $fmtsql, implode( ",", $fields ) ,  implode( ",", $values ) ) );
    ($verbose) && print "$sql<br />\n";
    if (!$this->query()) {
      return false;
    }
    $id = shield_insert_id( $this->_resource );
    ($verbose) && print "id=[$id]<br />\n";
    if ($keyName && $id) {
      $object->$keyName = $id;
    }
    return true;
  }

  /**
   * Document::db_updateObject()
   *
   * { Description }
   *
   * @param string $table This is expected to be a valid (and safe!) table name
   * @param [type] $updateNulls
   */
  function updateObject( $table, &$object, $keyName, $updateNulls=true ) {
    $fmtsql = "UPDATE $table SET %s WHERE %s";
    $tmp = array();
    foreach (get_object_vars( $object ) as $k => $v) {
      if( is_array($v) or is_object($v) or $k[0] == '_' ) { // internal or NA field
	continue;
      }
      if( $k == $keyName ) { // PK not to be updated
	$where = $keyName . '=' . $this->Quote( $v );
	continue;
      }
      if ($v === NULL && !$updateNulls) {
	continue;
      }
      if( $v == '' ) {
	$val = "''";
      } else {
	$val = $this->Quote( $v );
      }
      $tmp[] = $this->NameQuote( $k ) . '=' . $val;
    }
    $this->setQuery( sprintf( $fmtsql, implode( ",", $tmp ) , $where ) );
    return $this->query();
  }

  /**
   * @param boolean If TRUE, displays the last SQL statement sent to the database
   * @return string A standised error message
   */
  function stderr( $showSQL = false ) {
    return "DB function failed with error number $this->_errorNum"
      ."<br /><font color=\"red\">$this->_errorMsg</font>"
      .($showSQL ? "<br />SQL = <pre>$this->_sql</pre>" : '');
  }

  function insertid() {
    return shield_insert_id( $this->_resource );
  }

  function getVersion() {
    return shield_get_server_info( $this->_resource );
  }

  /**
   * @return array A list of all the tables in the database
   */
  function getTableList() {
    $this->setQuery( 'SHOW TABLES' );
    return $this->loadResultArray();
  }
  /**
   * @param array A list of valid (and safe!) table names
   * @return array A list the create SQL for the tables
   */
  function getTableCreate( $tables ) {
    $result = array();

    foreach ($tables as $tblval) {
      $this->setQuery( 'SHOW CREATE table ' . $this->getEscaped( $tblval ) );
      $rows = $this->loadRowList();
      foreach ($rows as $row) {
	$result[$tblval] = $row[1];
      }
    }

    return $result;
  }
  /**
   * @param array A list of valid (and safe!) table names
   * @return array An array of fields by table
   */
  function getTableFields( $tables ) {
    $result = array();

    foreach ($tables as $tblval) {
      $this->setQuery( 'SHOW FIELDS FROM ' . $tblval );
      $fields = $this->loadObjectList();
      foreach ($fields as $field) {
	$result[$tblval][$field->Field] = preg_replace("/[(0-9)]/",'', $field->Type );
      }
    }

    return $result;
  }

  /**
   * Fudge method for ADOdb compatibility
   */
  function GenID( $foo1=null, $foo2=null ) {
    return '0';
  }
}

/**
 * mosDBTable Abstract Class.
 * @abstract
 * @package Joomla
 * @subpackage Database
 *
 * Parent classes to all database derived objects.  Customisation will generally
 * not involve tampering with this object.
 * @author Andrew Eddie <eddieajau@users.sourceforge.net
 */
class mosDBTable {
  /** @var string Name of the table in the db schema relating to child class */
  var $_tbl 		= '';
  /** @var string Name of the primary key field in the table */
  var $_tbl_key 	= '';
  /** @var string Error message */
  var $_error 	= '';
  /** @var mosDatabase Database connector */
  var $_db 		= null;

  /**
   *	Object constructor to set table and key field
   *
   *	Can be overloaded/supplemented by the child class
   *	@param string $table name of the table in the db schema relating to child class
   *	@param string $key name of the primary key field in the table
   */
  function mosDBTable( $table, $key, &$db ) {
    $this->_tbl = $table;
    $this->_tbl_key = $key;
    $this->_db =& $db;
  }

  /**
   * Returns an array of public properties
   * @return array
   */
  function getPublicProperties() {
    static $cache = null;
    if (is_null( $cache )) {
      $cache = array();
      foreach (get_class_vars( get_class( $this ) ) as $key=>$val) {
	if (substr( $key, 0, 1 ) != '_') {
	  $cache[] = $key;
	}
      }
    }
    return $cache;
  }
  /**
   * Filters public properties
   * @access protected
   * @param array List of fields to ignore
   */
  function filter( $ignoreList=null ) {
    $ignore = is_array( $ignoreList );

    $iFilter = new InputFilter();
    foreach ($this->getPublicProperties() as $k) {
      if ($ignore && in_array( $k, $ignoreList ) ) {
	continue;
      }
      $this->$k = $iFilter->process( $this->$k );
    }
  }
  /**
   *	@return string Returns the error message
   */
  function getError() {
    return $this->_error;
  }
  /**
   * Gets the value of the class variable
   * @param string The name of the class variable
   * @return mixed The value of the class var (or null if no var of that name exists)
   */
  function get( $_property ) {
    if(isset( $this->$_property )) {
      return $this->$_property;
    } else {
      return null;
    }
  }

  /**
   * Set the value of the class variable
   * @param string The name of the class variable
   * @param mixed The value to assign to the variable
   */
  function set( $_property, $_value ) {
    $this->$_property = $_value;
  }

  /**
   * Resets public properties
   * @param mixed The value to set all properties to, default is null
   */
  function reset( $value=null ) {
    $keys = $this->getPublicProperties();
    foreach ($keys as $k) {
      $this->$k = $value;
    }
  }
  /**
   *	binds a named array/hash to this object
   *
   *	can be overloaded/supplemented by the child class
   *	@param array $hash named array
   *	@return null|string	null is operation was satisfactory, otherwise returns an error
   */
  function bind( $array, $ignore='' ) {
    if (!is_array( $array )) {
      $this->_error = strtolower(get_class( $this ))."::bind failed.";
      return false;
    } else {
      return mosBindArrayToObject( $array, $this, $ignore );
    }
  }

  /**
   *	binds an array/hash to this object
   *	@param int $oid optional argument, if not specifed then the value of current key is used
   *	@return any result from the database operation
   */
  function load_internal( $oid=null ) {

    $k = $this->_tbl_key;

    if ($oid !== null) {
      $this->$k = $oid;
    }

    $oid = $this->$k;

    if ($oid === null) {
      return false;
    }
    //Note: Prior to PHP 4.2.0, Uninitialized class variables will not be reported by get_class_vars().
    /*
		$class_vars = $this->getPublicProperties();
		foreach ($class_vars as $name => $value) {
			if ($name != $k) {
				$this->$name = $value;
			}
		}
    */
    $class_vars = get_class_vars(get_class($this));
    foreach ($class_vars as $name => $value) {
      if (($name != $k) and ($name != "_db") and ($name != "_tbl") and ($name != "_tbl_key")) {
	$this->$name = $value;
      }
    }

    $this->reset();

    $query = "SELECT *"
      . "\n FROM $this->_tbl"
      . "\n WHERE $this->_tbl_key = " . $this->_db->Quote( $oid )
      ;
    $this->_db->setQuery( $query );

    return $this->_db->loadObject( $this );
  }

  function load( $oid=null ) {
    //shield_begin_verbose ();
    return $this->load_internal ($oid);
    //shield_end_verbose ();
  }

  /**
   *	generic check method
   *
   *	can be overloaded/supplemented by the child class
   *	@return boolean True if the object is ok
   */
  function check() {
    return true;
  }

  /**
   * Inserts a new row if id is zero or updates an existing row in the database table
   *
   * Can be overloaded/supplemented by the child class
   * @param boolean If false, null object variables are not updated
   * @return null|string null if successful otherwise returns and error message
   */
  function store( $updateNulls=false ) {
    $k = $this->_tbl_key;

    if ($this->$k != 0) {
      $ret = $this->_db->updateObject($this->_tbl, $this, $this->_tbl_key, $updateNulls);
    } else {
      $ret = $this->_db->insertObject($this->_tbl, $this, $this->_tbl_key);
    }

    if (!$ret) {
      $this->_error = strtolower(get_class($this))."::store failed <br />" . $this->_db->getErrorMsg();
      return false;
    } else {
      return true;
    }
  }
  /**
   * @param string $where This is expected to be a valid (and safe!) SQL expression
   */
  function move( $dirn, $where='' ) {
    $k = $this->_tbl_key;

    $sql = "SELECT $this->_tbl_key, ordering FROM $this->_tbl";

    if ($dirn < 0) {
      $sql .= "\n WHERE ordering < " . (int) $this->ordering;
      $sql .= ($where ? "\n	AND $where" : '');
      $sql .= "\n ORDER BY ordering DESC";
      $sql .= "\n LIMIT 1";
    } else if ($dirn > 0) {
      $sql .= "\n WHERE ordering > " . (int) $this->ordering;
      $sql .= ($where ? "\n	AND $where" : '');
      $sql .= "\n ORDER BY ordering";
      $sql .= "\n LIMIT 1";
    } else {
      $sql .= "\nWHERE ordering = " . (int) $this->ordering;
      $sql .= ($where ? "\n AND $where" : '');
      $sql .= "\n ORDER BY ordering";
      $sql .= "\n LIMIT 1";
    }

    $this->_db->setQuery( $sql );
    //echo 'A: ' . $this->_db->getQuery();


    $row = null;
    if ($this->_db->loadObject( $row )) {
      $query = "UPDATE $this->_tbl"
	. "\n SET ordering = " . (int) $row->ordering
	. "\n WHERE $this->_tbl_key = " . $this->_db->Quote( $this->$k )
	;
      $this->_db->setQuery( $query );

      if (!$this->_db->query()) {
	$err = $this->_db->getErrorMsg();
	die( $err );
      }
      //echo 'B: ' . $this->_db->getQuery();

      $query = "UPDATE $this->_tbl"
	. "\n SET ordering = " . (int) $this->ordering
	. "\n WHERE $this->_tbl_key = " . $this->_db->Quote( $row->$k )
	;
      $this->_db->setQuery( $query );
      //echo 'C: ' . $this->_db->getQuery();

      if (!$this->_db->query()) {
	$err = $this->_db->getErrorMsg();
	die( $err );
      }

      $this->ordering = $row->ordering;
    } else {
      $query = "UPDATE $this->_tbl"
	. "\n SET ordering = " . (int) $this->ordering
	. "\n WHERE $this->_tbl_key = " . $this->_db->Quote( $this->$k )
	;
      $this->_db->setQuery( $query );
      //echo 'D: ' . $this->_db->getQuery();


      if (!$this->_db->query()) {
	$err = $this->_db->getErrorMsg();
	die( $err );
      }
    }
  }
  /**
   * Compacts the ordering sequence of the selected records
   * @param string Additional where query to limit ordering to a particular subset of records. This is expected to be a valid (and safe!) SQL expression
   */
  function updateOrder( $where='' ) {
    $k = $this->_tbl_key;

    if (!array_key_exists( 'ordering', get_class_vars( strtolower(get_class( $this )) ) )) {
      $this->_error = "WARNING: ".strtolower(get_class( $this ))." does not support ordering.";
      return false;
    }

    if ($this->_tbl == "#__content_frontpage") {
      $order2 = ", content_id DESC";
    } else {
      $order2 = '';
    }

    $query = "SELECT $this->_tbl_key, ordering"
      . "\n FROM $this->_tbl"
      . ( $where ? "\n WHERE $where" : '' )
      . "\n ORDER BY ordering$order2 "
      ;
    $this->_db->setQuery( $query );
    if (!($orders = $this->_db->loadObjectList())) {
      $this->_error = $this->_db->getErrorMsg();
      return false;
    }
    // first pass, compact the ordering numbers
    for ($i=0, $n=count( $orders ); $i < $n; $i++) {
      if ($orders[$i]->ordering >= 0) {
	$orders[$i]->ordering = $i+1;
      }
    }

    $shift = 0;
    $n=count( $orders );
    for ($i=0; $i < $n; $i++) {
      //echo "i=$i id=".$orders[$i]->$k." order=".$orders[$i]->ordering;
      if ($orders[$i]->$k == $this->$k) {
	// place 'this' record in the desired location
	$orders[$i]->ordering = min( $this->ordering, $n );
	$shift = 1;
      } else if ($orders[$i]->ordering >= $this->ordering && $this->ordering > 0) {
	$orders[$i]->ordering++;
      }
    }
    //echo '<pre>';print_r($orders);echo '</pre>';
    // compact once more until I can find a better algorithm
    for ($i=0, $n=count( $orders ); $i < $n; $i++) {
      if ($orders[$i]->ordering >= 0) {
	$orders[$i]->ordering = $i+1;
	$query = "UPDATE $this->_tbl"
	  . "\n SET ordering = " . (int) $orders[$i]->ordering
	  . "\n WHERE $k = " . $this->_db->Quote( $orders[$i]->$k )
	  ;
	$this->_db->setQuery( $query);
	$this->_db->query();
	//echo '<br />'.$this->_db->getQuery();
      }
    }

    // if we didn't reorder the current record, make it last
    if ($shift == 0) {
      $order = $n+1;
      $query = "UPDATE $this->_tbl"
	. "\n SET ordering = " . (int) $order
	. "\n WHERE $k = " . $this->_db->Quote( $this->$k )
	;
      $this->_db->setQuery( $query );
      $this->_db->query();
      //echo '<br />'.$this->_db->getQuery();
    }
    return true;
  }
  /**
   *	Generic check for whether dependancies exist for this object in the db schema
   *
   *	can be overloaded/supplemented by the child class
   *	@param string $msg Error message returned
   *	@param int Optional key index
   *	@param array Optional array to compiles standard joins: format [label=>'Label',name=>'table name',idfield=>'field',joinfield=>'field']. This is expected to hold only valid (and safe!) SQL expressions
   *	@return true|false
   */
  function canDelete( $oid=null, $joins=null ) {
    $k = $this->_tbl_key;
    if ($oid) {
      $this->$k = $oid;
    }
    if (is_array( $joins )) {
      $select = $k;
      $join = '';
      foreach( $joins as $table ) {
	$tblName = $this->getEscaped( $table['name'] );
	$idField = $this->getEscaped( $table['idfield'] );
	$jnField = $this->getEscaped( $table['joinfield'] );
	$select .= ",\n COUNT(DISTINCT `$tblName`.`$idField`) AS `count_".substr($tblName, 3)."_$idField`";
	$join .= "\n LEFT JOIN `$tblName` ON `$tblName`.`$jnField` = `$this->_tbl`.`$k`";
      }

      $query = "SELECT $select"
	. "\n FROM `$this->_tbl`"
	. $join
	. "\n WHERE `$this->_tbl`.`$k` = ". (int) $this->$k
	. "\n GROUP BY `$this->_tbl`.`$k`"
	;
      $this->_db->setQuery( $query );

      $obj = null;
      if (!$this->_db->loadObject($obj)) {
	$this->_error = $this->_db->getErrorMsg();
	return false;
      }
      $msg = array();
      foreach( $joins as $table ) {
	$tblName = $this->getEscaped( $table['name'] );
	$idField = $this->getEscaped( $table['idfield'] );
	$k = 'count_'.substr($tblName, 3).'_'.$idField;
	if ($obj->$k) {
	  $msg[] = $table['label'];
	}
      }

      if (count( $msg )) {
	$this->_error = "noDeleteRecord" . ": " . implode( ', ', $msg );
	return false;
      } else {
	return true;
      }
    }

    return true;
  }

  /**
   *	Default delete method
   *
   *	can be overloaded/supplemented by the child class
   *	@return true if successful otherwise returns and error message
   */
  function delete( $oid=null ) {
    //if (!$this->canDelete( $msg )) {
    //	return $msg;
    //}

    $k = $this->_tbl_key;
    if ($oid) {
      $this->$k = intval( $oid );
    }

    $query = "DELETE FROM $this->_tbl"
      . "\n WHERE $this->_tbl_key = " . $this->_db->Quote( $this->$k )
      ;
    $this->_db->setQuery( $query );

    if ($this->_db->query()) {
      return true;
    } else {
      $this->_error = $this->_db->getErrorMsg();
      return false;
    }
  }

  /**
   * Checks out an object
   * @param int User id
   * @param int Object id
   */
  function checkout( $user_id, $oid=null ) {
    if (!array_key_exists( 'checked_out', get_class_vars( strtolower(get_class( $this )) ) )) {
      $this->_error = "WARNING: ".strtolower(get_class( $this ))." does not support checkouts.";
      return false;
    }
    $k = $this->_tbl_key;
    if ($oid !== null) {
      $this->$k = $oid;
    }

    $time = date( 'Y-m-d H:i:s' );
    if (intval( $user_id )) {
      $user_id = intval( $user_id );
      // new way of storing editor, by id
      $query = "UPDATE $this->_tbl"
	. "\n SET checked_out = $user_id, checked_out_time = " . $this->_db->Quote( $time )
	. "\n WHERE $this->_tbl_key = " . $this->_db->Quote( $this->$k )
	;
      $this->_db->setQuery( $query );

      $this->checked_out = $user_id;
      $this->checked_out_time = $time;
    } else {
      $user_id = $this->_db->Quote( $user_id );
      // old way of storing editor, by name
      $query = "UPDATE $this->_tbl"
	. "\n SET checked_out = 1, checked_out_time = " . $this->_db->Quote( $time ) . ", editor = $user_id"
	. "\n WHERE $this->_tbl_key = " . $this->_db->Quote( $this->$k )
	;
      $this->_db->setQuery( $query );

      $this->checked_out = 1;
      $this->checked_out_time = $time;
      $this->checked_out_editor = $user_id;
    }

    return $this->_db->query();
  }

  /**
   * Checks in an object
   * @param int Object id
   */
  function checkin( $oid=null ) {
    if (!array_key_exists( 'checked_out', get_class_vars( strtolower(get_class( $this )) ) )) {
      $this->_error = "WARNING: ".strtolower(get_class( $this ))." does not support checkin.";
      return false;
    }

    $k 			= $this->_tbl_key;
    $nullDate 	= $this->_db->getNullDate();

    if ($oid !== null) {
      $this->$k = intval( $oid );
    }
    if ($this->$k == NULL) {
      return false;
    }

    $query = "UPDATE $this->_tbl"
      . "\n SET checked_out = 0, checked_out_time = " . $this->_db->Quote( $nullDate )
      . "\n WHERE $this->_tbl_key = " . $this->_db->Quote( $this->$k )
      ;
    $this->_db->setQuery( $query );

    $this->checked_out = 0;
    $this->checked_out_time = '';

    return $this->_db->query();
  }

  /**
   * Increments the hit counter for an object
   * @param int Object id
   */
  function hit( $oid=null ) {
    global $mosConfig_enable_log_items;

    $k = $this->_tbl_key;
    if ($oid !== null) {
      $this->$k = intval( $oid );
    }

    $query = "UPDATE $this->_tbl"
      . "\n SET hits = ( hits + 1 )"
      . "\n WHERE $this->_tbl_key = " . $this->_db->Quote( $this->id )
      ;
    $this->_db->setQuery( $query );
    $this->_db->query();

    if (@$mosConfig_enable_log_items) {
      $now = date( 'Y-m-d' );
      $query = "SELECT hits"
	. "\n FROM #__core_log_items"
	. "\n WHERE time_stamp = " . $this->_db->Quote( $now )
	. "\n AND item_table = " . $this->_db->Quote( $this->_tbl )
	. "\n AND item_id = " . $this->_db->Quote( $this->$k )
	;
      $this->_db->setQuery( $query );
      $hits = intval( $this->_db->loadResult() );
      if ($hits) {
	$query = "UPDATE #__core_log_items"
	  . "\n SET hits = ( hits + 1 )"
	  . "\n WHERE time_stamp = " . $this->_db->Quote( $now )
	  . "\n AND item_table = " . $this->_db->Quote( $this->_tbl )
	  . "\n AND item_id = " . $this->_db->Quote( $this->$k )
	  ;
	$this->_db->setQuery( $query );
	$this->_db->query();
      } else {
	$query = "INSERT INTO #__core_log_items"
	  . "\n VALUES ( " . $this->_db->Quote( $now ) . ", " . $this->_db->Quote( $this->_tbl ) . ", " . $this->_db->Quote( $this->$k ) . ", 1 )"
	  ;
	$this->_db->setQuery( $query );
	$this->_db->query();
      }
    }
  }

  /**
   * Tests if item is checked out
   * @param int A user id
   * @return boolean
   */
  function isCheckedOut( $user_id=0 ) {
    if ($user_id) {
      return ($this->checked_out && $this->checked_out != $user_id);
    } else {
      return $this->checked_out;
    }
  }

  /**
   * Generic save function
   * @param array Source array for binding to class vars
   * @param string Filter for the order updating. This is expected to be a valid (and safe!) SQL expression
   * @returns TRUE if completely successful, FALSE if partially or not succesful
   * NOTE: Filter will be deprecated in verion 1.1
   */
  function save( $source, $order_filter='' ) {
    if (!$this->bind( $source )) {
      return false;
    }
    if (!$this->check()) {
      return false;
    }
    if (!$this->store()) {
      return false;
    }
    if (!$this->checkin()) {
      return false;
    }

    if ($order_filter) {
      $filter_value = $this->$order_filter;
      $this->updateOrder( $order_filter ? "`$order_filter` = " . $this->_db->Quote( $filter_value ) : '' );
    }
    $this->_error = '';
    return true;
  }

  /**
   * @deprecated As of 1.0.3, replaced by publish
   */
  function publish_array( $cid=null, $publish=1, $user_id=0 ) {
    $this->publish( $cid, $publish, $user_id );
  }

  /**
   * Generic Publish/Unpublish function
   * @param	array	An array of id numbers
   * @param	integer	0 if unpublishing, 1 if publishing
   * @param	integer	The id of the user performnig the operation
   * @since	1.0.4
   */
  function publish( $cid=null, $publish=1, $user_id=0 ) {
    mosArrayToInts( $cid, array() );
    $user_id	= (int) $user_id;
    $publish	= (int) $publish;
    $k			= $this->_tbl_key;

    if (count( $cid ) < 1) {
      $this->_error = "No items selected.";
      return false;
    }

    $cids = $this->_tbl_key . '=' . implode( ' OR ' . $this->_tbl_key . '=', $cid );

    $query = "UPDATE $this->_tbl"
      . "\n SET published = " . (int) $publish
      . "\n WHERE ($cids)"
      . "\n AND (checked_out = 0 OR checked_out = " . (int) $user_id . ")"
      ;
    $this->_db->setQuery( $query );
    if (!$this->_db->query()) {
      $this->_error = $this->_db->getErrorMsg();
      return false;
    }

    if (count( $cid ) == 1) {
      $this->checkin( $cid[0] );
    }
    $this->_error = '';
    return true;
  }

  /**
   * Export item list to xml
   * @param boolean Map foreign keys to text values
   */
  function toXML( $mapKeysToText=false ) {
    $xml = '<record table="' . $this->_tbl . '"';

    if ($mapKeysToText) {
      $xml .= ' mapkeystotext="true"';
    }
    $xml .= '>';
    foreach (get_object_vars( $this ) as $k => $v) {
      if (is_array($v) or is_object($v) or $v === NULL) {
	continue;
      }
      if ($k[0] == '_') { // internal field
	continue;
      }
      $xml .= '<' . $k . '><![CDATA[' . $v . ']]></' . $k . '>';
    }
    $xml .= '</record>';

    return $xml;
  }

	
}


?>

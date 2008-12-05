#! /bin/env php
<?php

putenv('LD_LIBRARY_PATH=/usr/lib/oracle/xe/app/oracle/product/10.2.0/server/lib');

function get_stack_trace ()
{
  $st = debug_backtrace ();
  $res = "<table>
<tr><th>Function</th><th>File</th><th>Line</th></tr>\n";

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
	      $arg_str[] = "&lt;null&gt;";
	    }
	  else if (is_array($arg))
	    {
	      $arg_str [] = "&lt;array&gt;[" . count ($arg) . "]";
	    }
	  else if (is_object ($arg))
	    {
	      $arg_str [] = "&lt;object&gt;";
	    }
	  else 
	    {
	      $arg_str [] = "&lt;unknown&gt;";
	    }
	}

      $string = "<tr><td><b>";
      if (isset ($i['class']))
	$string .= $i['class']. "::";

      if (isset ($i['function']))
	$string .= $i['function'];
      
      $string .=" (". implode (", ", $arg_str). ")</b>: ";
      $string .= "</td><td>\n";


      if (isset ($i['file']))
	{
	  $file = $i['file'];
	  if (strlen ($file) > 30)
	    {
	      $file = "..." . substr ($file, strlen ($file)-27);
	    }
	  $string .= $file;
	}
      $string .= "</td><td style='text-align: right;'>\n";

      if (isset ($i['line']))
	$string .= $i['line'];

      $string .= "</td></tr>\n";
      $res  .= $string;
    }

  $res .= "</table>\n";

  return $res;
}



($res = ociplogon ("system", "changeme", "localhost/xe","WE8ISO8859P1"))|| die ("ociplogon");

$q = "select table_name from user_tables order by table_name";
($stmt = ociparse ($res, $q)) || die ("ociparse");
ociexecute ($stmt) || die ("ociexecute");


$ok = array ("demo6_", "f_", "log", "link", "help", "shield_table");

$kill = array ();

$row = array ();

echo "<h1>Oracle table entries</h1>";

while ($status = ocifetchinto ($stmt, $row, OCI_NUM))
{
  $table_name = $row[0];

  if (strchr($table_name, '$'))
    continue;

  $is_ok = false;
  foreach ($ok as $item)
    {

      if (strtolower (substr($table_name,0, strlen($item))) == strtolower ($item))
	$is_ok = true;
    }

  if ($is_ok )
    continue;

  echo $table_name . "<br>";
  $kill [] = $table_name;
}

ocifreecursor($stmt);

foreach ($kill as $table)
{
  echo $table ."<br>";

  $q = "drop table \"" . $table. "\"";
  ($stmt = ociparse ($res, $q)) || die ("ociparse");
  ociexecute ($stmt);
  ocifreecursor($stmt);

  $q = "drop table " . $table;
  ($stmt = ociparse ($res, $q)) || die ("ociparse");
  ociexecute ($stmt);
  ocifreecursor($stmt);
}

echo "<h1>Shield table entries</h1>";

$q = "select table_name from shield_table_column group by table_name order by table_name";
($stmt = ociparse ($res, $q)) || die ("ociparse");
ociexecute ($stmt) || die ("ociexecute");

$ok = array ("demo6_", "f_", "log", "link", "help", "shield_table");

$kill = array ();
$row = array ();

while ($status = ocifetchinto ($stmt, $row, OCI_NUM))
{
  $table_name = $row[0];

  if (strchr($table_name, '$'))
    continue;

  $is_ok = false;
  foreach ($ok as $item)
    {

      if (strtolower (substr($table_name,0, strlen($item))) == strtolower ($item))
	$is_ok = true;
    }
  
  if ($is_ok )
    continue;
  
  $kill [] = $table_name;
}

ocifreecursor($stmt);


foreach ($kill as $table)
{
  echo $table ."<br>";

  $q = "delete from shield_table_column where table_name = '$table'";

  ($stmt = ociparse ($res, $q)) || die ("ociparse");
  ociexecute ($stmt);
  ocifreecursor($stmt);
  
}


exit (0);

?>
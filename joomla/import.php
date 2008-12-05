#! /bin/env php
<?php
/**
@file import.php

A simple file that reads mysql queries from stdin, separates them on semicolons and executes the individual queries through shield and Oracle.

*/

define ('_VALID_MOS', 1);
require_once ("database.php");

function execute ($query)
{
  echo ".";
  $res = shield_query ($query);
  if ($res)
    {
      shield_free_result ($res);
    }
  else
    {
      echo "OH NOES!!!!\n" . shield_error () . "\n\n";
    }
  
}

$stdin = fopen("php://stdin", 'r');
shield_connect ("//db-ecco.elkjop.int:1521", "ecco", "Hnsk37sm%g3") || die ("shield_connect");
shield_select_db ("ecco") || die ("shield_select_db");
$q = false;
$res = "";

shield_begin_verbose ();
while (true)
{
  $c = fgetc ($stdin);

  if ($c === false)
    {
      execute ($res);
      break;
    }
  $res .= $c;
  
  if ($q)
    {
      if ($c == $q)
	{
	  $q = 0;
	}
      else if ($c == '\\')
	{
	  $c = fgetc ($stdin);
	  $res .= $c;
	}
    }
  else if ($c == ';')
    {
      execute ($res);
      $res = "";
    }
  else if ($c == "'" || $c == '"')
    {
      $q = $c;
    }
}
shield_end_verbose ();

?>
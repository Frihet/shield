#! /bin/env php
<?php

$f = fopen("php://stdin","r");

$prev="";

function handle($c) 
{
	if(!$c)
		return;
	
	$conv = iconv("utf-8","iso8859-1", $c);
	if ($conv) {
		echo $conv;
	}
	else {
		echo $c;
	}
}


while(true) {
	$c = fgetc($f);
	if ($c === false) {
		break;
	}
	if (ord($c)>127) {
		$prev .= $c;
	} else {
		handle($prev);
		$prev="";
		echo $c;
	}
	
}

handle($prev);



?>
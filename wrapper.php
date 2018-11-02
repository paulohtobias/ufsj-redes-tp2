<?php
    $parts = explode("?", $argv[2], 2);
    if (count($parts) == 2) {
		if (strcmp($argv[1], "GET") == 0) {
			parse_str($parts[1], $_GET);
		} else if (strcmp($argv[1], "POST") == 0) {
			parse_str($parts[1], $_POST);
		}
	}
	include($parts[0]);
?>

<?php
	$alarm_val = $_POST['alarm'];
	$garaje_val = $_POST['garaje'];
	$file = fopen("cqrty.json", "w") or die("can't open file");
 	fwrite($file, '{"alarm":' . $_POST['alarm'] . ',');
 	fwrite($file, '"garaje":' . $_POST['garaje'].'}');
 	fclose($file);
 	echo "Alarma configurada correctamente";
?>


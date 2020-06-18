<!DOCTYPE html>
<html lang="hu">
<head>
<title>Időjárás állomás - összes mérés</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<link href="style.css" rel="stylesheet" type="text/css">

<style>
html{font-family: "Roboto", sans-serif;}
.cards{Width:100%; display: grid; padding: 20px 20px 20px; grid-gap: 2rem; align-content: center;}
.card{box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2); transition: 0.3s; width: 100%; text-align:center; padding: 10px 1px 10px;}
h1{font-size: 150px; line-height: 0.5;}
h3{font-size: 75px; line-height: 0.5;}
h4{font-size: 50px; line-height: 0.5;}
</style>

</head>
<body>
<?php

$server = "localhost";
$database = "weatherdb";
$username = "weatherdb";
$password = "Szakdoga822";

$conn = new mysqli($server, $username, $password, $database);
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT * FROM `WeatherDatas` ORDER BY `id` DESC LIMIT 1";

 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_temperature = $row["temperature"];
        $row_humidity = $row["humidity"];
        $row_pressure = $row["pressure"];
        $row_windSpeed = $row["windSpeed"]; 
        $row_WindDirectionDegree = $row["WindDirectionDegree"]; 
		$row_WindDirectionCompass = $row["WindDirectionCompass"]; 
        $row_reading_time = $row["readingTime"];
      
        echo '<div class="cards">	
				<div class="card">
				  <h3>Hőmérséklet</h3>
				   <h1><b>' . $row_temperature . '</b></h1>
				    <h4>°C</h4>
				</div>
				<div class="card">
				  <h3>Páratartalom</h3>
				   <h1><b>' . $row_humidity . '</b></h1>
				    <h4>%</h4>
				</div>
				<div class="card">
				  <h3>Légnyomás</h3>
				   <h1><b>' . $row_pressure . '</b></h1>
				    <h4>hPa</h4>
				</div>
			  </div>';
    }
    $result->free();
	$conn->close();
}

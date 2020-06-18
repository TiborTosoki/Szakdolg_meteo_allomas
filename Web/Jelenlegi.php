<!-- A legutóbb beszúrt rekordot megjelenítő oldal. -->
<!-- Tósoki Tibor 2020 -->

<!DOCTYPE html>
<html lang="hu">
<head>
<title>Időjárás állomás - legutóbbi mérés</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<link href="style.css" rel="stylesheet" type="text/css">


<h1>Legutóbbi mérés értékei</h1>

<style>
	html{font-family: "Roboto", sans-serif;}
	h1{font-size: 40px; line-height: 0.05; text-align:center; padding-top: 25px; padding-bottom: 20px;}
	h2{font-size: 20px; line-height: 0.05; text-align:center; padding-bottom: 20px;}
	
	.cards{ width: 80%; margin-left: 25%;}
	.column { float: left; width: 20%; min-width: 200px; padding: 0 10px; margin-top: 20px}
	.row {margin: 0 auto;}
	.row:after {content: "";display: table; clear: both;}
	@media screen and (max-width: 600px) { .column {width: 100%;display: block; margin-bottom: 20px; } }
	.card { box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2); padding: 1px; text-align: center; background-color: #f1f1f1; overflow: auto;}
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
      
        echo '<h2>Rögzítés ideje: ' . $row_reading_time . '</h2>
		<div class="cards">
			<div class="row">
				<div class="column">
					<div class="card">
						<h3>Hőmérséklet</h3>
						<h1>' . $row_temperature . '</h1>
						<h2>°C</h2>
					</div>
				</div>

				<div class="column">
					<div class="card">
						<h3>Páratartalom</h3>
						<h1>' . $row_humidity . '</h1>
						<h2>%</h2>
					</div>
				</div>
				
				<div class="column">
					<div class="card">
						<h3>Légnyomás</h3>
						<h1>' . $row_pressure . '</h1>
						<h2>hPa</h2>
					</div>
				</div>
			</div>
			<div class="row">
				<div class="column">
					<div class="card">
						<h3>Szélsebesség</h3>
						<h1>' . $row_windSpeed . '</h1>
						<h2>km/h</h2>
					</div>
				</div>

				<div class="column">
					<div class="card">
						<h3>Szélirány</h3>
						<h1>' . $row_WindDirectionDegree . '</h1>
						<h2>°</h2>
					</div>
				</div>
  
				<div class="column">
					<div class="card">
						<h3>Szélirány (égtáj)</h3>
						<h1>' . $row_WindDirectionCompass . '</h1>
						<h2></h2>
					</div>
				</div>
			</div>
		</div>';
    }
    $result->free();
	$conn->close();
}

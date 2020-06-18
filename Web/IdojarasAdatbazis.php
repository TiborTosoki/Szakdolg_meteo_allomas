<!-- Az adatbázis teljes tartalmának megjelenítése egy táblázatban -->
<!-- Tósoki Tibor 2020 -->

<!DOCTYPE html>
<html lang="hu">
<head>
<title>Időjárás állomás - összes mérés</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<link href="style.css" rel="stylesheet" type="text/css">

<style>
table {border-collapse: collapse; width: 80%;}
tr:hover {background-color: #f5f5f5;}
table, th, td {border-bottom: 1px solid #ddd; padding: 8px; font-family: "Roboto", sans-serif;}
th {height: 50px; text-align: left; background-color: #0099ff; color: white;}
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

$sql = "SELECT * FROM `WeatherDatas` ORDER BY `id` DESC";

echo '<table>
      <tr> 
        <th>Hőmérséklet (°C)</th> 
        <th>Páratartalom (%)</th> 
        <th>Légnyomás (hPa)</th> 
        <th>Szélsebesség (km/h)</th>
        <th>Szélirány (fok)</th>
		<th>Szélirány (égtáj)</th>
        <th>Mérés ideje</th> 
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_temperature = $row["temperature"];
        $row_humidity = $row["humidity"];
        $row_pressure = $row["pressure"];
        $row_windSpeed = $row["windSpeed"]; 
        $row_WindDirectionDegree = $row["WindDirectionDegree"]; 
		$row_WindDirectionCompass = $row["WindDirectionCompass"]; 
        $row_reading_time = $row["readingTime"];
      
        echo '<tr> 
                <td>' . $row_temperature . '</td> 
                <td>' . $row_humidity . '</td> 
                <td>' . $row_pressure . '</td> 
                <td>' . $row_windSpeed . '</td>
                <td>' . $row_WindDirectionDegree . '</td>
				<td>' . $row_WindDirectionCompass . '</td> 
                <td>' . $row_reading_time . '</td> 
              </tr>';
    }
    $result->free();
	$conn->close();
}

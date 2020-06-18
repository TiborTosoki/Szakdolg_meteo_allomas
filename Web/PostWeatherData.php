<!-- Az adatbázisba való beszúrásért felelős oldal. -->
<!-- Tósoki Tibor 2020 -->

<?php

$server = "localhost";
$database = "weatherdb";
$username = "weatherdb";
$password = "Szakdoga822";

$client_api_key = $temperature = $humidity = $pressure = $windSpeed = $windDirectionDegree = $windDirectionCompass = "";

$api_key = "zP2kCAxc9YJ5YotgF9On7ytwXLXe4JXb";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
	
    $client_api_key = $_POST["client_api_key"];
    if($client_api_key == $api_key) {
        $temperature = $_POST["temperature"];
        $humidity = $_POST["humidity"];
        $pressure = $_POST["pressure"];
        $windSpeed = $_POST["windSpeed"];
        $windDirectionDegree = $_POST["windDirectionDegree"];
		$windDirectionCompass = $_POST["windDirectionCompass"];
        
        $conn = new mysqli($server, $username, $password, $database);
        if ($conn->connect_error) {
            die("Kapcsolódási hiba: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO WeatherDatas (temperature, humidity, pressure, windSpeed, windDirectionDegree, windDirectionCompass) VALUES ('" . $temperature . "', '" . $humidity . "', '" . $pressure . "', '" . $windSpeed . "', '" . $windDirectionDegree . "', '" . $windDirectionCompass . "')";
        
        $conn->query($sql);
        $conn->close();
    }
}









<?php
require 'config.php';


$dev= $dht1= $dht2="";
//Device
//DHT1,DHT2
if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $dev = test_input($_POST["dev"]);
        $dht1 = test_input($_POST["dht1"]);
        $dht2 = test_input($_POST["dht2"]);
        
        // Create connection
        $conn = new mysqli($servername, $userID, $pass, $database);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } ;
        $sql="UPDATE myesp SET DHT1='".$dht1."',DHT2='".$dht2."' WHERE DEVICEID='".$dev."'";
    
        if ($conn->query($sql) === TRUE) {
            echo "New DHT record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
?>
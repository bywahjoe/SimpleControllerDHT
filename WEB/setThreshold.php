<?php
require 'config.php';


$dev= $thr="";
//Device
//DHT1,DHT2
if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $dev = test_input($_POST["dev"]);
        $md=test_input($_POST["mode"]);
        $mymin1 = test_input($_POST["min1"]);
        $mymax1 = test_input($_POST["max1"]);
        $mymin2 = test_input($_POST["min2"]);
        $mymax2 = test_input($_POST["max2"]);

        // Create connection
        $conn = new mysqli($servername, $userID, $pass, $database);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } ;
        $sql="UPDATE myesp SET MODE='".$md."', vminDHT1='".$mymin1."', vmaxDHT1='".$mymax1."', vminDHT2='".$mymin2."', vmaxDHT2='".$mymax2."' WHERE DEVICEID='".$dev."'";
    
        if ($conn->query($sql) === TRUE) {
            echo "New THR record created successfully";
            header( "refresh:3; url=https://iot.partnerkita.com/" ); 
            //echo $dev;
            //echo $thr;
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
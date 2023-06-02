/**
 * Add gobals here
 */
var seconds = null;
var otaTimerVar = null;
var wifiConnectInterval = null;


/**
 * Initialize functions here.
 */
$(document).ready(function() { //Cuando ya haya cargado toda la pagina

    getUpdateStatus(); // JSON

    // $("#connect_wifi").on("click", function() {
    //     checkCredentials();
    // });
    // $("#disconnect_wifi").on("click", function() {
    //     disconnectWifi();
    // });

});


document.getElementById("dashboardForm").addEventListener("submit", function(event) { //Cuando yo presione el boton ejecuteme esta funcion
    event.preventDefault(); // Prevent form submission

    // Get form data
    var form = document.getElementById("dashboardForm");
    var formData_dashboard = new FormData(form);

    // Get values of the fields
    var thresholdValue = document.getElementById("threshold").value;
    var redValue = document.getElementById("red_val").value;
    var greenValue = document.getElementById("green_val").value;
    var blueValue = document.getElementById("blue_val").value;

    // Make to JSON object
    var dataDashboard = {
        threshold: thresholdValue,
        red_str: redValue,
        green_str: greenValue,
        blue_str: blueValue,
    };

    // Convert object to JSON
    var jsonDataDashboard = JSON.stringify(dataDashboard);

    // create and send request to server
    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/dashboard-post"); // Hicimos un POST a este URL -> /dashboard-post   ||   URL Server -> Le mandamos un JSON POST y dejo el puerto abierto hasta que responda o conteste
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.onload = function() {
        if (xhr.status === 200) { //Cuando el servidor contesta (si es 200 es que estuvo bien)
            // Request succeess
            console.log("POST request successful");
            console.log(xhr.responseText);
            document.getElementById("result").innerText = xhr.responseText;
        } else {
            // Request fail
            console.log("POST request failed");
            console.log("Error: " + xhr.status);
            document.getElementById("result").innerText = "Error: " + xhr.status;
        }
    };
    xhr.send(jsonDataDashboard);
});



$(document).ready(function(){
    startDHTSensorInterval();
    print("ready")
})


/**
 * Gets DHT22 sensor temperature and humidity values for display on the web page.
 */
function getDHTSensorValues()
{
	$.getJSON('/dhtSensor.json', function(data) {
        var temperature = parseFloat(data["temp"]);
        var humidity = parseFloat(data["humidity"]);

        var result = temperature * humidity;

		$("#temperature_reading").text(data["temp"]);
        $("#current_reading").text(data["humidity"]);
        $("#power_reading").text(result);
	});
}



/**
 * Sets the interval for getting the updated DHT22 sensor values.
 */
function startDHTSensorInterval()
{
	setInterval(getDHTSensorValues, 1000);    
}
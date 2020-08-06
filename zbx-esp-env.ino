// ====
  // Zabbbix-ESP-Environment Monitoring
  // Michał Sternadel 2020
  // Version: 1.0
  // Date: 2020-08-06
  // ====

  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <Adafruit_Sensor.h>
  #include <DHT.h>
  #include <Arduino_JSON.h>
  #include "wificonfig.h"
  #include "sensors.h"
  
  #define RESET false
  
  DHT dht0(DHT0_PIN, DHT0_TYPE);
  DHT dht1(DHT1_PIN, DHT1_TYPE);
  DHT dht2(DHT2_PIN, DHT2_TYPE);
  DHT dht3(DHT3_PIN, DHT3_TYPE);
  DHT dht4(DHT4_PIN, DHT4_TYPE);
  DHT dht5(DHT5_PIN, DHT5_TYPE);
  DHT dht6(DHT6_PIN, DHT6_TYPE);
  DHT dht7(DHT7_PIN, DHT7_TYPE);
  DHT dht8(DHT8_PIN, DHT8_TYPE);

  // current temperature & humidity, updated in loop()
  float temperature[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  float humidity[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

  // Create a webserver at port 80
  AsyncWebServer server(80);

  // Will hold last time the sensor was updated
  unsigned long dhtpreviousMillis = 0;
  unsigned long rstpreviousMillis = 0;

  // Updates sensor reading every 15 seconds
  const long dhtinterval = 15000;  

  // Reset interval every half hour
  const long rstinterval = 1800000;  
  
  const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML>
  <html><head><meta name="viewport" content="width=device-width, initial-scale=1"><link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous"><title>Environmental data</title> 
  <style>
    * { box-sizing: border-box; }
    body { font-size: 1.25rem; font-family: sans-serif; line-height: 150%%; text-shadow: 0 2px 2px #787878; padding: 0px; margin: 0px;}
    section { color: #fff; text-align: center; }
    div { height: 100%%; }
    article { position: absolute; top: 50%%; left: 50%%; transform: translate(-50%%, -50%%); width: 100%%; padding: 20px; }
    h1 { font-size: 1.75rem; margin: 0 0 0.75rem 0; }
    .container { display: table; width: 100%%; }
    .left-half { background-color: #c4c4c4; position: absolute; left: 0px; width: 50%%; }
    .right-half { background-color: #a8a8a8; position: absolute; right: 0px; width: 50%%; }
    .units { font-size: 1.2rem; }
    .dht-section { display: block; }
    .dht-labels { font-size: 1.5rem; vertical-align: middle; padding-bottom: 15px; padding-right: 5px;}
    .dht-values {  font-size: 1.5rem; vertical-align: middle; padding-bottom: 15px; }
    .s0 { display: none; }
    .s1 { }
  </style>
  <script src="/reload.js"></script>
  </head>
  <body>
    <section class="container">
      <div class="left-half">
        <article>
          <h1>Temperatures</h1>
          <span class="dht-section s%S0%"><i class="fas fa-thermometer-half" style="color:#059e8a;">&nbsp;</i><span id="tn0" class="dht-labels">%N0%</span><span class="dht-labels">: </span><span id="t0" class="dht-values">%T0%</span><sup class="units">o</sup>C</span>
          <span class="dht-section s%S1%"><i class="fas fa-thermometer-half" style="color:#059e8a;">&nbsp;</i><span id="tn1" class="dht-labels">%N1%</span><span class="dht-labels">: </span><span id="t1" class="dht-values">%T1%</span><sup class="units">o</sup>C</span>
          <span class="dht-section s%S2%"><i class="fas fa-thermometer-half" style="color:#059e8a;">&nbsp;</i><span id="tn2" class="dht-labels">%N2%</span><span class="dht-labels">: </span><span id="t2" class="dht-values">%T2%</span><sup class="units">o</sup>C</span>
          <span class="dht-section s%S3%"><i class="fas fa-thermometer-half" style="color:#059e8a;">&nbsp;</i><span id="tn3" class="dht-labels">%N3%</span><span class="dht-labels">: </span><span id="t3" class="dht-values">%T3%</span><sup class="units">o</sup>C</span>
          <span class="dht-section s%S4%"><i class="fas fa-thermometer-half" style="color:#059e8a;">&nbsp;</i><span id="tn4" class="dht-labels">%N4%</span><span class="dht-labels">: </span><span id="t4" class="dht-values">%T4%</span><sup class="units">o</sup>C</span>
          <span class="dht-section s%S5%"><i class="fas fa-thermometer-half" style="color:#059e8a;">&nbsp;</i><span id="tn5" class="dht-labels">%N5%</span><span class="dht-labels">: </span><span id="t5" class="dht-values">%T5%</span><sup class="units">o</sup>C</span>
          <span class="dht-section s%S6%"><i class="fas fa-thermometer-half" style="color:#059e8a;">&nbsp;</i><span id="tn6" class="dht-labels">%N6%</span><span class="dht-labels">: </span><span id="t6" class="dht-values">%T6%</span><sup class="units">o</sup>C</span>
          <span class="dht-section s%S7%"><i class="fas fa-thermometer-half" style="color:#059e8a;">&nbsp;</i><span id="tn7" class="dht-labels">%N7%</span><span class="dht-labels">: </span><span id="t7" class="dht-values">%T7%</span><sup class="units">o</sup>C</span>
          <span class="dht-section s%S8%"><i class="fas fa-thermometer-half" style="color:#059e8a;">&nbsp;</i><span id="tn0" class="dht-labels">%N8%</span><span class="dht-labels">: </span><span id="t8" class="dht-values">%T8%</span><sup class="units">o</sup>C</span>
        </article>
      </div>
      <div class="right-half">
        <article>
          <h1>Humidity</h1>
          <span class="dht-section s%S0%"><i class="fas fa-tint" style="color:#00add6;">&nbsp;</i><span id="hn0" class="dht-labels">%N0%</span><span class="dht-labels">: </span><span id="h0" class="dht-values">%H0%</span><sup class="units">%%</sup></span>
          <span class="dht-section s%S1%"><i class="fas fa-tint" style="color:#00add6;">&nbsp;</i><span id="hn1" class="dht-labels">%N1%</span><span class="dht-labels">: </span><span id="h1" class="dht-values">%H1%</span><sup class="units">%%</sup></span>
          <span class="dht-section s%S2%"><i class="fas fa-tint" style="color:#00add6;">&nbsp;</i><span id="hn2" class="dht-labels">%N2%</span><span class="dht-labels">: </span><span id="h2" class="dht-values">%H2%</span><sup class="units">%%</sup></span>
          <span class="dht-section s%S3%"><i class="fas fa-tint" style="color:#00add6;">&nbsp;</i><span id="hn3" class="dht-labels">%N3%</span><span class="dht-labels">: </span><span id="h3" class="dht-values">%H3%</span><sup class="units">%%</sup></span>
          <span class="dht-section s%S4%"><i class="fas fa-tint" style="color:#00add6;">&nbsp;</i><span id="hn4" class="dht-labels">%N4%</span><span class="dht-labels">: </span><span id="h4" class="dht-values">%H4%</span><sup class="units">%%</sup></span>
          <span class="dht-section s%S5%"><i class="fas fa-tint" style="color:#00add6;">&nbsp;</i><span id="hn5" class="dht-labels">%N5%</span><span class="dht-labels">: </span><span id="h5" class="dht-values">%H5%</span><sup class="units">%%</sup></span>
          <span class="dht-section s%S6%"><i class="fas fa-tint" style="color:#00add6;">&nbsp;</i><span id="hn6" class="dht-labels">%N6%</span><span class="dht-labels">: </span><span id="h6" class="dht-values">%H6%</span><sup class="units">%%</sup></span>
          <span class="dht-section s%S7%"><i class="fas fa-tint" style="color:#00add6;">&nbsp;</i><span id="hn7" class="dht-labels">%N7%</span><span class="dht-labels">: </span><span id="h7" class="dht-values">%H7%</span><sup class="units">%%</sup></span>
          <span class="dht-section s%S8%"><i class="fas fa-tint" style="color:#00add6;">&nbsp;</i><span id="hn8" class="dht-labels">%N8%</span><span class="dht-labels">: </span><span id="h8" class="dht-values">%H8%</span><sup class="units">%%</sup></span>
        </article>
      </div>
    </section>
  </body>
  </html>)rawliteral";

  const char sensors_json[] PROGMEM = R"rawliteral([{"name":"%N0%", "state": "%S0%", "index": "0", "temperature": "%T0%", "humidity": "%H0%"},{"name":"%N1%", "state": "%S1%", "index": "1", "temperature": "%T1%", "humidity": "%H1%"},{"name":"%N2%", "state": "%S2%", "index": "2", "temperature": "%T2%", "humidity": "%H2%"},{"name":"%N3%", "state": "%S3%", "index": "3", "temperature": "%T3%", "humidity": "%H3%"},{"name":"%N4%", "state": "%S4%", "index": "4", "temperature": "%T4%", "humidity": "%H4%"},{"name":"%N5%", "state": "%S5%", "index": "5", "temperature": "%T5%", "humidity": "%H5%"},{"name":"%N6%", "state": "%S6%", "index": "6", "temperature": "%T6%", "humidity": "%H6%"},{"name":"%N7%", "state": "%S7%", "index": "7", "temperature": "%T7%", "humidity": "%H7%"},{"name":"%N8%", "state": "%S8%", "index": "8", "temperature": "%T8%", "humidity": "%H8%"}])rawliteral";
  const char zabbix_json[] PROGMEM = R"rawliteral({
  "data":[
      {"{#NAME}":"%N0%", "{#STATE}":"%S0%", "{#INDEX}":"0", "{#AVERAGE_L}":"%AVGL0%", "{#AVERAGE_H}":"%AVGH0%", "{#HIGH_L}":"%HIL0%", "{#HIGH_H}":"%HIH0%", "{#DISASTER_L}":"%DISL0%", "{#DISASTER_H}":"%DISH0%"},
      {"{#NAME}":"%N1%", "{#STATE}":"%S1%", "{#INDEX}":"1", "{#AVERAGE_L}":"%AVGL1%", "{#AVERAGE_H}":"%AVGH1%", "{#HIGH_L}":"%HIL1%", "{#HIGH_H}":"%HIH1%", "{#DISASTER_L}":"%DISL1%", "{#DISASTER_H}":"%DISH1%"},
      {"{#NAME}":"%N2%", "{#STATE}":"%S2%", "{#INDEX}":"2", "{#AVERAGE_L}":"%AVGL2%", "{#AVERAGE_H}":"%AVGH2%", "{#HIGH_L}":"%HIL2%", "{#HIGH_H}":"%HIH2%", "{#DISASTER_L}":"%DISL2%", "{#DISASTER_H}":"%DISH2%"},
      {"{#NAME}":"%N3%", "{#STATE}":"%S3%", "{#INDEX}":"3", "{#AVERAGE_L}":"%AVGL3%", "{#AVERAGE_H}":"%AVGH3%", "{#HIGH_L}":"%HIL3%", "{#HIGH_H}":"%HIH3%", "{#DISASTER_L}":"%DISL3%", "{#DISASTER_H}":"%DISH3%"},
      {"{#NAME}":"%N4%", "{#STATE}":"%S4%", "{#INDEX}":"4", "{#AVERAGE_L}":"%AVGL4%", "{#AVERAGE_H}":"%AVGH4%", "{#HIGH_L}":"%HIL4%", "{#HIGH_H}":"%HIH4%", "{#DISASTER_L}":"%DISL4%", "{#DISASTER_H}":"%DISH4%"},
      {"{#NAME}":"%N5%", "{#STATE}":"%S5%", "{#INDEX}":"5", "{#AVERAGE_L}":"%AVGL5%", "{#AVERAGE_H}":"%AVGH5%", "{#HIGH_L}":"%HIL5%", "{#HIGH_H}":"%HIH5%", "{#DISASTER_L}":"%DISL5%", "{#DISASTER_H}":"%DISH5%"},
      {"{#NAME}":"%N6%", "{#STATE}":"%S6%", "{#INDEX}":"6", "{#AVERAGE_L}":"%AVGL6%", "{#AVERAGE_H}":"%AVGH6%", "{#HIGH_L}":"%HIL6%", "{#HIGH_H}":"%HIH6%", "{#DISASTER_L}":"%DISL6%", "{#DISASTER_H}":"%DISH6%"},
      {"{#NAME}":"%N7%", "{#STATE}":"%S7%", "{#INDEX}":"7", "{#AVERAGE_L}":"%AVGL7%", "{#AVERAGE_H}":"%AVGH7%", "{#HIGH_L}":"%HIL7%", "{#HIGH_H}":"%HIH7%", "{#DISASTER_L}":"%DISL7%", "{#DISASTER_H}":"%DISH7%"},
      {"{#NAME}":"%N8%", "{#STATE}":"%S8%", "{#INDEX}":"8", "{#AVERAGE_L}":"%AVGL8%", "{#AVERAGE_H}":"%AVGH8%", "{#HIGH_L}":"%HIL8%", "{#HIGH_H}":"%HIH8%", "{#DISASTER_L}":"%DISL8%", "{#DISASTER_H}":"%DISH8%"}
    ]
  })rawliteral";

  const char reload_js[] PROGMEM = R"rawliteral(
  //setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("tn0").innerHTML = this.responseText; document.getElementById("hn0").innerHTML = this.responseText;}}; xhttp.open("GET", "/n/0", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("tn1").innerHTML = this.responseText; document.getElementById("hn1").innerHTML = this.responseText;}}; xhttp.open("GET", "/n/1", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("tn2").innerHTML = this.responseText; document.getElementById("hn2").innerHTML = this.responseText;}}; xhttp.open("GET", "/n/2", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("tn3").innerHTML = this.responseText; document.getElementById("hn3").innerHTML = this.responseText;}}; xhttp.open("GET", "/n/3", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("tn4").innerHTML = this.responseText; document.getElementById("hn4").innerHTML = this.responseText;}}; xhttp.open("GET", "/n/4", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("tn5").innerHTML = this.responseText; document.getElementById("hn5").innerHTML = this.responseText;}}; xhttp.open("GET", "/n/5", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("tn6").innerHTML = this.responseText; document.getElementById("hn6").innerHTML = this.responseText;}}; xhttp.open("GET", "/n/6", true); xhttp.send();}, 15000 );
  //setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("tn7").innerHTML = this.responseText; document.getElementById("hn7").innerHTML = this.responseText;}}; xhttp.open("GET", "/n/7", true); xhttp.send();}, 15000 );
  //setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("tn8").innerHTML = this.responseText; document.getElementById("hn8").innerHTML = this.responseText; }}; xhttp.open("GET", "/n/8", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("t0").innerHTML = this.responseText; }}; xhttp.open("GET", "/t/0", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("t1").innerHTML = this.responseText; }}; xhttp.open("GET", "/t/1", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("t2").innerHTML = this.responseText; }}; xhttp.open("GET", "/t/2", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("t3").innerHTML = this.responseText; }}; xhttp.open("GET", "/t/3", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("t4").innerHTML = this.responseText; }}; xhttp.open("GET", "/t/4", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("t5").innerHTML = this.responseText; }}; xhttp.open("GET", "/t/5", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("t6").innerHTML = this.responseText; }}; xhttp.open("GET", "/t/6", true); xhttp.send();}, 15000 );
  //setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("t7").innerHTML = this.responseText; }}; xhttp.open("GET", "/t/7", true); xhttp.send();}, 15000 );
  //setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("t8").innerHTML = this.responseText; }}; xhttp.open("GET", "/t/8", true); xhttp.send();}, 15000 );
  //setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("h0").innerHTML = this.responseText; }}; xhttp.open("GET", "/h/0", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("h1").innerHTML = this.responseText; }}; xhttp.open("GET", "/h/1", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("h2").innerHTML = this.responseText; }}; xhttp.open("GET", "/h/2", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("h3").innerHTML = this.responseText; }}; xhttp.open("GET", "/h/3", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("h4").innerHTML = this.responseText; }}; xhttp.open("GET", "/h/4", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("h5").innerHTML = this.responseText; }}; xhttp.open("GET", "/h/5", true); xhttp.send();}, 15000 );
  setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("h6").innerHTML = this.responseText; }}; xhttp.open("GET", "/h/6", true); xhttp.send();}, 15000 );
  //setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("h7").innerHTML = this.responseText; }}; xhttp.open("GET", "/h/7", true); xhttp.send();}, 15000 );
  //setInterval(function ( ) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById("h8").innerHTML = this.responseText; }}; xhttp.open("GET", "/h/8", true); xhttp.send();}, 15000 );
  )rawliteral";

  // Replaces placeholder with DHT values
  String processor(const String& var) {
    // Serial.println(var);
    if(var == "N0") return String(DHT0_NAME);
    else if(var == "N1") return String(DHT1_NAME);
    else if(var == "N2") return String(DHT2_NAME);
    else if(var == "N3") return String(DHT3_NAME);
    else if(var == "N4") return String(DHT4_NAME);
    else if(var == "N5") return String(DHT5_NAME);
    else if(var == "N6") return String(DHT6_NAME);
    else if(var == "N7") return String(DHT7_NAME);
    else if(var == "N8") return String(DHT8_NAME);

    else if(var == "AVGL0") return String(DHT0_AVG_L);
    else if(var == "AVGL1") return String(DHT1_AVG_L);
    else if(var == "AVGL2") return String(DHT2_AVG_L);
    else if(var == "AVGL3") return String(DHT3_AVG_L);
    else if(var == "AVGL4") return String(DHT4_AVG_L);
    else if(var == "AVGL5") return String(DHT5_AVG_L);
    else if(var == "AVGL6") return String(DHT6_AVG_L);
    else if(var == "AVGL7") return String(DHT7_AVG_L);
    else if(var == "AVGL8") return String(DHT8_AVG_L);
    else if(var == "AVGH0") return String(DHT0_AVG_H);
    else if(var == "AVGH1") return String(DHT1_AVG_H);
    else if(var == "AVGH2") return String(DHT2_AVG_H);
    else if(var == "AVGH3") return String(DHT3_AVG_H);
    else if(var == "AVGH4") return String(DHT4_AVG_H);
    else if(var == "AVGH5") return String(DHT5_AVG_H);
    else if(var == "AVGH6") return String(DHT6_AVG_H);
    else if(var == "AVGH7") return String(DHT7_AVG_H);
    else if(var == "AVGH8") return String(DHT8_AVG_H);

    else if(var == "HIL0") return String(DHT0_HI_L);
    else if(var == "HIL1") return String(DHT1_HI_L);
    else if(var == "HIL2") return String(DHT2_HI_L);
    else if(var == "HIL3") return String(DHT3_HI_L);
    else if(var == "HIL4") return String(DHT4_HI_L);
    else if(var == "HIL5") return String(DHT5_HI_L);
    else if(var == "HIL6") return String(DHT6_HI_L);
    else if(var == "HIL7") return String(DHT7_HI_L);
    else if(var == "HIL8") return String(DHT8_HI_L);
    else if(var == "HIH0") return String(DHT0_HI_H);
    else if(var == "HIH1") return String(DHT1_HI_H);
    else if(var == "HIH2") return String(DHT2_HI_H);
    else if(var == "HIH3") return String(DHT3_HI_H);
    else if(var == "HIH4") return String(DHT4_HI_H);
    else if(var == "HIH5") return String(DHT5_HI_H);
    else if(var == "HIH6") return String(DHT6_HI_H);
    else if(var == "HIH7") return String(DHT7_HI_H);
    else if(var == "HIH8") return String(DHT8_HI_H);

    else if(var == "DISL0") return String(DHT0_DIS_L);
    else if(var == "DISL1") return String(DHT1_DIS_L);
    else if(var == "DISL2") return String(DHT2_DIS_L);
    else if(var == "DISL3") return String(DHT3_DIS_L);
    else if(var == "DISL4") return String(DHT4_DIS_L);
    else if(var == "DISL5") return String(DHT5_DIS_L);
    else if(var == "DISL6") return String(DHT6_DIS_L);
    else if(var == "DISL7") return String(DHT7_DIS_L);
    else if(var == "DISL8") return String(DHT8_DIS_L);
    else if(var == "DISH0") return String(DHT0_DIS_H);
    else if(var == "DISH1") return String(DHT1_DIS_H);
    else if(var == "DISH2") return String(DHT2_DIS_H);
    else if(var == "DISH3") return String(DHT3_DIS_H);
    else if(var == "DISH4") return String(DHT4_DIS_H);
    else if(var == "DISH5") return String(DHT5_DIS_H);
    else if(var == "DISH6") return String(DHT6_DIS_H);
    else if(var == "DISH7") return String(DHT7_DIS_H);
    else if(var == "DISH8") return String(DHT8_DIS_H);
    
    else if(var == "S0") return String(DHT0_ENABLED);
    else if(var == "S1") return String(DHT1_ENABLED);
    else if(var == "S2") return String(DHT2_ENABLED);
    else if(var == "S3") return String(DHT3_ENABLED);
    else if(var == "S4") return String(DHT4_ENABLED);
    else if(var == "S5") return String(DHT5_ENABLED);
    else if(var == "S6") return String(DHT6_ENABLED);
    else if(var == "S7") return String(DHT7_ENABLED);
    else if(var == "S8") return String(DHT8_ENABLED);
        
    else if(var == "T0") return String(temperature[0]);
    else if(var == "T1") return String(temperature[1]);
    else if(var == "T2") return String(temperature[2]);
    else if(var == "T3") return String(temperature[3]);
    else if(var == "T4") return String(temperature[4]);
    else if(var == "T5") return String(temperature[5]);
    else if(var == "T6") return String(temperature[6]);
    else if(var == "T7") return String(temperature[7]);
    else if(var == "T8") return String(temperature[8]);

    else if(var == "H0") return String(humidity[0]);
    else if(var == "H1") return String(humidity[1]);
    else if(var == "H2") return String(humidity[2]);
    else if(var == "H3") return String(humidity[3]);
    else if(var == "H4") return String(humidity[4]);
    else if(var == "H5") return String(humidity[5]);
    else if(var == "H6") return String(humidity[6]);
    else if(var == "H7") return String(humidity[7]);
    else if(var == "H8") return String(humidity[8]);
    
    return String();
  }

  void setup(){
    // Serial port for debugging purposes
    delay(1000);
    Serial.begin(115200);
    
    if (DHT0_ENABLED == true) { dht0.begin(); Serial.print("DHT0 sensor (Name: "); Serial.print(DHT0_NAME); Serial.print(" PIN: "); Serial.print(DHT0_PIN); Serial.println(") enabled."); }
    if (DHT1_ENABLED == true) { dht1.begin(); Serial.print("DHT1 sensor (Name: "); Serial.print(DHT1_NAME); Serial.print(" PIN: "); Serial.print(DHT1_PIN); Serial.println(") enabled."); }
    if (DHT2_ENABLED == true) { dht2.begin(); Serial.print("DHT2 sensor (Name: "); Serial.print(DHT2_NAME); Serial.print(" PIN: "); Serial.print(DHT2_PIN); Serial.println(") enabled."); }
    if (DHT3_ENABLED == true) { dht3.begin(); Serial.print("DHT3 sensor (Name: "); Serial.print(DHT3_NAME); Serial.print(" PIN: "); Serial.print(DHT3_PIN); Serial.println(") enabled."); }
    if (DHT4_ENABLED == true) { dht4.begin(); Serial.print("DHT4 sensor (Name: "); Serial.print(DHT4_NAME); Serial.print(" PIN: "); Serial.print(DHT4_PIN); Serial.println(") enabled."); }
    if (DHT5_ENABLED == true) { dht5.begin(); Serial.print("DHT5 sensor (Name: "); Serial.print(DHT5_NAME); Serial.print(" PIN: "); Serial.print(DHT5_PIN); Serial.println(") enabled."); }
    if (DHT6_ENABLED == true) { dht6.begin(); Serial.print("DHT6 sensor (Name: "); Serial.print(DHT6_NAME); Serial.print(" PIN: "); Serial.print(DHT6_PIN); Serial.println(") enabled."); }
    if (DHT7_ENABLED == true) { dht7.begin(); Serial.print("DHT7 sensor (Name: "); Serial.print(DHT7_NAME); Serial.print(" PIN: "); Serial.print(DHT7_PIN); Serial.println(") enabled."); }
    if (DHT8_ENABLED == true) { dht8.begin(); Serial.print("DHT8 sensor (Name: "); Serial.print(DHT8_NAME); Serial.print(" PIN: "); Serial.print(DHT8_PIN); Serial.println(") enabled."); }
    

    Serial.println();
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("----------------");
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/html", index_html, processor); });
    server.on("/reload.js", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "application/javascript", reload_js, processor); });

    server.on("/sensors", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "application/json", sensors_json, processor); });
    server.on("/discovery", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "application/json", zabbix_json, processor); });
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "image/gif", "GIF89a"); });
   
    
    server.on("/n/0", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", DHT0_NAME); });
    server.on("/n/1", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", DHT1_NAME); });
    server.on("/n/2", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", DHT2_NAME); });
    server.on("/n/3", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", DHT3_NAME); });
    server.on("/n/4", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", DHT4_NAME); });
    server.on("/n/5", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", DHT5_NAME); });
    server.on("/n/6", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", DHT6_NAME); });
    server.on("/n/7", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", DHT7_NAME); });
    server.on("/n/8", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", DHT8_NAME); });
    
    server.on("/t/0", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(temperature[0]).c_str()); });
    server.on("/t/1", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(temperature[1]).c_str()); });
    server.on("/t/2", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(temperature[2]).c_str()); });
    server.on("/t/3", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(temperature[3]).c_str()); });
    server.on("/t/4", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(temperature[4]).c_str()); });
    server.on("/t/5", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(temperature[5]).c_str()); });
    server.on("/t/6", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(temperature[6]).c_str()); });
    server.on("/t/7", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(temperature[7]).c_str()); });
    server.on("/t/8", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(temperature[8]).c_str()); });
    
    server.on("/h/0", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(humidity[0]).c_str()); });
    server.on("/h/1", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(humidity[1]).c_str()); });
    server.on("/h/2", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(humidity[2]).c_str()); });
    server.on("/h/3", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(humidity[3]).c_str()); });
    server.on("/h/4", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(humidity[4]).c_str()); });
    server.on("/h/5", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(humidity[5]).c_str()); });
    server.on("/h/6", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(humidity[6]).c_str()); });
    server.on("/h/7", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(humidity[7]).c_str()); });
    server.on("/h/8", HTTP_GET, [](AsyncWebServerRequest *request){ request->send_P(200, "text/plain", String(humidity[8]).c_str()); });
    
    server.begin();
  }

  void(* resetFunc) (void) = 0; 
 
  void loop(){
    unsigned long dhtcurrentMillis = millis();
    unsigned long rstcurrentMillis = millis();
    if (dhtcurrentMillis - dhtpreviousMillis >= dhtinterval) {
      dhtpreviousMillis = dhtcurrentMillis;
      Serial.println('-----------------------------');
      Serial.print("Begin DHT transaction ");
      Serial.println(dhtcurrentMillis);
      if (DHT0_ENABLED == true) { 
        float newT0 = dht0.readTemperature();
        if (isnan(newT0)) { Serial.print("Failed to read temperature from DHT0 sensor, name: "); Serial.println(DHT0_NAME); }
        else { temperature[0] = newT0; Serial.print(DHT0_NAME); Serial.print(" temperature: "); Serial.println(temperature[0]); }
        float newH0 = dht0.readHumidity();
        if (isnan(newH0)) { Serial.print("\tFailed to read humidity from DHT0 sensor, name:    "); Serial.println(DHT0_NAME); }
        else { humidity[0] = newH0; Serial.print(DHT0_NAME); Serial.print(" humidity:    "); Serial.println(humidity[0]); }
      }
      if (DHT1_ENABLED == true) { 
        float newT1 = dht1.readTemperature();
        if (isnan(newT1)) { Serial.print("Failed to read temperature from DHT1 sensor, name: "); Serial.println(DHT1_NAME); }
        else { temperature[1] = newT1; Serial.print(DHT1_NAME); Serial.print(" temperature: "); Serial.println(temperature[1]); }
        float newH1 = dht1.readHumidity();
        if (isnan(newH1)) { Serial.print("Failed to read humidity from DHT1 sensor, name:    "); Serial.println(DHT1_NAME); }
        else { humidity[1] = newH1; Serial.print(DHT1_NAME); Serial.print(" humidity:    "); Serial.println(humidity[1]); }
      }
      if (DHT2_ENABLED == true) { 
        float newT2 = dht2.readTemperature();
        if (isnan(newT2)) { Serial.print("Failed to read temperature from DHT2 sensor, name: "); Serial.println(DHT2_NAME); }
        else { temperature[2] = newT2; Serial.print(DHT2_NAME); Serial.print(" temperature: "); Serial.println(temperature[2]); }
        float newH2 = dht2.readHumidity();
        if (isnan(newH2)) { Serial.print("Failed to read humidity from DHT2 sensor, name:    "); Serial.println(DHT2_NAME); }
        else { humidity[2] = newH2; Serial.print(DHT2_NAME); Serial.print(" humidity:    "); Serial.println(humidity[2]); }
      }
      if (DHT3_ENABLED == true) { 
        float newT3 = dht3.readTemperature();
        if (isnan(newT3)) { Serial.print("Failed to read temperature from DHT3 sensor, name: "); Serial.println(DHT3_NAME); }
        else { temperature[3] = newT3; Serial.print(DHT3_NAME); Serial.print(" temperature: "); Serial.println(temperature[3]); }
        float newH3 = dht3.readHumidity();
        if (isnan(newH3)) { Serial.print("Failed to read humidity from DHT3 sensor, name:    "); Serial.println(DHT3_NAME); }
        else { humidity[3] = newH3; Serial.print(DHT3_NAME); Serial.print(" humidity:    "); Serial.println(humidity[3]); }
      }
      if (DHT4_ENABLED == true) { 
        float newT4 = dht4.readTemperature();
        if (isnan(newT4)) { Serial.print("Failed to read temperature from DHT4 sensor, name: "); Serial.println(DHT4_NAME); }
        else { temperature[4] = newT4; Serial.print(DHT4_NAME); Serial.print(" temperature: "); Serial.println(temperature[4]); }
        float newH4 = dht4.readHumidity();
        if (isnan(newH4)) { Serial.print("Failed to read humidity from DHT4 sensor, name:    "); Serial.println(DHT4_NAME); }
        else { humidity[4] = newH4; Serial.print(DHT4_NAME); Serial.print(" humidity:    "); Serial.println(humidity[4]); }
      }
      if (DHT5_ENABLED == true) { 
        float newT5 = dht5.readTemperature();
        if (isnan(newT5)) { Serial.print("Failed to read temperature from DHT5 sensor, name: "); Serial.println(DHT5_NAME); }
        else { temperature[5] = newT5; Serial.print(DHT5_NAME); Serial.print(" temperature: "); Serial.println(temperature[5]); }
        float newH5 = dht5.readHumidity();
        if (isnan(newH5)) { Serial.print("Failed to read humidity from DHT5 sensor, name:    "); Serial.println(DHT5_NAME); }
        else { humidity[5] = newH5; Serial.print(DHT5_NAME); Serial.print(" humidity:    "); Serial.println(humidity[5]); }
      }
      if (DHT6_ENABLED == true) { 
        float newT6 = dht6.readTemperature();
        if (isnan(newT6)) { Serial.print("Failed to read temperature from DHT6 sensor, name: "); Serial.println(DHT6_NAME); }
        else { temperature[6] = newT6; Serial.print(DHT6_NAME); Serial.print(" temperature: "); Serial.println(temperature[6]); }
        float newH6 = dht6.readHumidity();
        if (isnan(newH6)) { Serial.print("Failed to read humidity from DHT6 sensor, name:    "); Serial.println(DHT6_NAME); }
        else { humidity[6] = newH6; Serial.print(DHT6_NAME); Serial.print(" humidity:    "); Serial.println(humidity[6]); }
      }
      if (DHT7_ENABLED == true) { 
        float newT7 = dht7.readTemperature();
        if (isnan(newT7)) { Serial.print("Failed to read temperature from DHT7 sensor, name: "); Serial.println(DHT7_NAME); }
        else { temperature[7] = newT7; Serial.print(DHT7_NAME); Serial.print(" temperature: "); Serial.println(temperature[7]); }
        float newH7 = dht7.readHumidity();
        if (isnan(newH7)) { Serial.print("Failed to read humidity from DHT7 sensor, name:    "); Serial.println(DHT7_NAME); }
        else { humidity[7] = newH7; Serial.print(DHT7_NAME); Serial.print(" humidity:    "); Serial.println(humidity[7]); }
      }
      if (DHT8_ENABLED == true) { 
        float newT8 = dht8.readTemperature();
        if (isnan(newT8)) { Serial.print("Failed to read temperature from DHT8 sensor, name: "); Serial.println(DHT8_NAME); }
        else { temperature[8] = newT8; Serial.print(DHT8_NAME); Serial.print(" temperature: "); Serial.println(temperature[8]); }
        float newH8 = dht8.readHumidity();
        if (isnan(newH8)) { Serial.print("Failed to read humidity from DHT8 sensor, name:    "); Serial.println(DHT8_NAME); }
        else { humidity[8] = newH8; Serial.print(DHT8_NAME); Serial.print(" humidity:    "); Serial.println(humidity[8]); }
      }
      Serial.print("End DHT transaction ");
      Serial.println(dhtcurrentMillis);
      Serial.println('-----------------------------');
    }
    if (RESET == true) { 
      if (rstcurrentMillis - rstpreviousMillis >= rstinterval) {
        rstpreviousMillis = rstcurrentMillis;
        Serial.println("Reseting... ");
        delay(100);
        resetFunc();
        delay(100);
        Serial.println("NeverGonnaGiveYouUp");
      }
    }
  }

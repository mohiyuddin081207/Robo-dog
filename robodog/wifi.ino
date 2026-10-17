#include "wifi.h"
#include <Arduino.h>

// --- Visual Feedback UI (Flashes blue/cyan when clicked) ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head>
<meta name="viewport" content="width=device-width, initial-scale=1, user-select=none">
<style>
  body { font-family: Arial, sans-serif; text-align: center; background: #1a1a1a; color: #ffffff; margin: 0; padding: 20px; }
  .mode-select { width: 90%; max-width: 400px; height: 50px; font-size: 18px; font-weight: bold; margin-bottom: 25px; border-radius: 8px; background: #007bff; color: white; border: none; }
  
  /* Standard Button Style */
  .btn { 
    width: 80px; 
    height: 80px; 
    font-size: 22px; 
    margin: 5px; 
    border-radius: 15px; 
    background: #333; 
    color: white; 
    border: 2px solid #555; 
    transition: background 0.1s ease; /* Makes the color fade out smoothly */
    -webkit-tap-highlight-color: transparent; /* Removes the ugly default mobile gray box */
  }

  /* FIX: The moment you click or hold the button, it instantly flashes bright blue! */
  .btn:active { 
    background: #007bff !important; 
    border-color: #00cbcb !important; 
    color: #ffffff !important;
  } 

  .ui-box { background: #262626; padding: 20px; border-radius: 15px; box-shadow: 0 4px 10px rgba(0,0,0,0.3); margin-top: 10px; max-width: 400px; margin-left: auto; margin-right: auto; }
  .hidden { display: none; } 
  h3 { margin-top: 0; color: #007bff; }
  .slider-container { margin: 15px 0; text-align: left; }
  .slider-container label { display: inline-block; width: 30px; font-weight: bold; }
  .slider { width: 80%; height: 15px; border-radius: 5px; background: #444; outline: none; }
</style></head>
<body>
  <h1>Quadruped Commander</h1>
  
  <select class="mode-select" id="modeDropdown" onchange="switchMode(this.value)">
    <option value="0">Normal Mode (Hold to Walk)</option>
    <option value="1">Position Mode (XYZ Stance Sliders)</option>
  </select>
  
  <div id="normal-ui" class="ui-box">
    <h3>Normal Mode (Gait)</h3>
    <button class="btn" ontouchstart="send('/move?d=F')" ontouchend="send('/move?d=S')" onmousedown="send('/move?d=F')" onmouseup="send('/move?d=S')">▲</button><br>
    <button class="btn" ontouchstart="send('/move?d=L')" ontouchend="send('/move?d=S')" onmousedown="send('/move?d=L')" onmouseup="send('/move?d=S')">◀</button>
    <button class="btn" onclick="send('/move?d=S')">■</button>
    <button class="btn" ontouchstart="send('/move?d=R')" ontouchend="send('/move?d=S')" onmousedown="send('/move?d=R')" onmouseup="send('/move?d=S')">▶</button><br>
    <button class="btn" ontouchstart="send('/move?d=B')" ontouchend="send('/move?d=S')" onmousedown="send('/move?d=B')" onmouseup="send('/move?d=S')">▼</button>
  </div>

  <div id="position-ui" class="ui-box hidden">
    <h3>Position Mode (Stance)</h3>
    <div class="slider-container">
      <label>X:</label> <input type="range" min="0" max="10" step="0.1" value="5" class="slider" oninput="updateSliders()">
    </div>
    <div class="slider-container">
      <label>Y:</label> <input type="range" min="-5" max="5" step="0.1" value="0" class="slider" oninput="updateSliders()">
    </div>
    <div class="slider-container">
      <label>Z:</label> <input type="range" min="1" max="6" step="0.1" value="3" class="slider" oninput="updateSliders()">
    </div>
  </div>

  <script>
    function send(path) { 
      var xhr = new XMLHttpRequest(); 
      xhr.open("GET", path, true); 
      xhr.send(); 
    }

    function switchMode(val) {
      send('/changeMode?m=' + val);
      if (val == "0") {
        document.getElementById('normal-ui').classList.remove('hidden');
        document.getElementById('position-ui').classList.add('hidden');
      } else {
        document.getElementById('normal-ui').classList.add('hidden');
        document.getElementById('position-ui').classList.remove('hidden');
      }
    }

    // Fixed typo in previous version where "onmousedown" was missing its "on" prefix
    function updateSliders() {
      var sliders = document.getElementsByClassName('slider');
      var x = sliders[0].value;
      var y = sliders[1].value;
      var z = sliders[2].value;
      send('/set?x=' + x + '&y=' + y + '&z=' + z);
    }
  </script>
</body></html>)rawliteral";

// --- The Rest of Your Original Server Functions Remain Exactly the Same ---
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleChangeMode() {
  if (server.hasArg("m")) {
    robotMode = server.arg("m").toInt();
    Serial.print("Mode switched to: ");
    Serial.println(robotMode == 0 ? "Normal Walking" : "Position Stance");
  }
  server.send(200, "text/plain", "OK");
}

void handleMove() {
  // unsigned long currentTime = millis();
  // unsigned long progress = currentTime - webStepStartTime;

  // if (progress >= 400) {
    if (robotMode == 0) { 
      String dir = server.arg("d");
      if (dir == "F") targetX += 2.5;
      if (dir == "B") targetX -= 2.5;
      if (dir == "L") targetY -= 0.5;
      if (dir == "R") targetY += 0.5;
      if (dir == "S") { targetX = presentX; targetY =presentY; }
Serial.println(targetX);

    }
  //   webStepStartTime = currentTime; 
  // }
  server.send(200, "text/plain", "OK");
}

void handleSet() {
  if (robotMode == 1) { 
    if (server.hasArg("x")) targetX = server.arg("x").toFloat();
    if (server.hasArg("y")) targetY = server.arg("y").toFloat();
    if (server.hasArg("z")) targetZ = server.arg("z").toFloat();
  }
  server.send(200, "text/plain", "OK");
}

void setupWiFi() {
  WiFi.softAP("Robot_Brain", "12345678");
  
  server.on("/", handleRoot);
  server.on("/changeMode", handleChangeMode);
  server.on("/move", handleMove);
  server.on("/set", handleSet);
  
  server.begin();
}
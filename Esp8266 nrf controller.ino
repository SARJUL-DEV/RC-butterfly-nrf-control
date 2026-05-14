#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <RF24.h>

// RF24 pins
RF24 radio(D2, D1);
const byte addr[6] = "00001";

ESP8266WebServer server(80);

const char* ssid = "ESP-REMOTE";
const char* password = "12345678";

// Control structure (packed)
struct __attribute__((packed)) ControlData {
  float L;
  float R;
  uint8_t toggle;
};

ControlData ctrl;

// smoothing
float smoothL = 0;
float smoothR = 0;

bool autoStabilize = false;

unsigned long lastSend = 0;
bool lastSendStatus = false;

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  delay(100);

  radio.begin();
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setRetries(2, 3);

  radio.openWritingPipe(addr);
  radio.stopListening();

  if (!radio.isChipConnected()) {
    Serial.println("RF24 NOT CONNECTED");
  }

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/toggle", handleToggle);
  server.on("/status", handleStatus);

  server.begin();
}

// ===== LOOP =====
void loop() {
  server.handleClient();

  // Fast but controlled RF sending
  if (millis() - lastSend > 25) {

    lastSendStatus = radio.write(&ctrl, sizeof(ctrl));

    if (!lastSendStatus) {
      radio.flush_tx();
    }

    lastSend = millis();
  }
}

// ===== HANDLE DATA =====
void handleData() {
  if (server.hasArg("l")) {
    float newL = server.arg("l").toFloat();
    smoothL = (smoothL * 0.6) + (newL * 0.4);
    ctrl.L = smoothL;
  }

  if (server.hasArg("r")) {
    float newR = server.arg("r").toFloat();
    smoothR = (smoothR * 0.6) + (newR * 0.4);
    ctrl.R = smoothR;
  }

  ctrl.toggle = autoStabilize;

  server.send(200, "text/plain", "OK");
}

// ===== TOGGLE =====
void handleToggle() {
  autoStabilize = !autoStabilize;
  ctrl.toggle = autoStabilize;

  server.send(200, "text/plain", autoStabilize ? "ON" : "OFF");
}

// ===== STATUS =====
void handleStatus() {
  String msg;

  msg += lastSendStatus ? "CONNECTED\n" : "ERROR\n";
  msg += "L: " + String(ctrl.L) + "\n";
  msg += "R: " + String(ctrl.R) + "\n";
  msg += "AUTO: " + String(ctrl.toggle);

  server.send(200, "text/plain", msg);
}

// ===== UI =====
void handleRoot() {

String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { font-family:sans-serif; text-align:center; margin:0; touch-action:none; }

.container { display:flex; justify-content:center; gap:20px; margin-top:40px; }

.joy { width:120px; height:300px; background:#ddd; position:relative; border-radius:20px; }
.handle { width:100%; height:60px; background:#333; position:absolute; bottom:0; border-radius:20px; }

#toggleBtn { position:fixed; top:10px; right:10px; }

#statusBox {
 width:200px;
 margin:auto;
 margin-top:20px;
 padding:10px;
 border-radius:10px;
 background:black;
 color:lime;
 font-size:14px;
 text-align:left;
 white-space:pre;
}
</style>
</head>

<body>

<button id="toggleBtn" onclick="toggle()">OFF</button>

<h3>ESP Controller</h3>

<div id="statusBox">WAIT...</div>

<div class="container">
<div class="joy" id="joyL"><div class="handle" id="hL"></div></div>
<div class="joy" id="joyR"><div class="handle" id="hR"></div></div>
</div>

<script>

let valL=0,valR=0,lastSend=0;

function sendData(){
 let now=Date.now();
 if(now-lastSend<80) return;
 lastSend=now;
 fetch(`/data?l=${valL}&r=${valR}`);
}

function setupJoystick(j,h,s){

 let id=null;

 j.addEventListener("touchstart",e=>{
  if(id===null) id=e.changedTouches[0].identifier;
 });

 j.addEventListener("touchmove",e=>{
  e.preventDefault();
  let r=j.getBoundingClientRect();

  for(let t of e.touches){
   if(t.identifier===id){

    let y=t.clientY-r.top;
    y=Math.max(0,Math.min(r.height,y));

    let percent=1-(y/r.height);
    let value=Math.round(percent*255);

    h.style.top=(y-30)+"px";

    if(s=="L") valL=value;
    else valR=value;

    sendData();
   }
  }
 },{passive:false});

 j.addEventListener("touchend",e=>{
  if(e.changedTouches[0].identifier===id) id=null;
 });

}

setupJoystick(joyL,hL,"L");
setupJoystick(joyR,hR,"R");

setInterval(()=>{
 fetch("/status").then(r=>r.text()).then(t=>{
  statusBox.innerText = t;
 });
},800);

function toggle(){
 fetch("/toggle").then(r=>r.text()).then(t=>{
  toggleBtn.innerText=t;
 });
}

</script>

</body>
</html>
)rawliteral";

server.send(200,"text/html",html);
}
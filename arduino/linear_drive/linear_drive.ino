// linear_drive

#define WEB   // Enable Web control interface
#define STA   // Enable Wifi Station mode to connect to exisint network (else use AP mode)

#include <stdint.h>
// for AVR only #include <AVRStepperPins.h>
#include <Arduino.h>
#include <common.h>

#ifdef WEB
  // For web interface
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include "SPIFFS.h"
  #include <Arduino_JSON.h>
#endif

#include <FastAccelStepper.h>
#include <PoorManFloat.h>
#include <RampCalculator.h>
#include <RampGenerator.h>
#include <StepperISR.h>

// Put network info here:
#include "./network.h"

// Defines
#define PITCH_DIAM_MM   30
#define STEPS_REV       200
#define USTEPS          32L        // number of micro-steps per step

const int ledPin = 2;   // Hitlego module
//const int ledPin = 16;  // Elegoo module
const int pulsePin = 23;
const int dirPin  = 22;
const int enaPin = 21;
const int limitBackPin = 36;
const int limitFwdPin = 39;
const int speedAdcPin = 34;
const int strokeAdcPin = 35;

int dirFwd = true;
int runState = false;
int usePots = true;
int useWeb = false;
long stepSpeed = 125 * USTEPS;
long stepSpeedMax = 4000 * USTEPS;
long stepAcc = 125 * USTEPS;
long stepAccMax = 40000 * USTEPS;   // this really depends on weight of payload
long stroke = 125 * USTEPS;
long strokeMax = 160 * USTEPS;
long strokeOffset = 0 * USTEPS;
long startTime, deltaTime;

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

void homeStepper();
long setSpeedAcc(long newStepSpeed);


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WEB

  const char* sta_hostap_ssid = STA_HOSTAP_SSID;
  const char* sta_hostap_pwd = STA_HOSTAP_PWD;
  
  const char* ap_ssid = AP_SSID;
  const char* ap_pwd = AP_PWD;
  IPAddress Ip = AP_IPADDR;
  IPAddress NMask = AP_NETMASK;
  
  // Create AsyncWebServer object on port 80
  AsyncWebServer server(80);
  // Create a WebSocket object
  
  AsyncWebSocket ws("/ws");
  // Set LED GPIO
  const int ledPin1 = 12;
  const int ledPin2 = 13;
  const int ledPin3 = 14;
  
  String message = "";
  String sliderValue1 = "0";
  String sliderValue2 = "0";
  String sliderValue3 = "0";
  
  int dutyCycle1;
  int dutyCycle2;
  int dutyCycle3;
  
  // setting PWM properties
  const int freq = 5000;
  const int ledChannel1 = 0;
  const int ledChannel2 = 1;
  const int ledChannel3 = 2;
  
  const int resolution = 8;
  
  //Json Variable to Hold Slider Values
  JSONVar sliderValues;
  
  //Get Slider Values
  String getSliderValues(){
    sliderValues["sliderValue1"] = String(sliderValue1);
    sliderValues["sliderValue2"] = String(sliderValue2);
    sliderValues["sliderValue3"] = String(sliderValue3);
  
    String jsonString = JSON.stringify(sliderValues);
    return jsonString;
  }
  
  // Initialize SPIFFS
  void initFS() {
    if (!SPIFFS.begin()) {
      Serial.println("An error has occurred while mounting SPIFFS");
    }
    else{
     Serial.println("SPIFFS mounted successfully");
    }
  }
  
  // Initialize WiFi
 void initWiFi() {
    int i = 0;
#ifdef STA
    WiFi.mode(WIFI_STA);
    WiFi.begin(sta_hostap_ssid, sta_hostap_pwd);
    Serial.print("Connecting to WiFi ..");
    while ((i < 15) && (WiFi.status() != WL_CONNECTED)) {
      Serial.print('.');
      delay(1000);
      i++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Connected to AP ");
      Serial.println(sta_hostap_ssid);
      Serial.print("AP assigned IP ");
      Serial.println(WiFi.localIP());
    }
    else {
      Serial.print("STA mode could not connect to AP ");
      Serial.println(sta_hostap_ssid);
      Serial.println("Falling back to AP mode...");

      // fall back to AP mode
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ap_ssid, ap_pwd);
      delay(100);
      WiFi.softAPConfig(Ip, Ip, NMask);
      delay(100);
      Serial.print("Started AP SSID ");
      Serial.println(ap_ssid);
      Serial.print("Static IP ");
      Serial.println(WiFi.softAPIP());      
    }
#else
    // WIFI AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_pwd);
    delay(100);
    WiFi.softAPConfig(Ip, Ip, NMask);
    delay(100);
    Serial.print("Started AP SSID ");
    Serial.println(ap_ssid);
    Serial.print("Static IP ");
    Serial.println(WiFi.softAPIP());
#endif    
  }
  
  void notifyClients(String sliderValues) {
    ws.textAll(sliderValues);
  }
  
  void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      message = (char*)data;
      if (message.indexOf("1s") >= 0) {
        sliderValue1 = message.substring(2);
        dutyCycle1 = map(sliderValue1.toInt(), 0, 100, 0, 255);
        Serial.println(dutyCycle1);
        Serial.print(getSliderValues());
        notifyClients(getSliderValues());
      }
      if (message.indexOf("2s") >= 0) {
        sliderValue2 = message.substring(2);
        dutyCycle2 = map(sliderValue2.toInt(), 0, 100, 0, 255);
        Serial.println(dutyCycle2);
        Serial.print(getSliderValues());
        notifyClients(getSliderValues());
      }    
      if (message.indexOf("3s") >= 0) {
        sliderValue3 = message.substring(2);
        dutyCycle3 = map(sliderValue3.toInt(), 0, 100, 0, 255);
        Serial.println(dutyCycle3);
        Serial.print(getSliderValues());
        notifyClients(getSliderValues());
      }
      if (strcmp((char*)data, "getValues") == 0) {
        notifyClients(getSliderValues());
      }
    }
  }
  void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        usePots = false;
        useWeb = true;
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
    }
  }
  
  void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
  }

#endif  // ifdef WEB
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {

  // Limit Switches
  pinMode(limitBackPin, INPUT_PULLUP);
  pinMode(limitFwdPin, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("*** Hello from linear_drive! ***");
  Serial.println("");
  Serial.println("");
  Serial.println("");

#ifdef WEB
  initFS();
  initWiFi();
  initWebSocket();
  
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.serveStatic("/", SPIFFS, "/");

  // Start server
  server.begin();
#endif

   engine.init();
   engine.setDebugLed(ledPin);
   stepper = engine.stepperConnectToPin(pulsePin);

   if (stepper) {
      stepper->setDirectionPin(dirPin, true);
      stepper->setEnablePin(enaPin, true);
      stepper->setAutoEnable(false);
      // If auto enable/disable needs, just add (one or both):
      stepper->setDelayToEnable(50);
      stepper->setDelayToDisable(1000);

      stepSpeed = 125 * USTEPS;
      stepper->setSpeedInHz(stepSpeed);       // steps/s
      stepper->setAcceleration(stepSpeed);  // steps/s²

      Serial.println("Stepper alignment starting...");
      homeStepper();
   } else {
      Serial.println("ERROR - Create stepper failed!");    
      while(true){};
   }

    engine.setDebugLed(ledPin);
    startTime = millis();
}

////////////////////////////////////////////////////////////////////////////////////
// put your main code here, to run repeatedly
////////////////////////////////////////////////////////////////////////////////////
void loop() {


    // Check Limit Switches
    if (!digitalRead(limitBackPin)) {
        stepper->forceStopAndNewPosition(-USTEPS * 10);  // stops stepper the fastest
        Serial.println("Backward Limit Switch Tripped!");
        delay(200);        
        stepper->move(10*USTEPS); // back off
        delay(200);         // give it time to release limit sw
        homeStepper();
        runState = false;
    }
    else if (!digitalRead(limitFwdPin)) {
        stepper->forceStopAndNewPosition(strokeMax + USTEPS * 10);  // stops stepper the fastest, call this new spot past the end
        Serial.println("Forward Limit Switch Tripped!");
        delay(200);
        stepper->move(-10*USTEPS); // back off
        delay(200);         // give it time to release limit sw
        homeStepper();
        runState = false;
    }
      
    if (runState && !stepper->isRunning()) {
      if (dirFwd)
        stepper->moveTo(stroke);
      else
        stepper->moveTo(strokeOffset);
      dirFwd = !dirFwd;
      deltaTime = millis() - startTime;
      startTime = millis();
//        Serial.println(deltaTime);
//        delay(1);
    }

    // Check control knobs if enabled
    if (usePots) {
      strokeOffset = 0;
      readSpeedPot();
      readStrokePot();        
    }

    // Check web interface if enabled
    if (useWeb) {
      stroke = strokeMax *  dutyCycle2 / 255L; 
      if (stroke < (3 * USTEPS)) stroke = 3 * USTEPS;
      
      strokeOffset = strokeMax *  dutyCycle3 / 255L;
      if (strokeOffset > (stroke - 3 * USTEPS))
        strokeOffset = stroke - 3 * USTEPS;
        
      if (dutyCycle1 > 1) {
        setSpeedAcc(dutyCycle1 * stepSpeedMax / 511L);
        runState = true;          
      }
      else {
        if (runState)
          homeStepper();
        runState = false;          
      }        
    }

    // Check for serial interface commands
    checkSerial();

#ifdef WEB
  ws.cleanupClients();
#endif    
}

/////////////////////////////////////////////////////////////////////
// Bring linear drive to resting "home" position
/////////////////////////////////////////////////////////////////////
void homeStepper() {

    // gracefull stop 1st
    stepper->stopMove();
    while (stepper->isRunning()) {};
    stepper->enableOutputs();
    stepper->setAcceleration(250 * USTEPS);      // steps/s/s
    stepper->setSpeedInHz(125 * USTEPS);       // steps/s
    stepper->move((-strokeMax - 20) * USTEPS);
 
    while (digitalRead(limitBackPin)) {
        if (!digitalRead(limitFwdPin)) {
          stepper->forceStopAndNewPosition(strokeMax + 20 * USTEPS);
          Serial.println("Forward limit switch tripped while reversing!");
          stepper->disableOutputs();
          while(true){};
        }      
    }
    // Tripped reverse stop 
    stepper->forceStopAndNewPosition(-10 * USTEPS);  // stops stepper the fastest
    delay(100);    

    // Move forward until switch clears
    stepper->move(50 * USTEPS);    
    while (!digitalRead(limitBackPin)) {
    }
    stepper->forceStopAndNewPosition(-2 * USTEPS);  // call this spot "-2"    
    delay(50);
    stepper->moveTo(0);
    Serial.println("Stepper home finished.");
}

/////////////////////////////////////////////////////////////////
// Sets travel movement speed limit and accelleration
// Accelleration is set to reach speed limit in 1/3 of stroke
// So movement will consist of 3 equal distance phases:
// 1. Linear accelleration to speed
// 2. Coast at speed
// 3. Deaccelleration to stop
/////////////////////////////////////////////////////////////////
long setSpeedAcc(long newStepSpeed) {

  long distance = stroke - strokeOffset;
  stepSpeed = newStepSpeed;
  
  if (stepSpeed < 100 * USTEPS)
    stepSpeed = 100 * USTEPS;
  
  if (stepSpeed > stepSpeedMax)
    stepSpeed = stepSpeedMax;
     
  // set accelleration based on kinematic eq Vf^2 = 2*a*d => a = v^2 /(2*d))
  // The value of "a" will reach full velocity at 1/3 of stroke is:
  //      a = v^2 /(2*d/3))
  // Scaled to avoid integer overflow in square => a = 128 * (v/16)^2 * (d/3)
  stepAcc = 128L * (((stepSpeed/16)*(stepSpeed/16)) / (distance/3));
  if (stepAcc > stepAccMax)
    stepAcc = stepAccMax;
  else if (stepAcc < 125 * USTEPS)
    stepAcc = 125 * USTEPS;
  stepper->setAcceleration(stepAcc);      // steps/s/s
  stepper->setSpeedInHz(stepSpeed);       // steps/s
  
  // this messes up moves in progress and hits limit sw!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // stepper->applySpeedAcceleration();        // Apply don't wait for next move command
  
//  Serial.print("stepSpeed ");
//  Serial.println(stepSpeed);
//  Serial.print("stepAcc ");
//  Serial.println(stepAcc);
  
  return stepSpeed;
}

/////////////////////////////////////////////////////////////////
long readSpeedPot() {  

  static long speedPotReadingAvg = 0;
  static long speedPotReadingLast = 0;
  long speedPotReading;
  
  speedPotReading = analogRead(speedAdcPin);
  speedPotReadingAvg = (speedPotReading + 127UL * speedPotReadingAvg) / 128UL;
  if (std::abs(speedPotReadingAvg - speedPotReadingLast) > 15)
  {
//    Serial.print("speedPotReadingAvg ");
//    Serial.println(speedPotReadingAvg);
    speedPotReadingLast = speedPotReadingAvg;
  
    if (speedPotReadingAvg > 50) {
      setSpeedAcc(speedPotReadingAvg * USTEPS / 2);
      runState = true;          
    }
    else {
      if (runState)
        homeStepper();
      runState = false;          
    }        
  }
  return speedPotReadingAvg;
}

/////////////////////////////////////////////////////////////////
long readStrokePot() {  

  static long strokePotReadingAvg = 0;
  static long strokePotReadingLast = 0;
  long strokePotReading;
  
  strokePotReading = analogRead(strokeAdcPin);
  strokePotReadingAvg = (strokePotReading + 127UL * strokePotReadingAvg) / 128UL;
  if (std::abs(strokePotReadingAvg - strokePotReadingLast) > 1)
  {
//    Serial.print("strokePotReadingAvg ");
//    Serial.println(strokePotReadingAvg);
    strokePotReadingLast = strokePotReadingAvg;
    stroke = strokeMax * strokePotReadingAvg / 4096L; 
    if (stroke < (3 * USTEPS)) stroke = 3 * USTEPS;
    // If stroke is changed we need to recompute accelleration
    // Call with current speed will recalculate acc for new stroke
    setSpeedAcc(stepSpeed);

    Serial.print("stroke ");
    Serial.println(stroke);
  }

  // vibration mode
//  if (stroke <= (5 * USTEPS)) {
//    setSpeedAcc(stepSpeedMax);
//  }

  return strokePotReadingAvg;
}

/////////////////////////////////////////////////////////////////
int checkSerial() {
  if (Serial.available()) {
    char rxc[2];
    Serial.readBytes(rxc,1);
    
    switch(rxc[0]) {
      case '1':  stepSpeed = 125 * USTEPS;
            runState = true;
          break;
      case '2':  stepSpeed = 185 * USTEPS;
            runState = true;
          break;
      case '3':  stepSpeed = 280 * USTEPS;
            runState = true;
          break;
      case '4':  stepSpeed = 425 * USTEPS;
            runState = true;
          break;
      case '5':  stepSpeed = 635 * USTEPS;
            runState = true;
          break;
      case '6':  stepSpeed = 950 * USTEPS;
            runState = true;
          break;
      case '7':  stepSpeed = 1425 * USTEPS;
            runState = true;
          break;
      case '8':  stepSpeed = 2130 * USTEPS;
            runState = true;
          break;
      case '9':  stepSpeed = 3200 * USTEPS;
            runState = true;
          break;
      case '0':  stepSpeed = 125 * USTEPS;            
            stepper->moveTo(0);
            runState = false;
            dirFwd = true;
          break;
      case 'h':  stepSpeed = 125 * USTEPS;
            runState = false;
            dirFwd = true;
            homeStepper();
          break;
      case 's':  stroke = 100 * USTEPS;
          break;
      case 'm':  stroke = 125 * USTEPS;
          break;
      case 'l':  stroke = strokeMax;
          break;
      case 'v':  stroke = USTEPS;
          break;
      case 'x':  stroke = strokeMax + 500;   // designed to overextend
          break;
      case '+':  stepSpeed += 125 * USTEPS;
          break;
      case '-':  stepSpeed -= 125 * USTEPS;
          break;
      case '>':  stroke += USTEPS;
          break;
      case '<':  stroke -= USTEPS;
          break;
      case 'p':  usePots = !usePots;
            if (usePots) Serial.println("Pots enabled");
            else Serial.println("Pots disabled");
          break;
      default:  break;
    }
    if (stroke < USTEPS) stroke = USTEPS;
    else if (stroke > 200 * USTEPS) stroke = 200 * USTEPS;
    setSpeedAcc(stepSpeed);
    return true;
  } else {
    return false;    
  }
}

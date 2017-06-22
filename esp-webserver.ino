#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <DHT.h>
#include <CircularBuffer.h>

//Temperature
//Sensor:       DHT11
//Max Value:    50.00 *C
//Min Value:    0.00 *C
//Resolution:   2.00 *C
//------------------------------------
//Humidity
//Sensor:       DHT11
//Max Value:    80.00%
//Min Value:    20.00%
//Resolution:   5.00%

#define DHTTYPE DHT11   // Sensor type DHT11/21/22/AM2301/AM2302
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define CONTENT_TYPE_TEXT_PLAIN "text/plain"
#define CONTENT_TYPE_TEXT_HTML "text/html"
#define CONTENT_TYPE_JSON "application/json"

const char *ssid = "myssid";
const char *password = "****";

ESP8266WebServer server ( 80 );

const int led = 13;

// Initialize DHT sensor.
DHT dht(D4, DHTTYPE);

const unsigned int wInterval = 2 * 60 * 60 * 1000UL; // week interval each 2 hours
const unsigned int mInterval = 8 * 60 * 60 * 1000UL; // month interval each 8 hours
const unsigned int askInterval = 5 * 60 * 1000UL; // ask sensors every 5 minutes and calculate average for week and month interavals
//const unsigned int wInterval = 10 * 1000UL; // week interval each 2 hours
//const unsigned int mInterval = 30 * 1000UL; // month interval each 8 hours
//const unsigned int askInterval = 5 * 1000UL; // ask sensors every 5 minutes and calculate average for week and month interavals
static unsigned long askLastTime = - askInterval;
static unsigned long wLastTime = - wInterval;
static unsigned long mLastTime = - mInterval;

typedef struct {
  float temp;
  float hum;
  float light;
} record_type;

const byte bufSize = 100; // store 100 values for weeks and months
static CircularBuffer <record_type, bufSize> w;
static CircularBuffer <record_type, bufSize> m;
static record_type lastSuccess;

//format bytes
String formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return CONTENT_TYPE_TEXT_PLAIN;
}

bool handleFileRead(String path){
  Serial.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleRoot() {
  if(!handleFileRead("/"))
    handleNotFound();
  digitalWrite ( led, 0 );
}

void handleNotFound() {
  digitalWrite ( led, 1 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, CONTENT_TYPE_TEXT_PLAIN, message );
  digitalWrite ( led, 0 );
}

void routes() {
  server.on ( "/", handleRoot );
  server.on ( "/temp", actionTemp );
  server.on ( "/light", actionLight );

  server.on ( "/week", actionWeek );
  server.on ( "/month", actionMonth );

  server.on ( "/connectionInfo", []() {
    String out = "{\"ip\":\"";
    out += WiFi.localIP().toString();
    out += "\",\"ssid\":\"";
    out += ssid;
    out += "\"}";
    server.send ( 200, CONTENT_TYPE_JSON, out );
  } );
  
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      handleNotFound();
  });
}

void setup ( void ) {
  Serial.begin ( 115200 );
  pinMode ( led, OUTPUT );
  digitalWrite ( led, 0 );
  WiFi.begin ( ssid, password );

  dht.begin();
  SPIFFS.begin();

  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }

  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  routes();

  server.begin();
  Serial.println ( "HTTP server started" );
}

record_type readRecord() {
  record_type rec;
  rec.temp = dht.readTemperature();
  rec.hum = dht.readHumidity();
  if (isnan(rec.hum) || isnan(rec.temp)) {
    delay(500);
    rec.temp = dht.readTemperature();
    rec.hum = dht.readHumidity();
  }
  rec.light = analogRead(A0);
  return rec;
}

bool isAfterInterval(unsigned long &lastTime, unsigned int interval) {
  unsigned long now = millis();
  if (now - lastTime >= interval) {
    lastTime += interval;
    return true;
  }
  return false;
}

void loop ( void ) {
  server.handleClient();

  if (isAfterInterval(wLastTime, wInterval)) {
    w.push(lastSuccess);
  }
  if (isAfterInterval(mLastTime, mInterval)) {
    m.push(lastSuccess);
  }
  if (isAfterInterval(askLastTime, askInterval)) {
    record_type cur = readRecord();

    // skip calculation if returned current value is NaN
    if (isnan(cur.hum) || isnan(cur.temp)) {
      Serial.print("\nValue is NaN");
      return;
    }
    lastSuccess = cur;
    
    if (w.size()>0) {
      cur.temp = (w.last().temp + cur.temp) / 2;
      cur.hum = (w.last().hum + cur.hum) / 2;
      cur.light = (w.last().light + cur.light) / 2;
      w.update(w.size()-1, cur);
      
//      Serial.print("\nWeek: ");
//      for (int i=0; i < w.size(); i++) {
//        Serial.print(w[i].temp);
//        Serial.print(" ");
//      }
    }
    
    if (m.size()>0) {
      cur.temp = (m.last().temp + cur.temp) / 2;
      cur.hum = (m.last().hum + cur.hum) / 2;
      cur.light = (m.last().light + cur.light) / 2;
      m.update(m.size()-1, cur);
      
//      Serial.print("\nMonth: ");
//      for (int i=0; i < m.size(); i++) {
//        Serial.print(m[i].temp);
//        Serial.print(" ");
//      }
    }
//    Serial.print("\ncur temp: ");
//    Serial.println(cur.temp);
  }
}

void actionTemp() {
  record_type rec = readRecord();
  if (isnan(rec.hum) || isnan(rec.temp)) {
      sendError();
      return;
  }
  
  String out = "{\"c\":\"";
  out += rec.temp;
  out += "\",\"h\":\"";
  out += rec.hum;
  out += "\",\"l\":\"";
  out += rec.light;
  out += "\"}";
  
  server.send ( 200, CONTENT_TYPE_JSON, out );
}

void actionLight() {
  String out = "{\"v\":\"";
  out += analogRead(A0);
  out += "\"}";
  server.send ( 200, CONTENT_TYPE_JSON, out );
}

String generateJson(CircularBuffer<record_type, bufSize> &buf, unsigned int interval) {
  uint8_t i=0;

  String out = "{\"";
  out += "i\":";
  out += interval;
  out += ",\"iu\":\"";
  out += "ms";
  out += "\",\"d\":";
  out += millis();
  out += ",\"v\":[";
  while (i<buf.size()) {
    if (i>0) out += ",";
    out += "{\"c\":";
    out += buf[i].temp;
    out += ",\"h\":";
    out += buf[i].hum;
    out += ",\"l\":";
    out += buf[i].light;
    out += "}";
    i++;
  }
  out += "]}";
  
  return out;
}

void actionWeek() {
  server.send ( 200, CONTENT_TYPE_JSON, generateJson(w, wInterval) );
}

void actionMonth() {
  server.send ( 200, CONTENT_TYPE_JSON, generateJson(m, mInterval) );
}

void sendError() {
  server.send ( 404, CONTENT_TYPE_JSON, "{\"success\":\"false\"}" );
}

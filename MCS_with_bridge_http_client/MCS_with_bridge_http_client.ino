#include <Bridge.h>
#include <HttpClient.h>
#include <BridgeClient.h>

#define DEVICEID "CHANGE_TO_DEVICE_ID" // Input your deviceId
#define DEVICEKEY "CHANGE_TO_DEVICE_KEY" // Input your deviceKey
#define SITE_URL "api.mediatek.com"

static unsigned long beat = 0;
static String commandServer;
static int commandPort = 0;

// This will be used to connect to command server
BridgeClient bc;

void setup()
{
  Bridge.begin();
  Serial.begin(115200);
  
  while(!Serial) delay(1000); /* comment out this line when Serial is not present, ie. run this demo without connect to PC */

  getCommandServer();
  beat = millis();
}

void getCommandServer()
{
  Serial.print("Query command server:");
  
  // Prepare header for MCS API authentication
  String header = "deviceKey: ";
  header += DEVICEKEY;
  header += "\r\n";
  header += "Connection: close";

  HttpClient c;
  c.setHeader(header);
  c.get(SITE_URL "/mcs/v2/devices/" DEVICEID "/connections.csv");
  c.setTimeout(1000);
  
  const String resp = c.readString();
  const int sep = resp.indexOf(',');
  if (-1 == sep)
  {
    Serial.println("Fail to get command server. Make sure you already configured LinkIt Smart 7688 to internet and setup DEVICEID and DEVICEKEY in the sketch.");
    Serial.flush();
    return;
  }
  commandServer = resp.substring(0, sep);
  commandPort = resp.substring(sep+1).toInt();  
  Serial.print(commandServer);
  Serial.print(":");
  Serial.println(commandPort);
  int ret = bc.connect(commandServer.c_str(), commandPort);
  Serial.print("connect result = ");
  Serial.println(ret);
  Serial.flush();
}

void heartBeat(Client &c)
{
  // Send a heart beat data;
  // format reference: https://mcs.mediatek.com/resources/latest/api_references/#get-connection
  static const char* heartbeat = DEVICEID "," DEVICEKEY ",0";
  c.println(heartbeat);
  c.println();
  Serial.println("HeartBeat sent.");
  Serial.flush();
  delay(100);
}

void loop()
{
  if(!bc.connected())
  {
    Serial.println("command server offline, try re-connect");
    int ret = bc.connect(commandServer.c_str(), commandPort);
    Serial.print("connect = ");
    Serial.println(ret);
    Serial.flush();
  }
  
  // send heart beat for every 30 seconds;
  // or our connection will be closed by command server.
  if(30000 < (millis() - beat))
  {
    heartBeat(bc);
    beat = millis();
  }

  // process commands if any
  // note that the command server will also echo our hearbeat.
  String tcpcmd;
  static const char* tcpcmd_led_on = "LED_Control,1";
  static const char* tcpcmd_led_off = "LED_Control,0";
  bool sendUpdate = false;
  while (bc.available())
  {
    int v = bc.read();
    if (v != -1)
    {
      Serial.print((char)v);
      tcpcmd += (char)v;
      if (tcpcmd.substring(40).equals(tcpcmd_led_on)){
        digitalWrite(13, HIGH);
        Serial.println("Switch LED ON ");
        tcpcmd="";
        sendUpdate = true;
      }else if(tcpcmd.substring(40).equals(tcpcmd_led_off)){  
        digitalWrite(13, LOW);
        Serial.println("Switch LED OFF");
        tcpcmd="";
        sendUpdate = true;
      }
    }
  }

  // POST LED value back to MCS.
  if(sendUpdate)
  {
    uploadstatus();
  }
  
  delay(300);
}

void uploadstatus()
{  
  // We update the LED_Display data value
  // to reflect the status of the board.
  String upload_led = (digitalRead(13)==1) ? "LED_Display,,1" : "LED_Display,,0";

  // Prepare HTTP header for MCS API authentication.
  // note that we don't have to add content-length and content-type.
  // these will be automatically added by HttpClient (with curl).
  // 
  // !We must put Content-Type in the first line to prevent
  // curl from sending "application/x-www-form-urlencoded" 
  // as content-type. MCS blocks "application/x-www-form-urlencoded"
  // just like many web services.
  String header = "Content-Type: text-csv\r\n";
  header += "deviceKey: ";
  header += DEVICEKEY;
  header += "\r\n";
  header += "Connection: close";

  // refer to https://mcs.mediatek.com/resources/latest/api_references/#upload-data-points
  HttpClient http;
  http.setHeader(header);
  http.post(SITE_URL "/mcs/v2/devices/" DEVICEID "/datapoints.csv", upload_led.c_str());
  http.setTimeout(1000);
  
  delay(300);

  String resp = http.readString();
  Serial.print("POST result:");
  Serial.println(resp);
}


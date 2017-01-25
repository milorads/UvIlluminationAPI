
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

int UVOUT = A0; //Output from the sensor
int REF_3V3 = A1; //3.3V power on the Arduino board

int BH1750_address = 0x23; // i2c Address for Illumination sensor
byte buff[2];

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

void setup() {
  // Bridge startup
  Bridge.begin();
  Wire.begin();
  BH1750_Init(BH1750_address);
  
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);

  
  server.listenOnLocalhost();
  server.begin();
}

void loop() {
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');

  if (command == "uv") {
    uvCommand(client);
  }

  if (command == "illumination") {
    illuminationCommand(client);
  }

  if (command == "led") {
    ledCommand(client);
  }
}

void uvCommand(YunClient client) {
  
}

void illuminationCommand(YunClient client) {
    float valf=0;

  if(BH1750_Read(BH1750_address)==2){
    
    valf=((buff[0]<<8)|buff[1])/1.2;
    
    if(valf<0)client.print("> 65535");
    else client.print((int)valf,DEC); 
    
    client.println(" lx"); 
  }
}

void ledCommand(YunClient client) {
  
}

void BH1750_Init(int address){
  
  Wire.beginTransmission(address);
  Wire.write(0x10);
  Wire.endTransmission();
}

byte BH1750_Read(int address){
  
  byte i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()){
    buff[i] = Wire.read(); 
    i++;
  }
  Wire.endTransmission();  
  return i;
}

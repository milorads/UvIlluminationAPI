
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
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);

  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / refLevel * uvLevel;

  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level

  client.print("UV Intensity (mW/cm^2): ");
  client.println(uvIntensity);
}

void ledCommand(YunClient client) {
  
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

//Takes an average of readings on a given pin
//Returns the average
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

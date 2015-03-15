#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>
#include<string.h>

char str[100];

//Defining the pins for all the sensors
#define st1 2
#define se1 3
#define st2 6
#define se2 7
#define st3 19
#define se3 18
#define st4 15
#define se4 14


#define LED RED_LED
//Defining the pins for all the valves

#define stv2 8
#define rv2 9
#define stv3 32
#define rv3 5



int pingPin=1;
// your network name also called SSID
char ssid[] = "nav";
// your network password
char password[] = "naveeN1995";

// Initialize the Wifi client library
WiFiClient client;

// server address:
char server[] = "autosewage.herokuapp.com";
//IPAddress server(50,62,217,1);

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
long duration, cm=50;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  // We are connected and have an IP address.
  // Print the WiFi status.
  printWifiStatus();
  
  //int c=1;
  /*pinMode(28,OUTPUT);
  digitalWrite(28,LOW);*/
  
 //assigning mode to all the relay pins
 Serial.println("Assigning valve pins");
 pinMode(stv2, OUTPUT);
 pinMode(rv3, OUTPUT);
 pinMode(stv3, OUTPUT);
 pinMode(rv2, OUTPUT);
  
Serial.println("Initialising the valve pins");
 //setting all the relay pins to low
 
 digitalWrite(stv2, LOW);
 digitalWrite(rv2, LOW);
 digitalWrite(stv3, LOW);
 digitalWrite(rv3, LOW);


 Serial.println("Assigning sensor pins");
 //assigning the sensor pins
  pinMode(st1, OUTPUT);
 pinMode(st2, OUTPUT);
 pinMode(st3, OUTPUT);
 pinMode(st4, OUTPUT);
 pinMode(se1, INPUT);
 pinMode(se2, INPUT);
 pinMode(se3, INPUT);
 pinMode(se4, INPUT);
Serial.println("All pins assigned!"); 
 

}

void loop() 
{
 
  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if ((millis() - lastConnectionTime) > postingInterval) 
    {
    Serial.println(" ");
     Serial.println("PS 1");
    pingPin=ping(st2,se2,1); 
      httpRequestPS();
      processing(stv2, rv2);
      
      delay(2000);
      
      Serial.println(" ");
     Serial.println("PS 2");
     // Serial.println("Hallelujah!");
    pingPin=ping(st3,se3,2); 
      httpRequestPS();
      processing(stv3, rv3);
      delay(4000);
    Serial.println(" ");
      Serial.println("TP");
      //Serial.println("Hallelujah!");
    pingPin=ping(st4,se4,1); 
      httpRequestST();
      delay(2000);
      while (client.available()) {
    //Serial.println("Reply recieved!");
    char c = client.read();
    Serial.write(c);
      }
      delay(4000);
      Serial.println(" ");
      Serial.println("MANHOLE");
      //Serial.println("Hallelujah!");
    pingPin=ping(st1,se1,1);
    
      httpRequestMS();
       delay(2000);
      while (client.available()) {
    //Serial.println("Reply recieved!");
    char c = client.read();
    Serial.write(c);
      }
      Serial.println(" ");
      //delay(4000);
      delay(2000);
  }
}


void processing(int stpin, int rvpin)
{
  
  /* if there's incoming data from the net connection.
   send it out the serial port.  This is for debugging
   purposes only:
  if(c>5)
    break;
  c++;*/
  delay(1000);
  if(client.available())
 {   //Serial.println("Client available in processing");
    Serial.println("Processing the data recieved...");
}
   else
   Serial.println("Client not available in processing");
  
  boolean b=false;
  boolean nxt=false;
  boolean rec1=false; //for st
  boolean rec2=false; //for rv
  int count=0;
  //boolean b=false;

  while (client.available()) {
    //Serial.println("Reply recieved!");
    char c = client.read();
    if(rec1)
    {
      if(c!='\n')
      str[count++]=c;
      else{
        str[count]='\0';
       if(strcmp(str,"true")==0)
        {
          digitalWrite(stpin,HIGH);
          Serial.println("TRUE");
        }
       else
         {digitalWrite(stpin,LOW);
         Serial.println("FALSE");
         }
        rec1=false;
       rec2=true;
      count=0; 
      }
    }
    else if(rec2)
    {
      if(c!='\n')
      str[count++]=c;
      else{
        str[count]='\0';
       if(strcmp(str,"true")==0)
        {
          digitalWrite(rvpin,HIGH);
          Serial.println("TRUE");
        }
       else
       {
         digitalWrite(rvpin,LOW);
        Serial.println("FALSE");
       }
        rec1=false;
       rec2=false; 
      }
    }
    else if(c=='O') b=true;
    
    
    else if(b&&c=='K') 
      {
        nxt=true;
      //Serial.println("OK");
      }
    else if(c=='\n'&&nxt)
    {
     nxt=false;
     b=false;
     rec1=true;
     count=0;
    }
    Serial.write(c);
  }
  delay(1000);
 client.stop(); 
}



int ping(int trig, int ech, int id){


  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:

  //Serial.println("Ultrasonic signal sent...");
  pinMode(trig, OUTPUT);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);

//the echo pin is used to read in the echo

  pinMode(ech, INPUT);
  duration = pulseIn(ech, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  Serial.println(id);
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();  
  Serial.println("Water level computed...");
  return id;
  
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}


// this method makes a HTTP connection to the server:
void httpRequestPS() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("Connecting to server...");
    //Serial.println("Connected");
    // send the HTTP PUT request:
    client.println("PUT /pumping_stations HTTP/1.1");
    client.println("Host: autosewage.herokuapp.com");
    //client.println("User-Agent: Energia/1.1");
    client.print("Content-Length: ");

    // calculate the length of the sensor reading in bytes:
    // 8 bytes for "sensor1," + number of digits of the data:
    int thisLength = 4;
    client.println(thisLength);

    // last pieces of the HTTP PUT request:
    client.println("Accept: text/plain");
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();
    
     client.print(pingPin);
    client.print(",");
    client.println(cm);
    
    
    /*while (client.available()) {
    if(client.available()){
    char c = client.read();
    Serial.write(c);
    }
  
  else
  {
 Serial.println("Client not connected");   
  }
  }*
    
  /*  if(client.available())
    Serial.println("Client available");
   else
   Serial.println("Client not available");
  while (client.available()) {
//    Serial.println("Reply recieved!");
    char c = client.read();
    Serial.write(c);
  }
  Serial.println("\n Returning to loop..");
  */
  
  
    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    Serial.println("Failed to make connection- PS");
  }
}


// this method makes a HTTP connection to the server:
void httpRequestST() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    //Serial.println("Connecting...");
    Serial.println("Connected");
    // send the HTTP PUT request:
    client.println("PUT /treatment_plants HTTP/1.1");
    client.println("Host: autosewage.herokuapp.com");
    //client.println("User-Agent: Energia/1.1");
    client.print("Content-Length: ");

    // calculate the length of the sensor reading in bytes:
    // 8 bytes for "sensor1," + number of digits of the data:
    int thisLength = 4;
    client.println(thisLength);

    // last pieces of the HTTP PUT request:
    client.println("Accept: text/plain");
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();
    
     client.print(pingPin);
    client.print(",");
    client.println(cm);
  
  
  
    // note the time that the connection was made:
    //lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    Serial.println("Connection failed-ST");
  }
}


void httpRequestMS() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");
    Serial.println("Connected");
    // send the HTTP PUT request:
    client.println("PUT /manhole_sensors HTTP/1.1");
    client.println("Host: autosewage.herokuapp.com");
    //client.println("User-Agent: Energia/1.1");
    client.print("Content-Length: ");

    // calculate the length of the sensor reading in bytes:
    // 8 bytes for "sensor1," + number of digits of the data:
    int thisLength = 4;
    client.println(thisLength);

    // last pieces of the HTTP PUT request:
    client.println("Accept: text/plain");
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();
    
     client.print(pingPin);
    client.print(",");
    client.println("17");
  
  
  
    // note the time that the connection was made:
    //lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    Serial.println("Connection failed-MS");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}



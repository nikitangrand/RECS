#include "main.h"

#define SS_PIN    21
#define RST_PIN   22
#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16// было 16
#define ll long long
//used in authentication
MFRC522::MIFARE_Key key;
//authentication return status code
MFRC522::StatusCode status;
// Defined pins to module RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); 

using namespace std;

Servo door;
//const char* ssid     = "RC_MRK";
//const char* password = "117RCMRK117";

const char* ssid     = "RC_MRK";
const char* password = "117RCMRK117";

//const char* ssid     = "BNTY park";
//const char* password = "bnty1920";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String formattedDate;
String dayStamp;
String timeStamp;

byte ver[] = {0x86,0x03,0xEF,0x90};
int correct=0;
void setup() 
{
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  door.attach(13);
  door.write(180);
  delay(200);
  //pinMode(33, OUTPUT);  
  // Init MFRC522
  WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

   

  Serial.println("\nConnected!!!");
  Serial.print("Access point: ");
  Serial.println(WiFi.SSID());

  //client.setServer("172.23.203.78",8889);
  //client.setServer("192.168.156.140", 8889);
  client.setServer("wqtt.ru", 5031);
  const char* nameA = "konik";
  const char* nameP = "konik";
  while(!client.connected())
  {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), nameA, nameP))
    //if (client.connect(clientId.c_str()))
    {
      client.subscribe("card01");
      client.subscribe("card02");
      Serial.println("connect to brocker successful");
      client.publish("test","hello");
    }
    else
    {
      Serial.println("connecting to brocker...");
      delay(300);
    }
  }

  mfrc522.PCD_Init(); 
  Serial.println("Approach your reader card...");
  Serial.println();

  timeClient.begin();
  timeClient.setTimeOffset(3600*3);
}

ll readingData()
{
  //prints the technical details of the card/tag
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); 
  
  //prepare the key - all keys are set to FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
  //byte uid[4];
  ll id=0;
  for(int i=0;i<4;i++){
    id += mfrc522.uid.uidByte[i]*pow(255,i);
    Serial.println(mfrc522.uid.uidByte[i]);
    if (mfrc522.uid.uidByte[i]==ver[i])
    {
      correct++;
    }
    
  }
  if (correct==4){
    Serial.println("correct");
    client.publish("card01","but1");
    digitalWrite(26, HIGH);
    delay(500);
    digitalWrite(26, LOW);
    door.write(0);  
    delay(2000);
    door.write(180);
    delay(500);
  }
  else{
    Serial.println("incorrect");
    digitalWrite(25, HIGH);
    delay(500);
    digitalWrite(25, LOW);
    client.publish("card01","but2");
  }
correct=0;





  //buffer for read data
  byte buffer[SIZE_BUFFER] = {0};
 
  //the block to operate
  byte block = 1;
  byte size = SIZE_BUFFER;//</p><p>  //authenticates the block to operate
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return(-1);
  }

  //read data from block
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return(-1);
  }

 
  
 //prints read data
  for (uint8_t i = 0; i < MAX_SIZE_BLOCK; i++)
  {
      Serial.write(buffer[i]);
      //id += buffer[i]*pow(128,i);
  }
  Serial.println(" ");
  return(id);
}


int rightCard(ll cardNum)
{
  for(int i=0;i<N;i++){
    if (cardNum == b[i]) return i;
  }
  return 0;
}
  
ll id=0;
String date_time,sum;

void loop() {
 
  //client.publish("test", "1");
  client.loop();

  if ( ! mfrc522.PICC_IsNewCardPresent())  
   {
    delay(100);
    return;  
    }
    
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    delay(700);
    return;
  }
  id = readingData();
  if (rightCard(id)==1);
  Serial.println(id);


  while(!timeClient.update()) {
        timeClient.forceUpdate();
      }
  formattedDate = timeClient.getFormattedDate();
  //Serial.println(formattedDate);
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  //Serial.print("DATE: ");
  //Serial.println(dayStamp);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  //Serial.print("HOUR: ");
  //Serial.println(timeStamp);
  date_time =  timeStamp + " " + dayStamp;



  mfrc522.PCD_StopCrypto1();  
  mfrc522.PICC_HaltA();
 /* byte *ser = mfrc522.uid.uidByte;
if (mfrc522.uid.uidByte == ser){
    Serial.println("correct");
    client.publish("card01","but1");
  }
  else {
    Serial.println("correct22");
    client.publish("card01","but2");
  }*/
  // put your main code here, to run repeatedly:
}
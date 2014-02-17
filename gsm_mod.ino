#include <SoftwareSerial.h>

#define GSM_STATE_NONE "NONE"
#define GSM_STATE_INC_CALL "INC_CALL"
#define GSM_STATE_INC_SMS "INC_SMS"

String gsm_resp = String("");
String gsm_state = GSM_STATE_NONE;

void setup()
{
  Serial.begin(19200);
  Serial1.begin(19200);
  delay(500);
  
  Serial.print("Starting GSM board powering up ...");
  
  // Automatically power up the SIM900.
  pinMode(9, OUTPUT);
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(2500);
  digitalWrite(9,LOW);
  delay(3000);
  // End of SIM900 power up.
  Serial.println("done!");
  
  Serial.print("Waiting for registering on network ...");
  delay(5000);//delay to register on network
  Serial.println("done");
  
  initTel();
  initSMS();
}

void initTel() {
  Serial1.print("AT+CLIP=1\r");
  delay(200);
  
  Serial.println("Telephony initiated!");
}

void initSMS() {
  Serial1.print("AT+CMGF=1\r");
  delay(200);
  Serial1.print("AT+CNMI=2,2,0,0,0\r");
  delay(200);

  Serial.println("SMS messaging initiated!");
}

void sendSMS(String message)
{
 Serial1.print("AT+CMGF=1\r");    //Because we want to send the SMS in text mode
 delay(200);
 Serial1.println("AT+CMGS=\"26359162\"");//send sms message, be careful need to add a country code before the cellphone number
 delay(100);
 Serial1.println(message);//the content of the message
 delay(100);
 Serial1.println((char)26);//the ASCII code of the ctrl+z is 26
 delay(100);
 Serial1.println();
 delay(100);
 
 Serial.println("SMS sent!");
}

void loop()
{  
  if(Serial1.available() > 0) {
    while(Serial1.available() > 0) {
      char inc_char = (char)Serial1.read();
      
      if(inc_char == 13) {
        //Serial.println("");
        if(!gsm_resp.equals("")) {
          Serial.println("RESP:"+gsm_resp);
          
          parseResponse(gsm_resp);
        }
        
        gsm_resp = "";
      }
      else if(inc_char == 10) {
      }
      else {
        //Serial.print(inc_char);
        gsm_resp += String(inc_char);
      }
    }
  }

  if(Serial.available() > 0) {//if data available from PC then write it to SIM mod. directly
    Serial1.write(Serial.read());
  }
}

void parseResponse(String gsm_response) {
  gsm_response.trim();
  
  if(gsm_state.equals(GSM_STATE_INC_SMS)) {
    Serial.println("MSG TEXT: "+gsm_response);
    
    gsm_response.toLowerCase();
    if(gsm_response.equals("gt")) {
      Serial.println("GET TEMP PROCEDURE");
      
      sendSMS("TEST TEMP: +21C");
    }
    
    gsm_state = GSM_STATE_NONE;
  }
  else {
    if(gsm_response == "RING") {
      Serial1.println("ATA");
      
      gsm_state = GSM_STATE_INC_CALL;
      
      Serial.println("Answered on incoming call...");
    }
    else if(gsm_response == "NO CARRIER") {
      Serial1.println("ATH");
      
      gsm_state = GSM_STATE_NONE;
    }
    else if(gsm_response.indexOf("+CMT:") == 0) {
      Serial.println("Received incoming SMS message!");
      
      gsm_state = GSM_STATE_INC_SMS;
    }
  }
}


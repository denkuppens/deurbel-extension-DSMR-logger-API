/**
 * Deurbel.h
 * Deurbel specifieke code voor ons huis
 *
 * Als er op de bel gedrukt wordt worden berichten naar 2 android apps gestuurd:
 * http://www.pushsafer.com/ Deze is niet gratis maar werkt wel het beste
 * http://alertzy.app/ Deze is gratis maar er zit iets meer vertraging tov pushsafer
 * 
 * Voor bijde apps moet een unique key aangemaakt worden. Als je de key hebt aangemaakt
 * via de websites hierboven, vul die dan in hieronder in waar nu sterretjes *********** staan
 * op regel 90 en 103 in dit bestand.
 * 
 *******
 * Aanwijzingen om deze code in DSMRloggerAPI.ino te integreren:
//
// DSMRloggerAPI.ino aanpassingen
//
// Voeg de volgende code toe ergens bovenaan:
-----------------------------
#include "Deurbel.h"

// Onderstaande in de 'void loop()' functie toevoegen na 'loopCount++;' 
// -----------------------------------
  if DUE(checkDoorbellTimer)
  {
    // Sample the Kuppens specific doorbell and broadcast
    checkDoorbell();
  }

// En onderstaande ook in de 'void loop()' functie na 'doTaskTelegram();' 
// om de analoge input te loggen. Dat kan handig zijn als het om een of andere
// reden niet werkt. 
// ----------------------------------
    logAnalogInputValue();

 */

#include <ESP8266HTTPClient.h>

#define USE_ALERTZY
#define USE_PUSHSAFER

// Define poll delay time to check the door bell input
DECLARE_TIMER_MS(checkDoorbellTimer,  50);

// Forward declares
void checkDoorbell();
void broadcastDoorbellMessage();
void logAnalogInputValue();

// https://randomnerdtutorials.com/esp8266-adc-reading-analog-values-with-nodemcu/
const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0

// The analog input is used to check if the doorbell is pressed. The signal does not have
// to be debounced because the electrical signal is pepared by an analog filter.
void checkDoorbell()
{
  static const int ThresholdLow = 900;
  static bool prevState = false;
  bool doorbellPressed = false;

  // read the analog in value
  int sensorValue = analogRead(analogInPin);

  // Doorbell pressed?
  doorbellPressed = sensorValue <= ThresholdLow;
  
  if((prevState == false) && (doorbellPressed == true))
  {
    DebugTf("Broadcast Doorbell Message\r\n");
    broadcastDoorbellMessage();
  }

  // update the prevState to the current state
  prevState = doorbellPressed;
}

void broadcastDoorbellMessage()
{
#ifdef USE_PUSHSAFER
  // The button has been pressed so push out the message to the pushsafer service and all connected devices such as smartphones, google nest etc.
  // Send event via PUSH method
  // change ************* to your specific key code
  if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      http.begin(client, "http://www.pushsafer.com/api");
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      http.POST("k=***************&c=%23ff0000&s=17&v=3&pr=2&t=De%20bel!&m=Er%20is%20iemand%20aan%20de%20deur.");
      http.end();
  }
#endif

#ifdef USE_ALERTZY
  // change ************* to your specific key code
  if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
     
      http.begin(client, "http://alertzy.app/send");
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      http.POST("accountKey=**************&title=Doorbell!&message=Someone is at the door.");
      http.end();
  }
#endif
}

void logAnalogInputValue()
{
  int sensorValue = analogRead(analogInPin);
  DebugTf("Analog input value [%d]\r\n", sensorValue);
}

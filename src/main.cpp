#include <Arduino.h>
#include <SoftwareSerial.h>
#include <math.h>
#include <Wire.h>


#define SERIESRESISTOR 2200 // the value of the 'other' resistor
#define DCCDINPUTPIN A3 // which pin the 5v input signal from the map dccd is connected to
#define REARDIFFPIN A2 // what pin the rear diff dignal sensor is connected to
#define THERMISTORPIN A1 // what pin to connect the temperature sensor to
#define PRESSUREPIN A0 // what pin to connect the pressure sensor to
#define SERIALSND A5 // what pin the sending signal is on
#define SERIALRCV A4 // what pin the receiving signal is on

SoftwareSerial mySerial(SERIALRCV, SERIALSND); // RX, TX software serial to teensy

const int sendSerial = 1;  // set whether to actually the results, for testing stuff
 
void setup(void) {
  Serial.begin(115200);
  delay(400);
  if (sendSerial) { 
    mySerial.begin(9600); 
    delay(400); 
  }
}



void loop(void) {

  // read the 5v signal sent by the MAP dccd controller
  int dccdValue = analogRead(DCCDINPUTPIN);
  float dccdVoltage = dccdValue * (5.0 / 1023.0);



  // read #PRESSUREPIN, then convert it to voltage based on 5v source
  // using the AEM linear function for oil pressure, calculate the pressure value
  int sensorValue = analogRead(PRESSUREPIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  int psi = (37.5*(voltage))-18.75;

  // read #THERMISTORPIN pin, then calculate the resistance based on 2.2k resistor
  float resistance = analogRead(THERMISTORPIN);
  resistance = (1023 / resistance)  - 1;     // (1023/ADC - 1)
  resistance = SERIESRESISTOR / resistance;  // resistor / (1023/ADC - 1)

  float resistance2 = analogRead(REARDIFFPIN);
  resistance2 = (1023 / resistance2)  - 1;     // (1023/ADC - 1)
  resistance2 = SERIESRESISTOR / resistance2;  // resistor / (1023/ADC - 1)

  // calculating the aem temp sensor temperature using ai generated polynomial, dont know if this will work
  float x = log(resistance2);
  float dTemp = ((-0.25057752f * x + 9.81514216f) * x - 157.22950552f) * x + 892.12702771f;
  int diffTemperature = (int)round(dTemp);



  // aem is non-linear function, the best I could do is the middle using a function, the rest use a bunch of MAP()'s.  sue me
  float tempF;

  // -40 - 140
  if (resistance >= 2701) {
    //Serial.print("Manual mapping ");

    if (resistance > 402000) {
      tempF = -40; // below sensor range, clamp to minimum
    }
    else if (resistance <= 402000 && resistance >= 289001) {
      //Serial.println("-40 to -31");
      tempF = map(resistance, 289001, 402000, -31, -40);
    }
    else if (resistance <= 289000 && resistance >= 210001) {
      //Serial.println("-31 to -22");
      tempF = map(resistance, 210001, 289000, -22, -31);
    }
    else if (resistance <= 210000 && resistance >= 154001) {
      //Serial.println("-22 to -13");
      tempF = map(resistance, 154001, 210000, -13, -22);
    }
    else if (resistance <= 154000 && resistance >= 114001) {
      //Serial.println("-13 to -4");
      tempF = map(resistance, 114001, 154000, -4, -13);
    }
    else if (resistance <= 114000 && resistance >= 85001) {
      //Serial.println("-4 to 5");
      tempF = map(resistance, 85001, 114000, 5, -4);
    }
    else if (resistance <= 85000 && resistance >= 64301) {
      //Serial.println("5 to 14");
      tempF = map(resistance, 64301, 85000, 14, 5);
    }
    else if (resistance <= 64300 && resistance >= 48901) {
      //Serial.println("14 to 23");
      tempF = map(resistance, 48901, 64300, 23, 14);
    }
    else if (resistance <= 48900 && resistance >= 37501) {
      //Serial.println("23 to 32");
      tempF = map(resistance, 37501, 48900, 32, 23);
    }
    else if (resistance <= 37500 && resistance >= 29001) {
      //Serial.println("32 to 41");
      tempF = map(resistance, 29001, 37500, 41, 32);
    }
    else if (resistance <= 29000 && resistance >= 22501) {
      //Serial.println("41 to 50");
      tempF = map(resistance, 22501, 29000, 50, 41);
    }
    else if (resistance <= 22500 && resistance >= 17701) {
      //Serial.println("50 to 59");
      tempF = map(resistance, 17701, 22500, 59, 50);
    }
    else if (resistance <= 17700 && resistance >= 14001) {
      //Serial.println("59 to 68");
      tempF = map(resistance, 14001, 17700, 68, 59);
    }
    else if (resistance <= 14000 && resistance >= 11101) {
      //Serial.println("68 to 77");
      tempF = map(resistance, 11101, 14000, 77, 68);
    }
    else if (resistance <= 11100 && resistance >= 8901) {
      //Serial.println("77 to 86");
      tempF = map(resistance, 8901, 11100, 86, 77);
    }
    else if (resistance <= 8900 && resistance >= 7201) {
      //Serial.println("86 to 95");
      tempF = map(resistance, 7201, 8900, 95, 86);
    }
    else if (resistance <= 7200 && resistance >= 5801) {
      //Serial.println("95 to 104");
      tempF = map(resistance, 5801, 7200, 104, 95);
    }
    else if (resistance <= 5800 && resistance >= 4701) {
      //Serial.println("104 to 113");
      tempF = map(resistance, 4701, 5800, 113, 104);
    }
    else if (resistance <= 4700 && resistance >= 3901) {
      //Serial.println("113 to 122");
      tempF = map(resistance, 3901, 4700, 122, 113);
    }
    else if (resistance <= 3900 && resistance >= 3201) {
      //Serial.println("122 to 131");
      tempF = map(resistance, 3201, 3900, 131, 122);
    }
    else if (resistance <= 3200 && resistance >= 2701) {
      //Serial.println("131 to 140");
      tempF = map(resistance, 2701, 3200, 140, 131);
    }
  }

  // 140 - 230
  else if (resistance <= 2700 && resistance >= 531) {
    //Serial.println("Quadratic calulation");
    tempF = (-0.0000000096*resistance*resistance*resistance) + (0.0000635181*resistance*resistance) - (0.1610960986*resistance) + 298.35;
  }

  // 230 - 302
  else {
    //Serial.print("Manual mapping ");

    if (resistance < 189) {
      tempF = 302; // above sensor range, clamp to maximum
    }
    else if (resistance <= 531 && resistance >= 463) {
      //Serial.println("230 to 239");
      tempF = map(resistance, 463, 531, 239, 230);
    }
    else if (resistance <= 462 && resistance >= 404) {
      //Serial.println("239 to 248");
      tempF = map(resistance, 404, 462, 248, 239);
    }
    else if (resistance <= 403 && resistance >= 353) {
      //Serial.println("248 to 257");
      tempF = map(resistance, 353, 403, 257, 248);
    }
    else if (resistance <= 352 && resistance >= 310) {
      //Serial.println("257 to 266");
      tempF = map(resistance, 310, 352, 266, 257);
    }
    else if (resistance <= 309 && resistance >= 273) {
      //Serial.println("267 to 275");
      tempF = map(resistance, 273, 309, 275, 266);
    }
    else if (resistance <= 272 && resistance >= 242) {
      //Serial.println("276 to 284");
      tempF = map(resistance, 242, 272, 284, 275);
    }
    else if (resistance <= 241 && resistance >= 214) {
      //Serial.println("284 to 293");
      tempF = map(resistance, 214, 241, 293, 284);
    }
    else if (resistance <= 213 && resistance >= 189) {
      //Serial.println("293 to 302");
      tempF = map(resistance, 189, 213, 302, 293);
    }
  }

  int temperature = (int)round(tempF);


  /* Current send method, not using a string on the teensy side */
  Serial.print("a");
  Serial.print(temperature);
  Serial.print(",");
  Serial.print("b");
  Serial.print(psi);
  Serial.print(",c");
  Serial.print(diffTemperature);
  Serial.print(",d");
  Serial.println(dccdVoltage);
  
  if (sendSerial) {
    mySerial.print("a");
    mySerial.print(temperature);
    mySerial.print(",");
    mySerial.print("b");
    mySerial.print(psi);
    mySerial.print(",c");
    mySerial.print(diffTemperature);
    mySerial.print(",d");
    mySerial.println(dccdVoltage);
  }








  /* New send method that will parse as a string on the other side */
  /*
  Serial.print(temperature);
  Serial.print(",");
  Serial.println(psi);
  
  if (sendSerial) {
    Serial.println("Sending...");
    mySerial.print(temperature);
    mySerial.print(",");
    mySerial.println(psi);
  }
  */

  // in my case, this needs to be slower than the time it takes to run a loop on the teensy at the far end
  delay(50);
}


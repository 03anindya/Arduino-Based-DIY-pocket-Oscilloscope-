#include <Wire.h>                         //lib for control 
#include <Adafruit_GFX.h>                //lib for oled 
#include <Adafruit_SSD1306.h>            //lib for oled
#include <Adafruit_SH1106.h>             //lib for OLED  
#include <EEPROM.h>                      //special lib for oscilloscope  
#define SCREEN_WIDTH 128                
#define SCREEN_HEIGHT 64                
#define REC_LENG 200                    
#define MIN_TRIG_SWING 5                
#define OLED_RESET     -1      
Adafruit_SH1106 oled(OLED_RESET);       
//declearation of the variable ranges 
const char vRangeName[10][5] PROGMEM = {"A50V", "A 5V", " 50V", " 20V", " 10V", "  5V", "  2V", "  1V", "0.5V", "0.2V"};
const char * const vstring_table[] PROGMEM = {vRangeName[0], vRangeName[1], vRangeName[2], vRangeName[3], vRangeName[4], vRangeName[5], vRangeName[6], vRangeName[7], vRangeName[8], vRangeName[9]};
const char hRangeName[10][6] PROGMEM = {"200ms", "100ms", " 50ms", " 20ms", " 10ms", "  5ms", "  2ms", "  1ms", "500us", "200us"};  
const char * const hstring_table[] PROGMEM = {hRangeName[0], hRangeName[1], hRangeName[2], hRangeName[3], hRangeName[4], hRangeName[5], hRangeName[6], hRangeName[7], hRangeName[8], hRangeName[9]};
const PROGMEM float hRangeValue[] = { 0.2, 0.1, 0.05, 0.02, 0.01, 0.005, 0.002, 0.001, 0.5e-3, 0.2e-3}; 
int waveBuff[REC_LENG];   //define of variables     
char chrBuff[8];              
char hScale[] = "xxxAs";       
char vScale[] = "xxxx";        
float lsb5V = 0.00566826;     
float lsb50V = 0.05243212;     
volatile int vRange;          
volatile int hRange;          
volatile int trigD;            
volatile int scopeP;          
volatile boolean hold = false;
volatile boolean switchPushed = false; 
volatile int saveTimer;        
int timeExec;                 
int dataMin;                   
int dataMax;                   
int dataAve;                  
int rangeMax;                  
int rangeMin;                  
int rangeMaxDisp;             
int rangeMinDisp;             
int trigP;                    
boolean trigSync;             
int att10x;                   
float waveFreq;                
float waveDuty;                
void setup() 
{
  //pin assigning 
  pinMode(2, INPUT_PULLUP);             
  pinMode(8, INPUT_PULLUP);             
  pinMode(9, INPUT_PULLUP);            
  pinMode(10, INPUT_PULLUP);            
  pinMode(11, INPUT_PULLUP);            
  pinMode(12, INPUT);                   
  pinMode(13, OUTPUT);                  
  oled.begin(SH1106_SWITCHCAPVCC, 0x3C);  
  auxFunctions();                       
  loadEEPROM();                         
  analogReference(INTERNAL);            
  attachInterrupt(0, pin2IRQ, FALLING);
  startScreen(); 
}                       
void loop()
{
  setConditions();                     
  digitalWrite(13, HIGH);               
  readWave();                         
  digitalWrite(13, LOW);                
  setConditions();                    
  dataAnalize();                       
  writeCommonImage();                   
  plotData();                          
  dispInf();                            
  oled.display();                       
  saveEEPROM();                         
  while (hold == true) {                
    dispHold();
    delay(10);
  }                                     // loop cycle speed = 60-470ms (buffer size = 200)
}
void setConditions() 
{           
  strcpy_P(hScale, (char*)pgm_read_word(&(hstring_table[hRange])));  
  strcpy_P(vScale, (char*)pgm_read_word(&(vstring_table[vRange])));  
  switch (vRange) 
  {              
    case 0: 
      {                    
        att10x = 1;              
        break;
      }
    case 1: 
      {                    
        att10x = 0;              
        break;
      }
    case 2: 
      {                   
        rangeMax = 50 / lsb50V;  
        rangeMaxDisp = 5000;     
        rangeMin = 0;
        rangeMinDisp = 0;
        att10x = 1;              
        break;
      }
    case 3: 
      {                   
        rangeMax = 20 / lsb50V;  
        rangeMaxDisp = 2000;
        rangeMin = 0;
        rangeMinDisp = 0;
        att10x = 1;              
        break;
      }
    case 4: 
      {                   
        rangeMax = 10 / lsb50V;  
        rangeMaxDisp = 1000;
        rangeMin = 0;
        rangeMinDisp = 0;
        att10x = 1;              
        break;
      }
    case 5:
      {                    
        rangeMax = 5 / lsb5V;    
        rangeMaxDisp = 500;
        rangeMin = 0;
        rangeMinDisp = 0;
        att10x = 0;              
        break;
      }
    case 6:
      {                   
        rangeMax = 2 / lsb5V;    
        rangeMaxDisp = 200;
        rangeMin = 0;
        rangeMinDisp = 0;
        att10x = 0;              
        break;
      }
    case 7: 
      {                    
        rangeMax = 1 / lsb5V;    
        rangeMaxDisp = 100;
        rangeMin = 0;
        rangeMinDisp = 0;
        att10x = 0;             
        break;
      }
    case 8: 
      {                   
        rangeMax = 0.5 / lsb5V;  
        rangeMaxDisp = 50;
        rangeMin = 0;
        rangeMinDisp = 0;
        att10x = 0;              
        break;
      }
    case 9: 
      {                    
        rangeMax = 0.2 / lsb5V;  
        rangeMaxDisp = 20;
        rangeMin = 0;
        rangeMinDisp = 0;
        att10x = 0;             
        break;
      }
  }
}
void writeCommonImage() 
{                 
  oled.clearDisplay();                    
  oled.setTextColor(WHITE);               
  oled.setCursor(85, 0);                  
  oled.println(F("av    v"));            
  oled.drawFastVLine(26, 9, 55, WHITE);  
  oled.drawFastVLine(127, 9, 3, WHITE);   
  oled.drawFastVLine(127, 61, 3, WHITE);  
  oled.drawFastHLine(24, 9, 7, WHITE);    
  oled.drawFastHLine(24, 36, 2, WHITE);
  oled.drawFastHLine(24, 63, 7, WHITE);
  oled.drawFastHLine(51, 9, 3, WHITE);    
  oled.drawFastHLine(51, 63, 3, WHITE);
  oled.drawFastHLine(76, 9, 3, WHITE);    
  oled.drawFastHLine(76, 63, 3, WHITE);
  oled.drawFastHLine(101, 9, 3, WHITE);   
  oled.drawFastHLine(101, 63, 3, WHITE);
  oled.drawFastHLine(123, 9, 5, WHITE);   
  oled.drawFastHLine(123, 63, 5, WHITE);
  for (int x = 26; x <= 128; x += 5) {
    oled.drawFastHLine(x, 36, 2, WHITE);  
  }
  for (int x = (127 - 25); x > 30; x -= 25) {
    for (int y = 10; y < 63; y += 5) {
      oled.drawFastVLine(x, y, 2, WHITE); 
    }
  }
}
void readWave() {                            
  if (att10x == 1) {                        
    pinMode(12, OUTPUT);                    
    digitalWrite(12, LOW);                  
  } else {                                   
    pinMode(12, INPUT);                      
  }
  switchPushed = false;                     
  switch (hRange) {                         
    case 0: {                                
        timeExec = 1600 + 60;                
        ADCSRA = ADCSRA & 0xf8;              
        ADCSRA = ADCSRA | 0x07;              
        for (int i = 0; i < REC_LENG; i++) { 
          waveBuff[i] = analogRead(0);       
          delayMicroseconds(7888);          
          if (switchPushed == true) {        
            switchPushed = false;
            break;                           
          }
        }
        break;
      }
    case 1: {                                
        timeExec = 800 + 60;                 
        ADCSRA = ADCSRA & 0xf8;              
        ADCSRA = ADCSRA | 0x07;             
        for (int i = 0; i < REC_LENG; i++) {  
          waveBuff[i] = analogRead(0);       
          // delayMicroseconds(3888);           
          delayMicroseconds(3860);           
          if (switchPushed == true) {       
            switchPushed = false;
            break;                           
          }
        }
        break;
      }
    case 2: {                                
        timeExec = 400 + 60;                 
        ADCSRA = ADCSRA & 0xf8;              
        ADCSRA = ADCSRA | 0x07;              
        for (int i = 0; i < REC_LENG; i++) 
        { 
          waveBuff[i] = analogRead(0);      
          // delayMicroseconds(1888);           
          delayMicroseconds(1880);           
          if (switchPushed == true) 
          {        
            break;                           
          }
        }
        break;
      }
    case 3: 
      {                               
        timeExec = 160 + 60;                 
        ADCSRA = ADCSRA & 0xf8;              
        ADCSRA = ADCSRA | 0x07;              
        for (int i = 0; i < REC_LENG; i++)
        { 
          waveBuff[i] = analogRead(0);       
          // delayMicroseconds(688);            
          delayMicroseconds(686);            
          if (switchPushed == true) 
          {        
            break;                           
          }
        }
        break;
      }
    case 4: 
      {                                
        timeExec = 80 + 60;                  
        ADCSRA = ADCSRA & 0xf8;              
        ADCSRA = ADCSRA | 0x07;              
        for (int i = 0; i < REC_LENG; i++) 
        { 
          waveBuff[i] = analogRead(0);       
          // delayMicroseconds(288);            
          delayMicroseconds(287);            
          if (switchPushed == true) 
          {       
            break;                           
          }
        }
        break;
      }
    case 5: 
      {                                
        timeExec = 40 + 60;                  
        ADCSRA = ADCSRA & 0xf8;              
        ADCSRA = ADCSRA | 0x07;              
        for (int i = 0; i < REC_LENG; i++) { 
          waveBuff[i] = analogRead(0);       
          // delayMicroseconds(88);            
          delayMicroseconds(87);             
          if (switchPushed == true) {        
            break;                           
          }
        }
        break;
      }
    case 6: 
      {                                
        timeExec = 16 + 60;                  
        ADCSRA = ADCSRA & 0xf8;              
        ADCSRA = ADCSRA | 0x06;              // dividing ratio = 64 (0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
        for (int i = 0; i < REC_LENG; i++) 
        {
          waveBuff[i] = analogRead(0);       
          // delayMicroseconds(24);            
          delayMicroseconds(23);             
        }
        break;
      }
    case 7: 
      {                               
        timeExec = 8 + 60;                   
        ADCSRA = ADCSRA & 0xf8;              
        ADCSRA = ADCSRA | 0x05;              // dividing ratio = 16 (0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
        for (int i = 0; i < REC_LENG; i++) 
        { 
          waveBuff[i] = analogRead(0);       
          // delayMicroseconds(12);             
          delayMicroseconds(10);            
        }
        break;
      }
    case 8: 
      {                                
        timeExec = 4 + 60;                   
        ADCSRA = ADCSRA & 0xf8;              
        ADCSRA = ADCSRA | 0x04;              // dividing ratio = 16(0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
        for (int i = 0; i < REC_LENG; i++) 
        { 
          waveBuff[i] = analogRead(0);       
          delayMicroseconds(4);              
          // time fine adjustment 0.0625 x 8 = 0.5us（nop=0.0625us @16MHz)
          asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
        }
        break;
      }
    case 9: 
      {                                
        timeExec = 2 + 60;                   
        ADCSRA = ADCSRA & 0xf8;              
        ADCSRA = ADCSRA | 0x02;              // dividing ratio = 4(0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
        for (int i = 0; i < REC_LENG; i++) 
        { 
          waveBuff[i] = analogRead(0);       
          // time fine adjustment 0.0625 * 20 = 1.25us (nop=0.0625us @16MHz)
          asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
          asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
        }
        break;
      }
  }
}
void dataAnalize()
{                       
  int d;
  long sum = 0;
  dataMin = 1023;                          
  dataMax = 0;                             
  for (int i = 0; i < REC_LENG; i++) 
  {     
    d = waveBuff[i];
    sum = sum + d;
    if (d < dataMin) 
    {                     
      dataMin = d;
    }
    if (d > dataMax) 
    {                     
      dataMax = d;
    }
  }
  
  dataAve = (sum + 10) / 20;               // Average value calculation (calculated by 10 times to improve accuracy)
  if (vRange <= 1) 
  {                       
    rangeMin = dataMin - 20;               
    rangeMin = (rangeMin / 10) * 10;       
    if (rangeMin < 0) 
    {
      rangeMin = 0;                        
    }
    rangeMax = dataMax + 20;               
    rangeMax = ((rangeMax / 10) + 1) * 10; 
    if (rangeMax > 1020) 
    {
      rangeMax = 1023;                     
    }
    if (att10x == 1) 
    {                            
      rangeMaxDisp = 100 * (rangeMax * lsb50V);   
      rangeMinDisp = 100 * (rangeMin * lsb50V);   
    } 
    else 
    {                                      
      rangeMaxDisp = 100 * (rangeMax * lsb5V);
      rangeMinDisp = 100 * (rangeMin * lsb5V);
    }
  } 
  else
  {                                        
    // code to limit the  peak- peak range for customization (null for now) as this is a general purpose device  
  }
  for (trigP = ((REC_LENG / 2) - 51); trigP < ((REC_LENG / 2) + 50); trigP++) 
    { 
    if (trigD == 0) 
    {                             
      if ((waveBuff[trigP - 1] < (dataMax + dataMin) / 2) && (waveBuff[trigP] >= (dataMax + dataMin) / 2)) 
      {
        break;                                    
      }
    } 
    else 
    {                                      
      if ((waveBuff[trigP - 1] > (dataMax + dataMin) / 2) && (waveBuff[trigP] <= (dataMax + dataMin) / 2)) 
      {
        break;
      }                                           
    }
  }
  trigSync = true;
  if (trigP >= ((REC_LENG / 2) + 50)) 
  {           
    trigP = (REC_LENG / 2);                       
    trigSync = false;                             
  }
  if ((dataMax - dataMin) <= MIN_TRIG_SWING) 
  {    
    trigSync = false;                             
  }
  freqDuty();
}
void freqDuty() 
{                               
  int swingCenter;                              
  float p0 = 0;                                 
  float p1 = 0;                                
  float p2 = 0;                                 
  float pFine = 0;                              
  float lastPosiEdge;                           
  float pPeriod;                                
  float pWidth;                                 
  int p1Count = 0;                              
  int p2Count = 0;                              
  boolean a0Detected = false;
  boolean posiSerch = true;                     
  swingCenter = (3 * (dataMin + dataMax)) / 2;   
  for (int i = 1; i < REC_LENG - 2; i++) 
  {       
    if (posiSerch == true) 
    {   
      if ((sum3(i) <= swingCenter) && (sum3(i + 1) > swingCenter)) 
      {  
        pFine = (float)(swingCenter - sum3(i)) / ((swingCenter - sum3(i)) + (sum3(i + 1) - swingCenter) );  
        if (a0Detected == false) 
        {               
          a0Detected = true;                     
          p0 = i + pFine;                        
        } 
        else 
        {
          p1 = i + pFine - p0;                   
          p1Count++;
        }
        lastPosiEdge = i + pFine;                
        posiSerch = false;
      }
    } 
    else 
    {   
      if ((sum3(i) >= swingCenter) && (sum3(i + 1) < swingCenter)) 
      {  
        pFine = (float)(sum3(i) - swingCenter) / ((sum3(i) - swingCenter) + (swingCenter - sum3(i + 1)) );
        if (a0Detected == true) {
          p2 = p2 + (i + pFine - lastPosiEdge);  
          p2Count++;
        }
        posiSerch = true;
      }
    }
  }
  pPeriod = p1 / p1Count;                 
  pWidth = p2 / p2Count;                  
  waveFreq = 1.0 / ((pgm_read_float(hRangeValue + hRange) * pPeriod) / 25.0); 
  waveDuty = 100.0 * pWidth / pPeriod;                                      
}
int sum3(int k) 
{       
  int m = waveBuff[k - 1] + waveBuff[k] + waveBuff[k + 1];
  return m;
}
void startScreen() 
{                      
  oled.clearDisplay();
  oled.setTextSize(1);                    
  oled.setTextColor(WHITE);
  oled.setCursor(55, 0);
  oled.println(F("Mini"));  
  oled.setCursor(30, 20);
  oled.println(F("Oscilloscope")); 
  oled.setCursor(55, 42);            
  oled.println(F("v1.1"));                
  oled.display();                         
  delay(1500);
  oled.clearDisplay();
  oled.setTextSize(1);                   
}
void dispHold() 
{                        
  oled.fillRect(42, 11, 24, 8, BLACK);    
  oled.setCursor(42, 11);
  oled.print(F("Hold"));                 
  oled.display();                         
}
void dispInf() 
{                          
  float voltage;
  oled.setCursor(2, 0);                   
  oled.print(vScale);                     
  if (scopeP == 0) 
  {                      
    oled.drawFastHLine(0, 7, 27, WHITE);  
    oled.drawFastVLine(0, 5, 2, WHITE);
    oled.drawFastVLine(26, 5, 2, WHITE);
  }
  oled.setCursor(34, 0);                  
  oled.print(hScale);                    
  if (scopeP == 1) 
  {                      
    oled.drawFastHLine(32, 7, 33, WHITE); 
    oled.drawFastVLine(32, 5, 2, WHITE);
    oled.drawFastVLine(64, 5, 2, WHITE);
  }
  oled.setCursor(75, 0);                  
  if (trigD == 0) 
  {                       
    oled.print(char(0x18));               
  } 
  else 
  {
    oled.print(char(0x19));               // down mark              ↓
  }
  if (scopeP == 2) 
  {                      
    oled.drawFastHLine(71, 7, 13, WHITE); 
    oled.drawFastVLine(71, 5, 2, WHITE);
    oled.drawFastVLine(83, 5, 2, WHITE);
  }
  if (att10x == 1) 
  {                         
    voltage = dataAve * lsb50V / 10.0;       
  } 
  else 
  {                                   
    voltage = dataAve * lsb5V / 10.0;       
  }
  if (voltage < 10.0) 
  {                     
    dtostrf(voltage, 4, 2, chrBuff);        
  } 
  else 
  {                                  
    dtostrf(voltage, 4, 1, chrBuff);        
  }
  oled.setCursor(98, 0);                     
  oled.print(chrBuff);                       
  if (vRange == 1 || vRange > 4)
  {          
    dtostrf(voltage, 4, 2, chrBuff);        
  } 
  else 
  {                                  
    dtostrf(voltage, 4, 1, chrBuff);         
  }
  oled.setCursor(0, 9);
  oled.print(chrBuff);                      
  voltage = (rangeMaxDisp + rangeMinDisp) / 200.0; 
  if (vRange == 1 || vRange > 4) 
  {           
    dtostrf(voltage, 4, 2, chrBuff);         
  } 
  else 
  {                                  
    dtostrf(voltage, 4, 1, chrBuff);        
  }
  oled.setCursor(0, 33);
  oled.print(chrBuff);                       
  voltage = rangeMinDisp / 100.0;           
  if (vRange == 1 || vRange > 4) 
  {           
    dtostrf(voltage, 4, 2, chrBuff);       
  } 
  else 
  {                                  
    dtostrf(voltage, 4, 1, chrBuff);         
  }
  oled.setCursor(0, 57);
  oled.print(chrBuff);                       
  if (trigSync == false) 
  {                   
    oled.fillRect(92, 14, 24, 8, BLACK);     
    oled.setCursor(92, 14);                 
    oled.print(F("unSync"));                 
  } 
  else 
  {
    oled.fillRect(90, 12, 25, 9, BLACK);    
    oled.setCursor(91, 13);                 
    if (waveFreq < 100.0) 
    {                  
      oled.print(waveFreq, 1);               // display 99.9Hz
      oled.print(F("Hz"));
    } 
    else if (waveFreq < 1000.0) 
    {          // if less than 1000Hz
      oled.print(waveFreq, 0);               // display 999Hz
      oled.print(F("Hz"));
    } 
    else if (waveFreq < 10000.0) 
    {         // if less than 10kHz
      oled.print((waveFreq / 1000.0), 2);    // display 9.99kH
      oled.print(F("kH"));
    } 
    else 
    {                                 // if more
      oled.print((waveFreq / 1000.0), 1);    // display 99.9kH
      oled.print(F("kH"));
    }
    oled.fillRect(96, 21, 25, 10, BLACK);    
    oled.setCursor(97, 23);                  
    oled.print(waveDuty, 1);                 
    oled.print(F("%"));
  }
}
void plotData() 
{                    
  long y1, y2;
  for (int x = 0; x <= 98; x++) 
  {
    y1 = map(waveBuff[x + trigP - 50], rangeMin, rangeMax, 63, 9); 
    y1 = constrain(y1, 9, 63);                                     
    y2 = map(waveBuff[x + trigP - 49], rangeMin, rangeMax, 63, 9); 
    y2 = constrain(y2, 9, 63);                                     
    oled.drawLine(x + 27, y1, x + 28, y2, WHITE);                  
  }
}
void saveEEPROM() 
{                    
  if (saveTimer > 0) 
  {                 
    saveTimer = saveTimer - timeExec;  
    if (saveTimer < 0) 
    {               
      EEPROM.write(0, vRange);         
      EEPROM.write(1, hRange);
      EEPROM.write(2, trigD);
      EEPROM.write(3, scopeP);
    }
  }
}
void loadEEPROM() 
{                    
  int x;
  x = EEPROM.read(0);                  
  if ((x < 0) || (9 < x)) 
  {            
    x = 3;                             
  }
  vRange = x;
  x = EEPROM.read(1);                  
  if ((x < 0) || (9 < x)) 
  {            
    x = 3;                             
  }
  hRange = x;
  x = EEPROM.read(2);                  
  if ((x < 0) || (1 < x)) 
  {            
    x = 1;                             
  }
  trigD = x;
  x = EEPROM.read(3);                  
  if ((x < 0) || (2 < x)) 
  {            
    x = 1;                            
  }
  scopeP = x;
}
void auxFunctions() 
{                       
  float voltage;
  long x;
  if (digitalRead(8) == LOW) 
  {             
    analogReference(DEFAULT);               
    while (1) 
    {                             
      x = 0;
      for (int i = 0; i < 100; i++) 
      {       
        x = x + analogRead(1);             
      }
      voltage = (x / 100.0) * 5.0 / 1023.0;
      oled.clearDisplay();                 
      oled.setTextColor(WHITE);            
      oled.setCursor(20, 16);              
      oled.setTextSize(1);                  
      oled.println(F("Battery voltage"));
      oled.setCursor(35, 30);               
      oled.setTextSize(2);                  
      dtostrf(voltage, 4, 2, chrBuff);      
      oled.println(F("V"));
      oled.display();
      delay(150);
    }
  }
  if (digitalRead(9) == LOW) 
  {              
    analogReference(INTERNAL);
    pinMode(12, INPUT);                     
    while (1) 
    {                            
      digitalWrite(13, HIGH);              
      voltage = analogRead(0) * lsb5V;      
      oled.clearDisplay();                 
      oled.setTextColor(WHITE);             
      oled.setCursor(26, 16);               
      oled.setTextSize(1);                  
      oled.println(F("DVM 5V Range"));
      oled.setCursor(35, 30);               
      oled.setTextSize(2);                  
      dtostrf(voltage, 4, 2, chrBuff);      
      oled.print(chrBuff);
      oled.println(F("V"));
      oled.display();
      digitalWrite(13, LOW);                
      delay(150);
    }
  }
  if (digitalRead(10) == LOW) 
  {             
    analogReference(INTERNAL);
    pinMode(12, OUTPUT);                    
    digitalWrite(12, LOW);                  
    while (1) {                            
      digitalWrite(13, HIGH);              
      voltage = analogRead(0) * lsb50V;     
      oled.clearDisplay();                  
      oled.setTextColor(WHITE);            
      oled.setCursor(26, 16);              
      oled.setTextSize(1);                  
      oled.println(F("DVM 50V Range"));
      oled.setCursor(35, 30);              
      oled.setTextSize(2);                  
      dtostrf(voltage, 4, 1, chrBuff);      
      oled.print(chrBuff);
      oled.println(F("V"));
      oled.display();
      digitalWrite(13, LOW);                
      delay(150);
    }
  }
}
void uuPinOutputLow(unsigned int d, unsigned int a) 
{ 
  unsigned int x;
  x = d & 0x00FF; PORTD &= ~x; DDRD |= x;
  x = d >> 8;     PORTB &= ~x; DDRB |= x;
  x = a & 0x003F; PORTC &= ~x; DDRC |= x;
}
void pin2IRQ() 
{                  
  // Pin8,9,10,11 buttons are bundled with diodes and connected to Pin2.
  int x;                          
  x = PINB;                        
  if ( (x & 0x07) != 0x07)
   {       
    saveTimer = 5000;              
    switchPushed = true;           
  }
  if ((x & 0x01) == 0) 
  {                       // if select button(Pin8) pushed,
    scopeP++;                      
    if (scopeP > 2) 
    {              
      scopeP = 0;                 
    }
  }

  if ((x & 0x02) == 0)            // if UP button(Pin9) 
  {                        
    if (scopeP == 0) 
    {             
      vRange++;                    
      if (vRange > 9) 
      {            
        vRange = 9;                
      }
    }
    if (scopeP == 1) 
    {             
      hRange++;                   
      if (hRange > 9) 
      {           
        hRange = 9;               
      }
    }
    if (scopeP == 2) 
    {            
      trigD = 0;                   
    }
  }
  if ((x & 0x04) == 0) // if DOWN button(Pin10) pusshed
  {           
    if (scopeP == 0) 
    {             
      vRange--;                    
      if (vRange < 0) 
      {            
        vRange = 0;                
      }
    }
    if (scopeP == 1) 
    {             
      hRange--;                    
      if (hRange < 0) 
      {            
        hRange = 0;                
      }
    }
    if (scopeP == 2) 
    {             
      trigD = 1;                  
    }
  }
  if ((x & 0x08) == 0)    // if HOLD button(pin11) pushed
  {                      
    hold = ! hold;                 
  }
}
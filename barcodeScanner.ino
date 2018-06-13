#include <hidboot.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

int correct = 0;
boolean mask = false;
boolean merge = false;
boolean groomer = false;


void setupSound(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH); // Set the pin high as the default state
}

void activateSound(int pin) {
  digitalWrite(pin, LOW); // bring the pin low to begin the activation
  /*
  According to the documentation, the Audio FX board needs 50ms to trigger. However,
  I've found that coming from my 3.3v Arduino Pro, it needs 100ms to get the trigger
  the going
  */
  delay(100); // hold the pin low long enough to trigger the board; may need to be longer for consistent triggering
  digitalWrite(pin, HIGH); // bring the pin high again to end the activation
}

String barcode = "";
class KbdRptParser : public KeyboardReportParser
{
    void PrintKey(uint8_t mod, uint8_t key);

  protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);

    void OnKeyDown	(uint8_t mod, uint8_t key);
    void OnKeyUp	(uint8_t mod, uint8_t key);
    void OnKeyPressed(uint8_t key);
};

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
  MODIFIERKEYS mod;
  *((uint8_t*)&mod) = m;
  Serial.print((mod.bmLeftCtrl   == 1) ? "C" : " ");
  Serial.print((mod.bmLeftShift  == 1) ? "S" : " ");
  Serial.print((mod.bmLeftAlt    == 1) ? "A" : " ");
  Serial.print((mod.bmLeftGUI    == 1) ? "G" : " ");

  Serial.print(" >");
  PrintHex<uint8_t>(key, 0x80);
  Serial.print("< ");

  Serial.print((mod.bmRightCtrl   == 1) ? "C" : " ");
  Serial.print((mod.bmRightShift  == 1) ? "S" : " ");
  Serial.print((mod.bmRightAlt    == 1) ? "A" : " ");
  Serial.println((mod.bmRightGUI    == 1) ? "G" : " ");
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  Serial.print("DN ");
  PrintKey(mod, key);
  uint8_t c = OemToAscii(mod, key);

  if (c)
    OnKeyPressed(c);
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {

  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;

  if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl) {
    Serial.println("LeftCtrl changed");
  }
  if (beforeMod.bmLeftShift != afterMod.bmLeftShift) {
    Serial.println("LeftShift changed");
  }
  if (beforeMod.bmLeftAlt != afterMod.bmLeftAlt) {
    Serial.println("LeftAlt changed");
  }
  if (beforeMod.bmLeftGUI != afterMod.bmLeftGUI) {
    Serial.println("LeftGUI changed");
  }

  if (beforeMod.bmRightCtrl != afterMod.bmRightCtrl) {
    Serial.println("RightCtrl changed");
  }
  if (beforeMod.bmRightShift != afterMod.bmRightShift) {
    Serial.println("RightShift changed");
  }
  if (beforeMod.bmRightAlt != afterMod.bmRightAlt) {
    Serial.println("RightAlt changed");
  }
  if (beforeMod.bmRightGUI != afterMod.bmRightGUI) {
    Serial.println("RightGUI changed");
  }

}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  Serial.print("UP ");
  PrintKey(mod, key);
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
  if((int)key == 19){

  int code = barcode.toInt();
  barcode="";
  switch(code){

    case 854590007105:
    if(!merge){
    Serial.println("Merge Cube");
    activateSound(5);
    merge = true;
    correct++;
    }else{
    activateSound(8);
    }
    break;

    case 639277055309:
    if(!mask){
    Serial.println("Mask");
    activateSound(3);
    mask = true;
    correct++;
    }else{
      activateSound(8);
    }
    break;

    case 639277968036:
    if(!groomer){
    Serial.println("Groomer");
    activateSound(4);
    groomer = true;
    correct++;
    }else{
       activateSound(8);
    }
    break;

    
    default:
    Serial.println("Wrong!");
    activateSound(6);
    merge = false;
    mask = false;
    groomer = false;
    correct = 0;
    break;
  }
  

  
  if(correct==3){
    Serial.println("WIN!");
    activateSound(7);
    correct=0;
  }
  }else{

      barcode = barcode + char(key);
      
  }
  Serial.println(barcode);
};

USB     Usb;
//USBHub     Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);

KbdRptParser Prs;

void setup()
{
  Serial.begin( 115200 );
   setupSound(2);
   setupSound(3);
   setupSound(4);
   setupSound(5);
   setupSound(6);
   setupSound(7);
   setupSound(8);

#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");

  delay( 200 );
  
  HidKeyboard.SetReportParser(0, &Prs);


   Serial.println("Start!");
   activateSound(2);
  
}


void loop()
{
  Usb.Task();
  
}


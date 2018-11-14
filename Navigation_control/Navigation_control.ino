#include <Keyboard.h>
char inChar;
char string[15];
int index=0;
int CommandeOK=0;

void setup() {
  // put your setup code here, to run once:
Serial1.begin(2400);
//debugSerial.begin(115200);
index =0;
CommandeOK=0;
Keyboard.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(CommandeOK==1){

    //Right Arrow
    if(string[6]==0x4F and string[7]==0x4B and string[8]==0xFFFFFFCB and string[9]==0x7B and string[10]==0xFFFFFFCF ){
      Keyboard.press(KEY_RIGHT_ARROW);
      Keyboard.releaseAll();
    
    }
    //Left Arrow
    if(string[6]==0xFFFFFFCF and string[7]==0x4B and string[8]==0x4B and string[9]==0xFFFFFFCF and string[10]==0xFFFFFFCF ){
      Keyboard.press(KEY_LEFT_ARROW);
      Keyboard.releaseAll();
    }
    //Up Arrow
     if(string[6]==0x4F and string[7]==0x4B and string[8]==0xFFFFFFCB and string[9]==0xFFFFFFCF and string[10]==0xFFFFFFCF ){
      Keyboard.press(KEY_UP_ARROW);
      Keyboard.releaseAll();
    }
    //Down Arrow
     if(string[6]==0xFFFFFFCF and string[7]==0x4F and string[8]==0x4B and string[9]==0xFFFFFFCB and string[10]==0xFFFFFFCF ){
      Keyboard.press(KEY_DOWN_ARROW);
      Keyboard.releaseAll();
    }
    // Enter Button
     if(string[6]==0x4F and string[7]==0x7B and string[8]==0x4B and string[9]==0xFFFFFFCF and string[10]==0x7B ){
      Keyboard.press(KEY_RETURN);
      Keyboard.releaseAll();
    }
    // RET Button
    if(string[6]==0xFFFFFFCF and string[7]==0xFFFFFFCB and string[8]==0x4B and string[9]==0x7B and string[10]==0x7B ){
      Keyboard.press(KEY_ESC);
      Keyboard.releaseAll();
    }
    // Zoom In Button (arrow to front) need to modify navit keys.h
    if(string[6]==0x4F and string[7]==0x7B and string[8]==0x4B and string[9]==0x7B and string[10]==0xFFFFFFCF ){
      Keyboard.press(KEY_PAGE_UP);
      Keyboard.releaseAll();
    }
    // Zoom out Button (arrow to back) need to modify navit keys.h
    if(string[6]==0xFFFFFFCF and string[7]==0x4F and string[8]==0x4B and string[9]==0xFFFFFFCF and string[10]==0x7B ){
      Keyboard.press(KEY_PAGE_DOWN);
      Keyboard.releaseAll();
    }
    /*
    // Voice Button
    if(string[6]==0x4F and string[7]==0xFFFFFFCF and string[8]==0x4F and string[9]==0x7B and string[10]==0xFFFFFFCF ){
      Keyboard.press();
      Keyboard.releaseAll();
    }
    // Menu Button
    if(string[6]==0x4F and string[7]==0xFFFFFFCF and string[8]==0x4B and string[9]==0xFFFFFFCF and string[10]==0xFFFFFFCB ){
      Keyboard.press();
      Keyboard.releaseAll();
    }
     // POS Button
    if(string[6]==0xFFFFFFCF and string[7]==0x7B and string[8]==0x4B and string[9]==0x7B and string[10]==0x7B ){
      Keyboard.press();
      Keyboard.releaseAll();
    }
     // Haut Gauche
    if(string[6]==0x4F and string[7]==0x4F and string[8]==0x4B and string[9]==0x4F and string[10]==0xFFFFFFCF ){
      Keyboard.press();
      Keyboard.releaseAll();
    }
     // bas droite
    if(string[6]==0x4F and string[7]==0x4F and string[8]==0x4B and string[9]==0xFFFFFFCB and string[10]==0xFFFFFFCF ){
      Keyboard.press();
      Keyboard.releaseAll();
    }
     // haut droite
    if(string[6]==0xFFFFFFCF and string[7]==0x7B and string[8]==0x4B and string[9]==0x7B and string[10]==0xFFFFFFCF ){
      Keyboard.press();
      Keyboard.releaseAll();
    }
     // bas gauche
    if(string[6]==0xFFFFFFCF and string[7]==0xFFFFFFCF and string[8]==0x4B and string[9]==0x4B and string[10]==0xFFFFFFCF ){
      Keyboard.press();
      Keyboard.releaseAll();
    }
*/
    for (int i=0; i <= 10; i++){
        //DEBUG Serial.print(string[i],HEX);
        string[i]=0;
        CommandeOK=0;
        }
        Serial.println("");
  }
}

void serialEvent1(){
  while (Serial1.available()) {
    char inChar= (char)Serial1.read();
    if(inChar==0){
      index=0;
    }
    string[index]=inChar;
    index++;
    if(index>12){
      CommandeOK=1;
      index=0;
    }
  }
}


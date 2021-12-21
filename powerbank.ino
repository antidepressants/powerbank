#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

int inIndex = 0;

class InputDevice{
  int pin,index;
  
  String name;
  public:
    float vMax;
    float measureVoltage(){
      int voltage = analogRead(pin);
      return ((voltage*5.00)/1024.00)*2;
    }
    
    void displayVoltage(){
      lcd.setCursor(0,index);
      lcd.print(name);
      lcd.setCursor(11,index);
      lcd.print(measureVoltage());
      lcd.setCursor(15,index);
      lcd.print("V");
    }

    void displayPercentage(){
      int percentage = (measureVoltage()/vMax)*100;
      percentage = (percentage<100)?percentage:100;
      lcd.setCursor(0,index);
      lcd.print(name);
      lcd.setCursor(11,index);
      lcd.print(percentage);
      lcd.print("%");
    }

    InputDevice(int pin, String name, float maxVoltage){
      this->pin = pin;
      this->name = name;
      this->index = inIndex++;
      this->vMax = maxVoltage;
    }
};

class OutputDeivce{
  int pins,pin[2];
  public:
    void AC(){
        digitalWrite(pin[0],1);
        digitalWrite(pin[1],0);
        digitalWrite(pin[0],0);
        digitalWrite(pin[1],1);
    }

    OutputDeivce(int pin1, int pin2){
      this->pin[0]=pin;
      this->pin[1]=pin2;
    }
};

class VoltageControl{
  int pin[2];
  public:
    void voltageControl(float vIn, float minVoltage,float target){
      int val = (vIn<target)?map(vIn,minVoltage,target,200,0):0;
      analogWrite(pin[1],val);
    }

    void modeControl(float vTarget, float maxVoltage, float minVoltage, float vIn, float target){
      if(vTarget>=maxVoltage){
        digitalWrite(pin[0],0);
      }
      else{
        analogWrite(pin[0],255);
        voltageControl(vIn, minVoltage, target);
      }
    }

    VoltageControl(int pin1, int pin2){
      this->pin[0] = pin1;   //on off
      this->pin[1] = pin2; //step up
    }
};

InputDevice SolarPanel(A0,"Panel",9), BatteryInit(A2,"Battery",8), Battery(A1,"Battery",8);
OutputDeivce Coil(A6,A7);
VoltageControl PanelController(5,6), BatteryController(9,10);

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  SolarPanel.displayVoltage();
  BatteryInit.displayPercentage();

  PanelController.modeControl(BatteryInit.measureVoltage(),BatteryInit.vMax,0,SolarPanel.measureVoltage(),BatteryInit.measureVoltage()+1);
  BatteryController.modeControl(0,1,0,Battery.measureVoltage(),Battery.vMax);

  Coil.AC();
}
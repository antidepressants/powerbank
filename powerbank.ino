#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

int inIndex = 0;

class InputDevice{
  int pin,index;
  String name;
  public:
    float measureVoltage(){
      int voltage = analogRead(pin);
      return ((voltage*5.00)/1024.00)*2;
    }
    
    void displayVoltage(){
      lcd.setCursor(0,index);
      lcd.print(name);
      lcd.print("   ");
      lcd.print(measureVoltage());
      lcd.print(" V");
    }

    InputDevice(int pin, String name){
      this->pin = pin;
      this->name = name;
      this->index = inIndex++;
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

    OutputDeivce(int pin){
      this->pin[0]=pin;
      this->pin[1]=pin+1;
    }
};

class VoltageControl{
  int pin[2];
  public:
    void voltageControl(float vIn, float minVoltage,float target){
      int val = (vIn<target)?map(vIn,minVoltage,target,170,30):0;
      analogWrite(pin[1],val);
    }

    void modeControl(float vTarget, float maxVoltage, float minVoltage, float vIn, float target){
      if(vTarget>=maxVoltage || vTarget<minVoltage){
        digitalWrite(pin[0],0);
      }
      else{
        digitalWrite(pin[0],1);
        voltageControl(vIn, minVoltage,target);
      }
    }

    VoltageControl(int pin){
      this->pin[0] = pin;   //on off
      this->pin[1] = pin+1; //step up
    }
};

InputDevice SolarPanel(A0,"Panel  "), Battery(A1,"Battery");
OutputDeivce Coil(A2);
VoltageControl PanelController(5), BatteryController(9);

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  SolarPanel.displayVoltage();
  Battery.displayVoltage();

  PanelController.modeControl(Battery.measureVoltage(),7.4,3,SolarPanel.measureVoltage(),Battery.measureVoltage()+1);
  BatteryController.modeControl(0,1,0,Battery.measureVoltage(),6);

  Coil.AC();
}
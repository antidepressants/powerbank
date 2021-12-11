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
    void voltageControl(float vIn, float target){
      if(vIn<target){
        digitalWrite(pin[1],1);
        delay(10);
        digitalWrite(pin[1],0);
      }
    }

    void modeControl(float vTarget, float maxVoltage, float minVoltage, float vIn, float target){
      if(vTarget>=maxVoltage || vTarget<minVoltage){
        digitalWrite(pin[0],0);
      }
      else{
        digitalWrite(pin[0],1);
        voltageControl(vIn, target);
      }
    }

    VoltageControl(int pin){
      this->pin[0] = pin;   //on off
      this->pin[1] = pin+1; //step up
    }
};

InputDevice SolarPanel(A0,"Panel  "), Battery(A1,"Battery");
OutputDeivce Coil(A2);
VoltageControl PanelController(2), BatteryController(4);

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  SolarPanel.displayVoltage();
  Battery.displayVoltage();

  PanelController.modeControl(Battery.measureVoltage(),7.4,1.5,SolarPanel.measureVoltage(),Battery.measureVoltage()+1);
  BatteryController.modeControl(0,1,0,Battery.measureVoltage(),6);

  Coil.AC();
}
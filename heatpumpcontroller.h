#include "esphome.h"
#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "ir_Midea.h"


using namespace esphome::climate;

class MyHeatPumpClimate : public Component, public Climate {
 public:
  // constructor
  MyHeatPumpClimate() {
    //this->_led_pin = led_pin;
    //_led_pin->pin_->get_pin();
  }

  void setup() override {
    ESP_LOGI("custom", "Setup climat OK");
    // This will be called by App.setup()
  }

  ClimateTraits traits() override {
      ClimateTraits climateTraits;
      climateTraits.set_supports_current_temperature(true);
      climateTraits.set_supports_two_point_target_temperature(false);
      climateTraits.set_supports_auto_mode(true);
      climateTraits.set_supports_cool_mode(true);
      climateTraits.set_supports_heat_mode(true);
      climateTraits.set_supports_away(false);
      return climateTraits;
  }

  void control(const ClimateCall &call) override {
    if (call.get_mode().has_value()) {
      // User requested mode change
      ClimateMode mode = *call.get_mode();

      ESP_LOGI("custom", "Set temp to : %f", mode);
      // Send mode to hardware
      // ...

      // Publish updated state
      this->mode = mode;
      this->publish_state();
    }

    if (call.get_target_temperature().has_value()) {      
      ESP_LOGI("custom", "Set temp to : %f", *call.get_target_temperature());
      pinMode(2, OUTPUT);
      const uint16_t kIrLed = 2;
      IRMideaAC ac(kIrLed);
      ac.setPower(true);
      ac.setUseCelsius(true);
      ac.setTemp(*call.get_target_temperature(), true);
      ac.setFan(MIDEA_AC_FAN_LOW);
      ac.setMode(MIDEA_AC_COOL);
      ac.send();
    }

  }

 //protected:
    //esphome::gpio::GPIOBinaryOutput *_led_pin;

};
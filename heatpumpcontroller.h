#include "esphome.h"

#include <IRSender.h>
#include <R51MHeatpumpIR.h>


using namespace esphome::climate;

class MyHeatPumpClimate : public Component, public Climate {
 public:
  // constructor

  MyHeatPumpClimate(esphome::sensor::Sensor **sensor) {
    this->sensor_ = sensor;
  }

  void setup() override {
    heatpumpIR = new R51MHeatpumpIR();
    this->powerModeCmd = POWER_OFF;
    this->operatingModeCmd = MODE_AUTO;
    this->fanSpeedCmd = FAN_AUTO;
    this->temperatureCmd = 22;
    this->target_temperature = this->temperatureCmd;
    this->swingVCmd = VDIR_AUTO;
    this->swingHCmd = HDIR_AUTO;
    if (*this->sensor_ != nullptr) {
          (*this->sensor_)->add_on_state_callback([this](float state) {
            ESP_LOGI("custom", "Climat receive temperature update: %f", state);
            this->current_temperature = state;
            this->compute_state_();
            this->publish_state();
          });
    }
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

  void control(const ClimateCall &call) {
    if (call.get_mode().has_value())
        this->mode = *call.get_mode();
    if (call.get_target_temperature().has_value()) {
      ESP_LOGI("custom_heat", "Set target temperature to : %f", *call.get_target_temperature());
      this->temperatureCmd =  *call.get_target_temperature();
      this->target_temperature = temperatureCmd;     
    }
    //if (call.get_target_temperature_low().has_value())
    //    this->target_temperature_low = *call.get_target_temperature_low();
    //if (call.get_target_temperature_high().has_value())
    //    this->target_temperature_high = *call.get_target_temperature_high();
    //if (call.get_away().has_value())
    //    this->change_away_(*call.get_away());
    this->compute_state_();
    this->publish_state();
    this->transmit_state();
  }

  void compute_state_() {
    switch (this->mode) {
      case climate::CLIMATE_MODE_OFF:
        powerModeCmd = POWER_OFF;
        break;
      case climate::CLIMATE_MODE_AUTO:
        powerModeCmd = POWER_ON;
        operatingModeCmd = MODE_AUTO;
        break;
      case climate::CLIMATE_MODE_COOL:
        powerModeCmd = POWER_ON;
        operatingModeCmd = MODE_COOL;
        break;
      case climate::CLIMATE_MODE_HEAT:
        powerModeCmd = POWER_ON;
        operatingModeCmd = MODE_HEAT;
        break;
      default:
      ;
    }
  }


  void transmit_state() {
    ESP_LOGI("custom_heat", "Sed data via IR  power : %i, mode: %i, fan: %i, temp: %i, sV: %i, sH: %i",  powerModeCmd, operatingModeCmd, fanSpeedCmd, temperatureCmd, swingVCmd, swingHCmd);
    IRSenderESP32 irSender(2, 0);  
    heatpumpIR->send(irSender, powerModeCmd, operatingModeCmd, fanSpeedCmd, temperatureCmd, swingVCmd, swingHCmd);
  }

 protected:
    HeatpumpIR *heatpumpIR;
    uint8_t powerModeCmd;
    uint8_t operatingModeCmd;
    uint8_t fanSpeedCmd;
    uint8_t temperatureCmd;
    uint8_t swingVCmd;
    uint8_t swingHCmd;
    esphome::sensor::Sensor **sensor_{nullptr};
};
#pragma once

#include "esphome/components/dmx512/dmx512.h"
#include "esphome/components/light/addressable_light.h"

namespace esphome {
namespace dmx512 {

enum class DMX512PixelOrder {
  GBWR, GBRW, GBR , GWBR, GRBW, GRB , GWRB, GRWB,
  BGWR, BGRW, BGR , BWGR, BRGW, BRG , BWRG, BRWG,
  RGWB, RGBW, RGB , RWGB, RBGW, RBG , RWBG, RBWG,
  WGRB, WGBR,       WRGB, WBGR,       WRBG, WBRG,
  GRBWC,
  RGBCWE
};

class DMX512LightOutput : public light::AddressableLight {
 public:
  void clear_effect_data() override;
  void setup() override;
  void write_state(light::LightState *state) override;
  float get_setup_priority() const override;
  int32_t size() const override;
  light::LightTraits get_traits() override;
  light::ESPColorView get_view_internal(int32_t index) const override;

  void add_leds(uint16_t pixel_count, DMX512PixelOrder order, uint16_t channel_offset);
  void set_universe(DMX512 *universe);

 protected:
  uint8_t pixel_count_{1};
  DMX512PixelOrder order_{DMX512PixelOrder::RGB};
  uint8_t channel_offset_{0};

  uint8_t *data_{nullptr};
  uint8_t *effect_data_{nullptr};
  DMX512 *universe_{nullptr};

 private:
  uint8_t get_pixel_size() const;
  uint8_t get_pixel_offset() const;
};

}  // namespace dmx512
}  // namespace esphome

#include "dmx512_light.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dmx512 {

void DMX512LightOutput::clear_effect_data() {
  for (size_t i = 0; i < this->size(); i++)
    this->effect_data_[i] = 0;
}

void DMX512LightOutput::add_leds(uint16_t pixel_count, DMX512PixelOrder order, uint16_t channel_offset) {
  this->pixel_count_ = pixel_count;
  this->channel_offset_ = channel_offset;
  this->order_ = order;

  this->data_ = new uint8_t[this->size()*this->get_pixel_size()];  // NOLINT

  for (size_t i = 0; i < (this->size() * this->get_pixel_size()); i++) {
    this->universe_->set_channel_used(this->channel_offset_ * this->get_pixel_size() + i + 1);
  }
}

void DMX512LightOutput::setup() {
  for (size_t i = 0; i < this->size(); i++) {
    (*this)[i] = Color(0, 0, 0, 0);
  }

  this->effect_data_ = new uint8_t[this->size()];  // NOLINT
}

void DMX512LightOutput::write_state(light::LightState *state) {
  this->mark_shown_();
  if(this->universe_) {
    this->universe_->write_channels(this->channel_offset_ * this->get_pixel_size() + 1, this->data_, this->size() * this->get_pixel_size());
  }
}

float DMX512LightOutput::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

int32_t DMX512LightOutput::size() const {
  return this->pixel_count_;
}

void DMX512LightOutput::set_universe(DMX512 *universe) {
  this->universe_ = universe;
}

light::LightTraits DMX512LightOutput::get_traits() {
  auto traits = light::LightTraits();
  switch(this->order_)
  {
    case DMX512PixelOrder::GBR: case DMX512PixelOrder::GRB: case DMX512PixelOrder::BGR:
    case DMX512PixelOrder::RGB: case DMX512PixelOrder::BRG: case DMX512PixelOrder::RBG:
      traits.set_supported_color_modes({light::ColorMode::RGB});
      break;
    case DMX512PixelOrder::GBWR: case DMX512PixelOrder::GBRW: case DMX512PixelOrder::GWBR: case DMX512PixelOrder::GRBW:
    case DMX512PixelOrder::GWRB: case DMX512PixelOrder::GRWB: case DMX512PixelOrder::BGWR: case DMX512PixelOrder::BGRW:
    case DMX512PixelOrder::WGBR: case DMX512PixelOrder::RGBW: case DMX512PixelOrder::WGRB: case DMX512PixelOrder::RGWB:
    case DMX512PixelOrder::BWGR: case DMX512PixelOrder::BRGW: case DMX512PixelOrder::WBGR: case DMX512PixelOrder::RBGW:
    case DMX512PixelOrder::WRGB: case DMX512PixelOrder::RWGB: case DMX512PixelOrder::BWRG: case DMX512PixelOrder::BRWG:
    case DMX512PixelOrder::WBRG: case DMX512PixelOrder::RBWG: case DMX512PixelOrder::WRBG: case DMX512PixelOrder::RWBG:
      traits.set_supported_color_modes({light::ColorMode::RGB_WHITE});
      break;
    case DMX512PixelOrder::GRBWC:
      traits.set_supported_color_modes({light::ColorMode::RGB_WHITE});
      //traits.set_supported_color_modes({light::ColorMode::RGB_COLD_WARM_WHITE });
      break;
    case DMX512PixelOrder::RGBCWE:
      traits.set_supported_color_modes({light::ColorMode::RGB_WHITE});
      break;
  }
  return traits;
}

uint8_t DMX512LightOutput::get_pixel_size() const {
  switch(this->order_)
  {
    case DMX512PixelOrder::GBR: case DMX512PixelOrder::GRB: case DMX512PixelOrder::BGR:
    case DMX512PixelOrder::RGB: case DMX512PixelOrder::BRG: case DMX512PixelOrder::RBG:
      return 3;
    case DMX512PixelOrder::GBWR: case DMX512PixelOrder::GBRW: case DMX512PixelOrder::GWBR: case DMX512PixelOrder::GRBW:
    case DMX512PixelOrder::GWRB: case DMX512PixelOrder::GRWB: case DMX512PixelOrder::BGWR: case DMX512PixelOrder::BGRW:
    case DMX512PixelOrder::WGBR: case DMX512PixelOrder::RGBW: case DMX512PixelOrder::WGRB: case DMX512PixelOrder::RGWB:
    case DMX512PixelOrder::BWGR: case DMX512PixelOrder::BRGW: case DMX512PixelOrder::WBGR: case DMX512PixelOrder::RBGW:
    case DMX512PixelOrder::WRGB: case DMX512PixelOrder::RWGB: case DMX512PixelOrder::BWRG: case DMX512PixelOrder::BRWG:
    case DMX512PixelOrder::WBRG: case DMX512PixelOrder::RBWG: case DMX512PixelOrder::WRBG: case DMX512PixelOrder::RWBG:
      return 4;
    case DMX512PixelOrder::GRBWC:
      return 5;
    case DMX512PixelOrder::RGBCWE:
      return 6;
  }
  return 0;
}

const uint8_t RED{0};
const uint8_t GREEN{1};
const uint8_t BLUE{2};
const uint8_t WHITE{3};

const uint8_t RED_OFFSET{6};
const uint8_t GREEN_OFFSET{4};
const uint8_t BLUE_OFFSET{2};
const uint8_t WHITE_OFFSET{0};

#define create_conversion_map(c1, c2, c3, c4) uint8_t((c1 << RED_OFFSET) | (c2 << GREEN_OFFSET) | (c3 << BLUE_OFFSET) | (c4 << WHITE_OFFSET))

uint8_t DMX512LightOutput::get_pixel_offset() const {
  switch(this->order_)
  {
    case DMX512PixelOrder::GBR:    return 0b11000110;
    case DMX512PixelOrder::GRB:    return 0b10000111;
    case DMX512PixelOrder::BGR:    return 0b10000111;
    case DMX512PixelOrder::RGB:    return 0b11001001;
    case DMX512PixelOrder::BRG:    return 0b01001011;
    case DMX512PixelOrder::RBG:    return 0b01001011;
    case DMX512PixelOrder::GBWR:   return 0b10001101;
    case DMX512PixelOrder::GBRW:   return 0b01001110;
    case DMX512PixelOrder::GWBR:   return 0b11010010;
    case DMX512PixelOrder::GRBW:   return 0b10010011;
    case DMX512PixelOrder::GWRB:   return 0b10010011;
    case DMX512PixelOrder::GRWB:   return 0b11011000;
    case DMX512PixelOrder::BGWR:   return 0b00011011;
    case DMX512PixelOrder::BGRW:   return 0b00011011;
    case DMX512PixelOrder::WGBR:   return 0b10011100;
    case DMX512PixelOrder::RGBW:   return 0b00011110;
    case DMX512PixelOrder::WGRB:   return 0b11100001;
    case DMX512PixelOrder::RGWB:   return 0b01100011;
    case DMX512PixelOrder::BWGR:   return 0b01100011;
    case DMX512PixelOrder::BRGW:   return 0b11100100;
    case DMX512PixelOrder::WBGR:   return 0b00100111;
    case DMX512PixelOrder::RBGW:   return 0b00100111;
    case DMX512PixelOrder::WRGB:   return 0b01101100;
    case DMX512PixelOrder::RWGB:   return 0b00101101;
    case DMX512PixelOrder::BWRG:   return 0b10110001;
    case DMX512PixelOrder::BRWG:   return 0b01110010;
    case DMX512PixelOrder::WBRG:   return 0b10110100;
    case DMX512PixelOrder::RBWG:   return 0b00110110;
    case DMX512PixelOrder::WRBG:   return create_conversion_map(WHITE, RED  , BLUE, GREEN);
    case DMX512PixelOrder::RWBG:   return create_conversion_map(RED  , WHITE, BLUE, GREEN);
    case DMX512PixelOrder::GRBWC:  return create_conversion_map(GREEN, RED  , BLUE, WHITE);
    case DMX512PixelOrder::RGBCWE: return create_conversion_map(RED  , GREEN, BLUE, WHITE);
  }
  return 0b00011011;
}

light::ESPColorView DMX512LightOutput::get_view_internal(int32_t index) const {  // NOLINT
  uint8_t *base = this->data_ + this->get_pixel_size() * index;

  auto offset = get_pixel_offset();

  return light::ESPColorView(
      (this->get_pixel_size()>0) ? base + ((offset >> RED_OFFSET) & 0b11) : nullptr,
      (this->get_pixel_size()>1) ? base + ((offset >> GREEN_OFFSET) & 0b11) : nullptr,
      (this->get_pixel_size()>2) ? base + ((offset >> BLUE_OFFSET) & 0b11) : nullptr,
      (this->get_pixel_size()>3) ? base + ((offset >> WHITE_OFFSET) & 0b11) + 1 : nullptr,
      this->effect_data_ + index,
      &this->correction_);
}

}  // namespace dmx512
}  // namespace esphome

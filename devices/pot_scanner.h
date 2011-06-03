// Copyright 2009 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
//
// An alternative to InputArray that uses oversampling + averaging for getting
// stable potentiometers readouts.

#ifndef AVRLIB_DEVICES_POT_SCANNER_H_
#define AVRLIB_DEVICES_POT_SCANNER_H_

#include "avrlib/adc.h"

namespace avrlib {

template<
    uint8_t num_inputs,
    uint8_t oversampling = 8,
    uint8_t first_input_index = 0>
class PotScanner {
 public:
  PotScanner() { }

  static inline void Init() {
    Init();
    Adc::set_alignment(ADC_LEFT_ALIGNED);
    scan_cycle_ = 0;
    Adc::StartConversion(scan_cycle_ + first_input_index);
    history_ptr_ = 0;
    memset(history_, 0, sizeof(history_));
    memset(value_, 0, sizeof(value_));
  }
  
  static inline void Read() {
    uint8_t index = history_ptr_ + oversampling * scan_cycle_;
    value_[scan_cycle_] -= history_[index];
    Adc::Wait();
    history_[index] = Adc::ReadOut8();
    value_[scan_cycle_] += history_[index];
    ++scan_cycle_;
    if (scan_cycle_ == num_inputs) {
      scan_cycle_ = 0;
      ++history_ptr_;
      if (history_ptr_ == oversampling) {
        history_ptr_ = 0;
      }
    }
    Adc::StartConversion(scan_cycle_ + first_input_index);
  }
  
  static inline uint16_t value(uint8_t index) {
    return value_[index];
  }

 private:
  static uint8_t scan_cycle_;
  static uint8_t history_[num_inputs * oversampling];
  static uint16_t value_[num_inputs];
  static uint8_t history_ptr_;

  DISALLOW_COPY_AND_ASSIGN(PotScanner);
};

/* static */
template<uint8_t num_inputs, uint8_t oversampling, uint8_t first_input_index>
uint8_t PotScanner<num_inputs, oversampling, first_input_index>::scan_cycle_;

/* static */
template<uint8_t num_inputs, uint8_t oversampling, uint8_t first_input_index>
uint8_t PotScanner<num_inputs, oversampling, first_input_index>::history_ptr_;

/* static */
template<uint8_t num_inputs, uint8_t oversampling, uint8_t first_input_index>
uint16_t PotScanner<
    num_inputs,
    oversampling,
    first_input_index>::value_[num_inputs];

/* static */
template<uint8_t num_inputs, uint8_t oversampling, uint8_t first_input_index>
uint8_t PotScanner<
    num_inputs,
    oversampling,
    first_input_index>::history_[num_inputs * oversampling];


}  // namespace avrlib

#endif  // AVRLIB_DEVICES_POT_SCANNER_H_

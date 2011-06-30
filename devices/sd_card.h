// Copyright 2011 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
// 
// Mainly based on the Arduino Sd2Card Library by Arduino Sd2Card Library,
// Copyright (C) 2009 by William Greiman.
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
// SD / SDHC driver. Note that this only provides sector-level IO functions -
// the actual FS implementation is in a second class.

#ifndef AVRLIB_DEVICES_SD_CARD_H_
#define AVRLIB_DEVICES_SD_CARD_H_

#include "avrlib/avrlib.h"
#include "avrlib/time.h"

namespace avrlib {

enum SdCommands {
  SD_CMD_GO_IDLE_STATE = 0,
  SD_CMD_SEND_IF_COND = 8,
  SD_CMD_SEND_CSD = 9,
  SD_CMD_SEND_CID = 10,
  SD_CMD_SEND_STATUS = 13,
  SD_CMD_READ_SINGLE_BLOCK = 17,
  SD_CMD_WRITE_BLOCK = 24,
  SD_CMD_WRITE_MULTIPLE_BLOCK = 25,
  SD_CMD_ERASE_WR_BLK_START = 32,
  SD_CMD_ERASE_WR_BLK_END = 33,
  SD_CMD_ERASE = 38,
  SD_CMD_APP_CMD = 55,
  SD_CMD_READ_OCR = 58,
};

enum SdAppCommands {
  SD_AMCD_SET_WR_BLK_ERASE_COUNT = 23,
  SD_AMCD_SD_SEND_OP_COND = 41,
};

enum SdStatus {
  SD_OK = 0,
  SD_ERROR_INIT,

  SD_ERROR_READ_REG,
  SD_ERROR_BAD_VERSION,

  SD_ERROR_READ,
  SD_ERROR_WRITE,
  SD_ERROR_MULTIPLE_WRITE,
  SD_ERROR_ERASE,

  SD_ERROR_INIT_TIMEOUT,
  SD_ERROR_READ_TIMEOUT,
  SD_ERROR_WRITE_TIMEOUT,
  SD_ERROR_ERASE_TIMEOUT,
};

enum SdState {
  SD_STATE_READY = 0,
  SD_STATE_IDLE = 1,
  SD_STATE_ILLEGAL_COMMAND = 4,
};

enum SdCardType {
  SD_SD1,
  SD_SD2,
  SD_SDHC
};

struct CID {
  uint8_t manufacturer_id;
  uint8_t application_id[2];
  uint8_t product_name[5];
  unsigned product_revision_m : 4;
  unsigned product_revision_n : 4;
  uint32_t serial_number;
  unsigned manufacturing_year_hi : 4;
  unsigned reserved : 4;
  unsigned manufacturing_month: 4;
  unsigned manufacturing_year_low: 4;
  unsigned not_used : 1;
  unsigned crc7 : 7;
};

typedef struct CSDv1 {
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  uint8_t taac;
  uint8_t nsac;
  uint8_t tran_speed;
  uint8_t ccc_high;
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  unsigned c_size_high : 2;
  unsigned reserved2 : 2;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  uint8_t c_size_mid;
  unsigned vdd_r_curr_max : 3;
  unsigned vdd_r_curr_min : 3;
  unsigned c_size_low :2;
  unsigned c_size_mult_high : 2;
  unsigned vdd_w_cur_max : 3;
  unsigned vdd_w_curr_min : 3;
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned c_size_mult_low : 1;
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved3 : 2;
  unsigned wp_grp_enable : 1;
  unsigned reserved4 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  unsigned reserved5: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  unsigned always1 : 1;
  unsigned crc : 7;
};

typedef struct CSDv2 {
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  uint8_t taac;
  uint8_t nsac;
  uint8_t tran_speed;
  uint8_t ccc_high;
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  unsigned reserved2 : 4;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  unsigned reserved3 : 2;
  unsigned c_size_high : 6;
  uint8_t c_size_mid;
  uint8_t c_size_low;
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned reserved4 : 1;
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved5 : 2;
  unsigned wp_grp_enable : 1;
  unsigned reserved6 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  unsigned reserved7: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  unsigned always1 : 1;
  unsigned crc : 7;
};

union CSD {
  CSDv1 v1;
  CSDv2 v2;
};

struct BaseSdCardConfig {
  enum {
    busy_timeout = 0,
    init_timeout = 0,
    erase_timeout = 0,
    read_timeout = 0,
    write_timeout = 0,
    /*busy_timeout = 300,
    init_timeout = 1000,
    erase_timeout = 2000,
    read_timeout = 200,
    write_timeout = 400,*/
  };
};

template<typename Spi, typename Config = BaseSdCardConfig>
class SdCard {
 public:
  SdCard() { }
  
  uint8_t Init() {
    Spi::Init();
    pending_read_ = 0;
    block_offset_ = 0;
    status_ = 0;
    type_ = 0;
    
    Spi::End();
    // Look at me, I'm a clock!
    for (uint8_t i = 0; i < 10; ++i) {
      Spi::Send(0xff);
    }
    
    {
      // Make sure that the CS pin is set to high whenever we leave this block.
      scoped_resource<Spi> spi_session;
    
      // Ask card to go to idle mode.
      if (!WaitForStatus<Config::init_timeout>(
          SD_CMD_GO_IDLE_STATE,
          SD_STATE_IDLE)) {
        return SD_ERROR_INIT;
      }
    
      // Try a V2 command to determine the card version.
      if (Command(SD_CMD_SEND_IF_COND, 0x1aa) & SD_STATE_ILLEGAL_COMMAND) {
        type_ = SD_SD1;
      } else {
        Swallow(4);
        if (status_ != 0xaa) {
          return SD_ERROR_INIT;
        }
        type_ = SD_SD2;
      }
    
      // Initialize card.
      if (!WaitForAStatus<Config::init_timeout>(
          SD_AMCD_SD_SEND_OP_COND,
          type_ == SD_SD2 ? 0x40000000 : 0,
          SD_STATE_READY)) {
        return SD_ERROR_INIT;
      }
    
      // Check for SDHC.
      if (type_ == SD_SD2) {
        if (Command(SD_CMD_READ_OCR, 0)) {
          return SD_ERROR_INIT;
        }
        if ((Spi::Receive() & 0xc0) == 0xc0) {
          type_ = SD_SDHC;
        }
        Swallow(3);
      }
      return SD_OK;
    }
  }
  
  static inline uint8_t type() { return type_; }

 private:
  static void Swallow(uint8_t n) {
    for (uint8_t i = 0; i < n; ++i) {
      status_ = Spi::Receive();
    }
  }
  static uint8_t Command(uint8_t command, uint32_t argument) {
    FlushRead();
    
    Spi::Begin();
    WaitNotBusy<Config::busy_timeout>();
    Spi::Send(command | 0x40);
    LongWord l;
    l.value = argument;
    Spi::Send(l.bytes[3]);
    Spi::Send(l.bytes[2]);
    Spi::Send(l.bytes[1]);
    Spi::Send(l.bytes[0]);
    uint8_t crc = 0xff;
    if (command == SD_CMD_GO_IDLE_STATE) {
      crc = 0x95;
    }
    if (command == SD_CMD_SEND_IF_COND) {
      crc = 0x87;
    }
    Spi::Send(crc);
    status_ = 0x80;
    for (uint8_t i = 0; i < 255; i++) {
      status_ = Spi::Receive();
      if (!(status_ & 0x80)) {
        break;
      }
    }
    return status_;
  }
  
  static uint8_t ACommand(uint8_t command, uint32_t argument) {
    Command(SD_CMD_APP_CMD, 0);
    return Command(command, argument);
  }
  
  // Read a partially block until its end.
  static void FlushRead() {
    if (pending_read_) {
      while (block_offset_++ < 514) {
        Spi::Receive();  // > /dev/null
      }
      Spi::End();
      pending_read_ = 0;
    }
  }
  
  template<uint16_t timeout>
  static uint8_t WaitForStatus(uint8_t command, uint8_t status) {
    if (timeout == 0) {
      // Arbitrary timeout that doesn't use the system clock.
      for (uint16_t i = 0; i < 0xffff; ++i) {
        if (Command(command, 0) == status) {
          return 1;
        }
      }
      return 0;
    } else {
      uint16_t now = milliseconds();
      do {
        if (Command(command, 0) == status) {
          return 1;
        }
      } while ((milliseconds() - now) < timeout);
      return 0;
    }
  }
  
  template<uint16_t timeout>
  static uint8_t WaitForAStatus(uint8_t command, uint32_t arg, uint8_t status) {
    if (timeout == 0) {
      // Arbitrary timeout that doesn't use the system clock.
      for (uint16_t i = 0; i < 0xffff; ++i) {
        if (ACommand(command, arg) == status) {
          return 1;
        }
      }
      return 0;
    } else {
      uint16_t now = milliseconds();
      do {
        if (ACommand(command, arg) == status) {
          return 1;
        }
      } while ((milliseconds() - now) < timeout);
      return 0;
    }
  }
  
  template<uint16_t timeout>
  static uint8_t WaitNotBusy() {
    if (timeout == 0) {
      // Arbitrary timeout that doesn't use the system clock.
      for (uint16_t i = 0; i < 0xffff; ++i) {
        if (Spi::Receive() == 0xff) {
          return 1;
        }
      }
      return 0;
    } else {
      uint16_t now = milliseconds();
      do {
        if (Spi::Receive() == 0xff) {
          return 1;
        }
      } while ((milliseconds() - now) < timeout);
      return 0;
    }
  }
  
  static uint8_t pending_read_;
  static uint8_t status_;
  static uint8_t type_;
  static uint16_t block_offset_;
   
  DISALLOW_COPY_AND_ASSIGN(SdCard);
};

/* static */
template<typename Spi, typename Config>
uint8_t SdCard<Spi, Config>::pending_read_;

/* static */
template<typename Spi, typename Config>
uint16_t SdCard<Spi, Config>::block_offset_;

/* static */
template<typename Spi, typename Config>
uint8_t SdCard<Spi, Config>::status_;

/* static */
template<typename Spi, typename Config>
uint8_t SdCard<Spi, Config>::type_;

}  // namespace avrlib

#endif   // AVRLIB_DEVICES_SD_CARD_H_

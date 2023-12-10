/*
 *  Copyright (C) 2020-2024 Embedded AMS B.V. - All Rights Reserved
 *
 *  This file is part of Embedded Proto.
 *
 *  Embedded Proto is open source software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, version 3 of the license.
 *
 *  Embedded Proto  is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
 *
 *  For commercial and closed source application please visit:
 *  <https://EmbeddedProto.com/license/>.
 *
 *  Embedded AMS B.V.
 *  Info:
 *    info at EmbeddedProto dot com
 *
 *  Postal adress:
 *    Atoomweg 2
 *    1627 LE, Hoorn
 *    the Netherlands
 */

#include "BLEReadBuffer.h"
#include <algorithm>
#include <string.h>

BLEReadBuffer::BLEReadBuffer()
  : data_{0},
    write_index_(0),
    read_index_(0)
{

}

uint32_t BLEReadBuffer::get_size() const
{
  return write_index_;
}

uint32_t BLEReadBuffer::get_max_size() const
{
  return MAX_SIZE;
}

bool BLEReadBuffer::peek(uint8_t& byte) const
{
  bool return_value = write_index_ > read_index_;
  if(return_value)
  {
    byte = data_[read_index_];
  }
  return return_value;
}

bool BLEReadBuffer::advance()
{
  const bool return_value = write_index_ > read_index_;
  if(return_value)
  {
    ++read_index_;
  }
  return return_value;
}

bool BLEReadBuffer::advance(const uint32_t N)
{
  const uint32_t new_read_index = read_index_ + N;
  const bool return_value = write_index_ > new_read_index;
  if(return_value)
  {
    read_index_ = new_read_index;
  }
  return return_value;
}

bool BLEReadBuffer::pop(uint8_t& byte)
{
  bool return_value = write_index_ > read_index_;
  if(return_value) {
    byte = data_[read_index_];
    ++read_index_;
  }
  return return_value;
}

uint8_t* BLEReadBuffer::get_data_array()
{
  return data_;
}

uint32_t& BLEReadBuffer::get_bytes_written()
{
  return write_index_;
}

void BLEReadBuffer::clear()
{
  read_index_ = 0;
  write_index_ = 0;
  memset(data_, 0, MAX_SIZE);
}

bool BLEReadBuffer::push(uint8_t& byte)
{
  bool return_value = MAX_SIZE > write_index_;
  if(return_value)
  {
    data_[write_index_] = byte;
    ++write_index_;
  }
  return return_value;
}

bool BLEReadBuffer::set(const uint8_t* data, const uint32_t length)
{
  const bool return_value = get_max_size() >= length;
  const uint32_t copy_n_bytes = std::min(length, get_max_size());
  clear();
  memcpy(data_, data, copy_n_bytes);
  write_index_ = copy_n_bytes;
  return return_value;
}


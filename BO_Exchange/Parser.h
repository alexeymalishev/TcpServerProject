#pragma once

#include <boost\shared_array.hpp>
#include <boost\noncopyable.hpp>
#include "..\common\dllexport_types.h"
#include "..\common\IParser.h"

class CParser : public tcpserver::IParser
{
public:
  CParser();
  virtual ~CParser();

  virtual tcpserver::outbuffer_ptr Parse(boost::weak_ptr<tcpserver::Connection> connection_ptr,
                                        tcpserver::InArrayIterator in_data_start,
                                        tcpserver::InArrayIterator in_data_end) throw (std::exception);

private:
  enum ParserState
  {
    PS_Empty   = 0x00,
    PS_Filling = 0x01,
    PS_Token   = 0x02,
    PS_Error   = 0x04
  };

  const char kStartToken_ = 's';
  const char kEndToken_ = 'e';
  const char kReplaceToken_ = '/';
  const char kReplaceStartToken_ = 'a';
  const char kReplaceEndToken_ = 'b';

  ///TODO: check crc
  const uint16_t kCRC_ok_ = 0xF0B8;

  const uint8_t kProtocolVersion_ = 1;
  const size_t kMessageHeaderSize_ = 13;

  const uint16_t kTransmissionPeriod = 10;

  const static unsigned short Crc16Table[256];

  uint32_t device_id_;
  uint32_t state_;
  uint32_t in_message_pid_;
  uint32_t out_message_pid_;

  std::vector<char> message_;

  uint16_t crc_;

  inline void push(const char& ch)
  {
    message_.push_back(ch);
    crc16(ch);
  }
  inline void clear()
  {
    message_.clear();
    crc_ = 0xFFFF;
  }

  inline void crc16(char ch)
  {
    crc_ = (crc_ >> 8) ^ Crc16Table[(crc_ ^ ch) & 0xff];
  }

  inline void pushout(tcpserver::outbuffer_ptr& outbuffer, const char& ch)
  {
    if (ch == kStartToken_)
    {
      outbuffer->push_back(kReplaceToken_);
      outbuffer->push_back(kReplaceStartToken_);
    }
    else if (ch == kEndToken_)
    {
      outbuffer->push_back(kReplaceToken_);
      outbuffer->push_back(kReplaceEndToken_);
    }
    else if (ch == kReplaceToken_)
    {
      outbuffer->push_back(kReplaceToken_);
      outbuffer->push_back(kReplaceToken_);
    }
    else
    {
      outbuffer->push_back(ch);
    }
  }

  void Process(tcpserver::outbuffer_ptr& buf_list);
  void AddAnsMessage(tcpserver::outbuffer_ptr& buf_list, int16_t ec);
  void AddParamMessage(tcpserver::outbuffer_ptr& buf_list, uint8_t flags, uint8_t code, uint8_t lenght, uint8_t const * const data);
};


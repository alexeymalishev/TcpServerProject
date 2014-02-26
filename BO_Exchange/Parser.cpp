#include "stdafx.h"
#include "Parser.h"
#include "Message.h"
#include "DBInterface.h"

const unsigned short CParser::Crc16Table[256] = {
  0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
  0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
  0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
  0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
  0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
  0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
  0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
  0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
  0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
  0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
  0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
  0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
  0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
  0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
  0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
  0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
  0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
  0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
  0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
  0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
  0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
  0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
  0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
  0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
  0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
  0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
  0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
  0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
  0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
  0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
  0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
  0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78,
};

CParser::CParser() :
 device_id_(0),
 state_(PS_Empty),
 in_message_pid_(0),
 out_message_pid_(0),
 message_(1024),
 crc_(0xFFFF)
{
}

CParser::~CParser()
{
}

tcpserver::outbuffer_ptr CParser::Parse(boost::weak_ptr<tcpserver::Connection> connection_ptr,
  tcpserver::InArrayIterator in_data_start,
  tcpserver::InArrayIterator in_data_end)  throw (std::exception)
{
  tcpserver::outbuffer_ptr outbuffer(new std::vector<char>);
  outbuffer->reserve(1024);                       //reserve buffer for minimize reallocate opperations

  if (in_data_start < in_data_end)
  {
    //check connected device if we don't set device Id yet, we think, it's first data packet, and must started by start marker
    if ((device_id_ != 0) || (*in_data_start == kStartToken_) || (state_ & PS_Filling))
    {
      for (auto cur_element = in_data_start; cur_element != in_data_end; ++cur_element)
      {
        if ((state_ & PS_Filling) == 0)   //buffer empty
        {
          if (*cur_element != kStartToken_)  //we get not the start toket
          {
            if ((state_ & PS_Error) == 0)
            {
              //TODO: here we need send error message to DB
              parser::CDBInterface::Instance().PushLogMessage(device_id_, -1, "synchronization filed excess data between end&start");
              BOOST_LOG_TRIVIAL(error) << "synchronization filed excess data between end&start";
              AddAnsMessage(outbuffer, -1);
              /////////////////////////////////////////////
              state_ = PS_Error;
            }
            continue;  // process next element
          }
          state_ = PS_Filling;
          clear();   // clear message buffer
          continue; // proceed next element
        }
        else
        {
          if (*cur_element == kStartToken_)
          {
            //TODO: here we need send error message to DB
            parser::CDBInterface::Instance().PushLogMessage(device_id_, -1, "synchronization filed start without end");
            BOOST_LOG_TRIVIAL(error) << "synchronization filed start without end";
            AddAnsMessage(outbuffer, -1);
            /////////////////////////////////////////////
            state_ = PS_Filling;
            clear();   // clear message buffer
            continue; // proceed next element
          }
        }

        if (*cur_element == kEndToken_)
        {
          if (state_ & PS_Token)
          {
            //TODO: here we need send error message to DB
            parser::CDBInterface::Instance().PushLogMessage(device_id_, -1, "synchronization filed end token after '/' token");
            BOOST_LOG_TRIVIAL(error) << "synchronization filed end token after '/' token";
            AddAnsMessage(outbuffer, -1);
            /////////////////////////////////////////////
            state_ = PS_Error;
            continue;
          }

          Process(outbuffer);

          state_ = PS_Empty;
          continue;
        }
        if (state_ & PS_Token)
        {
          if (*cur_element == kReplaceToken_)
          {
            push(kReplaceToken_);
          }
          else if (*cur_element == kReplaceStartToken_)
          {
            push(kStartToken_);
          }
          else if (*cur_element == kReplaceEndToken_)
          {
            push(kEndToken_);
          }
          else
          {
            //TODO: here we need send error message to DB we recieve undefined token
            parser::CDBInterface::Instance().PushLogMessage(device_id_, -1, "synchronization filed wrong token after '/'");
            BOOST_LOG_TRIVIAL(error) << "synchronization filed wrong token after '/'";
            AddAnsMessage(outbuffer, -1);
            /////////////////////////////////////////////
            state_ = PS_Error;
            continue;
          }
          state_ ^= PS_Token;
          continue;
        }

        if (*cur_element == kReplaceToken_)
        {
          state_ |= PS_Token;
          continue;
        }

        push(*cur_element);
      }
    }
    else
    {
      //TODO: here we need send error message to DB we recieve undefined message
      parser::CDBInterface::Instance().PushLogMessage(device_id_, -1, "synchronization filed handshake error");
      BOOST_LOG_TRIVIAL(error) << "synchronization filed handshake error";
      throw std::exception("Recieve undefined message, close connection.");
    }
  }

  return outbuffer;
} // tcpserver::outbuffer_ptr CParser::Parse

void CParser::AddAnsMessage(tcpserver::outbuffer_ptr& outbuffer, int16_t ec)
{
  auto message_size = kMessageHeaderSize_ + 4 - 2;

  std::unique_ptr<char> tmpBuf(new char[message_size]);
  Message* mes = reinterpret_cast<Message*>(tmpBuf.get());

  mes->device_id = 0;
  mes->flags = 0x80;
  mes->length = message_size;
  mes->pid = out_message_pid_++;
  mes->type = MT_ANS;
  mes->protocol_version = kProtocolVersion_;

  uint8_t* mesData = &mes->data;

  mesData[0] = in_message_pid_ & 0xFF;
  mesData[1] = in_message_pid_ >> 8;

  mesData[2] = ec & 0xFF;
  mesData[3] = ec >> 8;

  outbuffer->push_back(kStartToken_);

  crc_ = 0xFFFF;

  for (size_t ii = 0; ii < message_size; ++ii) {
    crc16(tmpBuf.get()[ii]);
    pushout(outbuffer, tmpBuf.get()[ii]);
  }

  crc_ = ~crc_;

  pushout(outbuffer, crc_ & 0xFF);
  pushout(outbuffer, crc_ >> 8);

  outbuffer->push_back(kEndToken_);
} // void CParser::AddErrorMessage

void CParser::AddParamMessage(tcpserver::outbuffer_ptr& outbuffer, uint8_t flags, uint8_t code, uint8_t lenght, uint8_t const * const data)
{
  auto message_size = kMessageHeaderSize_ + 3 + lenght - 2;
  
  std::unique_ptr<char> tmpBuf(new char[message_size]);
  Message* mes = reinterpret_cast<Message*>(tmpBuf.get());

  mes->device_id = 0;
  mes->flags = 0;
  mes->length = message_size;
  mes->pid = out_message_pid_++;
  mes->type = MT_PARAM;
  mes->protocol_version = kProtocolVersion_;

  uint8_t* mesData = &mes->data;

  mesData[0] = flags;
  mesData[1] = code;
  mesData[2] = lenght;

  memcpy(mesData + 3, data, lenght);

  outbuffer->push_back(kStartToken_);

  crc_ = 0xFFFF;

  for (size_t ii = 0; ii < message_size; ++ii) {
    crc16(tmpBuf.get()[ii]);
    pushout(outbuffer, tmpBuf.get()[ii]);
  }

  crc_ = ~crc_;

  pushout(outbuffer, crc_ & 0xFF);
  pushout(outbuffer, crc_ >> 8);

  outbuffer->push_back(kEndToken_);

} // void CParser::AddParamMessage


void CParser::Process(tcpserver::outbuffer_ptr& outbuffer)
{
  if (message_.size() < kMessageHeaderSize_)
  {
    //TODO: proceed Error message size
    parser::CDBInterface::Instance().PushLogMessage(device_id_, -1, "wrong message size");
    BOOST_LOG_TRIVIAL(error) << "wrong message size";
    AddAnsMessage(outbuffer, -1);
    //////////////////////////////////
    return;
  }

  if (crc_ != kCRC_ok_)
  {
    //TODO: proceed Error crc
    parser::CDBInterface::Instance().PushLogMessage(device_id_, -1, "crc failed");
    BOOST_LOG_TRIVIAL(error) << "crc failed";
    AddAnsMessage(outbuffer, -1);
    //////////////////////////////////
    return;
  }

  Message* mes = reinterpret_cast<Message*>(&message_[0]);

  if (message_.size() != mes->length)
  {
    //TODO: proceed Error message size
    parser::CDBInterface::Instance().PushLogMessage(device_id_, -1, "wrong message size");
    BOOST_LOG_TRIVIAL(error) << "wrong message size";
    AddAnsMessage(outbuffer, -1);
    //////////////////////////////////
    return;
  }

  if (kProtocolVersion_ != mes->protocol_version)
  {
    //TODO: proceed Error protocol version
    parser::CDBInterface::Instance().PushLogMessage(device_id_, -1, "wrong protocol");
    BOOST_LOG_TRIVIAL(error) << "wrong protocol";
    AddAnsMessage(outbuffer, -1);
    //////////////////////////////////
    return;
  }

  if (mes->device_id == 0)
  {
    //TODO: proceed Error device_id
    parser::CDBInterface::Instance().PushLogMessage(device_id_, -1, "message devices id is 0");
    BOOST_LOG_TRIVIAL(error) << "message devices id is 0";
    AddAnsMessage(outbuffer, -1);
    //////////////////////////////////
    return;
  }

  if (device_id_ == 0)
  {
    device_id_ = mes->device_id;
  }
  else
  {
    if (device_id_ != mes->device_id)
    {
      //TODO: proceed Error incompatible device_id
      parser::CDBInterface::Instance().PushLogMessage(device_id_, -1, "incoming message devices id not equal to connection device id");
      BOOST_LOG_TRIVIAL(error) << "incoming message devices id not equal to connection device id";
      AddAnsMessage(outbuffer, -1);
      //////////////////////////////////
      return;
    }
  }
   
  switch (mes->type)
  {
  case MT_COORDS:
  {
    parser::CDBInterface::Instance().PushCoordData(device_id_, &mes->data);
  }
    break;
  case MT_FIRMWARE:
  {
    parser::CDBInterface::Instance().PushFirmwareData(device_id_, &mes->data);

    //set new transmission period
    AddParamMessage(outbuffer, 0x08, 0x01, 2, reinterpret_cast<const uint8_t*>(&kTransmissionPeriod));
  }
    break;
  case MT_ANS:
  {
                //TODO: here must be ans message handler
  }
    break;
  case MT_ACC:
  case MT_NOP:
  case MT_PARAM:
  case MT_TEMP:
  case MT_POWER:
  case MT_DIG_INPUTS:
  case MT_GET_MES:
  default:
    //TODO: proceed Error incompatible message type
    AddAnsMessage(outbuffer, -1);
    //////////////////////////////////
  }

   in_message_pid_ = mes->pid;

  //TODO: proceed OK Ans
  if (mes->flags & 0x80)
  {
    AddAnsMessage(outbuffer, 0);
  }
  //////////////////////////////////
} // void CParser::Process


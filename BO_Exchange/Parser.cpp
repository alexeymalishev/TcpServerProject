#include "stdafx.h"
#include "Parser.h"
#include "Message.h"
#include "DBInterface.h"

//TODO: recalculate CRC table
//const unsigned short CParser::Crc16Table[256] = {
//
//  0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
//  0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
//  0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
//  0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
//  0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
//  0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
//  0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
//  0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
//  0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
//  0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
//  0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
//  0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
//  0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
//  0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
//  0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
//  0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
//  0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
//  0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
//  0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
//  0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
//  0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
//  0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
//  0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
//  0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
//  0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
//  0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
//  0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
//  0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
//  0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
//  0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
//  0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
//  0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
//
//};

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
  size_t message_size = kMessageHeaderSize_ + 4;

  size_t head_ptr = outbuffer->size();
  outbuffer->resize(head_ptr + message_size);
  Message* mes = reinterpret_cast<Message*>(outbuffer->data() + head_ptr);

  mes->device_id = 0;
  mes->flags = 0x80;
  mes->length = message_size;
  mes->pid = out_message_pid_++;
  mes->type = MT_PARAM;
  mes->protocol_version = kProtocolVersion_;

  uint8_t* mesData = &mes->data;

  mesData[0] = in_message_pid_ & 0xFF;
  mesData[1] = in_message_pid_ >> 8;

  mesData[2] = ec & 0xFF;
  mesData[3] = ec >> 8;

  //TODO: check crc compute and function

  crc_ = 0xFFFF;

  for (size_t ii = 0; ii < message_size - 2; ++ii) {
    crc16(outbuffer->data()[head_ptr + ii]);
  }

  crc_ = ~crc_;

  mesData[message_size - 2] = crc_ & 0xFF;
  mesData[message_size - 1] = crc_ >> 8;
} // void CParser::AddErrorMessage

void CParser::AddParamMessage(tcpserver::outbuffer_ptr& outbuffer, uint8_t flags, uint8_t code, uint8_t lenght, uint8_t const * const data)
{
  auto message_size = kMessageHeaderSize_ + 5 + lenght;

  size_t head_ptr = outbuffer->size();
  outbuffer->resize(head_ptr + message_size);
  Message* mes = reinterpret_cast<Message*>(outbuffer->data() + head_ptr);

  mes->device_id = 0;
  mes->flags = 0;
  mes->length = message_size;
  mes->pid = out_message_pid_++;
  mes->type = MT_ANS;
  mes->protocol_version = kProtocolVersion_;

  uint8_t* mesData = &mes->data;

  mesData[0] = flags;
  mesData[1] = code;
  mesData[2] = lenght;

  memcpy(mesData + 3, data, lenght);

  //TODO: check crc compute and function

  crc_ = 0xFFFF;

  for (size_t ii = 0; ii < message_size - 2; ++ii) {
    crc16(outbuffer->data()[head_ptr + ii]);
  }

  crc_ = ~crc_;

  mesData[message_size - 2] = crc_ & 0xFF;
  mesData[message_size - 1] = crc_ >> 8;

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


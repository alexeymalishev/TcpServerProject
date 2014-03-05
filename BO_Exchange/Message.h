#pragma once

#pragma pack(push, 1)
struct Message
{
  uint16_t length;
  uint32_t device_id;
  uint8_t  protocol_version;
  uint16_t pid;
  uint8_t  flags;
  uint8_t  type;
  uint8_t  data;
};
#pragma pack(pop)

enum MessageTypes
{
  MT_COORDS     = 0x01,
  MT_ACC        = 0x02,
  MT_ANS        = 0x03,
  MT_NOP        = 0x04,
  MT_PARAM      = 0x05,
  MT_TEMP       = 0x06,
  MT_POWER      = 0x07,
  MT_DIG_INPUTS = 0x08,
  MT_FIRMWARE   = 0x09,
  MT_GET_MES    = 0x0A,
};

#pragma pack(push, 1)
struct CoordMessage{
  uint32_t time;
  int32_t  lat;
  int32_t  lon;
  int16_t  alt;
  uint16_t speed;
  uint16_t course;
  uint8_t  source;
  uint8_t  status;
};
#pragma pack(pop)

class CMessage : private boost::noncopyable
{
protected:
  CMessage(size_t sz) :
    m_nMessageSize(sz)
  {}

public:
  virtual ~CMessage() {};

  inline size_t GetSize() const { return m_nMessageSize; };

  /// add data to message
  void add(void** input_data, size_t& input_data_sz, void* output_data, size_t& output_data_sz, int* device_id)
  {
  }


protected:
  virtual void MessageProceed(void* output_data, size_t& output_data_sz, int* device_id) = 0;

  /// size of active message
  size_t m_nMessageSize;

  /// pointer to active message
  void* m_pBufferStart;
};


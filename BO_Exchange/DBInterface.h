#pragma once

#include "stdafx.h"

#include <Ole2.h>
#include <comutil.h>
#include <queue>
#include <list>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <boost\noncopyable.hpp>
//#include <boost\thread.hpp>
//#include <boost\thread\mutex.hpp>
//#include <boost\circular_buffer.hpp>

#import "c:\Program Files (x86)\Common Files\System\ado\msado15.dll" rename("EOF", "EndOfFile")

namespace parser {

class CDBInterface
	: private boost::noncopyable
{
public:
	~CDBInterface(void);

  inline static const CDBInterface& Instance()
  {
    static CDBInterface theSingleInstance;
    return theSingleInstance;
  }

	/// function for pushing data to message queue for waiting inserting to db
	void PushCoordData(const uint32_t nDeviceId, uint8_t* data) const;
  void PushFirmwareData(const uint32_t nDeviceId, uint8_t* data) const;
  void PushLogMessage(const uint32_t nDeviceId, int ec, std::string message) const;

  void Close();

private:
  CDBInterface(void);

	/// data inserter thread procedure
	void data_inserter_th();

	/// connection to database procedure
	void DBConnect();

	/// mutex for lock simultaneous access to message queue
	mutable std::mutex queue_rwmutex_;

	/// thread handler for insert data to db
	std::thread data_inserter_th_hnd_;

	/// input buffer full condition variable 
	mutable std::condition_variable write_ready_cv_;

	/// output buffer empty condition variable 
  mutable std::condition_variable read_ready_cv_;

	/// Number of row written to DB simultaneously
	const static size_t m_nMessageBufferSize;

	/// pointer to database connection
	ADODB::_ConnectionPtr m_pDBDataConnection;

	/// pointer to command for database 
	ADODB::_CommandPtr m_pDBDataCommand;


 	/// pointer to database connection
	ADODB::_ConnectionPtr m_pDBTextConnection;

	/// pointer to command for database 
	ADODB::_CommandPtr m_pDBTextCommand;

	//std::queue<std::tr1::shared_ptr<__sCoordMessage> > data_;
	
	void PrintProviderError(ADODB::_ConnectionPtr pConnection);
	void PrintComError(_com_error &e);

	inline void TESTHR(HRESULT x) {if FAILED(x) _com_issue_error(x);};


	/// thread terminate flag
	bool m_bTerminate;

	/// Pointer to command strings for writing
	bool m_bActiveCommandStr;

	mutable std::stringstream m_os[2];
	mutable volatile size_t m_nCntr;

};

} // namespace parser

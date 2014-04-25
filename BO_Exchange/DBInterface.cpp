#include "stdafx.h"
#include "DBInterface.h"

#include <iostream>
#include <iomanip>
#include <ostream>
#include <fstream>
#include <chrono>
#include <Ole2.h>

#include "../common/parameters.h"

namespace parser {

const size_t CDBInterface::m_nMessageBufferSize = 50;

CDBInterface::CDBInterface(void)
 : m_bTerminate(false)
 , m_bActiveCommandStr(false)
 , m_nCntr(0)
 , m_pDBDataConnection(0)
 , m_pDBDataCommand(0)
 , m_pDBTextConnection(0)
 , m_pDBTextCommand(0)
{
  CoInitialize(NULL);

  DBConnect();

  m_os[0] << "INSERT INTO [nav_data_table]\nVALUES ";
  m_os[1] << "INSERT INTO [nav_data_table]\nVALUES ";

  data_inserter_th_hnd_ = std::thread(&CDBInterface::DataInserterTh, this);
}


CDBInterface::~CDBInterface(void)
{
  CoUninitialize();
}

void CDBInterface::Close()
{
  m_bTerminate = true;

  data_inserter_th_hnd_.join();

  m_pDBDataConnection->Release();
  m_pDBDataConnection->Close();

  //m_pDBTextCommand->Release();
  //m_pDBTextCommand->Close();
}

void CDBInterface::DBConnect()
{
  try {
    std::string sTmpStr;

    parameters::GetParam("System\\CurrentControlSet\\Services\\TcpServer\\parameters", "DBPasswd", &sTmpStr);
    _bstr_t bstrDBPaswd = sTmpStr.c_str();

    parameters::GetParam("System\\CurrentControlSet\\Services\\TcpServer\\parameters", "DBUserName", &sTmpStr);
    _bstr_t bstrDPUsername = sTmpStr.c_str();
	
    parameters::GetParam("System\\CurrentControlSet\\Services\\TcpServer\\parameters", "DBServerName", &sTmpStr);
    _bstr_t bstrServer = sTmpStr.c_str();
	
    parameters::GetParam("System\\CurrentControlSet\\Services\\TcpServer\\parameters", "DBName", &sTmpStr);
    _bstr_t bstrDBName = sTmpStr.c_str();

    parameters::GetParam("System\\CurrentControlSet\\Services\\TcpServer\\parameters", "DBSecurity", &sTmpStr);
    _bstr_t bstrDBSecurity = sTmpStr.c_str();

    _bstr_t bstrConnection = "Provider=SQLOLEDB;Data Source=" + bstrServer + ";Initial Catalog=" + bstrDBName + ";Integrated Security=" + bstrDBSecurity + ";";

    BOOST_LOG_TRIVIAL(info) << "trying to connect to " << (LPSTR) bstrConnection;

    /// 
    TESTHR(m_pDBDataConnection.CreateInstance(__uuidof(ADODB::Connection)));

    m_pDBDataConnection->CommandTimeout = 10;
    m_pDBDataConnection->Open(bstrConnection, bstrDPUsername, bstrDBPaswd, ADODB::adConnectUnspecified);

	    // Create command object.
    TESTHR(m_pDBDataCommand.CreateInstance(__uuidof(ADODB::Command)));
    m_pDBDataCommand->ActiveConnection = m_pDBDataConnection;

    /// 
    //TESTHR(m_pDBTextConnection.CreateInstance(__uuidof(ADODB::Connection)));

    //m_pDBTextConnection->CommandTimeout = 10;
    //m_pDBTextConnection->Open(bstrConnection, bstrDPUsername, bstrDBPaswd, ADODB::adConnectUnspecified);
    ////m_pDBTextConnection->Open(bstrConnection, "", "", ADODB::adConnectUnspecified);

	   // // Create command object.
    //TESTHR(m_pDBTextCommand.CreateInstance(__uuidof(ADODB::Command)));
    //m_pDBTextCommand->ActiveConnection = m_pDBTextConnection;

    BOOST_LOG_TRIVIAL(info) << "db connection ok";
  }
  catch (_com_error &e) {
    PrintProviderError(m_pDBDataConnection);
    PrintComError(e);
    throw;
  }
  catch(std::exception& e) {
     BOOST_LOG_TRIVIAL(error) << e.what();
     throw;
  }
}

void CDBInterface::PushCoordData(const uint32_t nDeviceId, const CoordMessage* const msg) const
{
  std::unique_lock<std::mutex> lock(queue_rwmutex_);

  tm ltime;
  __time64_t time = msg->time;
  
  localtime_s(&ltime, &time);

  while(m_nCntr >= m_nMessageBufferSize)
		write_ready_cv_.wait(lock);

	if(m_nCntr)
		m_os[m_bActiveCommandStr] << ", ";

  //TODO: place data parser for place data to DB here
	m_os[m_bActiveCommandStr] << "("
			<< int(nDeviceId) << ", '"
			<< int(1900 + ltime.tm_year) << "-"
			<< std::setw(2) << std::setfill('0')
			<< int(ltime.tm_mon) << "-"
			<< std::setw(2) << std::setfill('0')
      << int(ltime.tm_mday) << "T"
			<< std::setw(2) << std::setfill('0')
      << int(ltime.tm_hour) << ":"
			<< std::setw(2) << std::setfill('0')
      << int(ltime.tm_min) << ":"
			<< std::setw(2) << std::setfill('0')
      << int(ltime.tm_sec) << "', "
			<< int(msg->lat) << ", "
			<< int(msg->lon) << ", "
			<< int(msg->speed) << ", "
			<< int(msg->course) << ", "
			<< int(msg->alt) << ", "
//			<< int(msg->source) << ", " 
			<< int(msg->status) << ")\n";

	if(m_nMessageBufferSize <= ++m_nCntr)
		read_ready_cv_.notify_one();
}

void CDBInterface::PushFirmwareData(const uint32_t nDeviceId, const uint8_t* const data) const
{
  std::stringstream str;

  std::unique_lock<std::mutex> lock(queue_rwmutex_);
  //TODO: place data parser for place data to DB here
//  str << "INSERT INTO [text_table]\nVALUES ";
//
//	//str << "("
//	//		<< int(nDeviceId) << ", '"
//	//		<< int(2000 + msg.uTxtHdr.sTxtHdr.tm_time.year) << "-"
//	//		<< std::setw(2) << std::setfill('0')
//	//		<< int(msg.uTxtHdr.sTxtHdr.tm_time.month) << "-"
//	//		<< std::setw(2) << std::setfill('0')
//	//		<< int(msg.uTxtHdr.sTxtHdr.tm_time.day) << "T"
//	//		<< std::setw(2) << std::setfill('0')
//	//		<< int(msg.uTxtHdr.sTxtHdr.tm_time.hour) << ":"
//	//		<< std::setw(2) << std::setfill('0')
//	//		<< int(msg.uTxtHdr.sTxtHdr.tm_time.minute) << ":"
//	//		<< std::setw(2) << std::setfill('0')
//	//		<< int(msg.uTxtHdr.sTxtHdr.tm_time.second) << "', '"
// //     << msg.msg_txt << "')\n";
//
//
//		try{
//			m_pDBTextCommand->CommandText = str.str().c_str();
//			m_pDBTextCommand->Execute(NULL, NULL, ADODB::adCmdText);
//		}
//		catch (_com_error &e) {
//			PrintProviderError(m_pDBTextConnection);
//			PrintComError(e);
//		}
}

void CDBInterface::DataInserterTh()
{
  std::unique_lock<std::mutex> lock(queue_rwmutex_);

	lock.unlock();

	while(!m_bTerminate) {
		lock.lock();

		while((m_nMessageBufferSize > m_nCntr) && !m_bTerminate) {
      if (std::cv_status::timeout == read_ready_cv_.wait_for(lock, std::chrono::milliseconds(2000)))
        break;
    }

    if (m_nCntr) {
      m_bActiveCommandStr = !m_bActiveCommandStr;
      m_nCntr = 0;

      m_os[m_bActiveCommandStr].str("");
      m_os[m_bActiveCommandStr].clear();

      m_os[m_bActiveCommandStr] << "INSERT INTO [nav_data_table]\nVALUES ";

      write_ready_cv_.notify_all();
      lock.unlock();

      try{
        m_pDBDataCommand->CommandText = m_os[!m_bActiveCommandStr].str().c_str();
        m_pDBDataCommand->Execute(NULL, NULL, ADODB::adCmdText);
      }
      catch (_com_error &e) {
        PrintProviderError(m_pDBDataConnection);
        PrintComError(e);
      }
    }
	}
}

void CDBInterface::PrintProviderError(ADODB::_ConnectionPtr pConnection) {
   // Print Provider Errors from Connection object.
   // pErr is a record object in the Connection's Error collection.
   ADODB::ErrorPtr pErr = NULL;

   if ( (pConnection->Errors->Count) > 0 ) {
      long nCount = pConnection->Errors->Count;
      // Collection ranges from 0 to nCount -1.
      for ( long i = 0 ; i < nCount ; i++ ) {
         pErr = pConnection->Errors->GetItem(i);
          BOOST_LOG_TRIVIAL(error) << "\t Error number: 0x" << std::hex << pErr->Number << "\t" << pErr->Description;
      }
   }
}

void CDBInterface::PrintComError(_com_error &e) {
   _bstr_t bstrSource(e.Source());
   _bstr_t bstrDescription(e.Description());
   _bstr_t bstrErrorMessage(e.ErrorMessage());

   // Print Com errors.
   BOOST_LOG_TRIVIAL(error) << "Code = 0x" << std::hex << e.Error();
   BOOST_LOG_TRIVIAL(error) << "Code meaning = " << (LPCSTR) bstrErrorMessage;
   BOOST_LOG_TRIVIAL(error) << "Source = " << (LPCSTR) bstrSource;
   BOOST_LOG_TRIVIAL(error) << "Description = " << (LPCSTR) bstrDescription;
} // void CDBInterface::PrintComError

void CDBInterface::PushLogMessage(const uint32_t nDeviceId, int ec, const std::string& message) const
{
  //TODO: place here DB log message procedure
} // void CDBInterface::PushLogMessage

} // namespace parser

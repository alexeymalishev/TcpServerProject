//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: MSG_INSTALL
//
// MessageText:
//
// TcpServer Service was installed.
//
#define MSG_INSTALL                      0x00000001L

//
// MessageId: MSG_PARAMETRS_CHANGED
//
// MessageText:
//
// parameters was changed successfully.
//
#define MSG_PARAMETRS_CHANGED            0x00000002L

//
// MessageId: MSG_FAIL
//
// MessageText:
//
// Error occured: "%1".
//
#define MSG_FAIL                         0xC0000003L

//
// MessageId: MSG_STARTED
//
// MessageText:
//
// Service started.
//
#define MSG_STARTED                      0x00000004L

//
// MessageId: MSG_STOPPED
//
// MessageText:
//
// Service stopped.
//
#define MSG_STOPPED                      0x00000005L


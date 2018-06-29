/**
 * Pre-set strings for display or logging.
 */

#ifndef ENVIRONMENT_MONITOR_PRESET_STR_H_
#define ENVIRONMENT_MONITOR_PRESET_STR_H_

#include <avr/pgmspace.h>

typedef PROGMEM const char PCHAR;

PCHAR emptyLine[] = "                ";

//Initial page
PCHAR psInit_1[] = " [Initializing] ";
PCHAR psInit_2[] = "Please note down";
PCHAR psInit_3[] = "  current time. ";

PCHAR psDataUpdated_3[] = " [Data Updated] ";

// SD error page
PCHAR psSDError_2[] = "   [SD Error]   ";
#ifdef SD_AUTO_RETRY
PCHAR psSDError_3[] = "   Retrying...  ";
#else
PCHAR psSDError_3[] = " Press to Retry ";
#endif

// Pause page
PCHAR psPause_2[] = "     [Pause]    ";

PCHAR logPause[] = "Pause";
PCHAR logResume[] = "Resume";
PCHAR logSDRecovered[] = "SD Recovered";
PCHAR logInited[] = "Inited";
PCHAR logPMS7003DataInvalid[] = "PMS7003 Data Invalid";

// USE_SIMPLE_DHT_LIB
PCHAR logDHT22ErrStartLow[] = "DHT22 Err - StartLow";
PCHAR logDHT22ErrStartHigh[] = "DHT22 Err - StartHigh";
PCHAR logDHT22ErrDataLow[] = "DHT22 Err - DataLow";
PCHAR logDHT22ErrDataRead[] = "DHT22 Err - DataRead";
PCHAR logDHT22ErrDataEOF[] = "DHT22 Err - DataEOF";
PCHAR logDHT22ErrDataChecksum[] = "DHT22 Err - Checksum";
PCHAR logDHT22ErrZeroSamples[] = "DHT22 Err - ZeroSamples";
const char* const logDHT22Err[] PROGMEM = {logDHT22ErrStartLow, 
                                           logDHT22ErrStartHigh, 
                                           logDHT22ErrDataLow,
                                           logDHT22ErrDataRead,
                                           logDHT22ErrDataEOF,
                                           logDHT22ErrDataChecksum,
                                           logDHT22ErrZeroSamples};
// USE_ADAFRUIT_LIB
PCHAR logDHT22Invalid[] = "DHT22 Err";

#define LOG_SD_ERR_CARD 0
#define LOG_SD_ERR_VOLUMN 1
#define LOG_SD_ERR_ROOT 2
#define LOG_SD_ERR_OPEN 3
#define LOG_SD_ERR_CLOSE 4
#define LOG_SD_ERR_NONE 9
PCHAR logSDErrCard[] = "SD Err - Card";
PCHAR logSDErrVolumn[] = "SD Err - Volumn";
PCHAR logSDErrRoot[] = "SD Err - Root";
PCHAR logSDErrOpen[] = "SD Err - Open";
PCHAR logSDErrClose[] = "SD Err - Close";
const char* const logSDErr[] PROGMEM = {logSDErrCard, logSDErrVolumn, logSDErrRoot, logSDErrOpen, logSDErrClose};

#endif // ENVIRONMENT_MONITOR_PRESET_STR_H_

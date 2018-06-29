/* Program Behaviors */

/**
 * @name SD_AUTO_RETRY
 * @brief When enabled, the program will try initializing SD card each 5s in sdErrorState.
 * @default true
 */
#define SD_AUTO_RETRY

/**
 * @name SD_INSTANT_DETECT
 * @brief When enabled, the program will try initializing SD card every time to store data.
 * This allow the program to detect the connection lost of SD card instantly,
 * but will lead to slower storage since sd initialization cause much time.
 * @default false
 */
#define SD_INSTANT_DETECT

/**
 * @name SD_TRUST_STORE
 * @brief When enabled, the program won't throw error when fail to open or close SD file.
 * Only failure in SD init can trigger sdErrorState
 * @default false
 */
//#define SD_TRUST_STORE

/* Sensor Enable */

/**
 * @name DISABLE_BMP180
 * @brief When enabled, BMP180 is disabled.
 * BMP180 uses I2C to communicate. When circuit is not connected properly, the program may get halt.
 * @default false
 */
//#define DISABLE_BMP180

/**
 * @name DISABLE_MQ2
 * @brief When enabled, MQ2 is disabled.
 * MQ2 costs too much energy.
 * @default false
 */
#define DISABLE_MQ2

/**
 * @name DEBUG_MODE
 * @brief When enabled, the program will output external log info to Serial.
 * @default false
 */
//#define DEBUG_MODE

/**
 * Select which library is used to control DHT22.
 * SimpleDHT is unstable during our test with SimpleDHT
 */
//#define USE_SIMPLE_DHT_LIB
#define USE_ADAFRUIT_LIB

/**
 * @name USE_CUSTOM_SD_LIB
 * @brief When enabled, the program will not use SD.h but libs inside.
 * Since this program only need to write file to the root of the SD card,
 * We can use inner SD libraries to save storage and calculation resource.
 * Also, this enable custom debug of SD lib.
 * @default false
 */
//#define USE_CUSTOM_SD_LIB

/* Since this program only need to write file to the root of the SD card,
   We use inner SD libraries to save storage and calculation resource. */

#ifndef USE_CUSTOM_SD_LIB
#include <SD.h>
#else
#include "SdFat.h"
#include "SdFatUtil.h"
#endif
#include <SPI.h>
#include <avr/pgmspace.h>

#include <pt.h>

#include "Data_Version.h"

/* Sensor Libraries */
#include "LCD12864RSPI.h"
#ifdef USE_SIMPLE_DHT_LIB
#include "DHT22_SimpleDHT.h"
#endif
#ifdef USE_ADAFRUIT_LIB
#include "DHT22_Adafruit.h"
#endif
#ifndef DISABLE_BMP180
#include "Adeept_BMP180.h"
#endif
#include "PMS7003.h"
#ifndef DISABLE_MQ2
#include "MQ2.h"
#endif
#include "Photosensor.h"

#include "preset_str.h"

/*** Component Definitions ***/

LCD12864RSPI LCD(8, 3, 9);  // LCD: latch_pin 8, clock_pin 3, data_pin 9

#ifdef USE_SIMPLE_DHT_LIB
My_DHT22 dht22(2);          // DHT22: pin 2
#endif
#ifdef USE_ADAFRUIT_LIB
My_DHT22 dht22;             // DHT22: pin 2
#endif
#ifndef DISABLE_BMP180
Adeept_BMP180 bmp180(0x77); // I2C address of BMP180
#endif
PMS7003 pms7003;            // PMS7003: RX_PIN 6, TX_PIN 7 (unused)
#ifndef DISABLE_MQ2
MQ2 mq2(1);                 // MQ2: analog pin A1
#endif
Photosensor photoSensor(0); // Photosensor: analog pin A0

#define PAUSE_BTN_PIN 7     // PauseButton: pin 7
#define SD_CS_PIN 4         // SDCard: CS pin 4


/*** Helper variables and functions ***/
#define BUTTON_PRESSED(pin) (digitalRead(pin) == LOW)

char tmp_cstr[32];
#define DISPLAY_CSTR(x, y, cstr) LCD.displayString(x, y, cstr, strlen(cstr))
#define DISPLAY_PSTR(x, y, pstr) LCD.displayString_P(x, y, pstr, strlen_P(pstr))

enum KitState {
    normalState = 0,
    pauseState = 1, 
    sdErrorState = 2
} globalState = normalState;

/*** Time ***/

char timeDD, timeHH, timeMM, timeSS;
char timeStr[16]; //String is pre-formatted with ':', ' ' and NULL ending.

inline void updateTime() {
    
    //TODO: These two variable can't be static or time won't get changed. Why?

    unsigned long ms = millis();
    
    timeDD = ms / 86400000; //1000*60*60*24
    ms -= timeDD * 86400000;
    timeHH = ms / 3600000; //1000*60*60
    ms -= timeHH * 3600000;
    timeMM = ms / 60000; //1000*60
    ms -= timeMM * 60000;
    timeSS = ms / 1000;

    char t;

    t = timeDD / 10;
    timeStr[0] = (t + '0');
    timeStr[1] = (timeDD - (t * 10) + '0');

    t = timeHH / 10;
    timeStr[3] = (t + '0');
    timeStr[4] = (timeHH - (t * 10) + '0');

    t = timeMM / 10;
    timeStr[6] = (t + '0');
    timeStr[7] = (timeMM - (t * 10) + '0');

    t = timeSS / 10;
    timeStr[9] = (t + '0');
    timeStr[10]= (timeSS - (t * 10) + '0');

    t = timeDD / 10;
    timeStr[0] = (t + '0');
    timeStr[1] = (timeDD - (t * 10) + '0');
}


/*** SD Card ***/

Sd2Card sdCard;
SdVolume sdVolume;
SdFile sdRoot;
SdFile sdFile;

char sdErrIndex = LOG_SD_ERR_NONE; // This variable record the initial error location

inline bool sdInit() {
    sdCard = Sd2Card();
    if (!sdCard.init(SPI_HALF_SPEED, SD_CS_PIN)) {
#ifdef DEBUG_MODE
        Serial.println(F("sdCard Fail"));
#endif
        if (sdErrIndex == LOG_SD_ERR_NONE) sdErrIndex = LOG_SD_ERR_CARD;
        return false;
    }
    sdVolume = SdVolume();
    if (!sdVolume.init(sdCard)) {
#ifdef DEBUG_MODE
        Serial.println(F("sdVolumn Fail"));
#endif
        if (sdErrIndex == LOG_SD_ERR_NONE) sdErrIndex = LOG_SD_ERR_VOLUMN;
        return false;
    }
    sdRoot = SdFile();
    if (!sdRoot.openRoot(sdVolume)) {
#ifdef DEBUG_MODE
        Serial.println(F("sdRoot Fail"));
#endif
        if (sdErrIndex == LOG_SD_ERR_NONE) sdErrIndex = LOG_SD_ERR_ROOT;
        return false;
    }
    sdFile = SdFile();
    return true;
}

/**
 * Write log to SD card.
 */
inline void writeLog(char *cstr) {
    if(sdFile.open(sdRoot, "kit.LOG", O_WRITE | O_CREAT)) {
      sdFile.seekSet(sdFile.fileSize());
      sdFile.print(timeStr);
      sdFile.println(cstr);
      sdFile.close();
    }
}

/**
 * Write log to SD card, for PROGMEM strings.
 */
inline void writeLog_P(PCHAR *pstr) {
    if(sdFile.open(sdRoot, "kit.LOG", O_WRITE | O_CREAT)) {
      sdFile.seekSet(sdFile.fileSize());
      sdFile.print(timeStr);
      
      char c;
      for (int i = 0; i < strlen_P(pstr); i++) {
          c = pgm_read_byte_near(pstr + i);
          sdFile.print(c);
      }
      sdFile.println();

      sdFile.close();
    }
}

char comma[] = ",";
/**
 * Store valid data into SD card.
 * Filenames are DD-HH.TXT
 */
inline bool storeData() {
    
    tmp_cstr[0] = timeStr[0];
    tmp_cstr[1] = timeStr[1];
    tmp_cstr[2] = '-';
    tmp_cstr[3] = timeStr[3];
    tmp_cstr[4] = timeStr[4];
    tmp_cstr[5] = '.';
    tmp_cstr[6] = 'T';
    tmp_cstr[7] = 'X';
    tmp_cstr[8] = 'T';
    tmp_cstr[9] = '\0';

#ifdef SD_INSTANT_DETECT
    if (!sdInit()) return false;
#endif
    if (!sdFile.open(sdRoot, tmp_cstr, O_WRITE | O_CREAT)) {
#ifdef DEBUG_MODE
        Serial.println(F("sdFile Open Fail"));
#endif
        if (sdErrIndex == LOG_SD_ERR_NONE) sdErrIndex = LOG_SD_ERR_OPEN;
        return false;
    }
    // Move the pointer to the end of the file to append data instead of overwriting
    sdFile.seekSet(sdFile.fileSize());
    sdFile.print(timeStr); sdFile.print(comma);
#ifdef USE_SIMPLE_DHT_LIB
    if (dht22.error == SimpleDHTErrSuccess) {
#endif
#ifdef USE_ADAFRUIT_LIB
    if(dht22.dataValid) {
#endif
        sdFile.print(dht22.humidity); sdFile.print(comma);
        sdFile.print(dht22.temperature); sdFile.print(comma);
    } else {
        sdFile.print(",,");
    }
#ifndef DISABLE_BMP180
    sdFile.print(bmp180.temperature); sdFile.print(comma);
    sdFile.print(bmp180.pressure); sdFile.print(comma);
    sdFile.print(bmp180.altitude); sdFile.print(comma);
#else
    sdFile.print(",,,");
#endif
    if (pms7003.dataValid) {
        sdFile.print(pms7003.pm2_5_cf1);  sdFile.print(comma);
        sdFile.print(pms7003.pm1_0_cf1);  sdFile.print(comma);
        sdFile.print(pms7003.pm10_0_cf1); sdFile.print(comma);
        sdFile.print(pms7003.pm1_0_amb);  sdFile.print(comma);
        sdFile.print(pms7003.pm2_5_amb);  sdFile.print(comma);
        sdFile.print(pms7003.pm10_0_amb); sdFile.print(comma);
        sdFile.print(pms7003.pm0_3_raw);  sdFile.print(comma);
        sdFile.print(pms7003.pm0_5_raw);  sdFile.print(comma);
        sdFile.print(pms7003.pm1_0_raw);  sdFile.print(comma);
        sdFile.print(pms7003.pm2_5_raw);  sdFile.print(comma);
        sdFile.print(pms7003.pm5_0_raw);  sdFile.print(comma);
        sdFile.print(pms7003.pm10_0_raw); sdFile.print(comma);
    } else {
        sdFile.print(",,,,,,,,,,,,");
    }
#ifndef DISABLE_MQ2
    sdFile.print(mq2.outVoltage); sdFile.print(comma);
#else
    sdFile.print(comma);
#endif
    sdFile.print(photoSensor.outVoltage); sdFile.println();
    if (!sdFile.close()) {
#ifdef DEBUG_MODE
        Serial.println(F("sdFile Close Fail"));
#endif
        if (sdErrIndex == LOG_SD_ERR_NONE) sdErrIndex = LOG_SD_ERR_CLOSE;
        return false;
    }
    return true;
} 

/*** Display ***/

inline void displayTitle() {
    DISPLAY_CSTR(0, 0, timeStr);
}

inline void displayInfo() {
    switch (globalState) {
        case normalState:
            DISPLAY_PSTR(1, 0, emptyLine);
            DISPLAY_CSTR(2, 0, timeStr);
            DISPLAY_PSTR(3, 0, psDataUpdated_3);
            break;
        case sdErrorState:
            DISPLAY_CSTR(1, 0, timeStr);
            DISPLAY_PSTR(2, 0, psSDError_2);
            DISPLAY_PSTR(3, 0, psSDError_3);
            break;
        case pauseState:
            DISPLAY_PSTR(1, 0, emptyLine);
            DISPLAY_PSTR(2, 0, psPause_2);
            DISPLAY_PSTR(3, 0, emptyLine);
    }
}

/*** Tasks and State Transistions ***/

static struct pt ptUpdateTime;
static struct pt ptUpdatePMS7003;
static struct pt ptUpdateSensors;
static struct pt ptStoreData;
#ifdef SD_AUTO_RETRY
static struct pt ptSDRecovery;
#endif
static struct pt ptDisplay;
static struct pt ptFuncBtn;

static bool shouldUpdateDisplay = false;
static bool dataUpdated = false;

inline void enterNormalState() {
    globalState = normalState;
    shouldUpdateDisplay = true;
}

inline void enterSDErrorState() {
    globalState = sdErrorState;
    shouldUpdateDisplay = true;
}

inline void enterPauseState() {
    globalState = pauseState;
    shouldUpdateDisplay = true;
}

/**
 * The pt to convey millis to DD:HH:MM:SS format automatically.
 */
static int thdUpdateTime(struct pt *pt) {
    static unsigned long lastUpdateTime = 0;
    PT_BEGIN(pt);
    while(1) {
        updateTime();
        lastUpdateTime = millis();
        displayTitle();

        //Update time and title each 1s
        PT_WAIT_UNTIL(pt, millis() > lastUpdateTime + 1000);
    }
    PT_END(pt);
}

/**
 * The pt to retrieve data from PMS7003 sensor.
 * PMS7003 is control individually to receive data in time and avoid checksum error
 */
static int thdUpdatePMS7003(struct pt *pt) {
    PT_BEGIN(pt);
    while(1) {
        // Wait until the information is fully received
        PT_WAIT_UNTIL(pt, pms7003.dataAvailable() && (globalState == normalState));;
        pms7003.update();
    }
    PT_END(pt);
}

/**
 * The pt to update other sensors.
 */
static int thdUpdateSensors(struct pt *pt) {
    static unsigned long lastSampleTime = 0;

    PT_BEGIN(pt);
    while(1) {
        
        // Sample each 5s
        PT_WAIT_UNTIL(pt, (millis() > lastSampleTime + 5000) && (globalState == normalState));
        
        dht22.update();
#ifdef USE_SIMPLE_DHT_LIB
        if (dht22.error != SimpleDHTErrSuccess) {
            strcpy_P(tmp_cstr, (char*)pgm_read_word(&(logDHT22Err[dht22.error - 100])));
            writeLog(tmp_cstr);
        }
#endif
#ifdef USE_ADAFRUIT_LIB
        if(!dht22.dataValid) {
            writeLog_P(logDHT22Invalid);
        }
#endif
#ifndef DISABLE_BMP180
        bmp180.update();      // Short delay
#endif
#ifndef DISABLE_MQ2
        mq2.update();         // Short delay
#endif
        photoSensor.update(); // Short delay

        // After data is updated, trigger data storing
        dataUpdated = true;

        lastSampleTime = millis();
    }
    PT_END(pt);
}

/**
 * The pt to store updated data into SD card.
 */
static int thdStoreData(struct pt *pt) {
    PT_BEGIN(pt);
    while(1) {
        // Wait until data is updated
        PT_WAIT_UNTIL(pt, dataUpdated && (globalState == normalState));
        if (!pms7003.dataValid) {
            writeLog_P(logPMS7003DataInvalid);
        }
#ifdef SD_TRUST_STORE
        storeData();
#else
        if (!storeData()) {
            enterSDErrorState();
        }
#endif
        dataUpdated = false;

        // After data is stored, update display
        shouldUpdateDisplay = true;
    }
    PT_END(pt);
}

/**
 * The pt to retry initializing SD card when in sdErrorState
 */
#ifdef SD_AUTO_RETRY
static int thdSDRecovery(struct pt *pt) {
    static unsigned long lastRecoveryTime = 0;
    PT_BEGIN(pt);
    while(1) {
        // If SD_AUTO_RETRY is enabled, when in sdErrorState, retry to initialize SD card each 5s
        PT_WAIT_UNTIL(pt, (globalState == sdErrorState) && (millis() > lastRecoveryTime + 5000));
        if (sdInit()) {
            // If recover SD successfully, write log of initial error info and recovery
            if (sdErrIndex != LOG_SD_ERR_NONE) {
                strcpy_P(tmp_cstr, (char*)pgm_read_word(&(logSDErr[sdErrIndex])));
                writeLog(tmp_cstr);
                sdErrIndex = LOG_SD_ERR_NONE;
            }
            writeLog_P(logSDRecovered);
            enterNormalState();
        } else {
            enterSDErrorState();
        }
        lastRecoveryTime = millis();
    }
    PT_END(pt);
}
#endif

/**
 * The pt to update display.
 */
static int thdDisplay(struct pt *pt) {
    PT_BEGIN(pt);
    while(1) {
        PT_WAIT_UNTIL(pt, shouldUpdateDisplay);
        displayInfo();
        shouldUpdateDisplay = false;
    }
    PT_END(pt);
}

/**
 * The pt to handle function button.
 */
static int thdFuncBtn(struct pt *pt) {
    static unsigned long lastDetactTime = 0;
    PT_BEGIN(pt);
    while(1) {
        PT_WAIT_UNTIL(pt, BUTTON_PRESSED(PAUSE_BTN_PIN));
        // Detect again after 100ms to advoid unstablility of the curcuit
        lastDetactTime = millis();
        PT_WAIT_UNTIL(pt, millis() > lastDetactTime + 100);
        if (!BUTTON_PRESSED(PAUSE_BTN_PIN)) continue;

        switch (globalState) {
            case normalState:
                // In normalState, press to pause
                writeLog_P(logPause);
                enterPauseState();
                break;
            case sdErrorState:
                // In sdErrorState, press to try recovering SD
                if (sdInit()) {
                    // If recover SD successfully, write log of initial error info and recovery
                    if (sdErrIndex != LOG_SD_ERR_NONE) {
                        strcpy_P(tmp_cstr, (char*)pgm_read_word(&(logSDErr[sdErrIndex])));
                        writeLog(tmp_cstr);
                        sdErrIndex = LOG_SD_ERR_NONE;
                    }
                    writeLog_P(logSDRecovered);
                    enterNormalState();
                }
                else enterSDErrorState();
                break;
            case pauseState:
                // In pauseState, press to try resuming
                if (sdInit()) {
                    writeLog_P(logResume);
                    enterNormalState();
                }
                else enterSDErrorState();
                break;
        }
        PT_WAIT_UNTIL(pt, !BUTTON_PRESSED(PAUSE_BTN_PIN));
    }
    PT_END(pt);
}

void setup() {

#ifdef DEBUG_MODE
    Serial.begin(115200);
    Serial.println(F("Debug Init"));
#endif

    // Init time variables
    timeStr[2] = timeStr[5] = timeStr[8] = ':';
    timeStr[11] = timeStr[12] = timeStr[13] = timeStr[14] = ' ';
    timeStr[15] = '\0';
    updateTime();

    // Init components
    LCD.init(); 
    delay(100);
#ifndef DISABLE_BMP180
    bmp180.begin();
#endif
    delay(100);
    pms7003.begin();
#ifndef DISABLE_MQ2
    mq2.begin();
#endif
    photoSensor.begin();

    // Init threads
    PT_INIT(&ptUpdateTime);
    PT_INIT(&ptUpdatePMS7003);
    PT_INIT(&ptUpdateSensors);
    PT_INIT(&ptStoreData);
#ifdef SD_AUTO_RETRY
    PT_INIT(&ptSDRecovery);
#endif
    PT_INIT(&ptDisplay);
    PT_INIT(&ptFuncBtn);

    // Display the init page for 5s
    LCD.clear();
    delay(100);
    DISPLAY_PSTR(1, 0, psInit_1);
    DISPLAY_PSTR(2, 0, psInit_2);
    DISPLAY_PSTR(3, 0, psInit_3);
    while(millis() < 5000) {
        updateTime();
        displayTitle();
        delay(1000);
    }
    LCD.clear();
    delay(100);

    // Enter normal operations
    if(sdInit()) enterNormalState();
    else {
#ifdef DEBUG_MODE
        Serial.println(F("SDErr at setup"));
#endif
        enterSDErrorState();
    }

    writeLog_P(logInited);
}


void loop()
{
    thdUpdateTime(&ptUpdateTime);
    thdUpdatePMS7003(&ptUpdatePMS7003);
    thdUpdateSensors(&ptUpdateSensors);
    thdStoreData(&ptStoreData);
#ifdef SD_AUTO_RETRY
    thdSDRecovery(&ptSDRecovery);
#endif
    thdDisplay(&ptDisplay);
    thdFuncBtn(&ptFuncBtn);
}



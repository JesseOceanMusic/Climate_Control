///↓↓↓ ТЕЛЕГРАМ ↓↓↓///


#include <ESP8266WiFi.h>                         // Telegram библиотеки //    
#include <WiFiClientSecure.h>                    // ↑↑↑ //    
#include <UniversalTelegramBot.h>                // ↑↑↑ //    
#include <ArduinoJson.h>                         // ↑↑↑ //  


#include "Sensetive_INFO.cpp"                    // логины/пароли/токены/id //
  /*
  WIFI_SSID
  WIFI_PASSWORD
  BOT_TOKEN1
  BOT_TOKEN2
  BOT_TOKEN3
  BOT_TOKEN4
  USER_ID0
  USER_ID1
  USER_ID2
  USER_ID3
  USER_ID4
  USER_ID5
*/

X509List cert(TELEGRAM_CERTIFICATE_ROOT);                                      // какой-то сертификат //
WiFiClientSecure secured_client;                                               // какой-то secured client //

//UniversalTelegramBot bot1(BOT_TOKEN1, secured_client);                       // BOT "Chat" //
UniversalTelegramBot bot2(BOT_TOKEN2, secured_client);                         // BOT "Logs + LED" //
//UniversalTelegramBot bot3(BOT_TOKEN3, secured_client);                       // BOT "Calibrate" //
UniversalTelegramBot bot4(BOT_TOKEN4, secured_client);                         // BOT "Backup" //


bool message_intruder_flag = true;
byte users_array_index;
bool shutdown_friends = false;
const byte user_array_length = 7;

class class_users
{
  public:
    class_users(byte users_array_index, String id, bool alert_flag, bool admin_flag, bool need_supervision, String name)
    {
      _users_array_index = users_array_index;
      _id = id;
      _alert_flag = alert_flag;
      _admin_flag = admin_flag;
      _need_supervision = need_supervision;
      _name = name;
      _MessageState = 1;
    }

    void check_id(String CHAT_IDcur);            // прототип метода, сам метод описан ниже. Необходимо, поскольку метод обращается к объекту, который еще не создан. //

    void send_message(String input);             // прототип метода, сам метод описан ниже. Необходимо, поскольку метод обращается к объекту, который еще не создан. //

    void send_message_second_chat(String input)
    {
      bot4.sendMessage(_id, input, "");
    }

    void send_alert(String input)
    {
      #ifdef Jesse_yield_enable
        yield();
      #endif

      if (_alert_flag == true)
      {
        bot2.sendMessage(_id, input, "");
      }
    }

    void set_id(String id)
    {
      _id = id;
    }

    String get_id()
    {
      return _id;
    }

    void set_alert_flag()
    {
      _alert_flag = !_alert_flag;
      if(_alert_flag == true)
      {
        bot2.sendMessage(_id, "Текстовые уведомления ВКЛЮЧЕНЫ.", "");
      }
      else
      {
        bot2.sendMessage(_id, "Текстовые уведомления ОТКЛЮЧЕНЫ.", "");
      }
    }

    unsigned int get_message_state()
    {
      return(_MessageState);
    }

    void set_message_state(unsigned int MessageState)
    {
      _MessageState = MessageState;
    }

    bool get_admin_flag()
    {
      return(_admin_flag);
    }

  private:
    byte _users_array_index;                               // индекс пользователя (объекта) в массиве //
    String _id;                                            // ID пользователя //
    bool _alert_flag;                                      // true - отправляются уведомления алерты //
    bool _admin_flag;                                      // true - права администратора //
    bool _need_supervision;                                // true - при запросе сообщает мне. Нужно, чтобы понимать если кто-то обращается к боту вне групп и я не вижу сообщений //
    String _name;                                          // Имя пользователя //
    unsigned int _MessageState;                            // стейт сообщений //
};

class_users object_array_users[user_array_length] =
{
  class_users(0, USER_ID0_me,            true,  true,  false, "Андрей"),                             // Мой айди //
  class_users(1, USER_ID1_guest,         false, false, true,  "Гостевой чат"),                       // гостевой юзер //  
  class_users(2, USER_ID2_debug,         false, false, false, "Debug"),                               // Группа для дебаг-сообщений //  
  class_users(3, USER_ID3_Kate_group,    false, false, false, "Катя - группа"),                      // Катя - айди группы //
  class_users(4, USER_ID4_Kate_personal, false, false, true,  "Катя - личная переписка"),            // Катя - личный айди //
  class_users(5, USER_ID5_Sasha_group,   false, false, false, "Саша - группа"),                      // Саша - айди группы //
  class_users(6, USER_ID6_Slava_Artem,   false, false, false, "Слава и Артем - группа"),             // Слава и Артём - айди группы //
};

void class_users::send_message(String input)
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  bot2.sendMessage(_id, input, "");
  if(_need_supervision == true)
  {
    object_array_users[0].send_message(String("Ответил, ") + _name + ":\n\n" + input);
  }
}

void class_users::check_id(String CHAT_IDcur)
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  if(CHAT_IDcur == _id)
  {
    users_array_index = _users_array_index;
    message_intruder_flag = false;
    
    if(_need_supervision == true)
    {
      object_array_users[0].send_message(_name + ", написал:\n\n" + bot2.messages[0].text);
    }        
  }
}

void message_id_check(String CHAT_IDcur)                          // Права доступа //
{
  message_intruder_flag = true;

  object_array_users[0].check_id(CHAT_IDcur);
  if(shutdown_friends == false)
  {
    for(int i = 1; i < user_array_length; i++)
    {
      object_array_users[i].check_id(CHAT_IDcur);
    }
  }
}

void send_alert(String input_message)
{
  for(int i = 0; i < user_array_length; i++)
  {
    object_array_users[i].send_alert(input_message);
  }
}


///↓↓↓ ВРЕМЯ ↓↓↓///


class class_TimeDate                             // класс Даты и Времени //
{
  public:
    class_TimeDate()                             // конструктор класса //
    {
      _UTC_time = 0;
    }

    void update_TimeDate()                       // обновление текущей даты и времени
    {
      #ifdef Jesse_yield_enable
        yield();
      #endif

      _UTC_time = time(nullptr);
      struct tm* L_tm = localtime(&_UTC_time);

      String buf_Date_YEAR = String(L_tm->tm_year + 1900);
      String buf_Date_MONTH = String(L_tm->tm_mon + 1);
      String buf_Date_DAY = String(L_tm->tm_mday);
      String buf_Time_HOUR = String(L_tm->tm_hour);
      String buf_Time_MIN = String(L_tm->tm_min);
      String buf_Time_SEC = String(L_tm->tm_sec);

      _TimeHOUR = buf_Time_HOUR.toInt();
      _TimeMIN = buf_Time_MIN.toInt();
      _TimeSEC = buf_Time_SEC.toInt();

      if (buf_Date_MONTH.toInt() >= 0 && buf_Date_MONTH.toInt() < 10)
      {
        buf_Date_MONTH = String("0" + buf_Date_MONTH);
      }

      if (buf_Date_DAY.toInt() >= 0 && buf_Date_DAY.toInt() < 10)
      {
        buf_Date_DAY = String("0" + buf_Date_DAY);
      }

      if (buf_Time_HOUR.toInt() >= 0 && buf_Time_HOUR.toInt() < 10)
      {
        buf_Time_HOUR = String("0" + buf_Time_HOUR);
      }

      if (buf_Time_MIN.toInt() >= 0 && buf_Time_MIN.toInt() < 10)
      {
        buf_Time_MIN = String ("0" + buf_Time_MIN);
      }

      if (buf_Time_SEC.toInt() >= 0 && buf_Time_SEC.toInt() < 10)
      {
        buf_Time_SEC = String ("0" + buf_Time_SEC);
      }

      _DateMONTH = buf_Date_MONTH.toInt();
      _DateFULL = String(buf_Date_YEAR + "-" + buf_Date_MONTH + "-" + buf_Date_DAY);
      _TimeA = String(buf_Time_HOUR + ":" + buf_Time_MIN + ":" + buf_Time_SEC);
      String buf_TimeB = String(buf_Time_HOUR + buf_Time_MIN + buf_Time_SEC);
      _TimeB = buf_TimeB.toInt();
      _DateTimeFULL = String(_DateFULL + " " + _TimeA);
    }

    void set_UTC_time()
    {
      configTime(10800, 0, "pool.ntp.org");            // get UTC time via NTP // не работает - "time.nist.gov" //
      secured_client.setTrustAnchors(&cert);                                  // Add root certificate for api.telegram.org //


      while (1717656000 > _UTC_time || _UTC_time > 4102444800)                  // не начинаем основной луп, пока не получим время //
      {
        _UTC_time = time(nullptr);
        delay (500);
      }
    }

    time_t get_UTC()
    {
      return(_UTC_time);
    }

    byte get_HOUR()
    {
      return(_TimeHOUR);
    }

    byte get_MIN()
    {
      return(_TimeMIN);
    }

    byte get_SEC()
    {
      return(_TimeSEC);
    }

    String get_DateFULL()
    {
      return(_DateFULL);
    }

    String get_DateTimeFULL()
    {
      return(_DateTimeFULL);
    }

    String get_TimeA()
    {
      return(_TimeA);
    }

    long get_TimeB()
    {
      return(_TimeB);
    }

    byte get_DateMONTH()
    {
      return(_DateMONTH);
    }
    
    bool get_FLAG_minute()
    {
      return(_flag_every_minute_timer);
    }

  private:
    time_t _UTC_time;                            // Время в секундах //
    bool _flag_every_minute_timer = false;       // Флаг для таймера каждую четную/нечетную минуту // true - четная // false - нечетная //


    byte _TimeHOUR;                              // часы //
    byte _TimeMIN;                               // минуты //
    byte _TimeSEC;                               // секунды //
    
    byte _DateMONTH;                             // месяц //

    String _DateFULL;                            //дата в формате 2024-06-24 //
    String _DateTimeFULL;                        //дата и время в формате 2024-06-24 11:48:39 //
    String _TimeA;                               //время в формате 22:01:02 //
    long _TimeB;                                 //время в формате 220102 //
};

class_TimeDate object_TimeDate;                  // создаем экземпляр класса class_TimeDate (объект) //

bool flag_every_minute_timer = false;            // флаг для таймера каждую четную/нечетную минуту //   


///↓↓↓ НОЧНОЙ РЕЖИМ ↓↓↓///


class class_NightTime                            // класс ночного режима //
{
  public:
    class_NightTime()                            // конструктор класса //
    {
      _NightTimeState = 3;
      _NightTimeDimState = 1;
    }

    void update_NightTime()                      // метод проверки пора ли переключить ночной режим //
    {
      #ifdef Jesse_yield_enable
        yield();
      #endif

      if(_NightTimeState <= 3)                             // Ночной режим ВКЛ с 21:00 до 6:48 //
      { 
        if (object_TimeDate.get_TimeB() > 210000 || object_TimeDate.get_TimeB() < 64800)
        {
          _NightTimeDimState = 2;                     // Уменьшение яркости (займет 10 минут при стандартной яркости) //
          _NightTimeState = 4;                        // 1 - выкл, 2 - вкл до след утра , 3 - в нейтральном положение, чтобы сработало сразу при включение, 4 -вкл , 5 - выкл след вечера //
        }
      }

      if(_NightTimeState >= 3)                             // Ночной режим ВЫКЛ c 6:50 до 20:58 // 
      {
        if (object_TimeDate.get_TimeB() > 65000 && object_TimeDate.get_TimeB() < 205800)
        {    
          _NightTimeDimState = 3;                     // Увеличение яркости (займет 40 минут при стандартной яркости) //
          _NightTimeState = 1;                        // 1 - выкл, 2 - вкл до след утра , 3 - в нейтральном положение, чтобы сработало сразу при включение, 4 -вкл , 5 - выкл след вечера //
        }
      }
    }

    byte get_NightTimeState()
    {
      return(_NightTimeState);
    }

    byte get_NightTimeDimState()
    {
      return(_NightTimeDimState);
    }

    void set_NightTimeState(byte NightTimeState)
    {
      _NightTimeState = NightTimeState;
    }

    void set_NightTimeDimState(byte NightTimeDimState)
    {
      _NightTimeDimState = NightTimeDimState;
    }

  private:
    byte _NightTimeState;                        // 1 - выкл, 2 - вкл до след утра , 3 - в нейтральном положение, чтобы сработало сразу при включение, 4 -вкл , 5 - выкл до след вечера //   
    byte _NightTimeDimState;                     // Флаг для ночного режима (1 - выкл, 2 - уменьшение яркости, 3 увеличение яркости) //
};

class_NightTime object_NightTime;                // создаем экземпляр класса class_NightTime (объект) //


/// ↓↓↓ АДРЕСНАЯ LED ЛЕНТА ↓↓↓ ///


#include "FastLED.h"                             // LED библиотека //   

#define NUM_LEDS1 240                            // Бра //       
#define DATA_PIN1 D1                             // ↑↑↑ //
CRGB array_LED_sconce [NUM_LEDS1];               // ↑↑↑ //

#define NUM_LEDS2 60                             // Полоска //   
#define DATA_PIN2 D2                             // ↑↑↑ //
CRGB array_LED_line [NUM_LEDS2];                 // ↑↑↑ //

#define NUM_LEDS3 191                            // Часы //   
#define DATA_PIN3 D4                             // ↑↑↑ //
CRGB array_LED_clock [NUM_LEDS3];                // ↑↑↑ //


int HSVhue1;                                     // переменная, которая хранит значение последнего случайного hue (цвет) //
int HSVsat1;                                     // переменная, которая хранит значение последнего случайного saturatuion (насыщенность) //
int HSVval1day = 80;                             // value (яркость) бра днём // 
int HSVval2day = 40;                             // value (яркость) линии днём //
int HSVval3day = 100;                            // value (яркость) часов днём //

int HSVhue_clock = 175;                          // hue (цвет) часов - меняется раз в час, чтобы не разражать. // пусть при запуске будет бирюзовым, раз в час цвет меняется //

int timer_min_hue_clock_cur = 0;                 // счетчик //
int timer_min_hue_clock_target = 60;             // интервал смены цвета часов в минутах //

int HSVsat_clock_day = 130;                      // saturation (насыщенность) часов днём //

int HSVsat_line_day = 70;                        // saturation (насыщенность) линии днём //

int HSVval1night = 0;                            // value (яркость) бра ночью //
int HSVval2night = 0;                            // value (яркость) линии ночью //
int HSVval3night = 1;                            // value (яркость) часов ночью //

int HSVval1cur = 0;                              // текущее значение яркости бра // нужно для плавного изменения яркости при изменение ночного режима по расписанию //
int HSVval2cur = 0;                              // текущее значение яркости линии // ↑↑↑ //
int HSVval3cur = 1;                              // текущее значение яркости часов // ↑↑↑ //

bool global_ERROR_flag = false;                  // Флаг для ошибок (1 - нет ошибок, 2 - в цикле были ошибки) // ps в какой-то момент появилось подозрение, что где-то в биоблетаках уже есть ErrorSTATE и возникают ошибки //

byte RGB_clock_night [3] = {1, 1, 0,};           // цвет часов ночью. по умолчанию желтый (yellow) 0,1,1,


/// ↓↓↓ АДРЕСНАЯ LED ЛЕНТА ↓↓↓ /// ↓↓↓ ЧАСЫ ↓↓↓ ///


// UPDATE v037 - одномерные массивы стали двумерными и код стал короче ~280 строк. Теперь разобраться в коде еще сложнее.
// UPDATE v041 - появился class_Clock, что позволило внутри него создавать временные массивы и не хранить глобальные для обращения из разных функций. Думаю, экономия составила байт 150 :-)

// общий смысл массивов такой:
// 1 - преобразуем цифру в формат семисегментного отображения
// 2 - преобразуем полученный результат с учетом смещения для конкретной цифры на табло (из-за того, что свтодиодная лента расположена змейкой и не одномерна)
// 3 - складываем отдельные цифры и знаки "%" "." ":"" в один большой массив из 0 и 1
// 4 - отображаем данные где для каждого светодиода Value в HSV умножаем на 0 или 1 для того, чтобы получились цифры на табло.

// последовательность сегментов используемая в digit_to_segment_converter. //
// первый сегмент - нижний-центральный. //
// второй сегмент - средний-центральный. //
// трейтий сегмент - верхний-центральный. //
// четвертый сегмент - нижний-левый. //
// пятый сегмент - верхний-левый. //
// шестой сегмент - нижний-правый. //
// седьмой сегмент - верхный-правый. //

byte ClockArray_main [NUM_LEDS3];                // основной массив для отображения данных (результат сложения 13 цифр + точка, двоеточие, процент) //
byte ArrayGlobalCounter = 0;                     // счетчик для сложения массивов //

class class_Clock                                // класс часов //
{
  public:
    class_Clock(byte key, byte array_length)               // конструктор класса //
    {
      _key_ID = key;                                       // ключ смещения цифр относительно описанного определения выше на 15 строк //
      _array_length = array_length;                        // длина массива. для точки будет 1, для цифры 14 итд. необходимо, чтобы потом складывать массивы //
    }
    
    void set_cur_number(byte cur_number)                   // может получить 1 из 12 цифр. 10 значений это цифры от 0 до 9 // плюс 2 значения - одно пустая заливка, другое полная заливка. используется для символов и когда не нужно отображать какую-то цифру. либо ночной режим, либо чтобы вместо 09:55 отображать 9:55 //
    {
      _cur_number = constrain(cur_number, 0, 11);          // защита от дурака. проверяет, чтобы точно не выйти за границу массива, когда будем использовать эту перменную в методе update //
    }

    void update()                                          // вызов update отдельных объектов должен быть обязательно последовательным, поскольку внутри есть сразу сложение в основной массив. если вызывать update у объектов в другой последовательности - отобразиться чушь. //
    { 
      #ifdef Jesse_yield_enable
        yield();
      #endif

      byte buf_array [14];

      for (int i = 0; i < 7; i++)                          // по сути, этот цикл является очень примитивным дешифратором, где _key_ID это индекс строки в массиве _key_ARRAY, на которой лежит ключ//
      {    
        buf_array [i*2] = _numbers_code_array [_cur_number][_key_ARRAY [_key_ID][i]];
        buf_array [(i*2)+1] = _numbers_code_array [_cur_number][_key_ARRAY [_key_ID][i]];
      }

      for (int i = 0; i < _array_length; i++)              // полученный результат переносим в основной массив ClockArray_main, чтобы при вызове следующих объектов сохранить индекс - храним его в ArrayGlobalCounter //
      {
        ClockArray_main [ArrayGlobalCounter++] = buf_array [i];
      }
    }

  private:
    byte _key_ID;
    byte _array_length;
    byte _cur_number;

    const byte _numbers_code_array [12][7] =               // цифры в семисегментном формате //
    {
      {1, 0, 1, 1, 1, 1, 1,},                              // 0
      {0, 0, 0, 0, 0, 1, 1,},                              // 1
      {1, 1, 1, 1, 0, 0, 1,},                              // 2
      {1, 1, 1, 0, 0, 1, 1,},                              // 3
      {0, 1, 0, 0, 1, 1, 1,},                              // 4
      {1, 1, 1, 0, 1, 1, 0,},                              // 5
      {1, 1, 1, 1, 1, 1, 0,},                              // 6
      {0, 0, 1, 0, 0, 1, 1,},                              // 7
      {1, 1, 1, 1, 1, 1, 1,},                              // 8
      {1, 1, 1, 0, 1, 1, 1,},                              // 9
      {0, 0, 0, 0, 0, 0, 0,},                              // empty
      {1, 1, 1, 1, 1, 1 ,1,},                              // fill
    };

    const byte _key_ARRAY [6][7] =                         // ключи смещения цифр //
    {
      {5, 0, 3, 1, 6, 2, 4,},                              // смещение сегментов (цифры 1, 2)
      {1, 5, 0, 3, 4, 2, 6,},                              // смещение сегментов (цифра 3)
      {4, 2, 6, 5, 0, 3, 1,},                              // смещение сегментов (цифры 4, 5)
      {5, 0, 3, 4, 2, 6, 1,},                              // смещение сегментов (цифры 6, 7, 8, 9)
      {4, 2, 6, 5, 0, 3, 1,},                              // смещение сегментов (цифры 10, 11, 12, 13)
      {0, 1, 2, 3, 4, 5, 6,},                              // прямая последовательность для точки, двоеточия, процента
    };
};

class_Clock object_clock_PERCENTAGE (5, 6);      // символ процента //
class_Clock object_clock_0 (0, 14);              // Влажность // 
class_Clock object_clock_1 (0, 14);              // Влажность //
class_Clock object_clock_2 (1, 14);              // Температура //
class_Clock object_clock_3 (2, 14);              // Температура //
class_Clock object_clock_DOT (5, 1);             // символ точки //
class_Clock object_clock_4 (2, 14);              // температура //
class_Clock object_clock_5 (3, 14);              // СО2 //
class_Clock object_clock_6 (3, 14);              // СО2 //
class_Clock object_clock_7 (3, 14);              // СО2 //
class_Clock object_clock_8 (3, 14);              // СО2 //
class_Clock object_clock_9 (4, 14);              // время //
class_Clock object_clock_10 (4, 14);             // время //
class_Clock object_clock_COLON (5, 2);           // символ двоеточия //
class_Clock object_clock_11 (4, 14);             // время //
class_Clock object_clock_12 (4, 14);             // время //

bool clock_night_indication_time = true;         // флаг отображаем/не отображаем ВРЕМЯ ночью на табло // 
bool clock_night_indication_co2 = true;          // ↑↑↑ // СО2 //
bool clock_night_indication_temperature = true;  // ↑↑↑ // температура //
bool clock_night_indication_humidity = true;     // ↑↑↑ // влажность //


///↓↓↓ SD КАРТА ↓↓↓///


#include <SPI.h>                                 // библиотека для SD карты //   
#include <SD.h>                                  // ↑↑↑ //  

File myFile;                                     // ипользуется и для отправки файла в телеграм и для работы с SD картой //

bool isMoreDataAvailable()                       // ↑↑↑ // функция для отправки файла в телеграм с сд карты //   
{
  return myFile.available();
}

byte getNextByte()                               // ↑↑↑ // 
{
  return myFile.read();
}

String SYNCdata;                                 // стринг для полученных данных из Serial Port //
bool flag_every_day_timer = false;               // флаг для отправки лога раз в сутки //


///↓↓↓ ПЕРЕЗАГРУЗКА ↓↓↓///


bool esp_restart_flag = false;
bool skip_one_iteration = true;

void restart_check()
{
  if(esp_restart_flag == true)
  {
    if(skip_one_iteration == true)
    {
      skip_one_iteration = false;
    }

    else
    {
      ESP.restart();
    }
  }
}


///↓↓↓ ОТЛАДКА ↓↓↓///


//#define Jesse_DEBUG_free_heap
#ifdef Jesse_DEBUG_free_heap
  time_t Jesse_debug_free_heap_timer;
#endif

//#define Jesse_DEBUG_loop_millis_measure
#ifdef Jesse_DEBUG_loop_millis_measure
  long test_timer;
#endif

#define Jesse_yield_enable                       // delay(0) и yield() одно и тоже... и то и то даёт возможность ESP в эти прерывания обработать wi-fi и внутренний код // https://arduino.stackexchange.com/questions/78590/nodemcu-1-0-resets-automatically-after-sometime //


///   ///   ///   ///   ///   ///   ///


void setup()                                     // стандартная функция Ардуино - выполняется один раз в начале //
{
  Serial.begin(9600);                                                     // запускаем Serial Port и определяем его скорость //
  Serial.setTimeout(200);                                                 // таймаут для .readString (ждет заданное значение на чтение Serial) // нет смысла ждать стандартную секунду(1000 милисекунд), поскольку синхронизация происходит асинхронно //

  WiFi.setOutputPower(15.00);                    // "When values higher than 19.25 are set, the board resets every time a connection with the AP is established." // https://stackoverflow.com/questions/75712199/esp8266-watchdog-reset-when-using-wifi // 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);          // подключаемся к Wi-Fi //

  object_TimeDate.set_UTC_time();                                         // устанавливаем связь с сервером реального времени и получаем актуальное время //

  FastLED.addLeds<WS2812, DATA_PIN1, GRB>(array_LED_sconce, NUM_LEDS1);   // определяем лед ленту БРА //
  FastLED.addLeds<WS2812, DATA_PIN2, GRB>(array_LED_line, NUM_LEDS2);     // определяем лед ленту ЛИНИИ //
  FastLED.addLeds<WS2812, DATA_PIN3, GRB>(array_LED_clock, NUM_LEDS3);    // определяем лед ленту ЧАСОВ //
  FastLED.setBrightness(255);                                             // задаём глобальную яркость всех лент, как максимальную //

  send_alert("Я проснулся.");
}

void loop()                                      // основной луп //
{
  #ifdef Jesse_DEBUG_free_heap
    if (object_TimeDate.get_UTC() - Jesse_debug_free_heap_timer > 60*5)
    {
      object_array_users[2].send_message("ESP.getFreeHeap(): " + String(ESP.getFreeHeap()));
      Jesse_debug_free_heap_timer = object_TimeDate.get_UTC();
    }
  #endif

  #ifdef Jesse_DEBUG_loop_millis_measure
    long buf_timer = millis() - test_timer;
    object_array_users[2].send_message(String(buf_timer));
    delay(3000);
    test_timer = millis();
  #endif

  object_TimeDate.update_TimeDate();                                           // получаем актуальное время с сервера //
  object_NightTime.update_NightTime();                                         // обновляем состояние ночного режима //


  if (bot2.getUpdates(bot2.last_message_received + 1) != 0)                    // если есть новые сообщения обрабатываем одно //
  {
    LEDhello();                                                                // включаем три светодиода для индикации начала обработки входящего сообщения //
    Message_from_Telegram_converter();
    LEDbye();                                                                  // выключем три светодиода для индикации начала обработки входящего сообщения //
  }


  if (global_ERROR_flag == true)                                                       // проверят были ли ошибки и если были - делает анимацию //
  {
    if (object_NightTime.get_NightTimeState() == 1)                            // 1 значит - днём можно включить ночной режим, чтобы отключить анимацию ошибки во время отладки // 1 - выкл, 2 - вкл до след утра , 3 - в нейтральном положение, чтобы сработало сразу при включение, 4 -вкл , 5 - выкл до след вечера //
    {    
      Led_animation_error();                                                   // если были ошибки и сейчас день, без включенного ночного режима - запускаем анимацию ошибки //
    }
    global_ERROR_flag = false;                                                          // в любом случае сбрасываем флаг ошибки, чтобы если ночью была ошибка, то утром первым делом она не захерачила анимацию //     
  }
  
  if(object_TimeDate.get_MIN() % 2 > 0 && flag_every_minute_timer == false)    // таймер каждую нечетную минуту //
  {
    #ifdef Jesse_yield_enable
      yield();
    #endif

    LOGtimer(); 
    NightTimeDim();
    
    #ifdef Jesse_yield_enable
      yield();
    #endif    
    
    Led_animation_up();
    clock_master();
    flag_every_minute_timer = true;
  }

  if(object_TimeDate.get_MIN() % 2 == 0 && flag_every_minute_timer == true)    // таймер каждую четную минуту //
  {
    #ifdef Jesse_yield_enable
      yield();
    #endif

    SYNCdata = "";                                                             // обнуляем стринг с датой, чтобы при ошибках синхронизации на часах не продолжали висеть старые данные //
    SYNCstart();
    NightTimeDim();

    #ifdef Jesse_yield_enable
      yield();
    #endif

    Led_animation_down();
    clock_master();

    restart_check();
    flag_every_minute_timer = false;
  }

}

void Message_from_Telegram_converter()           // преобразование сообщение из Телеграм в команду //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  String CHAT_IDcur = bot2.messages[0].chat_id;
  message_id_check(CHAT_IDcur);
  if (message_intruder_flag == false)
  {
    String text = bot2.messages[0].text;

    byte dividerIndex = text.indexOf('@');                              // ищем индекс разделителя @ // для того, чтобы работали команды из группы по запросу типа "/back@JOArduinoChatBOT" //
    String text_2nd_part = text.substring(dividerIndex + 1);            // записывает в text_2nd_part "JOArduinoChatBOT" //
    text = text.substring(0, dividerIndex);                             // записывает в text "/back" //

    if (text_2nd_part != "JOArduinoChatBOT")                            // если сообщение не для него, то пропускает его //
    {
      Message_command_executer(text);
    }
  }

  else
  {
    send_alert("INTRUDER: " + CHAT_IDcur);
  }
}

void Message_command_executer(String text)       // обработчик команд //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  if(text == "/back")                                            // отмена ввода //
  {
    if (object_array_users[users_array_index].get_message_state() == 1)
    {
      object_array_users[users_array_index].send_message("Команда для ввода данных не была выбрана. Посмотрите команды в меню.");
    }
    
    else
    {
      object_array_users[users_array_index].set_message_state(1);
      object_array_users[users_array_index].send_message("Ввод данных отменен.");
    }
  }

  else if (object_array_users[users_array_index].get_message_state() != 1)                                         // если MessageState != 1, то значит ожидаем ввод данных //
  {
    switch (object_array_users[users_array_index].get_message_state())
    {
      case 104:                                // яркость //
      {
        HSVval1day = constrain(text.toInt(), 0, 255);
        HSVval2day = (HSVval1day/2);
        HSVval1cur = HSVval1day;
        HSVval2cur = HSVval2day;
        FillSolidMY();
        object_array_users[users_array_index].send_message("Яркость бра и линии днём установлена: " + String(HSVval1day));
        break;            
      }

      case 105:                                // яркость часов днём //
      {
        HSVval3day = constrain(text.toInt(), 0, 255);
        HSVval3cur = HSVval3day;
        clock_master();
        object_array_users[users_array_index].send_message("Яркость часов днём установлена: " + String(HSVval3day));     
        break;   
      } 

      case 106:                                // яркость часов ночью //  
      {
        HSVval3night = constrain(text.toInt(), 0, 40);
        HSVval3cur = HSVval3night;
        clock_master();
        object_array_users[users_array_index].send_message("Яркость часов ночью установлена: " + String(HSVval3night));       
        break;   
      }

      case 110:                               // интервал смены цвета часов днём //  
      {
        timer_min_hue_clock_target = constrain(text.toInt(), 1, 10080);
        object_array_users[users_array_index].send_message("Интервал смена цвета часов установлен: " + String(timer_min_hue_clock_target));
        break;   
      }

      case 112:                               // ручной выбор цвета часов ночью //  
      {
        int text_int = text.toInt();
        RGB_clock_night [2] = text_int % 10;
        text_int /= 10;
        RGB_clock_night [1] = text_int % 10;
        text_int /= 10;
        RGB_clock_night [0] = text_int % 10;
        text_int /= 10;

        object_array_users[users_array_index].send_message("Цвет (RGB) часов ночью установлен - " + String(RGB_clock_night [0]) + String(RGB_clock_night [1]) + String(RGB_clock_night [2]));
        clock_master();
        break;   
      }

      case 390:                               // выбор гостевого чата вручную //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_array_users[1].set_id(text);
          object_array_users[users_array_index].send_message("Теперь я буду отвечать на входящие запросы с ID: " + text);
          object_array_users[1].send_message("Я проснулся.");
        }

        else 
        {
          object_array_users[users_array_index].send_message("Недостаточно прав доступа.");
        }
        break;  
      }
    }
    object_array_users[users_array_index].set_message_state(1);      
  }

  else                                                                // если MessageState == 1, то значит ожидаем команду //
  {
    byte dividerIndex = text.indexOf('/');   // ищем индекс разделителя "/" //
    text = text.substring(dividerIndex + 1); // оставляем только команду "1" //
    int text_int = text.toInt();

    switch (text_int)
    {
      case 101:                                                       // запрос лога за текущий день //
      {
        LOGread();
        break;
      }

      case 102:                                                       // запрос первой строки excel //
      {
        object_array_users[users_array_index].send_message("Дата и Время, Температура в комнате, Влажность в комнате, СО2, Рекуп. Приток (in), Рекуп. Приток (out), Рекуп. Вытяжка (in), Рекуп. Вытяжка (out), КПД рекуп. на приток (%), КПД рекуп. на вытяжку (%), C улицы (°C), C батареи (°C), Объединенный поток после фильтров(°C), Теплопотери от улицы до рекуператора (°C), Теплопотери воздухувода от кровати до рекуп. и нагрев вентилятором (°C), % воздуха c улицы, % воздуха c батареи, Текущее положение заслонок, Шагов за сутки (без учета калибровки)");
        break;
      }

      case 10300:                                                     // отключить ночной режим //
      {
        HSVval1cur = HSVval1day;
        HSVval2cur = HSVval2day;
        HSVval3cur = HSVval3day;
        object_NightTime.set_NightTimeState(5);
        object_NightTime.set_NightTimeDimState(1);                // необходимо, чтобы прервать плавное изменение яркости если ночной режим был переключен в процессе изменения яркости //
        FillSolidMY();
        clock_master();
        object_array_users[users_array_index].send_message("Ночной режим принудительно ВЫКЛЮЧЕН до вечера (Ночью в любом случае не будет появляться анимация ошибки).");
        break;
      }

      case 10301:                                                     // включить ночной режим //
      {
        HSVval1cur = HSVval1night;
        HSVval2cur = HSVval2night;
        HSVval3cur = HSVval3night;
        object_NightTime.set_NightTimeState(2);
        object_NightTime.set_NightTimeDimState(1);                // необходимо, чтобы прервать плавное изменение яркости если ночной режим был переключен в процессе изменения яркости //
        FillSolidMY();
        clock_master();
        object_array_users[users_array_index].send_message("Ночной режим принудительно ВКЛЮЧЕН до утра (Днём перестанет появляться анимация ошибки).");
        break;
      }

      case 104:                                                       // яркость бра и линии днём //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для изменения яркости бра и линии днём (от 0 до 255):\n\nТекущее значение: " + String(HSVval1day));
        object_array_users[users_array_index].set_message_state(104);
        break;
      }

      case 105:                                                       // яркость часов днём //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для изменения яркости часов в дневное время (от 0 до 255):\n\nТекущее значение: " + String(HSVval3day));
        object_array_users[users_array_index].set_message_state(105);
        break;
      }

      case 106:                                                       // яркость часов ночью //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для изменения яркости часов ночью (от 0 до 40):\n\nТекущее значение: " + String(HSVval3night));
        object_array_users[users_array_index].set_message_state(106);
        break;
      }

      case 107:                                                       // анимация "снизу вверх" //
      {
        Led_animation_up();
        break;
      }

      case 108:                                                       // анимация "сверху вниз" //
      {
        Led_animation_down();
        break;
      }

      case 109:                                                       // анимация "ошибка" //
      {
        Led_animation_error();
        break;
      }

      case 110:                                                       // интервал смены цвета часов днём //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для установки интервала смены цвета часов днём, в минутах от 1 минуты до 7 дней (от 1 до 10080):\n\nТекущее значение: " + String(timer_min_hue_clock_target));
        object_array_users[users_array_index].set_message_state(110);
        break;
      }

      case 111:                                                       // выбор индикации данных на табло ночью //
      {
        object_array_users[users_array_index].send_message(clock_indication());
        break;
      }

      case 11101:                                                     // ↑↑↑ // время //
      {
        clock_night_indication_time = !clock_night_indication_time;
        clock_master();
        object_array_users[users_array_index].send_message(clock_indication());
        break;
      }

      case 11102:                                                     // ↑↑↑ // co2 //
      {
        clock_night_indication_co2 = !clock_night_indication_co2;
        clock_master();
        object_array_users[users_array_index].send_message(clock_indication());
        break;
      }

      case 11103:                                                     // ↑↑↑ // температура //
      {
        clock_night_indication_temperature = !clock_night_indication_temperature;
        clock_master();
        object_array_users[users_array_index].send_message(clock_indication());
        break;
      }

      case 11104:                                                     // ↑↑↑ // влажность //
      {
        clock_night_indication_humidity = !clock_night_indication_humidity;
        clock_master();
        object_array_users[users_array_index].send_message(clock_indication());
        break;
      }

      case 112:                                                       // выбор цвета часов ночью //
      {
        String buf_message = "Выбор цвета часов ночью:";
        buf_message += "\n\nКрасный - /11201";
        buf_message += "\nЖелтый - /11202";
        buf_message += "\nЗеленый - /11203";
        buf_message += "\nГолубой - /11204";
        buf_message += "\nСиний - /11205";
        buf_message += "\nРозовый -/11206";
        buf_message += "\n\nБелый - /11207";
        buf_message += "\n\n\nВвести вручную RGB цвет - /11299";
        object_array_users[users_array_index].send_message(buf_message);
        break;
      }

      case 11201:                                                     // ↑↑↑ // Красный //
      {
        RGB_clock_night [0] = 1;
        RGB_clock_night [1] = 0;
        RGB_clock_night [2] = 0;
        clock_master();
        break;
      }

      case 11202:                                                     // ↑↑↑ // Желтый //
      {
        RGB_clock_night [0] = 1;
        RGB_clock_night [1] = 1;
        RGB_clock_night [2] = 0;
        clock_master();
        break;
      }

      case 11203:                                                     // ↑↑↑ // Зеленый //
      {
        RGB_clock_night [0] = 0;
        RGB_clock_night [1] = 1;
        RGB_clock_night [2] = 0;
        clock_master();
        break;
      }

      case 11204:                                                     // ↑↑↑ // Голубой //
      {
        RGB_clock_night [0] = 0;
        RGB_clock_night [1] = 1;
        RGB_clock_night [2] = 1;
        clock_master();
        break;
      }

      case 11205:                                                     // ↑↑↑ // Синий //
      {
        RGB_clock_night [0] = 0;
        RGB_clock_night [1] = 0;
        RGB_clock_night [2] = 1;
        clock_master();
        break;
      }

      case 11206:                                                     // ↑↑↑ // Розовый //
      {
        RGB_clock_night [0] = 1;
        RGB_clock_night [1] = 0;
        RGB_clock_night [2] = 1;
        clock_master();
        break;
      }

      case 11207:                                                     // ↑↑↑ // Белый //
      {
        RGB_clock_night [0] = 1;
        RGB_clock_night [1] = 1;
        RGB_clock_night [2] = 1;
        clock_master();
        break;
      }

      case 11299:                                                     // ручной выбор цвета часов ночью //
      {
        object_array_users[users_array_index].send_message("Отправьте 3 цифры для установки цвета часов ночью (текущее значение: " + String(RGB_clock_night [0]) + String(RGB_clock_night [1]) + String(RGB_clock_night [2]) + "). Первая цифра RED, вторая BLUE, третья GREEN. \n\n\n*Параметр яркости часов ночью является коэфициентов для RGB цвета.");
        object_array_users[users_array_index].set_message_state(112);
        break;
      }

      case 190:                              // ВКЛ/ВЫКЛ Текстовых уведомлений //
      {
        object_array_users[users_array_index].set_alert_flag();
        break;
      }

      case 370:
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_array_users[users_array_index].send_message("Поднял флаг для перезагрузки. Сработает через ~2 минуты.");
          esp_restart_flag = true;
        }

        else 
        {
          object_array_users[users_array_index].send_message("Недостаточно прав доступа.");
        }
        break;
      }

      case 380:
      {
        if (users_array_index == 0)
        {
          shutdown_friends = !shutdown_friends;
          if(shutdown_friends == true)
          {
            object_array_users[users_array_index].send_message("Отключил возможность управления из друх чатов.");
          }
          else
          {
            object_array_users[users_array_index].send_message("Восстановил возможность управления из других чатов.");
          }
        }

        else
        {
          object_array_users[users_array_index].send_message("Недостаточно прав доступа.");
        }        
        break;
      }

      case 390:                              // выбор гостевого чата кнопки //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_array_users[users_array_index].send_message("Отправьте ID группы или профиля, чтобы я отправился туда. Я продолжу отвечать на ваши запросы!");
          object_array_users[users_array_index].set_message_state(390);
        }

        else 
        {
          object_array_users[users_array_index].send_message("Недостаточно прав доступа.");
        }
        break;
      }
    }
  }
}

void SYNCstart()                                 // получение температур, проверка времени, синхронизация ошибок //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  Serial.println(object_TimeDate.get_UTC());                              // отправляем время для синхронизации //
  
  String SYNCmessage = Serial.readString();

  if(SYNCmessage.length() > 2)                                            // Проверка что вообще что-то пришло //
  {
    byte dividerIndex = SYNCmessage.indexOf(';');
    String buf_SYNCtime = SYNCmessage.substring(0, dividerIndex);         // строка с временем для проверки рассинхронизации по времени
    String buf_second_part = SYNCmessage.substring(dividerIndex + 1);     // сохраняем вторую часть строки, чтобы разделить её на две нужные части

    byte dividerIndex2 = buf_second_part.indexOf(';');
    SYNCdata = buf_second_part.substring(0, dividerIndex2);               // строка с основными данными для синхронизации
    String buf_error = buf_second_part.substring(dividerIndex2 + 1);      // строка содержит 1 или 2. Говорит о наличии ошибок в цикле у второго микроконтроллера //
    
    if (buf_error.toInt() == true)
    {
      global_ERROR_flag = true;
    }

    unsigned long SYNCtime = buf_SYNCtime.toInt();
    unsigned long UTC_timeLong = object_TimeDate.get_UTC();
    unsigned long unSYNC = UTC_timeLong - SYNCtime;

    if (unSYNC > 600)                                                     // Проверка что рассинхронизация меньше 10 минут (отрицательные значения не нужно проверять - это unsigned long) //
    {
      send_alert("ERROR: Рассинхронизация составила: "  + String(unSYNC) + " секунд.");
      global_ERROR_flag = true;
    }
  }

  else
  {
    send_alert("ERROR: Данные для синхронизации не были получены.");
    global_ERROR_flag = true;
  }

  LOGwrite();
}

void LOGtimer()                                  // отправка лога в 23:45 //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  if (flag_every_day_timer == false && object_TimeDate.get_TimeB() > 234500)          // ТАЙМЕР отправки лога в 23:45 //
  {
    if (!SD.begin(4))
    {
      send_alert("ERROR: SD card initialization FAILED");
    }

    myFile = SD.open(object_TimeDate.get_DateFULL() + ".txt");
    if (myFile)
    {
      int size = myFile.size();
      String response = bot4.sendMultipartFormDataToTelegram("sendDocument", "document", object_TimeDate.get_DateFULL() + ".txt", "", object_array_users[0].get_id(), size, isMoreDataAvailable, getNextByte, nullptr, nullptr);
    }
    else
    {
      send_alert("ERROR: Create or open .txt FAILED");
    }
    flag_every_day_timer = true;
  }

  if (flag_every_day_timer == true && object_TimeDate.get_TimeB() < 234400)           // Возвращает флаг обратно, чтобы лог отправился на следующий день // 
  {    
    flag_every_day_timer = false;
  }
}

void LOGwrite()                                  // запись в лог //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  if (!SD.begin(4))
  {
    send_alert("ERROR: SD card initialization FAILED");
  }

  myFile = SD.open(object_TimeDate.get_DateFULL() + ".txt", FILE_WRITE);

  if (myFile)
  {
  myFile.println(object_TimeDate.get_DateTimeFULL() + "," + SYNCdata);
  myFile.close();
  }
  else
  {
    send_alert("ERROR: Create or open .txt FAILED");
  }
}

void LOGread()                                   // чтение лога //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  if (!SD.begin(4))
  {
    send_alert("ERROR: SD card initialization FAILED");
  }

  myFile = SD.open(object_TimeDate.get_DateFULL() + ".txt");
  if (myFile)
  {
    int size = myFile.size();
    String response=bot2.sendMultipartFormDataToTelegram("sendDocument", "document", object_TimeDate.get_DateFULL() + ".txt", "", object_array_users[users_array_index].get_id(), size, isMoreDataAvailable, getNextByte, nullptr, nullptr);
  }
  else
  {
    send_alert("ERROR: Create or open .txt FAILED");
  }
}

void NightTimeDim()                              // плавное изменение яркости //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  if (object_NightTime.get_NightTimeDimState() == 2)                    // Уменьшение яркости (займет 10 минут при стандартной яркости) // 
  {
    HSVval1cur = HSVval1cur - 8;
    HSVval2cur = HSVval2cur - 4;
    HSVval3cur = HSVval3cur - 10;

    if (HSVval1cur <= HSVval1night)
    {
      HSVval1cur = HSVval1night;
      HSVval2cur = HSVval2night;
      HSVval3cur = HSVval3night;
      object_NightTime.set_NightTimeDimState(1);
    }    
  }
    
  else if (object_NightTime.get_NightTimeDimState() == 3)               // Увеличение яркости (займет 40 минут при стандартной яркости) //
  {
    HSVval1cur = HSVval1cur + 2;
    HSVval2cur = HSVval2cur + 1;
    HSVval3cur = HSVval3cur + 2;
      
    if (HSVval1cur >= HSVval1day)
    {
      HSVval1cur = HSVval1day;
      HSVval2cur = HSVval2day;
      HSVval3cur = HSVval3day;
      object_NightTime.set_NightTimeDimState(1);
    }     
  }
}

void FillSolidMY()                               // заливка, чтобы не ждать при изменение яркости и дневного/ночного режима через команду //
{
  fill_solid(array_LED_sconce, NUM_LEDS1, CHSV(HSVhue1, HSVsat1, HSVval1cur));
  fill_solid(array_LED_line, NUM_LEDS2, CHSV(HSVhue1, HSVsat_line_day, HSVval2cur));
  FastLED.delay(10);
}

void Led_animation_up()                          // анимация подсветка "идёт наверх" //
{
  HSVhue1 = random(0, 255);
  HSVsat1 = random(50, 255);
  int depth = random(0, 10);
  depth = (depth * 20);

  for(int i = NUM_LEDS2 - 1; i > -1; i--)
  {
    array_LED_line [i] = CHSV(HSVhue1, HSVsat_line_day, HSVval2cur);
    FastLED.delay(60);
  }

  delay(500);

  for(int i = 0; i < (NUM_LEDS1 - depth); i++)
  {
    array_LED_sconce [i] = CHSV(HSVhue1, HSVsat1, HSVval1cur);
    FastLED.delay(20);
  }
}

void Led_animation_down()                        // анимация подсветки "идёт вниз" //
{
  HSVhue1 = random(0, 255);
  HSVsat1 = random(50, 255);
  byte depth = random(0, 10);
  depth = (depth * 20);
  for(int i = (NUM_LEDS1)-1; i >= depth; i--)
  {
    array_LED_sconce [i] = CHSV(HSVhue1, HSVsat1, HSVval1cur);
    FastLED.delay(20);
  }

  delay(500);

  for(int i = 0; i < NUM_LEDS2; i++)
  {
    array_LED_line [i] = CHSV(HSVhue1, HSVsat_line_day, HSVval2cur);
    FastLED.delay(60);
  }
}

void Led_animation_error()                       // подсветка - ошибка //
{
  fill_solid(array_LED_sconce, NUM_LEDS1, CHSV(0, 0, 0));
  fill_solid(array_LED_line, NUM_LEDS2, CHSV(0, 0, 0));

  for(int i = (NUM_LEDS2)-1; i > -1; i--)
  {
    if(i % 2 > 0)
    {
      array_LED_line [i] = CHSV(140, 255, 255);
    }
    else
    {
      array_LED_line [i] = CHSV(255, 255, 255);
    }
    FastLED.delay(16);
  }

  for(int i = 0; i < 12; i++)
  {
    int k = i * 20;
    for(int d = 0; d < 11; d += 10)
    {
      for (int h = 0; h < 10; h++)
      {
        if (h >= 0 && h <= 4)
        {
          array_LED_sconce [k+h+d] = CHSV(140, 255, 255);
        }
        else
        {
          array_LED_sconce [k+h+d] = CHSV(255, 255, 255);
        }
      }       
    }
    FastLED.delay(500);
  }

  delay(100);

  int low = 0;
  int high = 5;
  int brightnes_local_val = 255;
  for (int s = 0; s < 27; s++)
  {
    for(int i = 0; i < 12; i++)
    {
      brightnes_local_val--;
      brightnes_local_val = constrain(brightnes_local_val, 0, 255);       // чтобы плавно снизить яркость в 0...
      int k = i*20;
      for(int d = 0; d < 11; d += 10)
      {
        array_LED_sconce [k+d+low] = CHSV(255, 255, brightnes_local_val);
        array_LED_sconce [k+d+high] = CHSV(140, 255, brightnes_local_val);           
      }
      FastLED.delay(30);
    }
    low++;
    high++;
    if (low > 9)
    {
      low = low % 10;
    }
    if (high > 9)
    {
      high = high % 10;
    }   
  }

  delay(100);

  for(int i = 0; i < NUM_LEDS2; i++)
  {
    array_LED_line [i] = CHSV(0, 0, 0);
    FastLED.delay(20);
  }
}

void LEDhello()                                  // индикация начала обработки команды из телеграма // 
{
  byte HSVval2curBUF = constrain(HSVval2cur, 4, 255);
  array_LED_line [0] = CHSV(255, 0, (HSVval2curBUF));
  array_LED_line [1] = CHSV(140, 255, (HSVval2curBUF));
  array_LED_line [2] = CHSV(255, 0, (HSVval2curBUF));
  FastLED.delay(30);
}

void LEDbye()                                    // индикация конца обработки команды из телеграма // 
{
  array_LED_line [0] = CHSV(HSVhue1, HSVsat_line_day, HSVval2cur);
  array_LED_line [1] = CHSV(HSVhue1, HSVsat_line_day, HSVval2cur);
  array_LED_line [2] = CHSV(HSVhue1, HSVsat_line_day, HSVval2cur);  
  FastLED.delay(30);
}

void clock_master()                              // мастер функция для часов вызывающая другие функции и методы. //
{
  clock_string_to_array_converter();

  ArrayGlobalCounter = 0;

  object_clock_PERCENTAGE.update();
  object_clock_0.update();
  object_clock_1.update();
  object_clock_2.update();
  object_clock_3.update();
  object_clock_DOT.update();
  object_clock_4.update();
  object_clock_5.update();
  object_clock_6.update();
  object_clock_7.update();
  object_clock_8.update();
  object_clock_9.update();
  object_clock_10.update();
  object_clock_COLON.update();
  object_clock_11.update();
  object_clock_12.update();

  clock_animation();
}

void clock_string_to_array_converter()           // конвертация показаний с датчиков в 13 цифр для индикации на табло //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  byte dividerIndex = SYNCdata.indexOf(',');
  String buf_temp = SYNCdata.substring(0, dividerIndex);
  String buf1 = SYNCdata.substring(dividerIndex + 1);

  byte dividerIndex2 = buf1.indexOf(',');
  String buf_hum = buf1.substring(0, dividerIndex2);
  String buf2 = buf1.substring(dividerIndex2 + 1);

  byte dividerIndex3 = buf2.indexOf(',');
  String buf_co2 = buf2.substring(0, dividerIndex3);


  float buf1_temp = buf_temp.toFloat();
  buf1_temp = buf1_temp*10;
  int buf2_temp = buf1_temp;
  int buf2_hum = buf_hum.toInt();
  int buf2_co2 = buf_co2.toInt();

  if (buf2_hum == 0)                                            // если влажность при синхронизации не пришла и равна 0, то выключает индикацию влажности //
  {
    object_clock_PERCENTAGE.set_cur_number(10);                                      // выключает подсветку % //
    object_clock_0.set_cur_number(10);
    object_clock_1.set_cur_number(10);
  }
  else
  {
    object_clock_PERCENTAGE.set_cur_number(11);                                      // включает подсветку % //
    object_clock_0.set_cur_number(buf2_hum % 10);
    buf2_hum /= 10;
    object_clock_1.set_cur_number(buf2_hum % 10);
  }

  if (buf2_temp == 0)                                           // если температура при синхронизации не пришла и равна 0, то выключает индикацию температуры //
  {
    object_clock_DOT.set_cur_number(10);                                              // выключает подсчетку точки //
    object_clock_4.set_cur_number(10);
    object_clock_3.set_cur_number(10);
    object_clock_2.set_cur_number(10);
  }
  else
  {
    object_clock_DOT.set_cur_number(11);                                              // включает подсветку точки //
    object_clock_4.set_cur_number(buf2_temp % 10);
    buf2_temp /= 10;
    object_clock_3.set_cur_number(buf2_temp % 10);
    buf2_temp /= 10;
    object_clock_2.set_cur_number(buf2_temp % 10);
  }

  if (buf2_co2 == 0)                                            // если со2 при синхронизации не пришла и равна 0, то выключает индакацию со2 //
  {
    object_clock_5.set_cur_number(10);
    object_clock_6.set_cur_number(10);
    object_clock_7.set_cur_number(10);
    object_clock_8.set_cur_number(10);
  }

  else
  {
    object_clock_5.set_cur_number(buf2_co2 % 10);
    buf2_co2 /= 10;
    object_clock_6.set_cur_number(buf2_co2 % 10);
    buf2_co2 /= 10;
    object_clock_7.set_cur_number(buf2_co2 % 10);
    buf2_co2 /= 10;
    
    if (buf2_co2 == 0)
    {
      object_clock_8.set_cur_number(10);
    }
    
    else
    {
      object_clock_8.set_cur_number(buf2_co2 % 10);
    }
  }

  if (object_TimeDate.get_HOUR() < 10)                                            // если меньше 10 утра, то выключает первую цифру, чтобы получить 8:55 вместо 08:55 //
  {
    object_clock_9.set_cur_number(10);
  }

  else
  {
    object_clock_9.set_cur_number(object_TimeDate.get_HOUR()/10);
  }

  object_clock_10.set_cur_number(object_TimeDate.get_HOUR()%10);
  object_clock_COLON.set_cur_number(11);
  object_clock_11.set_cur_number(object_TimeDate.get_MIN()/10);
  object_clock_12.set_cur_number(object_TimeDate.get_MIN()%10);

  if (object_NightTime.get_NightTimeState() == 2 || object_NightTime.get_NightTimeState() == 4)               // если влючен ночной режим, то проверяем какие данные выводим //
  {
    if (HSVval3cur == HSVval3night)                             // проверяем закончилось ли плавное диммирование яркости и потом выключаем лишние данные //
    {
      if (clock_night_indication_time == false)
      {
        object_clock_COLON.set_cur_number(10);                                             // выключает подсветку двоеточия //
        object_clock_9.set_cur_number(10);                  // выключает цифры //
        object_clock_10.set_cur_number(10);                 // ↑↑↑ //
        object_clock_11.set_cur_number(10);                 // ↑↑↑ //
        object_clock_12.set_cur_number(10);                 // ↑↑↑ //
      }

      if (clock_night_indication_co2 == false)
      {
        object_clock_5.set_cur_number(10);                  // выключает цифры //
        object_clock_6.set_cur_number(10);                  // ↑↑↑ //
        object_clock_7.set_cur_number(10);                  // ↑↑↑ //
        object_clock_8.set_cur_number(10);                  // ↑↑↑ //
      }

      if (clock_night_indication_temperature == false)
      {
        object_clock_DOT.set_cur_number(10);                                               // выключает подсчетку точки //
        object_clock_4.set_cur_number(10);                  // выключает цифры //
        object_clock_3.set_cur_number(10);                  // ↑↑↑ //
        object_clock_2.set_cur_number(10);                  // ↑↑↑ //
      }

      if (clock_night_indication_humidity == false)
      {
        object_clock_PERCENTAGE.set_cur_number(10);                                         // выключает подсветку % //
        object_clock_0.set_cur_number(10);                   // выключает цифры //
        object_clock_1.set_cur_number(10);                   // ↑↑↑ //
      }
    }
  }
}

void clock_animation()                           // анимация часов  //
{
  int HSVhue_clock_buf = HSVhue_clock;                          // буфер переменной для сравнения ниже //

  if (timer_min_hue_clock_target == 60)                         // если интервал 60 минут, то цвет меняется в 00 минут независимо от того, когда включили микроконтроллер //
  {
    if (object_TimeDate.get_MIN() == 0)                                           // в 00 минут меняем цвет //
    {
      HSVhue_clock = HSVhue1;
    }
  }
  
  else                                                          // в других случаях меняем просто по таймеру //
  {
    timer_min_hue_clock_cur++;                                  // поскольку функция clock_animation вызывается раз в минуту - делаем инкремент счетчика и получаем минутный таймер //
    if (timer_min_hue_clock_cur >= timer_min_hue_clock_target)  // HSVhue1 меняется каждую минуту. Как только подошло время по таймеру приравниваем к нему HSVhue_clock //
    {
      HSVhue_clock = HSVhue1;
      timer_min_hue_clock_cur = 0;
    }
  }

  if (HSVval3cur <= HSVval3night)                               // если яркость меньше или равна ночному режиму, значит сейчас ночной режим. анимации отключена, управление по RGB. //
  {
    for(int i = 0; i < NUM_LEDS3; i++)
    {
      array_LED_clock [i] = CRGB((RGB_clock_night [0]*HSVval3cur*ClockArray_main [i]),(RGB_clock_night [1]*HSVval3cur*ClockArray_main [i]),(RGB_clock_night [2]*HSVval3cur*ClockArray_main [i]));
    }
    FastLED.delay(50);
  }

  else if (HSVhue_clock != HSVhue_clock_buf)                    // если яркость поменялась с начала функции - значит нужна анимация по HSV //
  {
    if (object_TimeDate.get_MIN() % 2 > 0)                                        // анимация снизу вверх каждую нечетную минуту
    {
      for(int i = 0; i < NUM_LEDS3; i++)
      {
        array_LED_clock [i] = CHSV(HSVhue_clock, HSVsat_clock_day, (HSVval3cur*ClockArray_main [i]));
        FastLED.delay(20);
      }
    }

    else                                                        // анимация сверху вниз каждую четную минуту
    {
      for(int i = NUM_LEDS3 - 1; i >= 0; i--)
      {
        array_LED_clock [i] = CHSV(HSVhue_clock, HSVsat_clock_day, (HSVval3cur*ClockArray_main [i]));
        FastLED.delay(20);
      }
    }
  }

  else                                                          // во всех других случаях - анимация отключена, режим HSV //
  {
    for(int i = 0; i < NUM_LEDS3; i++)
    {
      array_LED_clock [i] = CHSV(HSVhue_clock, HSVsat_clock_day, (HSVval3cur*ClockArray_main [i]));
    }
    FastLED.delay(20);
  }
}

String clock_indication()                        // отображение текущего состояния отображения данных на табло в текстовом виде //
{
  String buf_time_message;
  String buf_co2_message;
  String buf_temp_message;
  String buf_hum_message;

  if (clock_night_indication_time == true)
  {
    buf_time_message = "\n  Время - включено ( /11101 ).";
  }
  else
  {
    buf_time_message = "\n  Время - отключено ( /11101 ).";
  }

  if (clock_night_indication_co2 == true)
  {
    buf_co2_message = "\n  СО2 - включено ( /11102 ).";    
  }
  else
  {
    buf_co2_message = "\n  СО2 - отключено ( /11102 ).";
  }

  if (clock_night_indication_temperature == true)
  {
    buf_temp_message = "\n  Температура - включена ( /11103 ).";     
  }
  else
  {
    buf_temp_message = "\n  Температура - отключена ( /11103 ).";
  }

  if (clock_night_indication_humidity == true)
  {
    buf_hum_message = "\n  Влажность - включена ( /11104 ).";     
  }
  else
  {
    buf_hum_message = "\n  Влажность - отключена ( /11104 ).";
  }

  String clock_indication_message = "Текущие настройки индикации данных ночью:" + buf_time_message + buf_co2_message + buf_temp_message + buf_hum_message;
  return(clock_indication_message);
}
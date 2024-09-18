///↓↓↓ ОТЛАДКА ↓↓↓///
//#define local_DEBUG

/// ↓↓↓ ТЕЛЕГРАМ ↓↓↓ ///


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

X509List cert(TELEGRAM_CERTIFICATE_ROOT);                       // какой-то сертификат //
WiFiClientSecure secured_client;                                // какой-то secured client //

UniversalTelegramBot bot1(BOT_TOKEN1, secured_client);          // BOT "Chat" //
//UniversalTelegramBot bot2(BOT_TOKEN2, secured_client);        // BOT "Logs + LED" //
UniversalTelegramBot bot3(BOT_TOKEN3, secured_client);          // BOT "Calibrate" //
//UniversalTelegramBot bot4(BOT_TOKEN4, secured_client);        // BOT "Backup" //

bool message_intruder_flag = true;
byte users_array_index;


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
      bot3.sendMessage(_id, input, "");
    }

    void send_alert(String input)
    {
      if (_alert_flag == true)
      {
        bot1.sendMessage(_id, input, "");
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
        bot1.sendMessage(_id, "Текстовые уведомления ВКЛЮЧЕНЫ.", "");
      }
      else
      {
        bot1.sendMessage(_id, "Текстовые уведомления ОТКЛЮЧЕНЫ.", "");
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

class_users object_array_users[6] =
{
  class_users(0, "USER_ID0",      true,  true,  false, "Андрей"),                        // Мой айди //
  class_users(1, "USER_ID1", false, false, false, "Катя - группа"),                      // Катя - айди группы //
  class_users(2, "USER_ID2",      false, false, true,  "Катя - личная переписка"),       // Катя - личный айди //
  class_users(3, "USER_ID3", false, false, false, "Саша - группа"),                      // Саша - айди группы //
  class_users(4, "USER_ID4", false, false, false, "Слава и Артем - группа"),             // Слава и Артём - айди группы //
  class_users(5, "USER_ID5", false, false, true,  "Гостевой чат"),                       // гостевой юзер //
};

void class_users::send_message(String input)
{
  bot1.sendMessage(_id, input, "");
  if(_need_supervision == true)
  {
    object_array_users[0].send_message(String("Ответил, ") + _name + ":\n\n" + input);
  }
}

void class_users::check_id(String CHAT_IDcur)
{
  if(CHAT_IDcur == _id)
  {
    users_array_index = _users_array_index;
    message_intruder_flag = false;
    
    if(_need_supervision == true)
    {
      object_array_users[0].send_message(_name + ", написал:\n\n" + bot1.messages[0].text);
    }        
  }
}

void message_id_check(String CHAT_IDcur)                          // Права доступа //
{
  message_intruder_flag = true;

  object_array_users[0].check_id(CHAT_IDcur);
  object_array_users[1].check_id(CHAT_IDcur);
  object_array_users[2].check_id(CHAT_IDcur);
  object_array_users[3].check_id(CHAT_IDcur);
  object_array_users[4].check_id(CHAT_IDcur);
  object_array_users[5].check_id(CHAT_IDcur);
}

void send_alert(String input_message)
{
  object_array_users[0].send_alert(input_message);
  object_array_users[1].send_alert(input_message);
  object_array_users[2].send_alert(input_message);
  object_array_users[3].send_alert(input_message);
  object_array_users[4].send_alert(input_message);
  object_array_users[5].send_alert(input_message);
}


/// ↓↓↓ ВРЕМЯ ↓↓↓ ///


class class_TimeDate                             // класс Даты и Времени //
{
  public:
    class_TimeDate()                             // конструктор класса //
    {
      _UTC_time = 0;
    }

    void update_TimeDate()                       // обновление текущей даты и времени
    {
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


/// ↓↓↓ ДЛЯ СИНХРОНИЗАЦИИ ОШИБОК ↓↓↓ ///


int ErrorSTATEmy = 1;                            // Флаг для ошибок (1 - нет ошибок, 2 - в цикле были ошибки) // ps в какой-то момент появилось подозрение, что где-то в биоблетаках уже есть ErrorSTATE и возникают ошибки //


/// ↓↓↓ ДАТЧИКИ ТЕМПЕРАТУРЫ ↓↓↓ ///


#include <OneWire.h>                             // библиотека для датчиков температуры //
#include <DallasTemperature.h>                   // ↑↑↑ //

OneWire OneWire(D5);                             // Определяем пин, на котором висят датчики температуры //
DallasTemperature ds(&OneWire);                  // ↑↑↑ //

float b23;                                       // Эффективность рекуперации на приток //
float b45;                                       // Эффективность рекуперации на вытяжку //

float b62;                                       // Теплопотери притока (если заслонки от батареи закрыты) //
float b82;                                       // Теплопотери воздухувода от кровати до рекуператора и нагрев вентилятором //

float b61;                                       // Процент воздуха с улицы //
float b71;                                       // Процент воздуха с батареи //

class class_ds18b20                              // класс датчиков температура ds18b20 //
{
  public:
    class_ds18b20(bool alert_flag, byte ar0, byte ar1, byte ar2, byte ar3, byte ar4, byte ar5, byte ar6, byte ar7, String name)  // конструктор класса //
    {
      _crit_temp_low_alert = 4.00;
      _crit_temp_low_error = -45.00;
      _crit_temp_high_error = 45.00;

      _alert_flag = alert_flag;
      _array_address[0]= ar0;
      _array_address[1]= ar1;
      _array_address[2]= ar2;
      _array_address[3]= ar3;
      _array_address[4]= ar4;
      _array_address[5]= ar5;
      _array_address[6]= ar6;
      _array_address[7]= ar7;
      _name = name;
    }
    
    void update()                                // обновление данных с датчика //
    {
      _temp = ds.getTempC(_array_address);
      if (_alert_flag == true)
      {
        _check_alerts();
      }
      _check_errors();
    }

    float get_temp()
    {
      return(_temp);
    }
    
    void set_crit_temp_low_alert(String crit_temp_low_alert)
    {
      _crit_temp_low_alert = constrain(crit_temp_low_alert.toFloat(), -55, 55);
      object_array_users[users_array_index].send_message("Оповещения будут приходить, если температура опустится ниже: " + String(_crit_temp_low_alert) + "°C");
    }

    float get_crit_temp_low_alert()
    {
      return(_crit_temp_low_alert);
    }

    void set_res_to_12_bit()                     // установка датчиков ds18b20 на разрешение 12 bit на случай если из-за лага эта настройка сбилась //
    {
      ds.setAutoSaveScratchPad(true);            // true - записываем в EEPROM, false - не записываем
      int8_t resolution_12bit = 12;              // (12bit - максимальное разрешение датчика 18b20)
      ds.setResolution(_array_address, resolution_12bit);
      object_array_users[users_array_index].send_message(_name + "Настройка разрешения 12bit записана в энергонезависимую память датчика.");
    }

  private:
    bool _alert_flag;                            // флаг - "нужна ли проверка по нижней границе? нужна только для рекуператор приток(in) и рекуператор вытяжка(out) //
    byte _array_address [8];                     // data-провод со всех датчиков приходит на один пин. по этому обращение происходит по адрессу датчика //
    String _name;                                // имя датчика, для формирования сообщения в телегу //

    float _temp;                                 // переменная дла хранения значения температуры с датчика //
        
    float _crit_temp_low_alert;                  // ЗНАЧЕНИЕ ДЛЯ ОТПРАВКИ УВЕДОМЛЕНИЯ О КРИТИЧЕСКОЙ ТЕМПЕРАТУРЕ //

    float _crit_temp_low_error;                  // Нижняя граница для проверки глючности датчиков //
    float _crit_temp_high_error;                 // Верхняя граница для проверки глючности датчиков //
  
    void _check_alerts()
    {
      if (_crit_temp_low_alert > _temp)
      {
        send_alert("ALERT: " + _name + String(_temp) + "°C");
        ErrorSTATEmy = 2;
      }
    }

    void _check_errors()
    {
      if (_crit_temp_low_error > _temp || _temp > _crit_temp_high_error)
      {
        send_alert("ERROR: " + _name + String(_temp) + "°C");
        ErrorSTATEmy = 2;
      }
    }

};

class_ds18b20 object_ds18b20_0(true, 0x28, 0x76, 0x6A, 0x39, 0x0, 0x0, 0x0, 0x43, "Рекуператор Приток (in): ");
class_ds18b20 object_ds18b20_1(false, 0x28, 0x5C, 0x4E, 0x3C, 0x0, 0x0, 0x0, 0x5C, "Рекуператор Приток (out): ");
class_ds18b20 object_ds18b20_2(false, 0x28, 0xAD, 0x18, 0x3A, 0x0, 0x0, 0x0, 0x19, "Рекуператор Вытяжка (in): ");
class_ds18b20 object_ds18b20_3(true, 0x28, 0x27, 0xB5, 0x3E, 0x0, 0x0, 0x0, 0xED, "Рекуператор Вытяжка (out): ");
class_ds18b20 object_ds18b20_4(false, 0x28, 0xB6, 0x76, 0x39, 0x0, 0x0, 0x0, 0x81, "c Улицы: ");
class_ds18b20 object_ds18b20_5(false, 0x28, 0xF, 0x2A, 0x3A, 0x0, 0x0, 0x0, 0x2C, "с Батареи: ");
class_ds18b20 object_ds18b20_6(false, 0x28, 0x5E, 0x81, 0x39, 0x0, 0x0, 0x0, 0xFE, "Объединенный поток: ");


/// ↓↓↓ УПРАВЛЕНИЕ ЗАСЛОНКАМИ ↓↓↓ ///


#define street_STEP D6                           // пин для управления шагами двигателя заслонки с улицы //
#define home_STEP D7                             // пин для управления шагами двигателя заслонки от батареи //
#define DIR D8                                   // общий пин для смены direction моторов. HIGH - Вниз , LOW - Вверх //
#define dir_UP LOW                               // чтобы не путаться, поскольку low это 0 вольт, а high это 3,3 вольта //
#define dir_DOWN HIGH                            // ↑↑↑ //

float TempMain = 4.00;                           // температура термостата //
float TempRange = 2.00;                          // чувствительность термостата //
int Step_Per_loop = 80;                          // количество шагов двигателя за цикл запуска термостата //

int AirLowLimit = 25;                            // минимальный процент воздуха с улицы //

#define pinKnobLow D3                            // нижние концевики  //
#define pinKnobHigh D4                           // верхние концевики //

const int KNOB_space = 110;                            // обратный отступ после срабатывания концевика //

int StepsGLOBAL = 3188;                          // одна переменная для определения положения ОБЕИХ заслонок // StepsGLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //

const int street_POSITION_low_const = -2912;                    // НИЖНЯЯ точка плюс отступ (константа для проверки отклонения от первоначальных данных) //
const int home_POSITION_low_const = 3188;                       // ВЕРХНЯЯ точка плюс отступ (константа для проверки отклонения от первоначальных данных) //

int street_POSITION_low_cur = street_POSITION_low_const;        // нижняя точка плюс отступ (выставляется после калибровки set_LOW_limit_home) //
int home_POSITION_low_cur = home_POSITION_low_const;            // нижняя точка плюс отступ (выставляется после калибровки set_LOW_limit_street) //

int street_POSITION_low_manual = street_POSITION_low_const;     // переменная для возможности выставления нижней границы с УЛИЦЫ вручную //
int home_POSITION_low_manual = home_POSITION_low_const;         // переменная для возможности выставления нижней границы от БАТАРЕИ вручную //

long doXsteps_counter = 0;                       // счетчик количества шагов сделанных за день //

int stepsAmount;                                 // количество шагов для функции doXsteps_func //

int steps_counter_limit4000;                     // счетчик шагов, чтобы не уйти в бесконечный цикл, если заклинит двигатель //
bool step_counter_Flag = 1;                      // флаг таймера для отправки отчета по количеству пройденных шагов за сутки // Нужно, чтобы быстро понять, не делают ли заслонки лишних движений летом //

int calibrate_state;                             // стейт калибровки заслонок // 0 - калибровка не нужна, 1 - полноценная калибровка, 2 - быстрая калибровка //
bool calibrate_ERROR = 0;                        // 0 - нет ошибок, 1 - ошибка при калибровки - двигатель сделал больше шагов, чем должен был и всё-равно не дошел до концевика. Либо сломан концевик, либо есть пропуск шагов. // Может сработать только при калибровке //
bool daily_calibrate_flag = 0;                   // флаг для таймера автоматической калибровки раз в сутки // стоит 0, чтобы если включить ардуино с 14 до 19 - не сработало две калибровки подряд (1 раз при включени и еще одна по таймеру) //


/// ↓↓↓ ДАТЧИК ТЕМПЕРАТУРЫ И ВЛАЖНОСТИ SHT41 + Реле ↓↓↓ ///


#include "Adafruit_SHT4x.h"

#define relay D0

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

float room_humidity_range = 5.00;                // чувствительность +- включения выключения увлажнителя //
float room_humidity_target = 35.00;              // желаемая влажность //

const int humidity_month_start = 9;              // первый месяц работы увлажнителя 9 (сентябрь) //
const int humidity_month_end = 4;                // последний месяц работы увлажнителя 4 (апрель) //

class SHT41                                      // класс датчика SHT41 температуры и влажности //
{
  public:
    SHT41()                                      // конструктор класса //
    {
      _humidity_low_alert = 15;
      _humidity_high_alert = 80;
      _crit_temp_low_error = -45.00;
      _crit_temp_high_error = 45.00;      
    }

    void update()                                // обновление данных с датчика //
    {
      sensors_event_t humidity, temp;                // запрашиваем температуру и влажность адафрут //
      sht4.getEvent(&humidity, &temp);               // обновляем показания температуры и влажность //
      _temp = temp.temperature;
      _humidity = humidity.relative_humidity;

      _check_errors();
    }
    
    float get_temp()
    {
      return(_temp);
    }

    float get_humidity()
    {
      return(_humidity);
    }

    void set_humidity_low_alert(String humidity_low_alert)
    {
      _humidity_low_alert = constrain(humidity_low_alert.toFloat(), 0, 100);
      object_array_users[users_array_index].send_message("Оповещения будут приходить, если влажность в квартире опустится ниже: " + String(_humidity_low_alert) + "%");
    }

    void set_humidity_high_alert (String humidity_high_alert)
    {
      _humidity_high_alert = constrain(humidity_high_alert.toFloat(), 0, 100);
      object_array_users[users_array_index].send_message("Оповещения будут приходить, если влажность в квартире поднимется выше: " + String(_humidity_high_alert) + "%");
    }

    float get_humidity_low_alert()
    {
      return(_humidity_low_alert);
    }

    float get_humidity_high_alert()
    {
      return(_humidity_high_alert);
    }
  private:
    float _temp;                                 // температура в комнате по датчику SHT41 //
    float _humidity;                             // влажность в коомнате по датчику SHT41 //

    float _humidity_low_alert;                   // нижняя граница для уведомления о низкой влажности в комнате //
    float _humidity_high_alert;                  // верхняя граница для уведомления о высокой влажности в комнате //

    float _crit_temp_low_error;                  // Нижняя граница для проверки глючности датчиков //
    float _crit_temp_high_error;                 // Верхняя граница для проверки глючности датчиков //

    void _check_errors()                         // Проверка показаний температур и влажности на ошибки //
    {
      if ((10 > int(_temp)) || (int(_temp) > _crit_temp_high_error))                         // Если датчик SHT41 отключен от питание значение будет 0 //
      {
        send_alert("ERROR Температура в комнате: " + String(_temp) + "°C");
        ErrorSTATEmy = 2;
      }

      if ((_humidity_low_alert > int(_humidity)) || (int(_humidity) > _humidity_high_alert))  // Если датчик SHT41 отключен от питание значение будет 0 //
      {
        send_alert("ERROR Влажность в комнате: " + String(_humidity) + "%");
        ErrorSTATEmy = 2;
      }
    }
};

SHT41 object_Temp_Humidity_sensor;               // создаем экземпляр класса SHT41 (объект) //


/// ↓↓↓ ДАТЧИК СО2 ↓↓↓ ///


#include <Arduino.h>
#include <SensirionI2CScd4x.h>
#include <Wire.h>

SensirionI2CScd4x scd4x;

class SCD41                                      // класс датчика СО2 //
{
  public:
    SCD41()
    {
      _co2_high_alert = 1200;
      _altitude = 215;
      _SCD41_timer_UTC = 0;
    }
  
    void restart()
    {
      uint16_t error_co2;
      error_co2 = scd4x.stopPeriodicMeasurement();
      if (error_co2)
      {
        send_alert("ERROR: Не смог остановить переодическое измерение датчика со2 перед запуском.");
        ErrorSTATEmy = 2;
      }
      else
      {
        object_array_users[0].send_message("ОСТАНОВИЛ периодическое измерение в датчике СО2.");
      }
      
      error_co2 = scd4x.setSensorAltitude(_altitude);
      if (error_co2)
      {
        send_alert("ERROR: Не смог установить Altitude (высоту над уровнем моря) в датчике со2.");
        ErrorSTATEmy = 2;
      }
      else
      {
        object_array_users[0].send_message("Выставил ALTITUDE (высоту над уровнем моря): " + String(_altitude) + "метров.");
      }      

      error_co2 = scd4x.startLowPowerPeriodicMeasurement();          // startLowPowerPeriodicMeasurement - данные обновляются раз в 30сек, startPeriodicMeasurement - данные обновляются раз в 5 секунд // SingleShot был изначально - на обоих датчиках проблема с повторяемостью показаний в +-50... // 
      if (error_co2)
      {
        send_alert("ERROR: Не смог запустить переодическое измерение датчика со2.");
        ErrorSTATEmy = 2;
      }
      else
      {
        object_array_users[0].send_message("ЗАПУСТИЛ периодическое измерение в датчике СО2.");
      }
      
    }

    void update()
    {
      if((object_TimeDate.get_UTC() - _SCD41_timer_UTC) > 40)        // если прошло больше 40 секунд с последнего обновлления - получаем новое значение СО2 // Если запросить со2 до того, как оно обновится в датчике - выдаст ошибку. //
      {
        uint16_t error_co2;                            // библиотечная переменная для проверки на ошибки в ходе опроса датчика
        float buf_temperature_co2 = 0.0f;              // без них не получается обратиться к функции readMeasurement
        float buf_humidity_co2 = 0.0f;                 // без них не получается обратиться к функции readMeasurement
        bool isDataReady = false;
        unsigned long timer_20_sec = millis();

        error_co2 = scd4x.readMeasurement(_CO2, buf_temperature_co2, buf_humidity_co2);
        if (error_co2)
        {
          send_alert("ERROR: не смог получить показания со2.");
          _CO2 = 0;
          ErrorSTATEmy = 2;
        }

        _check_errors();
        _check_alerts();
        _SCD41_timer_UTC = object_TimeDate.get_UTC();
      }
    }

    uint16_t get_CO2()
    {
      return(_CO2);
    }

    void set_co2_high_alert(uint16_t co2_high_alert)
    {
      _co2_high_alert = constrain(co2_high_alert, 0, 2500);
      object_array_users[users_array_index].send_message("Оповещения будут приходить, если СО2 в квартире поднимется выше: " + String(_co2_high_alert) + "ppm");
    }

    uint16_t get_co2_high_alert()
    {
      return(_co2_high_alert);
    }

  private:
    time_t _SCD41_timer_UTC;
    byte _id;
    uint16_t _CO2;                               // текущее показание со2
    uint16_t _co2_high_alert;                    // граница срабатывания оповещения
    uint16_t _altitude;                          // установил высоту над уровонем моря (altitude) 215 метров. По топографической карте моё местоположение - 190 метров + ~25 высота восьмого этажа.

    void _check_errors()
    {
      if (_CO2 < 200)
      {
        send_alert("ERROR уровень со2: " + String(_CO2) + "ppm" + "\n\n\n Если ошибка повторяется - попробуйте перезапуск /35101@JOArduinoChatBOT");
        ErrorSTATEmy = 2;
      }
    }

    void _check_alerts()
    {
      if (_CO2 > _co2_high_alert)
      {
        send_alert("ALERT уровень со2: " + String(_CO2) + "ppm");
      }
    }
};

SCD41 object_CO2_sensor;                         


///   ///   ///   ///   ///   ///   ///


void setup()                                     // стандартная функция Ардуино - выполняется один раз в начале //
{
  Serial.begin(9600);                            // запускаем Serial Port и определяем его скорость //
  Serial.setTimeout(200);                        // таймаут для .readString (ждет заданное значение на чтение Serial)

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);          // подключаемся к Wi-Fi //

  object_TimeDate.set_UTC_time();

  pinMode(pinKnobLow, INPUT_PULLUP);             // определяем пины // нижние концевики //
  pinMode(pinKnobHigh, INPUT_PULLUP);            // ↑↑↑ // верхние концевики //

  pinMode(street_STEP, OUTPUT);                  // ↑↑↑ // шаговый двигатель заслонки с улицы //
  pinMode(home_STEP, OUTPUT);                    // ↑↑↑ // шаговый двигатель заслонки от батареи //
  pinMode(DIR, OUTPUT);                          // ↑↑↑ // общий пин direction (направление вращения двигателей) //

  pinMode(relay, OUTPUT);                        // определяем пин реле, как output //
  digitalWrite(relay, LOW);                      // на всякий случай выключаем реле при запуске //

  ds.begin();                                    // инициализация датчиков температуры по шине OneWire //

  if (!sht4.begin())                             // инициализация адафрут датчика по шине i2c //
  {
    send_alert("ERROR: Ошибка инициализации датчика температуры и влажности.");
    ErrorSTATEmy = 2;
  }
  sht4.setPrecision(SHT4X_HIGH_PRECISION);       // Какие-то настройки в адафрут библиотеке отвечающие за точность //
  sht4.setHeater(SHT4X_NO_HEATER);               // Какие-то настройки в адафрут библиотеке отвечающие за "прогрев". Видимо, это необходимо при использование датчика в экстремальных температурах, хз. По умолчанию выключено. //

  scd4x.begin(Wire);                             // инициализация датчика СО2 по шине i2c //

  send_alert("Я проснулся.");
  calibrate_state = 2;
  object_array_users[0].send_message("Отправил запрос на быструю калибровку после включения.");
}

void loop()                                      // основной луп //
{
  #ifdef local_DEBUG
    Serial.print("ESP.getFreeHeap(): ");
    Serial.println(ESP.getFreeHeap());  
  #endif


  object_TimeDate.update_TimeDate();                                      // обновляем текущее время //
  
  if (bot1.getUpdates(bot1.last_message_received + 1) != 0)               // если есть новые сообщения обрабатываем одно //
  {
    Message_from_Telegram_converter();
  }

  if(object_TimeDate.get_MIN() % 2 > 0 && flag_every_minute_timer == false)                 // таймер каждую нечетную минуту //
  {
    update_sensors_data_and_calculations();                      

    SYNCstart();

    humidifier();
    thermostat();

    flag_every_minute_timer = true;
  }

  if(object_TimeDate.get_MIN() % 2 == 0 && flag_every_minute_timer == true)                 // таймер каждую четную минуту //
  {
    calibrate_test();                                                     // проверяет нужна ли калибровка //
    flag_every_minute_timer = false;
  }
}

void Message_from_Telegram_converter()           // преобразование сообщение из Телеграм в команду //
{
  String CHAT_IDcur = bot1.messages[0].chat_id;
  String text = bot1.messages[0].text;
  message_id_check(CHAT_IDcur);
  if (message_intruder_flag == false)
  {
    byte dividerIndex = text.indexOf('@');                              // ищем индекс разделителя @ // для того, чтобы работали команды из группы по запросу типа "/back@JOArduinoChatBOT" //
    String text_2nd_part = text.substring(dividerIndex + 1);            // записывает в text_2nd_part "JOArduinoChatBOT" //
    text = text.substring(0, dividerIndex);                             // записывает в text "/back" //

    if (text_2nd_part != "JOArduinoLogsBOT")                            // если сообщение не для него, то пропускает его //
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
  if (text == "/back")                      // отмена ввода //
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
  
  else if (object_array_users[users_array_index].get_message_state() != 1)               // если MessageState != 1, то значит ожидаем ввод данных //
  {
    switch (object_array_users[users_array_index].get_message_state())
    {
      case 10301:                                // установка температуры (нижняя граница) ALERT //
      {
        object_ds18b20_0.set_crit_temp_low_alert(text);
        object_ds18b20_3.set_crit_temp_low_alert(text);
        break;
      }

      case 10302:                                // установка влажность (нижняя граница) ALERT //
      {
        object_Temp_Humidity_sensor.set_humidity_low_alert(text);
        break;
      }

      case 10303:                                // установка влажность (верхняя граница) ALERT //
      {
        object_Temp_Humidity_sensor.set_humidity_high_alert(text);
        break;
      }

      case 10304:                                // установка СО2 (верхняя граница) ALERT //
      {
        object_CO2_sensor.set_co2_high_alert(text.toInt());
        break;
      }

      case 104:                                // установка температуры термостата //
      {
        TempMain = constrain(text.toFloat(), -55, 55);
        object_array_users[users_array_index].send_message("Термостат установлен на температуру: " + String(TempMain) + "°C");
        break;      
      }

      case 105:                                // установка чувствительности термостата //
      {
        TempRange = constrain(text.toFloat(), 0.2, 6);
        object_array_users[users_array_index].send_message("Термостат включится/выключится при отклонение +- " + String(TempRange) + "°C");
        break;   
      }

      case 106:                                // установка минимального процента воздуха с улицы //
      {
        AirLowLimit = constrain(text.toInt(), 10, 100);
        object_array_users[users_array_index].send_message("Минимальный процент воздуха с улицы: " + String(AirLowLimit) + "%");
        break;  
      }

      case 107:                                // установка шага заслонок //
      {
        Step_Per_loop = constrain(text.toInt(), 10, 1000);  // Ограничивает диапазон возможных значений //
        float calculation_buf = (home_POSITION_low_manual - street_POSITION_low_manual) / Step_Per_loop;
        int calculation_buf2 = int(calculation_buf) * 2;

        String buf = "Шаг изменения положения заслонок: " + String(Step_Per_loop) +\
                     "\n\n*Термостат запускается раз в две минуты и при отклонение от заданной температуры меняет положение заслонок. \n\n*Текущее значение количества шагов значит, что обе заслонки могут поменять своё положение на противоположенное за " + String(calculation_buf2) + " минут(ы).";
        object_array_users[users_array_index].send_message(buf);
        break;  
      }

      case 108:                                // ручное управление заслонками //
      {
        stepsAmount = constrain(text.toInt(), -10000, 10000);
        object_array_users[users_array_index].send_message("Принял количество шагов: " + String(stepsAmount));

        doXsteps_func();
        object_array_users[users_array_index].send_message(getMotorPositions());
        break;  
      }

      case 111:                               // ручная установка крайнего нижнего положения заслонки с улицы //
      {
        street_POSITION_low_manual = constrain(text.toInt(), street_POSITION_low_const, 0);
        if (StepsGLOBAL < street_POSITION_low_manual)
        {
          stepsAmount = street_POSITION_low_manual - StepsGLOBAL;
        }
        doXsteps_func();
        object_array_users[users_array_index].send_message("Установлено крайнее положение заслонки с улицы: " + String(street_POSITION_low_manual));
        break;  
      }

      case 112:                               // ручная установка крайнего нижнего положения заслонки от батареи //
      {
        home_POSITION_low_manual = constrain(text.toInt(), 0, home_POSITION_low_const);
        if (StepsGLOBAL > home_POSITION_low_manual) {
          stepsAmount = home_POSITION_low_manual - StepsGLOBAL;
        }
        doXsteps_func();
        object_array_users[users_array_index].send_message("Установлено крайнее положение заслонки от батареи: " + String(home_POSITION_low_manual));
        break;  
      }

      case 113:                               // Установка желаемой влажности //
      {
        room_humidity_target = constrain(text.toFloat(), 5, 65);
        float hum_buf_low = room_humidity_target - room_humidity_range;
        float hum_buf_high = room_humidity_target + room_humidity_range;

        String buf = "Установлена желаемая влажность: " + String(room_humidity_target) +\
                     "\n\n*Увлажнитель включится, когда влажность опуститься до: " + String(hum_buf_low) +\
                     "\nУвлажнитель выключится, когда влажность поднимется до: " +  String(hum_buf_high);
        object_array_users[users_array_index].send_message(buf);
        break;  
      }

      case 114:                               // Установка чувствительности увлажнителя //
      {
        room_humidity_range = constrain(text.toFloat(), 3, 20);
        float hum_buf_low = room_humidity_target - room_humidity_range;
        float hum_buf_high = room_humidity_target + room_humidity_range;

        String buf = "Чувствительность увлажнителя установлена: " + String(room_humidity_range) +\
                     "\n\n*Увлажнитель включится, когда влажность опуститься до: " + String(hum_buf_low) +\
                     "\nУвлажнитель выключится, когда влажность поднимется до: " + String(hum_buf_high);
        object_array_users[users_array_index].send_message(buf);
        break;  
      }

      case 390:                               // выбор гостевого чата вручную //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_array_users[5].set_id(text);
          object_array_users[users_array_index].send_message("Теперь я буду отвечать на входящие запросы с ID: " + text);
          object_array_users[5].send_message("Я проснулся.");
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

  else                                       // если MessageState == 1, то значит ожидаем команду //
  {
    byte dividerIndex = text.indexOf('/');   // ищем индекс разделителя "/" //
    text = text.substring(dividerIndex + 1); // оставляем только команду "1" //
    int text_int = text.toInt();

    switch (text_int)                        // 1** команды доступные всем, 3** команды требующие доступ администратора //
    {
      case 101:                              // текущие температуры в текстовом виде //
      {
        update_sensors_data_and_calculations();
        object_array_users[users_array_index].send_message(TEMP_text_output() + "\n\n\n\n↓↓↓ЗАСЛОНКИ↓↓↓" + getMotorPositions());
        break;
      }
      
      case 102:                              // текущие температуры в формате excel //
      {
        update_sensors_data_and_calculations(); 
        object_array_users[users_array_index].send_message(TEMP_excel_output() + "\n\n*Дата и время добавляется LOG ботом, по этому её здесь нет.");
        break;
      }

      case 103:                              // установка оповещений (ALERT) //
      { 
        String buf = "Установка оповещений: \n\n Температура нижняя граница (/10301@JOArduinoChatBOT). Текущее значение: " + String(object_ds18b20_0.get_crit_temp_low_alert()) +\
                     "\n Влажность нижняя граница (/10302@JOArduinoChatBOT). Текущее значение: " + String(object_Temp_Humidity_sensor.get_humidity_low_alert()) +\
                     "\n Влажность верхняя граница (/10303@JOArduinoChatBOT). Текущее значение: " + String(object_Temp_Humidity_sensor.get_humidity_high_alert()) +\
                     "\n СО2 верхняя граница (/10304@JOArduinoChatBOT). Текущее значение: " + String(object_CO2_sensor.get_co2_high_alert());
        object_array_users[users_array_index].send_message(buf);
        break;
      }

      case 10301:                            // ↑↑↑ // температура // 
      {
        String buf = "Отправьте сообщение для установки температуры, НИЖЕ которой будут приходить уведомления (ALERT) (от -55 до 55):\n\n   Текущее значение Рекуператор Приток (in): " + String(object_ds18b20_0.get_crit_temp_low_alert()) +\
                     "\n   Текущее значение Рекуператор Вытяжка (out): " + String(object_ds18b20_3.get_crit_temp_low_alert());
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(10301);
        break;
      }

      case 10302:                            // ↑↑↑ // влажность нижняя граница //      
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для установки влажности, НИЖЕ которой будут приходить уведомления (ALERT) (от 0 до 100):\n\nТекущее значение: " + String(object_Temp_Humidity_sensor.get_humidity_low_alert()));
        object_array_users[users_array_index].set_message_state(10302);
        break;            
      }

      case 10303:                            // ↑↑↑ // влажность верхняя граница //      
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для установки влажности, ВЫШЕ которой будут приходить уведомления (ALERT) (от 0 до 100):\n\nТекущее значение: " + String(object_Temp_Humidity_sensor.get_humidity_high_alert()));
        object_array_users[users_array_index].set_message_state(10303);
        break;            
      }

      case 10304:                            // ↑↑↑ // со2 // 
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для установки СО2, ВЫШЕ котрого будут приходить уведомления (ALERT) (от 0 до 2500):\n\nТекущее значение: " + String(object_CO2_sensor.get_co2_high_alert()));
        object_array_users[users_array_index].set_message_state(10304);
        break;            
      }

      case 104:                              // установка температуры термостата //
      {
        String buf = "Отправьте сообщение для установки температуры термостата в °C (от -55 до 55):\n\nТекущее значение: " + String(TempMain) +\
                     "\n\n*Термостат управляет температурой по датчику Рекуператор Приток(in).\n\n**Термостат не будет работать пока разница температур на улице и дома не достигнет 8 градусов (Пока этого не произойдет - процент водуха с улицы и от батареи будет равен нулю).";
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(104);
        break;
      }

      case 105:                              // установка чувствительности термостата //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение дла установки чувствительности термостата в °C (от 0.2 до 6):\n\nТекущее значение: " + String(TempRange));
        object_array_users[users_array_index].set_message_state(105);
        break;
      }

      case 106:                              // установка минимального процента воздуха с улицы //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение дла установки минимального процента воздуха с улицы +-5% (от 10 до 100):\n\nТекущее значение: " + String(AirLowLimit));
        object_array_users[users_array_index].set_message_state(106);
        break;
      }

      case 107:                              // установка шага заслонок //
      {
        float calculation_buf = (home_POSITION_low_manual - street_POSITION_low_manual) / Step_Per_loop;
        int calculation_buf2 = int(calculation_buf) * 2;

        String buf = "Отправьте сообщение для установки шага изменения положения заслонок (от 10 до 1000):\n\nТекущее значение: " + String(Step_Per_loop) +\
                     "\n\n*Термостат запускается раз в две минуты и при отклонение от заданной температуры меняет положение заслонок. \n\n*Текущее значение количество шагов значит, что обе заслонки могут поменять своё положение на противоположенное за " + String(calculation_buf2) + " минут(ы).";
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(107);
        break;
      }

      case 108:                              // ручное управление заслонками //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение с количеством шагов от -10000 до 10000:" + getMotorPositions());
        object_array_users[users_array_index].set_message_state(108);
        break;
      }

      case 109:                              // ручная инициализация быстрой калибровки заслонок //
      {
        calibrate_state = 2;
        object_array_users[users_array_index].send_message("Принял запрос на быструю калиброку.\n\n*Быстрая калибровка не сбрасывает крайние положения заслонок выставленные вручную.");
        break;
      }

      case 310:                              // ручная инициализация полноценной калибровки заслонок //
      {
        object_array_users[users_array_index].send_message("Полноценная калибровка займет несколько минут. \n\n*Полноценная калибровка сбрасывает крайние положения заслонок выставленные вручную.\n\n\nПродолжить? - /31001@JOArduinoChatBOT");
        break;
      }

      case 31001:                            // ручная инициализация полноценной калибровки заслонок // подтверждение //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          calibrate_state = 1;
          object_array_users[users_array_index].send_message("Отправил запрос на полноценную калибровку.");
        }
        else
        {
          object_array_users[users_array_index].send_message("Недостаточно прав доступа для того, чтобы отправить меня в другой чат.");
        }
        break;
      }

      case 111:                              // ручная установка крайнего нижнего положения заслонки с улицы //
      {
        String buf = "Отправьте сообщение для установки крайнего нижнего положения заслонки с улицы (во избежание ошибок его невозможно выставить меньше, чем значение после калибровки).\n\nЗначение крайнего нижнего положения заслонки с улицы после калибровки: " + String(street_POSITION_low_const) +\
                     "\nЗначение выставленное вручную: " + String(street_POSITION_low_manual);
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(111);
        break;
      }

      case 112:                              // ручная установка крайнего нижнего положения заслонки от батареи //
      {
        String buf = "Отправьте сообщение для установки крайнего нижнего положения заслонки от батареи (во избежание ошибок его невозможно выставить меньше, чем значение после калибровки).\n\nЗначение крайнего нижнего положения заслонки от батареи после калибровки: " + String(home_POSITION_low_const) +\
                     "\nЗначение выставленное вручную: " + String(home_POSITION_low_manual);
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(112);
        break;
      }

      case 113:                              // Установка желаемой влажности //
      {
        String buf = "Отправьте сообщение для установки желаемой влажности(от 5 до 65).\n\nТекущее значение: " + String(room_humidity_target) +\
                     "\n\n*Напоминаю, что увлажнитель не будет включатся в летние месяцы.\nПервый месяц работы: " + String(humidity_month_start) +\
                     "\nПоследний месяц работы: " + String(humidity_month_end);
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(113);
        break;
      }

      case 114:                              // Установка чувствительности увлажнителя //
      {
        String buf = "Отправьте сообщение для установки чувствительности увлажнителя (от 3 до 20).\n\nТекущее значение: " + String(room_humidity_range) +\
                     "\n\n*Напоминаю, что увлажнитель не будет включатся в летние месяцы.\nПервый месяц работы: " + String(humidity_month_start) +\
                     "\nПоследний месяц работы: " + String(humidity_month_end);
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(114);
        break;
      }

      case 190:                              // ВКЛ/ВЫКЛ Текстовых уведомлений //
      {
        object_array_users[users_array_index].set_alert_flag();
        break;
      }

      case 350:                              // установка разрешения ds12b20 на 12bit //
      {
        object_array_users[users_array_index].send_message("Эта комана необходима в случае, если настройки датчиков температуры сбились и показывают значения с разрешением 9бит вместо 12бит (разрешение 9бит - 0,5, разрешение 12бит - 0.0625)\n\n\nПродолжить? - /35001@JOArduinoChatBOT");
        break;
      }

      case 35001:                            // ↑↑↑ // Подтверждение //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
            object_ds18b20_0.set_res_to_12_bit();
            object_ds18b20_1.set_res_to_12_bit();
            object_ds18b20_2.set_res_to_12_bit();
            object_ds18b20_3.set_res_to_12_bit();
            object_ds18b20_4.set_res_to_12_bit();
            object_ds18b20_5.set_res_to_12_bit();
            object_ds18b20_6.set_res_to_12_bit();
        }
        else
        {
          object_array_users[users_array_index].send_message("Недостаточно прав доступа для выполнения команды.");
        }
        break;
      }

      case 351:                             // перезапуск датчика со2 //
      {
        object_array_users[users_array_index].send_message("Если датчик СО2 перестал отдавать показания или глючит - можно попробовать его перезапустить.\n\n\nПродолжить? - /35101@JOArduinoChatBOT");
        break;
      }

      case 35101:                           // ↑↑↑ // Подтверждение //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_CO2_sensor.restart();
        }

        else
        {
          object_array_users[users_array_index].send_message("Недостаточно прав доступа.");
        }
        break;
      }

      case 390:                             // выбор гостевого чата //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_array_users[users_array_index].send_message("Отправьте ID группы или профиля, чтобы определить гостевой чат (гостевой чат может быть только 1).");
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

void update_sensors_data_and_calculations()      // Опрос датчиков и расчеты //
{
  object_Temp_Humidity_sensor.update();
 
  ds.requestTemperatures();                              // запрашиваем температуру со всех датчиков ds18b20 сразу, а дальше только получаем данные //

  object_ds18b20_0.update();            // Рекуператор Приток (in) //
  object_ds18b20_1.update();            // Рекуператор Приток (out) //
  object_ds18b20_2.update();            // Рекуператор Вытяжка (in) //
  object_ds18b20_3.update();            // Рекуператор Вытяжка (out) //
  object_ds18b20_4.update();            // c Улицы //
  object_ds18b20_5.update();            // с Батареи //
  object_ds18b20_6.update();            // Объединенный поток //

  b62 = object_ds18b20_0.get_temp() - object_ds18b20_4.get_temp();                        // Теплопотери притока (если заслонки от батареи закрыты) //
  b82 = object_ds18b20_0.get_temp() - object_ds18b20_6.get_temp();                        // Теплопотери воздухувода от кровати до рекуператора и нагрев вентилятором //
  
  object_CO2_sensor.update();          // СО2 // хоть и вызываем update, но если не прошло 40 секунд - показание не обновит и выдаст последние значение. //

  float buf_c1 = (object_ds18b20_2.get_temp() - object_ds18b20_0.get_temp());

  calculations_b23(buf_c1);            // Эффективность рекуперации на приток //
  calculations_b45(buf_c1);            // Эффективность рекуперации на вытяжку //
  calculations_b61_b71(buf_c1);        // Процент воздуха с улицы   и   Процент воздуха с батареи //
}

void calculations_b23(float buf_c1)              // расчет b23 (Эффективность рекуперации на приток) //
{
  if (object_ds18b20_0.get_temp() == object_ds18b20_1.get_temp())                                  // Исключить деление на 0 //
  {
    b23 = 0;
  }
  
  else if (buf_c1 > -1 && buf_c1 < 1)                    //  Чтобы не считать КПД при разницах Притока и вытяжки меньше 1°C //
  {
    b23 = 0;
  }
  
  else                                           // ЭФФЕКТИВНОСТЬ РЕКУПЕРАЦИИ НА ПРИТОК //
  {
    b23 = 100 / (object_ds18b20_2.get_temp() - object_ds18b20_0.get_temp()) * (object_ds18b20_1.get_temp() - object_ds18b20_0.get_temp());
  }
}

void calculations_b45(float buf_c1)              // расчет b45 (Эффективность рекуперации на вытяжку) // 
{
  if (object_ds18b20_2.get_temp() == object_ds18b20_3.get_temp())                                  // Исключить деление на 0 //
  {
    b45 = 0;
  }

  else if (buf_c1 > -1 && buf_c1 < 1)                    //  Чтобы не считать КПД при разницах Притока и вытяжки меньше 1°C //
  {
    b45 = 0;
  }

  else                                           // ЭФФЕКТИВНОСТЬ РЕКУПЕРАЦИИ НА ВЫТЯЖКУ //
  {
    b45 = 100 / (object_ds18b20_2.get_temp() - object_ds18b20_0.get_temp()) * (object_ds18b20_2.get_temp() - object_ds18b20_3.get_temp());
  }
}

void calculations_b61_b71(float buf_c1)          // расчет b61 (Процент воздуха с улицы) и b71 (Процент воздуха с батареи) //
{
  if ((object_ds18b20_5.get_temp() > object_ds18b20_4.get_temp() + 7) && object_ds18b20_6.get_temp() < object_ds18b20_5.get_temp() && object_ds18b20_6.get_temp() > object_ds18b20_4.get_temp())       // Условия при которых формула должна работать корректно //
  {
    b61 = ((object_ds18b20_5.get_temp() - object_ds18b20_6.get_temp()) / (object_ds18b20_5.get_temp() - object_ds18b20_4.get_temp())) * 100;         // ПРОЦЕНТ ВОЗДУХА С УЛИЦЫ //
    b71 = ((object_ds18b20_6.get_temp() - object_ds18b20_4.get_temp()) / (object_ds18b20_5.get_temp() - object_ds18b20_4.get_temp())) * 100;         // ПРОЦЕНТ ВОЗДУХА С БАТАРЕИ //
  }

  else
  {
    b61 = 0;
    b71 = 0;
  }
}

void humidifier()                                // увлажнитель //
{
  if (object_TimeDate.get_DateMONTH() >= humidity_month_start || object_TimeDate.get_DateMONTH() <= humidity_month_end)
  {
    if (object_Temp_Humidity_sensor.get_humidity() < (room_humidity_target - room_humidity_range))
    {
      digitalWrite(relay, HIGH);
    }

    if (object_Temp_Humidity_sensor.get_humidity() > (room_humidity_target + room_humidity_range))
    {
      digitalWrite(relay, LOW);
    }
  }
}

void thermostat()                                // термостат //
{
  if ((object_ds18b20_0.get_temp() < TempMain - TempRange) && (b61 > AirLowLimit + 5))
  {
    stepsAmount = (0 - Step_Per_loop);           // отрицательные значения открываем батарею, закрываем улицу //
    doXsteps_func();
  }

  if ((object_ds18b20_0.get_temp() > TempMain + TempRange) || (b61 < AirLowLimit - 5))
  {
    stepsAmount = Step_Per_loop;                 // положительные значения открываем улицу, закрываем батарею //
    doXsteps_func();
  }
}

String getMotorPositions()                         // создает стринг с текстовым описанием положения заслонок как в абсолютном значении, так и в процентах //
{
  int position_street = 0;
  int position_home = 0;

  if (StepsGLOBAL < 0) {
    position_street = map(StepsGLOBAL, 0, street_POSITION_low_manual, 0, 100);
  }
  if (StepsGLOBAL > 0) {
    position_home = map(StepsGLOBAL, 0, home_POSITION_low_manual, 0, 100);
  }

  String Message_Motor_Positions = "\n\nКрайнее нижнее положение заслонки от батареи: " + String(home_POSITION_low_manual) +\
                                   "\nТекущее положение заслонок(StepsGLOBAL*): " + String(StepsGLOBAL) +\
                                   "\nКрайнее нижнее положение заслонки с улицы: " + String(street_POSITION_low_manual) +\
                                   "\n\n*Заслонка с улицы закрыта на " + String(position_street) +\
                                   "%.\n*Заслонка от батареи закрыта на " + String(position_home) +\
                                   "%.\n\n**Шагов сделано за сегодня (без учета калибровки): " + String(doXsteps_counter);

  return (Message_Motor_Positions);
}

void doXsteps_func()                             // меняет положение заслонок. достаточно приравнять глобальную переменную stepsAmount к необходимому количеству шагов и вызвать функцию doXsteps_func //
{
  if (calibrate_ERROR == 0) {
    if (stepsAmount < 0)                                                                                // отрицательные значения открываем батарею, закрываем улицу //
    {
      digitalWrite(DIR, dir_UP);
      while (stepsAmount < 0 && StepsGLOBAL > 0 && digitalRead(pinKnobHigh) == 1)                           // открываем батарею // от 0 вверх - "батарея", от 0 вниз = "улица" //
      {
        stepsHOME8();
        doXsteps_counter++;
        StepsGLOBAL--;
        stepsAmount++;
      }
      digitalWrite(DIR, dir_DOWN);

      while (stepsAmount < 0 && StepsGLOBAL > street_POSITION_low_manual && digitalRead(pinKnobLow) == 1)  // закрываем улицу // от 0 вверх - "батарея", от 0 вниз = "улица" //
      {
        stepsSTREET8();
        doXsteps_counter++;
        StepsGLOBAL--;
        stepsAmount++;
      }
    }

    if (stepsAmount > 0)                                                                                // положительные значения открываем улицу, закрываем батарею //
    {
      digitalWrite(DIR, dir_UP);
      while (stepsAmount > 0 && StepsGLOBAL < 0 && digitalRead(pinKnobHigh) == 1)                           // открываем улицу // от 0 вверх - "батарея", от 0 вниз = "улица" //
      {
        stepsSTREET8();
        doXsteps_counter++;
        StepsGLOBAL++;
        stepsAmount--;
      }

      digitalWrite(DIR, dir_DOWN);
      while (stepsAmount > 0 && StepsGLOBAL < home_POSITION_low_manual && digitalRead(pinKnobLow) == 1)     // закрываем батарею // от 0 вверх - "батарея", от 0 вниз = "улица" //
      {
        stepsHOME8();
        doXsteps_counter++;
        StepsGLOBAL++;
        stepsAmount--;
      }
    }

    if (digitalRead(pinKnobHigh) == 0 || digitalRead(pinKnobLow) == 0)                                          //Отправляем ошибку, если коснулись кнопки (не должны) //
    {
      send_alert("ERROR: Сработал концевик на заслонке.");
      calibrate_state = 1;
      ErrorSTATEmy = 2;
    }
  }

  else {
    ErrorSTATEmy = 2;
    send_alert("ERROR: Управление заслонками невозможно - заклинил двигатель или не работают концевики.");
  }
}

void calibrate_test()                            // проверяет нужна ли калибровка и если нужна делает. так же по таймеру, раз в сутки, запускает быструю калибровыку //
{
  int StepsGLOBALbuf = StepsGLOBAL;

  if (calibrate_state == 2 && calibrate_ERROR == 0)                       // быстрая калибровка //
  {
    object_array_users[0].send_message_second_chat("Начинаю процесс быстрой калибровки.");

    if (digitalRead(pinKnobLow) == 1 && digitalRead(pinKnobHigh) == 1) {
      set_HIGH_limit_street();
      calibrate_ERROR_test();

      if (calibrate_ERROR == 0) {
        steps_counter_limit4000 = 4000;                                   // считаем количество шагов, чтобы не уйти в бесконечный цикл, если заклинит двигатель //
        digitalWrite(DIR, dir_DOWN);
        while (digitalRead(pinKnobLow) == 1 && steps_counter_limit4000 > 0)   // Идём вниз пока не сработает кнопка //
        {
          steps_counter_limit4000--;
          stepsHOME8();
        }

        digitalWrite(DIR, dir_UP);
        for (int i = 0; i < KNOB_space; i++)                              // Отступаем обратно //
        {
          stepsHOME8();
        }
      }
      calibrate_ERROR_test();
      StepsGLOBAL = home_POSITION_low_cur;
      object_array_users[0].send_message_second_chat("Быстрая калибровка выполнена.\n\nТекущее положение заслонок: " + String(StepsGLOBAL));
    } else {
      send_alert("Быстрая калибровка невозможна. Концевик нажат.");
      calibrate_state = 1;
    }
  }

  if (calibrate_state == 1 && calibrate_ERROR == 0)                       // полноценная калибровка //
  {
    object_array_users[0].send_message_second_chat("Начинаю процесс полноценной калибровки.");

    StepsGLOBAL = 0;                                                      // StepsGLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
    street_POSITION_low_cur = 0;                                          // нижняя точка плюс отступ (выставляется после калибровки set_LOW_limit_home) //
    home_POSITION_low_cur = 0;                                            // нижняя точка плюс отступ (выставляется после калибровки set_LOW_limit_street) //

    long street_POSITION_low_manual = 0;
    long home_POSITION_low_manual = 0;

    set_HIGH_limit_street();                                              // Запускаем по два раза, чтобы отступить KNOB_space*2 //
    calibrate_ERROR_test();

    if (calibrate_ERROR == 0) {
      set_HIGH_limit_street();
      calibrate_ERROR_test();
    }

    if (calibrate_ERROR == 0) {
      set_HIGH_limit_home();                                              // Запускаем по два раза, чтобы отступить KNOB_space*2 //
      calibrate_ERROR_test();
    }

    if (calibrate_ERROR == 0) {
      set_HIGH_limit_home();
      calibrate_ERROR_test();
    }

    if (calibrate_ERROR == 0) {
      set_HIGH_limit_street();                                            // Производим калибровку второй раз на случай, если была зажата противоположенный концевик! //
      calibrate_ERROR_test();
      object_array_users[0].send_message_second_chat("Верхнее положение заслонки с улицы откалибровано.");
    }

    if (calibrate_ERROR == 0) {
      set_HIGH_limit_home();                                              // Производим калибровку второй раз на случай, если была зажата противоположенный концевик! //
      calibrate_ERROR_test();
      object_array_users[0].send_message_second_chat("Верхнее положение заслонки от батареи откалибровано.");
    }

    if (calibrate_ERROR == 0) {
      set_LOW_limit_street();
      calibrate_ERROR_test();
      StepsGLOBAL = street_POSITION_low_cur;
      street_POSITION_low_manual = street_POSITION_low_cur;
      long cal_dif1 = street_POSITION_low_const - street_POSITION_low_cur;
      object_array_users[0].send_message_second_chat("Нижнее положожение заслонки с улицы откалибровано. Результат: " + String(street_POSITION_low_cur) + "\n\nОтклонение от константного значения: " + String(cal_dif1));

      stepsAmount = (0 - street_POSITION_low_cur);
      doXsteps_counter = doXsteps_counter - stepsAmount;                  // вычитаем количество шагов, чтобы не учитывать их в количестве шагов за день //
      doXsteps_func();                                                    // Полностью открываем заслонку с улицы //

      object_array_users[0].send_message_second_chat("Положение заслонки с улицы возвращено в открытое положение. Текущее положение StepsGLOBAL: " + String(StepsGLOBAL));
    }

    if (calibrate_ERROR == 0) {
      set_LOW_limit_home();
      calibrate_ERROR_test();
      StepsGLOBAL = home_POSITION_low_cur;
      home_POSITION_low_manual = home_POSITION_low_cur;
      long cal_dif2 = home_POSITION_low_const - home_POSITION_low_cur;
      object_array_users[0].send_message_second_chat("Нижнее положожение заслонки от батареи откалибровано. Результат: " + String(home_POSITION_low_cur) + "\n\nОтклонение от константного значения: " + String(cal_dif2));
    }
  }

  if (calibrate_state == 1 || calibrate_state == 2)                       // завершающий процесс любой калибровки //
  {
    if (calibrate_ERROR == 0)                                             // если нет ошибок - возвращаем положение заслонок в положение до калибровки //
    {
      stepsAmount = StepsGLOBALbuf - StepsGLOBAL;
      doXsteps_counter = doXsteps_counter + stepsAmount;                  // вычитаем количество шагов, чтобы не учитывать их в количестве шагов за день //
      doXsteps_func();
      calibrate_state = 0;
      object_array_users[0].send_message_second_chat("Положение заслонок возвращено в положение до калибровки. Текущее положение StepsGLOBAL: " + String(StepsGLOBAL));
    }
    if (calibrate_ERROR == 1)                                             // если есть ошибки - отправляем уведомление о невозможности калибровки //
    {
      ErrorSTATEmy = 2;
      send_alert("ERROR: Калибровка невозможна - заклинил двигатель или не работают концевики.");
    }
  }

  if (daily_calibrate_flag == 1 && object_TimeDate.get_TimeB() > 140101 && object_TimeDate.get_TimeB() < 185959)      // автоматическая калибровка раз в сутки в 14:01 //
  {
    calibrate_state = 2;
    daily_calibrate_flag = 0;
    object_array_users[0].send_message_second_chat("Отправил запрос на быструю калибровку по таймеру (раз в сутки в период с 14:00 до 19:00).");
  }

  if (daily_calibrate_flag == 0)                                          // поднимаем флаг калибровки //
  {
    if (object_TimeDate.get_TimeB() < 135959 || object_TimeDate.get_TimeB() > 190101) {
      daily_calibrate_flag = 1;
    }
  }

  if (step_counter_Flag == 1 && object_TimeDate.get_TimeB() > 234500)                           // отправка отчета по количеству пройденых шагов за день //
  {
    step_counter_Flag = 0;
    object_array_users[0].send_message_second_chat("Шагов сделано за сегодня (без учета калибровки): " + String(doXsteps_counter));
    doXsteps_counter = 0;
  }

  if (step_counter_Flag == 0 && object_TimeDate.get_TimeB() < 234400)                           // возвращает влаг обратно, чтобы отчет отправился на следующий день //
  {
    step_counter_Flag = 1;
  }
}

void calibrate_ERROR_test()                      // проверяет дошел ли счетчик шагов до 0 и если дошел, то ставит флаг ошибки //
{
  if (steps_counter_limit4000 == 0)
  {
    calibrate_ERROR = 1;
  }
  
  if (calibrate_ERROR == 1)
  {
    ErrorSTATEmy = 2;
    send_alert("!!!FATAL_ERROR!!!_From_Chat: Заклинил двигатель или не работают концевики.");
  }
}

void set_HIGH_limit_street()                     // Калибровка заслонки с улицы верхнее положение //
{
  steps_counter_limit4000 = 4000;                                         // считаем количество шагов, чтобы не уйти в бесконечный цикл, если заклинит двигатель //
  digitalWrite(DIR, dir_UP);
  while (digitalRead(pinKnobHigh) == 1 && steps_counter_limit4000 > 0)        // Идём вверх пока не сработает кнопка //
  {
    stepsSTREET8();
  }

  digitalWrite(DIR, dir_DOWN);
  for (int i = 0; i < KNOB_space; i++)                                    // Отступаем обратно //
  {
    stepsSTREET8();
  }
}

void set_HIGH_limit_home()                       // Калибровка заслонки с улицы верхнее положение //
{
  steps_counter_limit4000 = 4000;                                         // считаем количество шагов, чтобы не уйти в бесконечный цикл, если заклинит двигатель //
  digitalWrite(DIR, dir_UP);
  while (digitalRead(pinKnobHigh) == 1 && steps_counter_limit4000 > 0)        // Идём вверх пока не сработает кнопка //
  {
    stepsHOME8();
  }

  digitalWrite(DIR, dir_DOWN);
  for (int i = 0; i < KNOB_space; i++)                                    // Отступаем обратно //
  {
    stepsHOME8();
  }
}

void set_LOW_limit_street()                      // Калибровка заслонки с улицы нижнее положение //
{
  steps_counter_limit4000 = 4000;                                         // считаем количество шагов, чтобы не уйти в бесконечный цикл, если заклинит двигатель //
  digitalWrite(DIR, dir_DOWN);
  while (digitalRead(pinKnobLow) == 1 && steps_counter_limit4000 > 0)         // Идём вниз пока не сработает кнопка //
  {
    stepsSTREET8();
    street_POSITION_low_cur--;                                            // StepsGLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
  }

  digitalWrite(DIR, dir_UP);
  for (int i = 0; i < KNOB_space; i++)                                    // Отступаем обратно //
  {
    stepsSTREET8();
    street_POSITION_low_cur++;                                            // StepsGLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
  }
}

void set_LOW_limit_home()                        // Калибровка заслонки с улицы нижнее положение //
{
  steps_counter_limit4000 = 4000;                                         // считаем количество шагов, чтобы не уйти в бесконечный цикл, если заклинит двигатель //
  digitalWrite(DIR, dir_DOWN);
  while (digitalRead(pinKnobLow) == 1 && steps_counter_limit4000 > 0)         // Идём вниз пока не сработает кнопка //
  {
    stepsHOME8();
    home_POSITION_low_cur++;                                              // StepsGLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
    steps_counter_limit4000--;
  }

  digitalWrite(DIR, dir_UP);
  for (int i = 0; i < KNOB_space; i++)                                    // Отступаем обратно //
  {
    stepsHOME8();
    home_POSITION_low_cur--;                                              // StepsGLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
  }
}

void stepsSTREET8()                              // Сделать 8 микрошагов (1 реальный шаг при режиме 1к8) //
{
  for (int i = 0; i < 8; i++) {
    digitalWrite(street_STEP, HIGH);
    delay(1);                                    // delayMicroseconds на ESP даёт глюки из-за того, что в отличие от delay не даёт возможности делать прервывания и это накапливает ошибки и уводит ESP в перезагрузку //
    digitalWrite(street_STEP, LOW);
    delay(1);                                    // delayMicroseconds на ESP даёт глюки из-за того, что в отличие от delay не даёт возможности делать прервывания и это накапливает ошибки и уводит ESP в перезагрузку //
  }
}

void stepsHOME8()                                // Сделать 8 микрошагов (1 реальный шаг при режиме 1к8) //
{
  for (int i = 0; i < 8; i++) {
    digitalWrite(home_STEP, HIGH);
    delay(1);                                    // delayMicroseconds на ESP даёт глюки из-за того, что в отличие от delay не даёт возможности делать прервывания и это накапливает ошибки и уводит ESP в перезагрузку //
    digitalWrite(home_STEP, LOW);
    delay(1);                                    // delayMicroseconds на ESP даёт глюки из-за того, что в отличие от delay не даёт возможности делать прервывания и это накапливает ошибки и уводит ESP в перезагрузку //
  }
}

void SYNCstart()                                 // Отправка температур и проверка времени на синхронизацию //
{
  Serial.print(object_TimeDate.get_UTC());
  Serial.print(";");
  Serial.print(TEMP_excel_output());
  Serial.print(";");
  Serial.print(ErrorSTATEmy);

  ErrorSTATEmy = 1;

  String SYNCmessage = Serial.readString();

  if (SYNCmessage.length() > 2)                  // Проверка что вообще что-то пришло //
  {
    unsigned long SYNCtime = SYNCmessage.toInt();
    unsigned long UTC_timeLong = object_TimeDate.get_UTC();
    unsigned long unSYNC = UTC_timeLong - SYNCtime;

    if (unSYNC > 600)                            // Проверка что рассинхронизация меньше 10 минут (отрицательные значения не нужно проверять - это unsugned long) //
    {
      send_alert("ERROR: Рассинхронизация составила: " + String(unSYNC) + " секунд.");
    }
  }

  else
  {
    send_alert("ERROR: Данные для синхронизации не были получены.");
  }
}

String TEMP_text_output()                          // Команда вывода данных в текстовом виде //
{
  int air_exchange_caclulated = 0;
  if(object_CO2_sensor.get_CO2() > 410)                                             // чтобы исключить значение на 0 и не показывать нереалистичные значения //
  {
    air_exchange_caclulated = 18000 / (object_CO2_sensor.get_CO2() - 400);          // этот коэффициент взят из таблички расчета со2 и актуален для одного человека находящегося в покое //
  }

  String Message = ("Температура в комнате: " + String(object_Temp_Humidity_sensor.get_temp()) +\
                    "°C\nВлажность в комнате: " + String(object_Temp_Humidity_sensor.get_humidity()) +\
                    "\nСодержание СО2 в воздухе: " + String(object_CO2_sensor.get_CO2()) +\
                    "\nПримерный воздухообмен*: " + String(air_exchange_caclulated) +\
                    "м³\n*Расчет на одного человека, в состояние покоя. Для более-менее правильного расчета нужно несколько часов, из-за инертности вентиляции." +\
                    "\n\nРекуп. Приток (in): " + String(object_ds18b20_0.get_temp()) +\
                    "°C\nPeкyп. Приток (out): " + String(object_ds18b20_1.get_temp()) +\
                    "°C\nPeкyп. Вытяжка (in): " + String(object_ds18b20_2.get_temp()) +\
                    "°C\nPeкyп. Вытяжка (out): " + String(object_ds18b20_3.get_temp()) +\
                    "°C\n\nKПД на приток: " + String(b23) +\
                    "%\nKПД на вытяжку: " + String(b45) +\
                    "%\n\nC улицы: " + String(object_ds18b20_4.get_temp()) +\
                    "°C\nC батареи: " + String(object_ds18b20_5.get_temp()) +\
                    "°C\nObъeдинeнный поток: " + String(object_ds18b20_6.get_temp()) +\
                    "°C\n\nTeплoпoтepи от кровати до рекуп. и нагрев вентилятором: " + String(b82) +\
                    "°C\nTeплoпoтepи от улицы до рекуператора: " + String(b62) +\
                    "°C\n\n\n\n↓↓↓ТОЛЬКО ДЛЯ ХОЛОДОВ↓↓↓" + "\n\nBoздyxa c улицы: " + String(b61) +\
                    "%\nBoздyxa c батареи: " + String(b71) + "%");

  return(Message);
}

String TEMP_excel_output()                         // Команда вывода данных для записи на SD карту и формирование лога для экселя //
{
  String Message = (String(object_Temp_Humidity_sensor.get_temp()) + "," +\
             String(object_Temp_Humidity_sensor.get_humidity()) + "," +\
             String(object_CO2_sensor.get_CO2()) + "," +\
             String(object_ds18b20_0.get_temp()) + "," +\
             String(object_ds18b20_1.get_temp()) + "," +\
             String(object_ds18b20_2.get_temp()) + "," +\
             String(object_ds18b20_3.get_temp()) + "," +\
             String(b45) + "," + String(b23) + "," +\
             String(object_ds18b20_4.get_temp()) + "," +\
             String(object_ds18b20_5.get_temp()) + "," +\
             String(object_ds18b20_6.get_temp()) + "," +\
             String(b62) + "," + String(b82) + "," +\
             String(b61) + "," + String(b71) + "," +\
             String(StepsGLOBAL) + "," + String(doXsteps_counter));
  return(Message);
}
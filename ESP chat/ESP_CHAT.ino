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
  USER_ID6
*/

X509List cert(TELEGRAM_CERTIFICATE_ROOT);                       // какой-то сертификат //
WiFiClientSecure secured_client;                                // какой-то secured client //

UniversalTelegramBot bot1(BOT_TOKEN1, secured_client);          // BOT "Chat" //
//UniversalTelegramBot bot2(BOT_TOKEN2, secured_client);        // BOT "Logs + LED" //
UniversalTelegramBot bot3(BOT_TOKEN3, secured_client);          // BOT "Calibrate" //
//UniversalTelegramBot bot4(BOT_TOKEN4, secured_client);        // BOT "Backup" //

bool message_intruder_flag = true;
byte users_array_index;
bool shutdown_friends = false;

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
      #ifdef Jesse_yield_enable
        yield();
      #endif

      bot3.sendMessage(_id, input, "");
    }

    void send_alert(String input)
    {
      #ifdef Jesse_yield_enable
        yield();
      #endif
      
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

const byte user_array_length = 7;
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

  bot1.sendMessage(_id, input, "");
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
      object_array_users[0].send_message(_name + ", написал:\n\n" + bot1.messages[0].text);
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

      #ifdef Jesse_yield_enable
        yield();
      #endif

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

      #ifdef Jesse_yield_enable
        yield();
      #endif

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


bool global_ERROR_flag = false;                          // Флаг для ошибок (1 - нет ошибок, 2 - в цикле были ошибки) // ps в какой-то момент появилось подозрение, что где-то в биоблетаках уже есть ErrorSTATE и возникают ошибки //


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

      #ifdef Jesse_yield_enable
        yield();
      #endif

      _array_address[4]= ar4;
      _array_address[5]= ar5;
      _array_address[6]= ar6;
      _array_address[7]= ar7;
      _name = name;
    }
    
    void update()                                // обновление данных с датчика //
    {
      #ifdef Jesse_yield_enable
        yield();
      #endif

      _temp = ds.getTempC(_array_address);
      if (_alert_flag == true)
      {
        _check_alerts();
      }
      _check_errors();

      #ifdef Jesse_yield_enable
        yield();
      #endif      
    }

    float get_temp()
    {
      #ifdef Jesse_yield_enable
        yield();
      #endif
      
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
        global_ERROR_flag = true;
      }
    }

    void _check_errors()
    {
      if (_crit_temp_low_error > _temp || _temp > _crit_temp_high_error)
      {
        send_alert("ERROR: " + _name + String(_temp) + "°C");
        global_ERROR_flag = true;
      }
    }

};

class_ds18b20 object_ds18b20_0(true, 0x28, 0x76, 0x6A, 0x39, 0x0, 0x0, 0x0, 0x43,  "Рекуператор Приток (in): ");
class_ds18b20 object_ds18b20_1(false, 0x28, 0x5C, 0x4E, 0x3C, 0x0, 0x0, 0x0, 0x5C, "Рекуператор Приток (out): ");
class_ds18b20 object_ds18b20_2(false, 0x28, 0xAD, 0x18, 0x3A, 0x0, 0x0, 0x0, 0x19, "Рекуператор Вытяжка (in): ");
class_ds18b20 object_ds18b20_3(true, 0x28, 0x27, 0xB5, 0x3E, 0x0, 0x0, 0x0, 0xED,  "Рекуператор Вытяжка (out): ");
class_ds18b20 object_ds18b20_4(false, 0x28, 0xB6, 0x76, 0x39, 0x0, 0x0, 0x0, 0x81, "c Улицы: ");
class_ds18b20 object_ds18b20_5(false, 0x28, 0xF, 0x2A, 0x3A, 0x0, 0x0, 0x0, 0x2C,  "с Батареи: ");
class_ds18b20 object_ds18b20_6(false, 0x28, 0x5E, 0x81, 0x39, 0x0, 0x0, 0x0, 0xFE, "Объединенный поток: ");


/// ↓↓↓ УПРАВЛЕНИЕ ЗАСЛОНКАМИ ↓↓↓ ///


float TempMain = 4.00;                           // температура термостата //
float TempRange = 2.00;                          // чувствительность термостата //
int Step_Per_loop = 80;                          // количество шагов двигателя за цикл запуска термостата //
int AirLowLimit = 25;                            // минимальный процент воздуха с улицы //

#define dir_UP LOW                     // чтобы не путаться, поскольку low это 0 вольт, а high это 3,3 вольта //
#define dir_DOWN HIGH                  // ↑↑↑ //

const byte pin_step_SREET = 12;    //D6              // пин для управления шагами двигателя заслонки с улицы //
const byte pin_step_HOME = 13;     //D7               // пин для управления шагами двигателя заслонки от батареи //
const byte pin_DIR = 15;           //D8                     // общий пин для смены direction моторов. HIGH - Вниз , LOW - Вверх //
const byte pin_knob_LOW = 0;       //D3                // нижние концевики  //
const byte pin_knob_HIGH = 2;      //D4               // верхние концевики //


byte calibrate_state;
const int street_LOWEST_position_const = -2912;                    // НИЖНЯЯ точка плюс отступ (константа для проверки отклонения от первоначальных данных) //
const int home_LOWEST_position_const = 3188;                       // ВЕРХНЯЯ точка плюс отступ (константа для проверки отклонения от первоначальных данных) //

int street_LOWEST_position_cur = street_LOWEST_position_const;     // нижняя точка плюс отступ (выставляется после калибровки set_LOW_limit_home) //
int home_LOWEST_position_cur = home_LOWEST_position_const;         // нижняя точка плюс отступ (выставляется после калибровки set_LOW_limit_street) //

class class_motor
{
  public:
    class_motor(byte pin_step)
    {
      _pin_step = pin_step;
    }

    void do_8_microsteps()
    {
      for (int i = 0; i < 8; i++)
      {
        digitalWrite(_pin_step, HIGH);
        delay(1);                                    // delayMicroseconds на ESP даёт глюки из-за того, что в отличие от delay не даёт возможности делать прервывания и это накапливает ошибки и уводит ESP в перезагрузку //
        digitalWrite(_pin_step, LOW);
        delay(1);                                    // delayMicroseconds на ESP даёт глюки из-за того, что в отличие от delay не даёт возможности делать прервывания и это накапливает ошибки и уводит ESP в перезагрузку //
      }
    }

  protected:
    byte _pin_step;
};

class class_motor_plus_knobs : public class_motor
{
  public:
    class_motor_plus_knobs(byte pin_step) : class_motor(pin_step)
    {
    }

    bool go_to_HIGH_limit()                       // Калибровка заслонки с улицы верхнее положение //
    {
      int steps_counter_limit4000 = 4000;                                      // считаем количество шагов, чтобы не уйти в бесконечный цикл, если заклинит двигатель //
      _position_before_calibration = 0;
      digitalWrite(pin_DIR, dir_UP);
      while (digitalRead(pin_knob_HIGH) == 1 && steps_counter_limit4000 > 0)  // Идём вверх пока не сработает кнопка //
      {
        do_8_microsteps();
        _position_before_calibration++;
      }

      digitalWrite(pin_DIR, dir_DOWN);
      for (int i = 0; i < _knob_space; i++)                                    // Отступаем обратно //
      {
        do_8_microsteps();
        _position_before_calibration--;
      }

      return(_check_limit4000(steps_counter_limit4000));
    }

    bool set_LOW_limit()                        // Калибровка заслонки с улицы нижнее положение //
    {
      _LOWEST_position = 0;
      int steps_counter_limit4000 = 4000;                                         // считаем количество шагов, чтобы не уйти в бесконечный цикл, если заклинит двигатель //
      _position_before_calibration = 0;

      digitalWrite(pin_DIR, dir_DOWN);
      while (digitalRead(pin_knob_LOW) == 1 && steps_counter_limit4000 > 0)         // Идём вниз пока не сработает кнопка //
      {
        do_8_microsteps();
        _LOWEST_position++;                                              // _steps_GLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
        steps_counter_limit4000--;
        _position_before_calibration++;
      }

      digitalWrite(pin_DIR, dir_UP);
      for (int i = 0; i < _knob_space; i++)                                    // Отступаем обратно //
      {
        do_8_microsteps();
        _LOWEST_position--;                                              // _steps_GLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
        _position_before_calibration--;
      }

      return(_check_limit4000(steps_counter_limit4000));
    }

    int get_LOWEST_position()
    {
      return(_LOWEST_position);
    }

    int get_position_before_calibration()
    {
      return(_position_before_calibration);
    }
  private:
    const byte _knob_space = 110;

    int _position_before_calibration;

    int _LOWEST_position;

    bool _check_limit4000(int steps_counter_limit4000)
    {
      if (steps_counter_limit4000 <= 0)
      {
        global_ERROR_flag = true;
        send_alert("!!!FATAL_ERROR!!!_From_Chat: Заклинил двигатель или не работают концевики.");
        return (true);
      }
      return(false);
    }
};

class_motor_plus_knobs object_STREET_motor_plus_knobs(pin_step_SREET);
class_motor_plus_knobs object_HOME_motor_plus_knobs  (pin_step_HOME);



class class_motor_main
{
  public:

    class_motor_main()
    {
    }

    void doXsteps_func(int steps_amount)
    {
      if (_calibrate_ERROR == true)
      {
        global_ERROR_flag = true;
        send_alert("ERROR: Управление заслонками невозможно - заклинил двигатель или не работают концевики.");
        return;
      }

      if (steps_amount < 0)                                                                                // отрицательные значения открываем батарею, закрываем улицу //
      {
        digitalWrite(pin_DIR, dir_UP);
        while (steps_amount < 0 && _steps_GLOBAL > 0 && digitalRead(pin_knob_HIGH) == 1)                           // открываем батарею // от 0 вверх - "батарея", от 0 вниз = "улица" //
        {
          object_HOME_motor_plus_knobs.do_8_microsteps();
          _doXsteps_counter++;
          _steps_GLOBAL--;
          steps_amount++;
        }
        
        digitalWrite(pin_DIR, dir_DOWN);
        while (steps_amount < 0 && _steps_GLOBAL > street_LOWEST_position_cur && digitalRead(pin_knob_LOW) == 1)  // закрываем улицу // от 0 вверх - "батарея", от 0 вниз = "улица" //
        {
          object_STREET_motor_plus_knobs.do_8_microsteps();
          _doXsteps_counter++;
          _steps_GLOBAL--;
          steps_amount++;
        }
      }

      if (steps_amount > 0)                                                                                // положительные значения открываем улицу, закрываем батарею //
      {
        digitalWrite(pin_DIR, dir_UP);
        while (steps_amount > 0 && _steps_GLOBAL < 0 && digitalRead(pin_knob_HIGH) == 1)                           // открываем улицу // от 0 вверх - "батарея", от 0 вниз = "улица" //
        {
          object_STREET_motor_plus_knobs.do_8_microsteps();
          _doXsteps_counter++;
          _steps_GLOBAL++;
          steps_amount--;
        }

        digitalWrite(pin_DIR, dir_DOWN);
        while (steps_amount > 0 && _steps_GLOBAL < home_LOWEST_position_cur && digitalRead(pin_knob_LOW) == 1)     // закрываем батарею // от 0 вверх - "батарея", от 0 вниз = "улица" //
        {
          object_HOME_motor_plus_knobs.do_8_microsteps();
          _doXsteps_counter++;
          _steps_GLOBAL++;
          steps_amount--;
        }
      }

      if(digitalRead(pin_knob_HIGH) == 0 || digitalRead(pin_knob_LOW) == 0)                                          //Отправляем ошибку, если коснулись кнопки (не должны) //
      {
        send_alert("ERROR: Сработал концевик на заслонке.");
        calibrate_state = 1;
        global_ERROR_flag = true;
      }
    }

    void calibrate_test()                            // проверяет нужна ли калибровка и если нужна делает. так же по таймеру, раз в сутки, запускает быструю калибровыку //
    {
      #ifdef Jesse_yield_enable
        yield();
      #endif

      int buf_steps_GLOBAL = _steps_GLOBAL;                          // сохраняем положение заслонок до калибровки

      int street_motor_position_before_calibration;                  // необходимо на случай, если ESP перезагрузилась и после калибровки нужно вернуть заслонки
      int home_motor_position_before_calibration;                    // ↑↑↑
      

      if(calibrate_state == 2 && _calibrate_ERROR == false)                      // быстрая калибровка //
      {
        object_array_users[0].send_message_second_chat("Начинаю процесс быстрой калибровки.");

        if(digitalRead(pin_knob_LOW) == 1 && digitalRead(pin_knob_HIGH) == 1)
        {
          _calibrate_ERROR = object_STREET_motor_plus_knobs.go_to_HIGH_limit();
          street_motor_position_before_calibration = object_STREET_motor_plus_knobs.get_position_before_calibration();
          _calibrate_ERROR = object_HOME_motor_plus_knobs.set_LOW_limit();
          home_motor_position_before_calibration = object_HOME_motor_plus_knobs.get_position_before_calibration();

          _steps_GLOBAL = home_LOWEST_position_cur;
          object_array_users[0].send_message_second_chat("Быстрая калибровка выполнена.\n\nТекущее положение заслонок: " + String(_steps_GLOBAL));
        }
        
        else
        {
          send_alert("Быстрая калибровка невозможна. Концевик нажат.");
          calibrate_state = 1;
        }
      }

      if(calibrate_state == 1 && _calibrate_ERROR == false)                        // полноценная калибровка //
      {
        object_array_users[0].send_message_second_chat("Начинаю процесс полноценной калибровки.");

        for(int i = 0; i < 2; i++)                                             // Запускаем по два раза, чтобы отступить KNOB_space*2 //
        {
          if(_calibrate_ERROR == false)
          {
            _calibrate_ERROR = object_STREET_motor_plus_knobs.go_to_HIGH_limit();
          } 
        }

        for(int i = 0; i < 2; i++)                                             // Запускаем по два раза, чтобы отступить KNOB_space*2 //
        {
          if (_calibrate_ERROR == false)
          {
            _calibrate_ERROR = object_HOME_motor_plus_knobs.go_to_HIGH_limit();
          } 
        }

        if (_calibrate_ERROR == false)
        {
          _calibrate_ERROR = object_STREET_motor_plus_knobs.go_to_HIGH_limit();  // Производим калибровку второй раз на случай, если была зажата противоположенный концевик! //
          object_array_users[0].send_message_second_chat("Верхнее положение заслонки с улицы откалибровано.");
        }

        if (_calibrate_ERROR == false)
        {
          _calibrate_ERROR = object_HOME_motor_plus_knobs.go_to_HIGH_limit();    // Производим калибровку второй раз на случай, если была зажата противоположенный концевик! //
          object_array_users[0].send_message_second_chat("Верхнее положение заслонки от батареи откалибровано.");
        }
        

        if (_calibrate_ERROR == false)                                              // калибровка нижнего положения STREET //
        {
          _calibrate_ERROR = object_STREET_motor_plus_knobs.set_LOW_limit();

          street_LOWEST_position_cur = object_STREET_motor_plus_knobs.get_LOWEST_position();

          int cal_dif1 = street_LOWEST_position_const - street_LOWEST_position_cur;
          object_array_users[0].send_message_second_chat("Нижнее положожение заслонки с улицы откалибровано. Результат: " + String(street_LOWEST_position_cur) + "\n\nОтклонение от константного значения: " + String(cal_dif1));

          _calibrate_ERROR = object_STREET_motor_plus_knobs.go_to_HIGH_limit();          // открываем заслонку обратно //

          object_array_users[0].send_message_second_chat("Положение заслонки с улицы возвращено в открытое положение. Текущее положение _steps_GLOBAL: " + String(_steps_GLOBAL));
        }

        if (_calibrate_ERROR == false)                                              // калибровка нижнего положения HOME //
        {
          _calibrate_ERROR = object_HOME_motor_plus_knobs.set_LOW_limit();

          home_LOWEST_position_cur = object_HOME_motor_plus_knobs.get_LOWEST_position();

          _steps_GLOBAL = home_LOWEST_position_cur;

          int cal_dif2 = home_LOWEST_position_const - home_LOWEST_position_cur;
          object_array_users[0].send_message_second_chat("Нижнее положожение заслонки от батареи откалибровано. Результат: " + String(home_LOWEST_position_cur) + "\n\nОтклонение от константного значения: " + String(cal_dif2));
        }
      }

      #ifdef Jesse_yield_enable
        yield();
      #endif

      if (calibrate_state == 1 || calibrate_state == 2)                        // завершающий процесс любой калибровки //
      {
        if (_calibrate_ERROR == false)                                              // если нет ошибок - возвращаем положение заслонок в положение до калибровки //
        {
          int buf_steps_amount = 0;

          if(buf_steps_GLOBAL - _steps_GLOBAL != 0)                            // _steps_GLOBAL равно 0 при перезагрузке
          {
            buf_steps_amount = buf_steps_GLOBAL - _steps_GLOBAL;               
          }
          
          else if(street_motor_position_before_calibration > 100)             // если _steps_GLOBAL равно 0, значит либо заслонки в нулевом положение, либо была перезагрузка... проверяем какая заслонка была до перезагрузки в другом положение и была ли вообще...
          {
            buf_steps_amount = 0 - street_motor_position_before_calibration - object_HOME_motor_plus_knobs.get_LOWEST_position();
          }

          else if(home_motor_position_before_calibration > 100)                // ↑↑↑
          {
            buf_steps_amount = 0 - home_motor_position_before_calibration;
          }

          _doXsteps_counter = _doXsteps_counter + buf_steps_amount;              // вычитаем количество шагов, чтобы не учитывать их в количестве шагов за день //
          doXsteps_func(buf_steps_amount);
          calibrate_state = 0;
          object_array_users[0].send_message_second_chat("Положение заслонок возвращено в положение до калибровки. Текущее положение _steps_GLOBAL: " + String(_steps_GLOBAL));
        }
        if (_calibrate_ERROR == true)                                              // если есть ошибки - отправляем уведомление о невозможности калибровки //
        {
          global_ERROR_flag = true;
          send_alert("ERROR: Калибровка невозможна - заклинил двигатель или не работают концевики.");
        }
      }

      if (_daily_calibrate_flag == true && object_TimeDate.get_TimeB() > 140101 && object_TimeDate.get_TimeB() < 142929)      // автоматическая калибровка раз в сутки в 14:01 //
      {
        calibrate_state = 2;
        _daily_calibrate_flag = false;
        object_array_users[0].send_message_second_chat("Отправил запрос на быструю калибровку по таймеру (раз в сутки в период с 14:00 до 14:30).");
      }

      #ifdef Jesse_yield_enable
        yield();
      #endif

      if (_daily_calibrate_flag == false)                                                                // поднимаем флаг калибровки //
      {
        if (object_TimeDate.get_TimeB() < 135959 || object_TimeDate.get_TimeB() > 143131)
        {
          _daily_calibrate_flag = true;
        }
      }

      if (_step_counter_Flag == 1 && object_TimeDate.get_TimeB() > 234500)   // отправка отчета по количеству пройденых шагов за день //
      {
        _step_counter_Flag = 0;
        object_array_users[0].send_message_second_chat("Шагов сделано за сегодня (без учета калибровки): " + String(_doXsteps_counter));
        _doXsteps_counter = 0;
      }

      #ifdef Jesse_yield_enable
        yield();
      #endif

      if (_step_counter_Flag == 0 && object_TimeDate.get_TimeB() < 234400)   // возвращает влаг обратно, чтобы отчет отправился на следующий день //
      {
        _step_counter_Flag = 1;
      }
    }

    String getMotorPositions()                         // создает стринг с текстовым описанием положения заслонок как в абсолютном значении, так и в процентах //
    {
      int position_street = 0;
      int position_home = 0;

      if (_steps_GLOBAL < 0) {
        position_street = map(_steps_GLOBAL, 0, street_LOWEST_position_cur, 0, 100);
      }
      if (_steps_GLOBAL > 0) {
        position_home = map(_steps_GLOBAL, 0, home_LOWEST_position_cur, 0, 100);
      }

      String Message_Motor_Positions = "\n\nКрайнее нижнее положение заслонки от батареи: " + String(home_LOWEST_position_const) +\
                                      "\nТекущее положение заслонок(_steps_GLOBAL*): " + String(_steps_GLOBAL) +\
                                      "\nКрайнее нижнее положение заслонки с улицы: " + String(street_LOWEST_position_cur) +\
                                      "\n\n*Заслонка с улицы закрыта на " + String(position_street) +\
                                      "%.\n*Заслонка от батареи закрыта на " + String(position_home) +\
                                      "%.\n\n**Шагов сделано за сегодня (без учета калибровки): " + String(_doXsteps_counter);

      return (Message_Motor_Positions);
    }

    int get_steps_GLOBAL()
    {
      return(_steps_GLOBAL);
    }

    long get_doXsteps_counter()
    {
      return(_steps_GLOBAL);
    }
  private:
    bool _calibrate_ERROR;
    bool _daily_calibrate_flag = false;               // флаг для таймера автоматической калибровки раз в сутки // стоит false, чтобы если включить ESP с 14 до 19 - не сработало две калибровки подряд (1 раз при включени и еще одна по таймеру) //
    bool _step_counter_Flag = 1;                      // флаг таймера для отправки отчета по количеству пройденных шагов за сутки // Нужно, чтобы быстро понять, не делают ли заслонки лишних движений летом //
    long _doXsteps_counter = 0;                       // счетчик количества шагов сделанных за день //
    int _steps_GLOBAL = 3188;                         // одна переменная для определения положения ОБЕИХ заслонок // _steps_GLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
};

class_motor_main object_motor_main;


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
      #ifdef Jesse_yield_enable
        yield();
      #endif

      sensors_event_t humidity, temp;                // запрашиваем температуру и влажность адафрут //
      sht4.getEvent(&humidity, &temp);               // обновляем показания температуры и влажность //
      _temp = temp.temperature;
      _humidity = humidity.relative_humidity;

      #ifdef Jesse_yield_enable
        yield();
      #endif

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
        global_ERROR_flag = true;
      }

      if ((_humidity_low_alert > int(_humidity)) || (int(_humidity) > _humidity_high_alert))  // Если датчик SHT41 отключен от питание значение будет 0 //
      {
        send_alert("ERROR Влажность в комнате: " + String(_humidity) + "%");
        global_ERROR_flag = true;
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
      _stop_periodioc_mesuarement();
      _set_altitude();
      _start_periodioc_mesuarement();
    }

    void recalibration()
    {
      object_array_users[0].send_message("Начинаю процесс принудительной рекалибровки. Это займет 5 минут.");

      _stop_periodioc_mesuarement();
      _set_altitude();
      _start_periodioc_mesuarement();

      delay(1000*60*5);                      // минимум 3 минуты датчик должен замерять СО2 //

      _stop_periodioc_mesuarement();
      delay(500);                            // необходимая задержка перед рекалибровкой //    
      _forced_recalibration();
      _start_periodioc_mesuarement();
    }

    void factory_reset()
    {
      _stop_periodioc_mesuarement();
      _factory_reset();
      _set_altitude();
      _start_periodioc_mesuarement();   
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

        #ifdef Jesse_yield_enable
          yield();
        #endif

        error_co2 = scd4x.readMeasurement(_CO2, buf_temperature_co2, buf_humidity_co2);
        if (error_co2)
        {
          send_alert("ERROR: не смог получить показания со2.");
          _CO2 = 0;
          global_ERROR_flag = true;
        }

        #ifdef Jesse_yield_enable
          yield();
        #endif

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
        global_ERROR_flag = true;
      }
    }

    void _check_alerts()
    {
      if (_CO2 > _co2_high_alert)
      {
        send_alert("ALERT уровень со2: " + String(_CO2) + "ppm");
      }
    }

    void _stop_periodioc_mesuarement()
    {
      uint16_t error_co2;
      error_co2 = scd4x.stopPeriodicMeasurement();
      if (error_co2)
      {
        send_alert("ERROR: Не смог остановить переодическое измерение датчика СО2.");
        global_ERROR_flag = true;
      }
      else
      {
        object_array_users[0].send_message("ОСТАНОВИЛ периодическое измерение в датчике СО2.");
      }
    }

    void _set_altitude()
    {
      uint16_t error_co2;
      error_co2 = scd4x.setSensorAltitude(_altitude);
      if (error_co2)
      {
        send_alert("ERROR: Не смог установить Altitude (высоту над уровнем моря) в датчике со2.");
        global_ERROR_flag = true;
      }
      else
      {
        object_array_users[0].send_message("Выставил ALTITUDE (высоту над уровнем моря): " + String(_altitude) + "метров.");
      } 
    }

    void _start_periodioc_mesuarement()
    {
      uint16_t error_co2;
      error_co2 = scd4x.startPeriodicMeasurement();          // startLowPowerPeriodicMeasurement - данные обновляются раз в 30сек, startPeriodicMeasurement - данные обновляются раз в 5 секунд // SingleShot был изначально - на обоих датчиках проблема с повторяемостью показаний в +-50... // 
      if (error_co2)
      {
        send_alert("ERROR: Не смог запустить переодическое измерение датчика со2.");
        global_ERROR_flag = true;
      }
      else
      {
        object_array_users[0].send_message("ЗАПУСТИЛ периодическое измерение в датчике СО2.");
      }      
    }

    void _forced_recalibration()
    {
      uint16_t error_co2;
      uint16_t targetCo2Concentration = 400;
      uint16_t frcCorrection = 0x0000;

      error_co2 = scd4x.performForcedRecalibration(targetCo2Concentration, frcCorrection);
      if (error_co2)
      {
        send_alert("ERROR: Не смог произвести принудительную рекалибровку.");
        global_ERROR_flag = true;
      }
      else if (frcCorrection == 0xFFFF)                               // 0xFFFF = 65535 //
      {
        send_alert("ERROR: Ошибка в процессе рекалибровки.");
        global_ERROR_flag = true;        
      }
      else
      {
        uint16_t Correction_in_ppm = frcCorrection - 0x8000;                 // 0x8000 = 32768 //
        object_array_users[0].send_message("Рекалибровка прошла успешно. \n\nРезультаты:\n Текущее значение СО2: " + String(targetCo2Concentration) + "ppm.\n Коррекция: " + String(Correction_in_ppm) + "ppm." + "\n\n frcCorrection: " + String(frcCorrection));
      }
    }

    void _factory_reset()
    {
      uint16_t error_co2;

      error_co2 = scd4x.performFactoryReset();
      if (error_co2)
      {
        send_alert("ERROR: Не смог произвести сброс на заводские настройки.");
        global_ERROR_flag = true;
      }
      else
      {
        object_array_users[0].send_message("Сброс на заводские настройки прошёл успешно.");
      }
    }
};

SCD41 object_CO2_sensor;                         


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
  Serial.begin(9600);                            // запускаем Serial Port и определяем его скорость //
  Serial.setTimeout(200);                        // таймаут для .readString (ждет заданное значение на чтение Serial)

  WiFi.setOutputPower(15.00);                    // "When values higher than 19.25 are set, the board resets every time a connection with the AP is established." // https://stackoverflow.com/questions/75712199/esp8266-watchdog-reset-when-using-wifi // 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);          // подключаемся к Wi-Fi //

  object_TimeDate.set_UTC_time();

  pinMode(pin_knob_LOW, INPUT_PULLUP);             // определяем пины // нижние концевики //
  pinMode(pin_knob_HIGH, INPUT_PULLUP);            // ↑↑↑ // верхние концевики //

  pinMode(pin_step_SREET, OUTPUT);                  // ↑↑↑ // шаговый двигатель заслонки с улицы //
  pinMode(pin_step_HOME, OUTPUT);                    // ↑↑↑ // шаговый двигатель заслонки от батареи //
  pinMode(pin_DIR, OUTPUT);                          // ↑↑↑ // общий пин direction (направление вращения двигателей) //

  pinMode(relay, OUTPUT);                        // определяем пин реле, как output //
  digitalWrite(relay, LOW);                      // на всякий случай выключаем реле при запуске //

  ds.begin();                                    // инициализация датчиков температуры по шине OneWire //

  if (!sht4.begin())                             // инициализация адафрут датчика по шине i2c //
  {
    send_alert("ERROR: Ошибка инициализации датчика температуры и влажности.");
    global_ERROR_flag = true;
  }
  sht4.setPrecision(SHT4X_HIGH_PRECISION);       // Какие-то настройки в адафрут библиотеке отвечающие за точность //
  sht4.setHeater(SHT4X_NO_HEATER);               // Какие-то настройки в адафрут библиотеке отвечающие за "прогрев". Видимо, это необходимо при использование датчика в экстремальных температурах, хз. По умолчанию выключено. //

  scd4x.begin(Wire);                             // инициализация датчика СО2 по шине i2c //

  calibrate_state = 2;
  object_motor_main.calibrate_test();
  
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

  object_TimeDate.update_TimeDate();                                      // обновляем текущее время //

  if (bot1.getUpdates(bot1.last_message_received + 1) != 0)               // если есть новые сообщения обрабатываем одно //
  {
    Message_from_Telegram_converter();
  }

  if(object_TimeDate.get_MIN() % 2 > 0 && flag_every_minute_timer == false)              // таймер каждую нечетную минуту //
  {
    #ifdef Jesse_yield_enable
      yield();
    #endif    

    update_sensors_data_and_calculations();                      
    SYNCstart();

    #ifdef Jesse_yield_enable
      yield();
    #endif

    humidifier();
    thermostat();

    flag_every_minute_timer = true;
  }

  if(object_TimeDate.get_MIN() % 2 == 0 && flag_every_minute_timer == true)              // таймер каждую четную минуту //
  {
    object_motor_main.calibrate_test();                                                  // проверяет нужна ли калибровка //

    restart_check();
    flag_every_minute_timer = false;
  }
}

void Message_from_Telegram_converter()           // преобразование сообщение из Телеграм в команду //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  String CHAT_IDcur = bot1.messages[0].chat_id;
  message_id_check(CHAT_IDcur);

  #ifdef Jesse_yield_enable
    yield();
  #endif
    
  if (message_intruder_flag == false)
  {
    String text = bot1.messages[0].text;

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
  #ifdef Jesse_yield_enable
    yield();
  #endif

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
    #ifdef Jesse_yield_enable
      yield();
    #endif

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
        float calculation_buf = (home_LOWEST_position_cur - street_LOWEST_position_cur) / Step_Per_loop;
        int calculation_buf2 = int(calculation_buf) * 2;

        String buf = "Шаг изменения положения заслонок: " + String(Step_Per_loop) +\
                     "\n\n*Термостат запускается раз в две минуты и при отклонение от заданной температуры меняет положение заслонок. \n\n*Текущее значение количества шагов значит, что обе заслонки могут поменять своё положение на противоположенное за " + String(calculation_buf2) + " минут(ы).";
        object_array_users[users_array_index].send_message(buf);
        break;  
      }

      case 108:                                // ручное управление заслонками //
      {
        int buf_steps_amount = constrain(text.toInt(), -10000, 10000);
        object_array_users[users_array_index].send_message("Принял количество шагов: " + String(buf_steps_amount));

        object_motor_main.doXsteps_func(buf_steps_amount);
        object_array_users[users_array_index].send_message(object_motor_main.getMotorPositions());
        break;  
      }

      case 111:                               // ручная установка крайнего нижнего положения заслонки с улицы //
      {
        street_LOWEST_position_cur = constrain(text.toInt(), street_LOWEST_position_const, 0);
        if (object_motor_main.get_steps_GLOBAL() < street_LOWEST_position_cur)
        {
          object_motor_main.doXsteps_func(street_LOWEST_position_cur - object_motor_main.get_steps_GLOBAL());
        }
        object_array_users[users_array_index].send_message("Установлено крайнее положение заслонки с улицы: " + String(street_LOWEST_position_cur));
        break;  
      }

      case 112:                               // ручная установка крайнего нижнего положения заслонки от батареи //
      {
        home_LOWEST_position_cur = constrain(text.toInt(), 0, home_LOWEST_position_const);
        if (object_motor_main.get_steps_GLOBAL() > home_LOWEST_position_cur)
        {
          object_motor_main.doXsteps_func(home_LOWEST_position_cur - object_motor_main.get_steps_GLOBAL());
        }
        object_array_users[users_array_index].send_message("Установлено крайнее положение заслонки от батареи: " + String(home_LOWEST_position_cur));
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

  else                                       // если MessageState == 1, то значит ожидаем команду //
  {
    #ifdef Jesse_yield_enable
      yield();
    #endif

    byte dividerIndex = text.indexOf('/');   // ищем индекс разделителя "/" //
    text = text.substring(dividerIndex + 1); // оставляем только команду "1" //
    int text_int = text.toInt();

    switch (text_int)                        // 1** команды доступные всем, 3** команды требующие доступ администратора //
    {
      case 101:                              // текущие температуры в текстовом виде //
      {
        update_sensors_data_and_calculations();
        object_array_users[users_array_index].send_message(TEMP_text_output() + "\n\n\n\n↓↓↓ЗАСЛОНКИ↓↓↓" + object_motor_main.getMotorPositions());
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
        float calculation_buf = (home_LOWEST_position_cur - street_LOWEST_position_cur) / Step_Per_loop;
        int calculation_buf2 = int(calculation_buf) * 2;

        String buf = "Отправьте сообщение для установки шага изменения положения заслонок (от 10 до 1000):\n\nТекущее значение: " + String(Step_Per_loop) +\
                     "\n\n*Термостат запускается раз в две минуты и при отклонение от заданной температуры меняет положение заслонок. \n\n*Текущее значение количество шагов значит, что обе заслонки могут поменять своё положение на противоположенное за " + String(calculation_buf2) + " минут(ы).";
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(107);
        break;
      }

      case 108:                              // ручное управление заслонками //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение с количеством шагов от -10000 до 10000:" + object_motor_main.getMotorPositions());
        object_array_users[users_array_index].set_message_state(108);
        break;
      }

      case 109:                              // ручная инициализация быстрой калибровки заслонок //
      {
        calibrate_state = 2;
        object_array_users[users_array_index].send_message("Принял запрос на быструю калиброку.\n\n*Быстрая калибровка не сбрасывает крайние положения заслонок выставленные вручную.");
        object_motor_main.calibrate_test();
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
          object_array_users[users_array_index].send_message("Принял запрос на полноценную калибровку.");
          object_motor_main.calibrate_test();
        }
        else
        {
          object_array_users[users_array_index].send_message("Недостаточно прав доступа для того, чтобы отправить меня в другой чат.");
        }
        break;
      }

      case 111:                              // ручная установка крайнего нижнего положения заслонки с улицы //
      {
        String buf = "Отправьте сообщение для установки крайнего нижнего положения заслонки с улицы (во избежание ошибок его невозможно выставить меньше, чем значение после калибровки).\n\nЗначение крайнего нижнего положения заслонки с улицы после калибровки: " + String(street_LOWEST_position_const) +\
                     "\nЗначение выставленное вручную: " + String(street_LOWEST_position_cur);
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(111);
        break;
      }

      case 112:                              // ручная установка крайнего нижнего положения заслонки от батареи //
      {
        String buf = "Отправьте сообщение для установки крайнего нижнего положения заслонки от батареи (во избежание ошибок его невозможно выставить меньше, чем значение после калибровки).\n\nЗначение крайнего нижнего положения заслонки от батареи после калибровки: " + String(home_LOWEST_position_const) +\
                     "\nЗначение выставленное вручную: " + String(home_LOWEST_position_cur);
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

      case 352:
      {
        object_array_users[users_array_index].send_message("Рекалибровка датчика СО2 нужна в случае, если даже при открытых окнах он не показывает значение около 400 и нужно его рекалибровать.\n\nПри рекалибровке необходимо максимально проветрить помещение, чтобы воздух был близок к уличному.\n\n\nПродолжить? - /35201@JOArduinoChatBOT");
        break;      
      }

      case 35201:
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_CO2_sensor.recalibration();
        }

        else
        {
          object_array_users[users_array_index].send_message("Недостаточно прав доступа.");
        }
        break;
      }

      case 353:
      {
        object_array_users[users_array_index].send_message("Сброс на заводские настройки сбрасывает все настройки, в том числе параметры калбировки датчика.\n\n\nПродолжить? - /35301@JOArduinoChatBOT");
        break;      
      }

      case 35301:
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_CO2_sensor.factory_reset();
        }

        else
        {
          object_array_users[users_array_index].send_message("Недостаточно прав доступа.");
        }
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
  #ifdef Jesse_yield_enable
    yield();
  #endif

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
  #ifdef Jesse_yield_enable
    yield();
  #endif

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
  #ifdef Jesse_yield_enable
    yield();
  #endif

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
  #ifdef Jesse_yield_enable
    yield();
  #endif
  
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
  #ifdef Jesse_yield_enable
    yield();
  #endif

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
  #ifdef Jesse_yield_enable
    yield();
  #endif

  if ((object_ds18b20_0.get_temp() < TempMain - TempRange) && (b61 > AirLowLimit + 5))
  {       
    object_motor_main.doXsteps_func(0 - Step_Per_loop);            // отрицательные значения открываем батарею, закрываем улицу //
  }

  if ((object_ds18b20_0.get_temp() > TempMain + TempRange) || (b61 < AirLowLimit - 5))
  {             
    object_motor_main.doXsteps_func(Step_Per_loop);                // положительные значения открываем улицу, закрываем батарею //
  }
}

void SYNCstart()                                 // Отправка температур и проверка времени на синхронизацию //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  Serial.print(object_TimeDate.get_UTC());
  Serial.print(";");
  Serial.print(TEMP_excel_output());
  Serial.print(";");
  Serial.print(global_ERROR_flag);

  global_ERROR_flag = false;

  String SYNCmessage = Serial.readString();

  if (SYNCmessage.length() > 2)                  // Проверка что вообще что-то пришло //
  {
    #ifdef Jesse_yield_enable
      yield();
    #endif

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
  #ifdef Jesse_yield_enable
    yield();
  #endif

  int air_exchange_caclulated = 0;
  if(object_CO2_sensor.get_CO2() > 410)                                             // чтобы исключить значение на 0 и не показывать нереалистичные значения //
  {
    air_exchange_caclulated = 18000 / (object_CO2_sensor.get_CO2() - 400);          // этот коэффициент взят из таблички расчета со2 и актуален для одного человека находящегося в покое //
  }

  #ifdef Jesse_yield_enable
    yield();
  #endif

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
                    "°C\nОбъeдинeнный поток: " + String(object_ds18b20_6.get_temp()) +\
                    "°C\n\nTeплoпoтepи от кровати до рекуп. и нагрев вентилятором: " + String(b82) +\
                    "°C\nTeплoпoтepи от улицы до рекуператора: " + String(b62) +\
                    "°C\n\n\n\n↓↓↓ТОЛЬКО ДЛЯ ХОЛОДОВ↓↓↓" + "\n\nBoздyxa c улицы: " + String(b61) +\
                    "%\nBoздyxa c батареи: " + String(b71) + "%");

  #ifdef Jesse_yield_enable
    yield();
  #endif

  return(Message);
}

String TEMP_excel_output()                         // Команда вывода данных для записи на SD карту и формирование лога для экселя //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

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
             String(object_motor_main.get_steps_GLOBAL()) + "," + String(object_motor_main.get_doXsteps_counter()));
  return(Message);
}
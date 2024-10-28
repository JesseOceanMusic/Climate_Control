#ifdef Jesse_DEBUG_free_heap
  time_t Jesse_debug_free_heap_timer;
#endif

#ifdef Jesse_DEBUG_loop_millis_measure
  long test_timer;
#endif

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

X509List cert(TELEGRAM_CERTIFICATE_ROOT);                                      // какой-то сертификат //
WiFiClientSecure secured_client;                                               // какой-то secured client //

#ifdef THIS_IS_CHAT_CODE
  UniversalTelegramBot bot_main(BOT_TOKEN1, secured_client);                   // BOT "Chat" //
  UniversalTelegramBot bot_second(BOT_TOKEN3, secured_client);                 // BOT "Calibrate" //
  const String Opposite_bot_name = "JOArduinoLogsBOT";
#endif

#ifdef THIS_IS_LOGGER_CODE
  UniversalTelegramBot bot_main(BOT_TOKEN2, secured_client);                   // BOT "Logs + LED" //
  UniversalTelegramBot bot_second(BOT_TOKEN4, secured_client);                 // BOT "Backup" //
  const String Opposite_bot_name = "JOArduinoChatBOT";
#endif

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

    bool check_id(String CHAT_IDcur);            // прототип метода, сам метод описан ниже. Необходимо, поскольку метод обращается к объекту, который еще не создан. //

    void send_message(String input);             // прототип метода, сам метод описан ниже. Необходимо, поскольку метод обращается к объекту, который еще не создан. //

    void send_message_second_chat(String input)
    {
      #ifdef Jesse_yield_enable
        yield();
      #endif

      bot_second.sendMessage(_id, input, "");
    }

    void send_alert(String input)
    {
      #ifdef Jesse_yield_enable
        yield();
      #endif

      if (_alert_flag == true)
      {
        bot_main.sendMessage(_id, input, "");
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
        bot_main.sendMessage(_id, "Текстовые уведомления ВКЛЮЧЕНЫ.", "");
      }
      else
      {
        bot_main.sendMessage(_id, "Текстовые уведомления ОТКЛЮЧЕНЫ.", "");
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
  class_users(2, USER_ID2_debug,         false, false, false, "Debug"),                              // Группа для дебаг-сообщений //  
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

  bot_main.sendMessage(_id, input, "");
  if(_need_supervision == true)
  {
    object_array_users[0].send_message(String("Ответил, ") + _name + ":\n\n" + input);
  }
}

bool class_users::check_id(String CHAT_IDcur)
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  if(CHAT_IDcur == _id)
  {
    users_array_index = _users_array_index;
   
    if(_need_supervision == true)
    {
      object_array_users[0].send_message(_name + ", написал:\n\n" + bot_main.messages[0].text);
    }

    return(true);
  }

  return(false);
}

void send_alert(String input_message)            // функция для отправки уведомлений пользователям, которые на них подписаны //
{
  for(int i = 0; i < user_array_length; i++)
  {
    object_array_users[i].send_alert(input_message);
  }
}

bool Message_is_it_known_user()                // Проверка входящего сообщения от пользователя на права доступа //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  String CHAT_IDcur = bot_main.messages[0].chat_id;

  if(object_array_users[0].check_id(CHAT_IDcur) == true)        // вначале проверяем мой айди //
  {
    return(true);
  }

  if(shutdown_friends == false)                                 // если не отключена возможность других пользователей общаться с ботом - проверяем их на совпадение //
  {
    for(int i = 1; i < user_array_length; i++)
    {
      if(object_array_users[i].check_id(CHAT_IDcur) == true)
      {
        return(true);
      }
    }
  }

  send_alert("INTRUDER: " + CHAT_IDcur);
  return(false);
}

void Message_command_get_data(String text);      // прототип функции //

void Message_command_send_data(int text_int);     // прототип функции //

bool Message_is_it_back_command(String text)     // проверяет является ли текст пришедший из телеграма "/back" //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  if (text == "/back")                           // отмена ввода //
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

    return(true);
  }
  return(false);
}

void Message_from_Telegram_converter()           // преобразование сообщение из Телеграм в команду //
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  if (Message_is_it_known_user() == true)
  {
    String income_message = bot_main.messages[0].text;

    byte dividerIndex_1 = income_message.indexOf('@');                              // ищем индекс разделителя @ // для того, чтобы работали команды из группы по запросу типа "/back@JOArduinoChatBOT" //
    String message_part_2 = income_message.substring(dividerIndex_1 + 1);           // записывает в message_part_2 "JOArduinoChatBOT" //
    String message_part_1 = income_message.substring(0, dividerIndex_1);            // записывает в message_part_1 "/back" //

    if (message_part_2 != Opposite_bot_name)                                        // если сообщение не для него, то пропускает его //
    {
      if(Message_is_it_back_command(message_part_1) == false)
      {
        if (object_array_users[users_array_index].get_message_state() != 1)         // если MessageState != 1, то значит ожидаем ввод данных //
        {
          Message_command_get_data(message_part_1);
        }

        else                                                                        // если MessageState == 1, то значит ожидаем команду //
        {
          byte dividerIndex_2 = message_part_1.indexOf('/');                        // ищем индекс разделителя "/" //
          String buf_message_part_1 = message_part_1.substring(dividerIndex_2 + 1); // оставляем только команду "back" //
          int message_int = buf_message_part_1.toInt();

          Message_command_send_data(message_int);
        }
      }
    }
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


///↓↓↓ Общие функции ↓↓↓///


bool esp_restart_flag = false;

void restart_check()
{
  #ifdef Jesse_yield_enable
    yield();
  #endif

  static bool skip_one_iteration = true;         // static - глобальная переменная с зоной видимости внутри функции // если не пропустить один цикл, то не удаляет прочитанную команду в телеграме и каждый раз включаясь получает команду перезагрузки по кругу //

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

void send_reset_info()                           // отчёт о причине перезагрузки //
{
  String Jesse_reset_reason = ESP.getResetReason();
  String Jesse_reset_info = ESP.getResetInfo();
  String buf_message = "Причина перезагрузки: ";
  buf_message += Jesse_reset_reason;
  buf_message += "\nДополнительная информация: ";
  buf_message += Jesse_reset_info;
  send_alert(buf_message);
}
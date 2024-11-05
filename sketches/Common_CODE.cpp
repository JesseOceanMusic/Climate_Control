/// ↓↓↓ Телеграм

  #include <FastBot2.h>
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


  #ifdef THIS_IS_CHAT_CODE
    FastBot2 bot_main;                             // BOT "Chat" //
    #define bot_main_Token   BOT_TOKEN1
    FastBot2 bot_second;                           // BOT "Calibrate" //
    #define bot_second_Token BOT_TOKEN3

    const String This_bot_name     = "JOArduinoChatBOT";
    const String Opposite_bot_name = "JOArduinoLogsBOT";
  #endif

  #ifdef THIS_IS_LOGGER_CODE
    FastBot2 bot_main;                             // BOT "Logs + LED" //
    #define bot_main_Token BOT_TOKEN2
    FastBot2 bot_second;                           // BOT "Backup" //
    #define bot_second_Token BOT_TOKEN4

    const String This_bot_name     = "JOArduinoLogsBOT";
    const String Opposite_bot_name = "JOArduinoChatBOT";
  #endif

  void jesse_yield_func()
  {
    #ifdef Jesse_yield_enable
      yield();
    #endif
  }

  bool message_intruder_flag = true;
  byte users_array_index;
  bool shutdown_friends = false;
  const byte user_array_length = 7;

  String buf_alert_message;

  const String memes_dir = "https://raw.githubusercontent.com/JesseOceanMusic/Climate_Control/refs/heads/main/memes/";
  const byte memes_amount = 9;
  const String memes_array[memes_amount] =
  {
    "01.jpg",
    "02.jpg",
    "03.jpg",
    "04.jpg",
    "05.jpg",
    "06.jpg",
    "07.jpg",
    "08.jpg",
    "09.jpg",
  };

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

      bool check_id(String CHAT_IDcur, String income_message);         // прототип метода, сам метод описан ниже. Необходимо, поскольку метод обращается к объекту, который еще не создан. //

      void send_message(String input);                                 // прототип метода, сам метод описан ниже. Необходимо, поскольку метод обращается к объекту, который еще не создан. //

      void send_message_second_chat(String input)
      {
        jesse_yield_func();

        bot_second.sendMessage(fb::Message(input, _id));
      }

      void send_alert(String input)
      {
        jesse_yield_func();

        if (_alert_flag == true)
        {
          bot_main.sendMessage(fb::Message(input, _id));
        }
      }

      void send_alert_from_buf_alert_message()
      {
        jesse_yield_func();

        if (_alert_flag == true)
        {
          bot_main.sendMessage(fb::Message(buf_alert_message, _id));
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
          bot_main.sendMessage(fb::Message("Текстовые уведомления ВКЛЮЧЕНЫ.", _id));
        }
        else
        {
          bot_main.sendMessage(fb::Message("Текстовые уведомления ОТКЛЮЧЕНЫ.", _id));
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
        if(_admin_flag == false) 
        {
          byte meme_index = random(0,   (memes_amount + 1));
            //                     min: нижняя граница случайных значений, включительно. (опционально)
            //                           max: верхняя граница случайных значений, не включительно.

          String buf_URL = memes_dir + memes_array[meme_index];

          String meme_name = "This_Is_Jesse_O_Meme_" + memes_array[meme_index];
          fb::File f(meme_name, fb::File::Type::photo, buf_URL);
          f.chatID = _id;
          delay(20);
          bot_main.sendFile(f);

          delay(20);
          this->send_message("Недостаточно прав доступа.");  // this-> обращение к вызванному объекту(экземпляру класса) //
        }
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
    jesse_yield_func();

    bot_main.sendMessage(fb::Message(input, _id));
    if(_need_supervision == true)
    {
      String echo_supervision = "Ответил, ";
      echo_supervision += _name;
      echo_supervision += ":\n\n";
      echo_supervision += input;
      object_array_users[0].send_message(echo_supervision);
    }  
  }

  bool class_users::check_id(String CHAT_IDcur, String income_message)
  {
    jesse_yield_func();

    if(CHAT_IDcur == _id)
    {
      users_array_index = _users_array_index;
    
      if(_need_supervision == true)
      {
        object_array_users[0].send_message(_name + ", написал:\n\n" + income_message);
      }

      return(true);
    }

    return(false);
  }

  void send_alert(String input_message)            // функция для отправки уведомлений пользователям, которые на них подписаны //
  {
    if(buf_alert_message.length() < 800)
    {
      if(buf_alert_message.length() < 2)
      {
        buf_alert_message = input_message;
      }

      else
      {
        buf_alert_message += "\n\n";
        buf_alert_message += input_message;
      }
    }

    else
    {
      for(int i = 0; i < user_array_length; i++)
      {
        object_array_users[i].send_alert("ERROR: Переполнен стринг buf_alert_message, отправка сообщения вне очереди:");
        delay(20);
        object_array_users[i].send_alert_from_buf_alert_message();
      }
      buf_alert_message = "";
    }
  }

  void is_there_an_alert_to_send(bool can_i_send_message_flag)
  {
    if(can_i_send_message_flag == true && buf_alert_message.length() > 2)
    {
      for(int i = 0; i < user_array_length; i++)
      {  
        object_array_users[i].send_alert_from_buf_alert_message();
      }
      buf_alert_message = "";
    }
  }

  bool Message_is_it_known_user(String CHAT_IDcur, String income_message)                  // Проверка входящего сообщения от пользователя на права доступа //
  {
    jesse_yield_func();

    if(object_array_users[0].check_id(CHAT_IDcur, income_message) == true)        // вначале проверяем мой айди //
    {
      return(true);
    }

    if(shutdown_friends == false)                                 // если не отключена возможность других пользователей общаться с ботом - проверяем их на совпадение //
    {
      for(int i = 1; i < user_array_length; i++)
      {
        if(object_array_users[i].check_id(CHAT_IDcur, income_message) == true)
        {
          return(true);
        }
      }
    }

    send_alert("INTRUDER ID: " + CHAT_IDcur + "\nINTRUDER MESSAGE: " + income_message);
    return(false);
  }

  void Message_command_get_data(String text);      // прототип функции //

  void Message_command_send_data(int text_int);    // прототип функции //

  bool Message_is_it_back_command(String text)     // проверяет является ли текст пришедший из телеграма "/back" //
  {
    jesse_yield_func();

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

  void Message_from_Telegram_converter(fb::Update& u)           // преобразование сообщение из Телеграм в команду //
  {
    jesse_yield_func();

    String CHAT_IDcur;
    String income_message;

    if (u.isQuery() == true)
    {
      CHAT_IDcur = u.query().message().chat().id().toString();
      income_message = u.query().data().toString();    
    }

    else
    {
      CHAT_IDcur = u.message().chat().id().toString();
      income_message = u.message().text().toString();
    }

    if (Message_is_it_known_user(CHAT_IDcur, income_message))
    {
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

  #define bot_main_poll_period 27000
                            // ↑↑   13/17/23/27/33/37 секунды, чтобы максимально редко было совпадение с моими циклами раз в 60 секунд //
                            // уведомления (send_alert) и дебаг-сообщения отправляются только между этими периодами, то есть раз в 13/17/23 секунды //
  #define bot_both_time_out 60000
          // .setTimeout это не время ожидания ответа от сервера внутри одного .tick //
          // это время ожидания ответа после окончания bot_main_poll_period //
          // то есть, если последнее сообщение было 17 секунд назад (bot_main_poll_period) и плюс прошло еще 8 секунд (bot_both_time_out) //
          // то библиотека разорвет соединение и заного его создаст. Это займет 1-2 секунды. //
          // при этом этот таймаут (bot_both_time_out) программа не будет заблокированна //
          // она всё так же будет вызывать .tick каждый луп и ждать соединения //
          // !!!не должно быть меньше 1000!!! // https://core.telegram.org/bots/api // Timeout in seconds for long polling. Defaults to 0, i.e. usual short polling. Should be positive, short polling should be used for testing purposes only. //

  void setup_telegram_bots()                       // настройки телеграм ботов //
  {
    // Main bot
      bot_main.setToken(F(bot_main_Token));                          // установить токен
      bot_main.attachUpdate(Message_from_Telegram_converter);        // подключить обработчик обновлений
      bot_main.setPollMode(fb::Poll::Long, bot_main_poll_period);
                                  //Sync   синхронный               (рекомендуемый период > 3500 мс)
                                  //Async  асинхронный              (рекомендуемый период > 3500 мс)
                                  //Long   асинхронный long polling (рекомендуемый период > 20000 мс) // Самый быстрый... //
    
      bot_main.setMemLimit(120);                                     // установить лимит памяти на ответ сервера (библиотека начнёт пропускать сообщения), умолч. 20.000
      bot_main.setLimit(1);                                          // установить лимит - кол-во сообщений в одном обновлении (умолч. 3)
      bot_main.setTimeout(bot_both_time_out);                        // установить таймаут ожидания ответа сервера (умолч. 2000 мс)

      bot_main.updates.clearAll();
      bot_main.updates.set(fb::Updates::Type::Message | fb::Updates::Type::InlineQuery | fb::Updates::Type::CallbackQuery);

    // Second bot
      bot_second.setToken(F(bot_second_Token));              // установить токен
      bot_second.setPollMode(fb::Poll::Async, bot_main_poll_period);
                                     //Sync   синхронный               (рекомендуемый период > 3500 мс)
                                     //Async  асинхронный              (рекомендуемый период > 3500 мс)
                                     //Long   асинхронный long polling (рекомендуемый период > 20000 мс) // Самый быстрый... //  
      bot_second.setTimeout(bot_both_time_out);                            // установить таймаут ожидания ответа сервера (умолч. 2000 мс)  
      bot_second.updates.clearAll();
  }

/// ↓↓↓ Debug 

  ;

  class class_debug_jesse
  {
    public:
      unsigned long loop_length_ms;
      unsigned long tick_max_length_ms;
      unsigned long send_alert_max_length_ms;
      unsigned int tick_above_900_counter;

      class_debug_jesse()
      {
      }

      void my_switch()                              // вкл/выкл дебага
      {
        _debug_enable_flag = !_debug_enable_flag;

        String buf_msg  = "Отправка дебаг-сообщений ";
               buf_msg += (_debug_enable_flag ? "включена." : "отключена.");
                          // тернарный оператор
                          // если _debug_enable_flag == true, то " включена"
                          // если _debug_enable_flag == false, то " отключена"
        object_array_users[0].send_message(buf_msg);
      }

      bool tick(bool can_i_send_message_flag)
      {
        if(_loop_max_length_ms < loop_length_ms)
        {
          _loop_max_length_ms = loop_length_ms;
        }

        if(esp_lowest_heap < ESP.getFreeHeap())
        {
          esp_lowest_heap = ESP.getFreeHeap();
        }

        _debug_loop_counter++;

        if(_debug_enable_flag == true)
        {   
          unsigned long debug_elapsed_time_in_ms = millis() - _debug_time_start;

          if (can_i_send_message_flag == true && _debug_loop_counter != 0 && debug_elapsed_time_in_ms > (debug_send_msg_period_ms))         // на всякий случай исключить деление на 0 // будет срабатывать каждые цикл с задержкой указанной в .setPollMode (раз в 20 секунд). или по таймеру debug_elapsed_time_in_ms //
          {
            unsigned int debug_elapsed_time_in_sec = debug_elapsed_time_in_ms / 1000;
            float avg_loop_length = (float)debug_elapsed_time_in_ms / (float)_debug_loop_counter;
            unsigned int loops_per_second = 0;
            if(debug_elapsed_time_in_sec != 0)
            {
              loops_per_second = _debug_loop_counter / debug_elapsed_time_in_sec;
            }
            
            String buf_message  = "Прошло времени: "              + String(debug_elapsed_time_in_sec) + " sec.\n"    +\
                                  "Количество циклов: "           + String(_debug_loop_counter)        + "\n"        +\
                                  "Циклов в секунду: "            + String(loops_per_second)          + "\n\n"       +\
                                  "max loop length: "             + String(_loop_max_length_ms)        + " ms.\n"    +\
                                  "avg loop length: "             + String(avg_loop_length)             + " ms.\n\n" +\
                                  "max .tick length: "            + String(tick_max_length_ms)        + " ms.\n"     +\
                                  "above_900 .tick: "             + String(tick_above_900_counter)    + " раз\n\n"   +\
                                  "max send_alert length: "       + String(send_alert_max_length_ms)  + " ms.\n"     +\
                                  "last debug_send_msg length: "  + String(_send_debug_msg_length_ms)  + " ms.\n\n"  +\
                                  "lowest ESP.getFreeHeap(): "    + String(esp_lowest_heap)           + " байт.";

            unsigned long local_timestamp_in_ms = millis();
            object_array_users[2].send_message(buf_message);
            _send_debug_msg_length_ms = millis() - local_timestamp_in_ms;

            start_over();
            return false;
          }
        }
        return true;
      }
    private:
      bool          _debug_enable_flag = false;
      unsigned int  _debug_loop_counter;
      unsigned int  _loop_max_length_ms;
      unsigned int  _send_debug_msg_length_ms;
      unsigned long _debug_time_start;
      size_t esp_lowest_heap;

      const unsigned int debug_send_msg_period_ms = 1000 * 60 * 10;

      void start_over()
      {
        esp_lowest_heap = 0;
        send_alert_max_length_ms = 0;
        _loop_max_length_ms = 0;
        _debug_time_start = millis();
        tick_max_length_ms = 0;
        tick_above_900_counter = 0;
        _debug_loop_counter = 0;
      }
  };

  class_debug_jesse obj_debug_jesse;  

  void bot_tick_and_call_debug()               // позволяет отправить сообщение без задержки в 1-2 секунды в режиме Long. Проблема есть только если сообщение отправляется вне обработчика входящий сообщений. // https://github.com/GyverLibs/FastBot2/blob/main/docs/3.start.md //
  {
    unsigned long local_timestamp_ms = millis();
    jesse_yield_func();

    static unsigned long millis_timer_for_tick;
    bool can_i_send_message_flag = false;
    unsigned long local_timestamp_in_ms = millis();

    if(millis() - millis_timer_for_tick > 5)
    {
      if(bot_main.tick() == true)
      {
        if(bot_main.isPolling() == false)
        {
          can_i_send_message_flag = true;
        }
      }
      millis_timer_for_tick = millis();
    }

    if(millis() - local_timestamp_in_ms > obj_debug_jesse.tick_max_length_ms)
    {
      obj_debug_jesse.tick_max_length_ms = millis() - local_timestamp_in_ms;
    }

    if (millis() - local_timestamp_in_ms > 900)
    {
      obj_debug_jesse.tick_above_900_counter++;
    }

    if(obj_debug_jesse.tick(can_i_send_message_flag) == false)
    {
      can_i_send_message_flag = false;
    }

    local_timestamp_in_ms = millis();
    is_there_an_alert_to_send(can_i_send_message_flag);
    if(millis() - local_timestamp_in_ms > obj_debug_jesse.send_alert_max_length_ms)
    {
      obj_debug_jesse.send_alert_max_length_ms = millis() - local_timestamp_in_ms;
    }
  }

/// ↓↓↓ Время

  class class_TimeDate                             // класс Даты и Времени //
  {
    public:
      class_TimeDate()                             // конструктор класса //
      {
        _UTC_time = 0;
      }

      void update_TimeDate()                       // обновление текущей даты и времени
      {
        jesse_yield_func();

        if(millis() - _millis_timer_for_update > 200)   // чтобы не обновлять время чаще чем раз в 200мс //
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

          _millis_timer_for_update = millis();
        }
      }

      void set_UTC_time()
      {
        configTime(10800, 0, "pool.ntp.org");            // get UTC time via NTP // не работает - "time.nist.gov" //

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

      unsigned long _millis_timer_for_update;
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

/// ↓↓↓ Restart

  bool esp_restart_flag = false;

  void restart_check()                             // реализация перезагрузки из команды в телеграме //
  {
    jesse_yield_func();

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

/// ↓↓↓ Причина перезагрузки

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


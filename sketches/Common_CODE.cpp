/// ↓↓↓ StopWatch

  class class_stopwatch_ms
  {
    public: 
      class_stopwatch_ms(String name, bool calculate_low, bool calculate_max, bool calculate_avg, bool count_above_limit, bool is_it_loop)
      {
        _name = name;
        _calculate_low_flag = calculate_low;
        _calculate_max_flag = calculate_max;
        _calculate_avg_flag = calculate_avg;
        _count_above_limit = count_above_limit;
        _is_it_loop = is_it_loop;
      }

      void start()
      {
        _last_timestamp = millis();
      }

      void stop()
      {
        f_calcilate_low();
        f_calculate_max();
        f_calculate_avg();
        f_check_high_limit();
      }

      unsigned int get_low()
      {
        return _low;
      }

      unsigned int get_max()
      {
        return _max;
      }

      float get_avg()
      {
        if(_iteretion_counter > 0)
        {
          if(_is_it_loop == true)
          {
            unsigned int elapsed_time_in_ms = millis() - _start_timestamp;
            float avg = (float)elapsed_time_in_ms / (float)_iteretion_counter;
            return avg;
          }
          else
          {
            float avg = (float)_loop_length_sum_for_avg / (float)_iteretion_counter;
            return avg;
          }
        }
        return 0;
      }

      String get_string_info()
      {
        const unsigned long elapsed_time_ms  = millis() - _start_timestamp;
        const unsigned int elapsed_time_sec  = elapsed_time_ms / 1000 % 60;
        const unsigned int elapsed_time_min  = elapsed_time_ms / 1000 / 60 % 60;
        const unsigned int elapsed_time_hour = elapsed_time_ms / 1000 / 60 / 60;
        String buf_info  = String(_name) + " " + String(elapsed_time_hour) + ":" + String(elapsed_time_min) + ":" + String(elapsed_time_sec) + "\n";

        if(_calculate_low_flag == true)
        {
          buf_info += "low: " + String(_low) + " ms.\n";
        }

        if(_calculate_avg_flag == true)
        {
          buf_info += "avg: " + String(get_avg()) + " ms.\n";
        }
        if(_count_above_limit == true)
        {
          buf_info += "\n";
          for (int i = 0; i < _high_limit_array_length; i++)
          {
            buf_info += "above" + String(_high_limit_array[i]) + ": ";
            buf_info += String(_high_limit_counter_array[i]) + "\n";
          }
          buf_info += "\n";
        }

        if(_calculate_max_flag == true)
        {
          buf_info += "max: " + String(_max) + " ms.\n\n";
        }

        return buf_info;
      }

      void reset()
      {
        _low = 999999;
        _max = 0;
        _iteretion_counter = 0;
        _loop_length_sum_for_avg = 0;
        for(int i = 0; i < _high_limit_array_length; i++)
        {
          _high_limit_counter_array [i] = 0;
        }
        _start_timestamp = millis();
      }

    private:
      String _name;
      bool _is_it_loop;

      unsigned long _start_timestamp;
      unsigned long _last_timestamp;

      bool _calculate_low_flag;
      unsigned int _low = 999999;

      bool _calculate_max_flag;
      unsigned int _max = 0;

      bool _calculate_avg_flag;
      unsigned int _iteretion_counter = 0;
      unsigned int _loop_length_sum_for_avg;

      bool _count_above_limit;
      const byte _high_limit_array_length = 13;
      const unsigned int _high_limit_array [13] = {50, 100, 150, 300, 500, 1000, 1500, 2000, 3000, 5000, 10000, 15000, 20000, };
      unsigned int _high_limit_counter_array [13];

      void f_calcilate_low()
      {
        if(_calculate_low_flag == true)
        {
          if(_low > millis() - _last_timestamp)
          {
            _low = millis() - _last_timestamp;
          }
        }
      }

      void f_calculate_max()
      {
        if(_calculate_max_flag == true)
        {
          if(_max < millis() - _last_timestamp)
          {
            _max = millis() - _last_timestamp;
          }
        }
      }

      void f_calculate_avg()
      {
        if(_calculate_avg_flag == true)
        {
          _iteretion_counter++;
          if(_is_it_loop == false)
          {
            _loop_length_sum_for_avg += (millis() - _last_timestamp);
          }
        }
      }

      void f_check_high_limit()
      {
        const unsigned int elapsed_time = millis() - _last_timestamp;
        for (int i = 0; i < _high_limit_array_length; i++)
        {
          if(elapsed_time > _high_limit_array[i])
          {
            _high_limit_counter_array[i]++;
          }
        }
      }
  };

  class_stopwatch_ms obj_stopwatch_ms_loop       ("LOOP",       false, true, true, true,  true);
  class_stopwatch_ms obj_stopwatch_ms_bot_tick   ("TICK",       false, true, false, true, false);

  #ifdef THIS_IS_LOGGER_CODE
    class_stopwatch_ms obj_stopwatch_ms_send_log ("SEND LOG",   false, true, false, true, false);
  #endif

/// ↓↓↓ Телеграм

  #include <FastBot2.h>
  #include "Sensetive_INFO.cpp"                    // логины/пароли/токены/id //

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
  const byte user_array_length = 9;

  String global_buf_alert_msg;
  String global_buf_debug_msg;

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

      void send_alert_from_global_string()
      {
        jesse_yield_func();

        if (_alert_flag == true)
        {
          bot_main.sendMessage(fb::Message(global_buf_alert_msg, _id));
        }
      }    

      void send_debug_from_global_string()
      {
        jesse_yield_func();

        bot_main.sendMessage(fb::Message(global_buf_debug_msg, _id));
        global_buf_debug_msg = "";
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
          byte meme_index = random(0, memes_amount);
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

  namespace USERS
  {
    enum
    {
      ME                = 0,
      GUEST             = 1,
      DEBUG             = 2,
      KATE_GROUP        = 3,
      KATE_PERSONAL     = 4,
      SASHA_GROUP       = 5,
      SLAVA_ARTEM_GROUP = 6,
      NOELE_GROUP       = 7,
      RUHADZE_GROUP     = 8,
    };
  }

  class_users object_array_users[user_array_length] =
  {
    //         /users_array_index        /telegram ID                 /alert /admin /supervision /name 
    class_users(USERS::ME,                USER_ID0_me_personal,        true,  true,  false,      "Андрей"),                   // Мой айди //
    class_users(USERS::GUEST,             USER_ID1_guest,              false, false, true,       "Гостевой чат"),             // гостевой юзер //  
    class_users(USERS::DEBUG,             USER_ID2_debug,              false, false, false,      "Debug"),                    // Группа для дебаг-сообщений //  
    class_users(USERS::KATE_GROUP,        USER_ID3_Kate_group,         false, true,  false,      "Катя - группа"),            // Катя - айди группы //
    class_users(USERS::KATE_PERSONAL,     USER_ID4_Kate_personal,      false, true,  true,       "Катя - личная переписка"),  // Катя - личный айди //
    class_users(USERS::SASHA_GROUP,       USER_ID5_Sasha_group,        false, false, false,      "Саша - группа"),            // Саша - айди группы //
    class_users(USERS::SLAVA_ARTEM_GROUP, USER_ID6_Slava_Artem_group,  false, false, false,      "Слава и Артем - группа"),   // Слава и Артём - айди группы //
    class_users(USERS::NOELE_GROUP,       USER_ID7_Noele_group,        false, false, false,      "Ноэль - группа"),           // Ноэль - айди группы //
    class_users(USERS::RUHADZE_GROUP,     USER_ID8_Ruhadze_group,      false, false, false,      "Рухадзе - группа"),         // Рухадзе - айди группы //
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

  void send_alert(String input_message)                    // функция для отправки уведомлений пользователям, которые на них подписаны //
  {
    if(global_buf_alert_msg.length() < 800)
    {
      if(global_buf_alert_msg.length() < 2)
      {
        global_buf_alert_msg = input_message;
      }

      else
      {
        global_buf_alert_msg += "\n\n";
        global_buf_alert_msg += input_message;
      }
    }

    else
    {
      for(int i = 0; i < user_array_length; i++)
      {
        object_array_users[i].send_alert("ERROR: Переполнен стринг global_buf_alert_msg, отправка сообщения вне очереди:");
        delay(20);
        object_array_users[i].send_alert_from_global_string();
      }
      global_buf_alert_msg = "";
    }
  }

  void is_there_an_alert_or_debug_to_send(bool can_i_send_message_flag)
  {
    if(can_i_send_message_flag == true)
    {
      if(global_buf_debug_msg.length() > 2)
      {
        object_array_users[USERS::DEBUG].send_debug_from_global_string();
      }

      else if(global_buf_alert_msg.length() > 2)
      {
        for(int i = 0; i < user_array_length; i++)
        {
          object_array_users[i].send_alert_from_global_string();
        }
        global_buf_alert_msg = "";
      }
    }
  }

  bool Message_is_it_known_user(String CHAT_IDcur, String income_message)                  // Проверка входящего сообщения от пользователя на права доступа //
  {
    jesse_yield_func();

    if(object_array_users[USERS::ME].check_id(CHAT_IDcur, income_message) == true)        // вначале проверяем мой айди //
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

  void Message_command_get_data(String text);              // прототип функции //

  void Message_command_send_data(int text_int);            // прототип функции //

  bool Message_is_it_back_command(String text)             // проверяет является ли текст пришедший из телеграма "/back" //
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

  void Message_from_Telegram_converter(fb::Update& u)      // преобразование сообщение из Телеграм в команду //
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

  #define bot_main_poll_period 23000
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

  void setup_telegram_bots()                               // настройки телеграм ботов //
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

  void check_timer_for_sending_debug_report();             // прототип функции

  void bot_tick_and_call_debug()                           // позволяет отправить сообщение без задержки в 1-2 секунды в режиме Long. Проблема есть только если сообщение отправляется вне обработчика входящий сообщений. // https://github.com/GyverLibs/FastBot2/blob/main/docs/3.start.md //
  {
    jesse_yield_func();

    static unsigned long timer_for_tick_ms;
    bool can_i_send_message_flag = false;

    obj_stopwatch_ms_bot_tick.start();
    if(millis() - timer_for_tick_ms > 5)
    {
      if(bot_main.tick() == true)
      {
        if(bot_main.isPolling() == false)
        {
          can_i_send_message_flag = true;
        }
      }
      timer_for_tick_ms = millis();
    }
    obj_stopwatch_ms_bot_tick.stop();

    check_timer_for_sending_debug_report();
    is_there_an_alert_or_debug_to_send(can_i_send_message_flag);
  }

/// ↓↓↓ Время

  class class_TimeDate                                     // класс Даты и Времени //
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

  class_TimeDate object_TimeDate;                          // создаем экземпляр класса class_TimeDate (объект) //

  bool flag_every_minute_timer = false;                    // флаг для таймера каждую четную/нечетную минуту //   

/// ↓↓↓ Restart

  bool esp_restart_flag = false;

  void restart_check()                                     // реализация перезагрузки из команды в телеграме //
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

  void send_reset_info()                                   // отчёт о причине перезагрузки //
  {
    String Jesse_reset_reason = ESP.getResetReason();
    String Jesse_reset_info = ESP.getResetInfo();
    String buf_message = "Причина перезагрузки: ";
    buf_message += Jesse_reset_reason;
    buf_message += "\nДополнительная информация: ";
    buf_message += Jesse_reset_info;
    send_alert(buf_message);
  }

/// ↓↓↓ Debug

  namespace free_heap
  {
    enum
    {
      AFTER_30_MIN,
      AFTER_60_MIN,
      AFTER_6_HOURS,
      AFTER_12_HOURS,
      AFTER_24_HOURS,
      AFTER_2_DAYS,
      AFTER_3_DAYS,
      AFTER_4_DAYS,
      END,
    }state = free_heap::AFTER_30_MIN;

    unsigned int after_30_min = 0;
    unsigned int after_60_min = 0;
    unsigned int after_6_hours = 0;
    unsigned int after_12_hours = 0;
    unsigned int after_24_hours = 0;
    unsigned int after_2_days = 0;
    unsigned int after_3_days = 0;
    unsigned int after_4_days = 0;  
    unsigned int lowest = 999999;
  }

  void heap_control()
  {
    if(free_heap::lowest > ESP.getFreeHeap())
    {
      free_heap::lowest = ESP.getFreeHeap();
    }

    switch(free_heap::state)
    {
      case free_heap::AFTER_30_MIN:
        if(millis() > 1000 * 60 * 30)
        {
          free_heap::after_30_min = ESP.getFreeHeap();
          free_heap::state = free_heap::AFTER_60_MIN;
          break;
        }
      case free_heap::AFTER_60_MIN:
        if(millis() > 1000 * 60 * 60)
        {
          free_heap::after_60_min = ESP.getFreeHeap();
          free_heap::state = free_heap::AFTER_6_HOURS;
          break;
        }
      case free_heap::AFTER_6_HOURS:
        if(millis() > 1000 * 60 * 60 * 6)
        {
          free_heap::after_6_hours = ESP.getFreeHeap();
          free_heap::state = free_heap::AFTER_12_HOURS;
          break;
        }
      case free_heap::AFTER_12_HOURS:
        if(millis() > 1000 * 60 * 60 * 12)
        {
          free_heap::after_12_hours = ESP.getFreeHeap();
          free_heap::state = free_heap::AFTER_24_HOURS;
          break;
        }
      case free_heap::AFTER_24_HOURS:
        if(millis() > 1000 * 60 * 60 * 24)
        {
          free_heap::after_24_hours = ESP.getFreeHeap();
          free_heap::state = free_heap::AFTER_2_DAYS;
          break;
        }
      case free_heap::AFTER_2_DAYS:
        if(millis() > 1000 * 60 * 60 * 24 * 2)
        {
          free_heap::after_2_days = ESP.getFreeHeap();
          free_heap::state = free_heap::AFTER_3_DAYS;
          break;
        }
      case free_heap::AFTER_3_DAYS:
        if(millis() > 1000 * 60 * 60 * 24 * 3)
        {
          free_heap::after_3_days = ESP.getFreeHeap();
          free_heap::state = free_heap::AFTER_4_DAYS;
          break;
        }
      case free_heap::AFTER_4_DAYS:
        if(millis() > 1000 * 60 * 60 * 24 * 4)
        {
          free_heap::after_4_days = ESP.getFreeHeap();
          free_heap::state = free_heap::END;
          break;
        }
      case free_heap::END:
          break;
    }
  }

  void debug_report_constructor()
  {
    global_buf_debug_msg  = obj_stopwatch_ms_loop.get_string_info();
    global_buf_debug_msg += obj_stopwatch_ms_bot_tick.get_string_info();
    #ifdef THIS_IS_LOGGER_CODE
      global_buf_debug_msg += obj_stopwatch_ms_send_log.get_string_info();
    #endif

    global_buf_debug_msg += "FREE HEAP:\n";
    global_buf_debug_msg += "after30min:"   + String(free_heap::after_30_min)   + " bytes.\n";
    global_buf_debug_msg += "after60min:"   + String(free_heap::after_60_min)   + " bytes.\n";
    global_buf_debug_msg += "after6hours:"  + String(free_heap::after_6_hours)  + " bytes.\n";
    global_buf_debug_msg += "after12hours:" + String(free_heap::after_12_hours) + " bytes.\n";
    global_buf_debug_msg += "after1day:"    + String(free_heap::after_24_hours) + " bytes.\n";
    global_buf_debug_msg += "after2days:"   + String(free_heap::after_2_days)   + " bytes.\n";
    global_buf_debug_msg += "after3days:"   + String(free_heap::after_3_days)   + " bytes.\n";
    global_buf_debug_msg += "after4days:"   + String(free_heap::after_4_days)   + " bytes.\n\n";
    global_buf_debug_msg += "lowest:"       + String(free_heap::lowest)           + " bytes.\n";
    global_buf_debug_msg += "now: "         + String(ESP.getFreeHeap())         + " bytes.";
  }

  void send_debug_repor_by_command()
  {
    debug_report_constructor();
    object_array_users[users_array_index].send_debug_from_global_string();
  }

  void check_timer_for_sending_debug_report()
  {
    static bool flag_every_day_debug_timer;
    if (flag_every_day_debug_timer == false && object_TimeDate.get_TimeB() > 234500)          // ТАЙМЕР отправки дебаг отчёта в 23:45 //
    {
      debug_report_constructor();
      flag_every_day_debug_timer = true;
    }

    if (flag_every_day_debug_timer == true && object_TimeDate.get_TimeB() < 234400)           // Возвращает флаг обратно, чтобы дебаг отправился на следующий день // 
    {    
      flag_every_day_debug_timer = false;
    }
  }
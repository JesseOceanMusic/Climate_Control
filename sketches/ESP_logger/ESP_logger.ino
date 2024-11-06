//Tools > MMU 16KB cache + 48KB IRAM and 2nd HEAP (shared)
//Tools > Stack Protection Enable
//NodeMCU 1.0 (ESP-12E Module)

#define THIS_IS_LOGGER_CODE
#define Jesse_yield_enable                       // delay(0) и yield() одно и тоже... и то и то даёт возможность ESP в эти прерывания обработать wi-fi и внутренний код // https://arduino.stackexchange.com/questions/78590/nodemcu-1-0-resets-automatically-after-sometime //
//#define FB_USE_LOG Serial

#include "A:\1 - important\PROJECTS\Arduino\!Climate_Control\! GEN 8\Gen_8_ver_009\Common_CODE.cpp"

/// ↓↓↓ SD карта

  #include <SPI.h>                                 // библиотека для SD карты //   
  #include <SD.h>                                  // ↑↑↑ //  

  String SYNCdata;                                 // стринг для полученных данных из Serial Port //

  void LOG_write_sync_data()                       // запись лога //
  {
    if (!SD.begin(4))
    {
      send_alert("ERROR: Ошибка инициализации SD карты");
    }

    jesse_yield_func();

    File myFile = SD.open(object_TimeDate.get_DateFULL() + ".txt", FILE_WRITE);

    if (myFile)
    {
      myFile.println(object_TimeDate.get_DateTimeFULL() + "," + SYNCdata);
      myFile.close();
    }

    else
    {
      send_alert("ERROR: Ошибка записи на SD карту");
    }
  }

  void LOG_send_by_timer()                        // отправка лога в 23:45 //
  {
    static bool flag_every_day_log_timer;                                                   // флаг для отправки лога раз в сутки //
    if (flag_every_day_log_timer == false && object_TimeDate.get_TimeB() > 234500)          // ТАЙМЕР отправки лога в 23:45 //
    {
      if (!SD.begin(4))
      {
        send_alert("ERROR: Ошибка инициализации SD карты");
      }

      String file_name = object_TimeDate.get_DateFULL() + ".txt";
      String buf_user_ID = object_array_users[0].get_id();
      jesse_yield_func();
      File myFile = SD.open(file_name);

      if (myFile)
      {
        fb::File f(file_name, fb::File::Type::document, myFile);
        f.chatID = buf_user_ID;

        jesse_yield_func();

        bot_second.sendFile(f, false);

        myFile.close();
      }

      else
      {
        send_alert("ERROR: Create or open .txt FAILED");
      }
      flag_every_day_log_timer = true;
    }


    if (flag_every_day_log_timer == true && object_TimeDate.get_TimeB() < 234400)           // Возвращает флаг обратно, чтобы лог отправился на следующий день // 
    {    
      flag_every_day_log_timer = false;
    }
  }

  void LOG_send_by_command()                      // отправка лога по команде //
  {
    if (!SD.begin(4))
    {
      send_alert("ERROR: Ошибка инициализации SD карты");
    }
  
    String file_name = object_TimeDate.get_DateFULL() + ".txt";
    String buf_user_ID = object_array_users[users_array_index].get_id();
    jesse_yield_func();
    File myFile = SD.open(file_name);

    if (myFile)
    {
      fb::File f(file_name, fb::File::Type::document, myFile);
      f.chatID = buf_user_ID;

      bot_main.sendMessage(fb::Message("Ожидайте...\nОтправляю файл: " + file_name, object_array_users[users_array_index].get_id()));
      delay(20);
      bot_main.sendFile(f, false);
      delay(20);
      bot_main.sendMessage(fb::Message("Отправка файла завершена.", object_array_users[users_array_index].get_id())); 

      myFile.close();
    }

    else
    {
      send_alert("ERROR: Ошибка чтения SD карты");
    }
  }  

/// ↓↓↓ Адресная лента бра и линия

  #include "FastLED.h"                             // LED библиотека //   

  #define Bra_Leds_Amount 240                      // Бра //       
  #define Bra_Pin         D1                       // ↑↑↑ //
  CRGB array_LED_bra [Bra_Leds_Amount];            // ↑↑↑ //

  #define Line_Leds_Amount 60                      // Полоска //   
  #define Line_Pin         D2                      // ↑↑↑ //
  CRGB array_LED_line [Line_Leds_Amount];          // ↑↑↑ //

  #define Clock_Leds_Amount 191                    // Часы //   
  #define Clock_Pin         D4                     // ↑↑↑ //
  CRGB array_LED_clock [Clock_Leds_Amount];        // ↑↑↑ //

  byte depth;

  int timer_min_hue_clock_cur = 0;                 // счетчик //
  int timer_min_hue_clock_target = 60;             // интервал смены цвета часов в минутах //

  int hue_HSV_clock_day_random = 175;                  // hue (цвет) часов - меняется раз в час, чтобы не разражать. // пусть при запуске будет бирюзовым, раз в час цвет меняется //
  int hue_HSV_bra_and_line_day_random;                 // переменная, которая хранит значение последнего случайного hue (цвет) //

  int val_HSV_bra_day     = 80;                    // value (яркость) бра днём // 
  int val_HSV_line_day    = 40;                    // value (яркость) линии днём //
  int val_HSV_clock_day   = 100;                   // value (яркость) часов днём //

  int sat_HSV_bra_day_random;                      // переменная, которая хранит значение последнего случайного saturatuion (насыщенность) //
  int sat_HSV_line_day    = 70;                    // saturation (насыщенность) линии днём //
  int sat_HSV_clock_day   = 130;                   // saturation (насыщенность) часов днём //

  int val_HSV_bra_night   = 0;                     // value (яркость) бра ночью //
  int val_HSV_line_night  = 0;                     // value (яркость) линии ночью //
  int val_HSV_clock_night = 1;                     // value (яркость) часов ночью //
  byte RGB_clock_night [3] = {1, 1, 1,};           // цвет часов ночью. по умолчанию белый (white) 1,1,1,

  int val_HSV_bra_cur     = val_HSV_bra_night;     // текущее значение яркости бра // нужно для плавного изменения яркости при изменение ночного режима по расписанию //
  int val_HSV_line_cur    = val_HSV_line_night;    // текущее значение яркости линии // ↑↑↑ //
  int val_HSV_clock_cur   = val_HSV_clock_night;   // текущее значение яркости часов // ↑↑↑ //

  bool global_ERROR_flag = false;                  // Флаг для ошибок (1 - нет ошибок, 2 - в цикле были ошибки) // ps в какой-то момент появилось подозрение, что где-то в биоблетаках уже есть ErrorSTATE и возникают ошибки //

  void FillSolidMY()                               // заливка, чтобы не ждать при изменение яркости и дневного/ночного режима через команду //
  {
    fill_solid(array_LED_bra, Bra_Leds_Amount, CHSV(hue_HSV_bra_and_line_day_random, sat_HSV_bra_day_random, val_HSV_bra_cur));
    fill_solid(array_LED_line, Line_Leds_Amount, CHSV(hue_HSV_bra_and_line_day_random, sat_HSV_line_day, val_HSV_line_cur));
    FastLED.delay(10);
  }

  void random_colour_line_and_bra()
  {
    hue_HSV_bra_and_line_day_random = random(0, 255);
    sat_HSV_bra_day_random = random(50, 255);
    depth   = random(0, 10);
    depth   = (depth * 20);  
  }

  namespace Animation
  {
    enum
    {
      NONE,
      UP_PART_0,
      UP_PART_1,
      UP_PART_2,
      DOWN_PART_0,
      DOWN_PART_1,
      DOWN_PART_2,
      CLOCK,
      ERROR_PART_0,
      ERROR_PART_1,
      ERROR_PART_2,
      ERROR_PART_3,
      ERROR_PART_4,
    } state;

    unsigned long timer_millis;
    int counter;

    int error_p3_low;
    int error_p3_high;
    int error_p3_brightnes;
  }

  void UP_Func_Part_0()
  {
    random_colour_line_and_bra();

    Animation::state   = Animation::UP_PART_1;
    Animation::counter = Line_Leds_Amount - 1;
  }

  void UP_Func_Part_1()
  {
    if(Animation::counter > -1)
    {
      array_LED_line [Animation::counter] = CHSV(hue_HSV_bra_and_line_day_random, sat_HSV_line_day, val_HSV_line_cur);
      FastLED.delay(60);
      Animation::counter--;
    }

    else
    {
      Animation::state = Animation::UP_PART_2;
      Animation::counter = 0;
    }
  }

  void UP_Func_Part_2()
  {
    if(Animation::counter < (Bra_Leds_Amount - depth))
    {
      array_LED_bra [Animation::counter] = CHSV(hue_HSV_bra_and_line_day_random, sat_HSV_bra_day_random, val_HSV_bra_cur);
      FastLED.delay(20);
      Animation::counter++;
    }

    else
    {
      Animation::state = Animation::CLOCK;
    }
  }

  void DOWN_Func_Part_0()
  {
    random_colour_line_and_bra();

    Animation::state   = Animation::DOWN_PART_1;
    Animation::counter = Bra_Leds_Amount - 1;  
  }

  void DOWN_Func_Part_1()
  {
    if(Animation::counter >= depth)
    {
      array_LED_bra [Animation::counter] = CHSV(hue_HSV_bra_and_line_day_random, sat_HSV_bra_day_random, val_HSV_bra_cur);
      FastLED.delay(20);
      Animation::counter--;
    }

    else
    {
      Animation::state   = Animation::DOWN_PART_2;
      Animation::counter = 0;
    }
  }

  void DOWN_Func_Part_2()
  {
    if(Animation::counter < Line_Leds_Amount)
    {
      array_LED_line [Animation::counter] = CHSV(hue_HSV_bra_and_line_day_random, sat_HSV_line_day, val_HSV_line_cur);
      FastLED.delay(60);
      Animation::counter++;
    }

    else
    {
      Animation::state = Animation::CLOCK;
    }
  }

  void ERROR_Func_Part_0()
  {
    fill_solid(array_LED_bra, Bra_Leds_Amount,  CHSV(0, 0, 0));
    fill_solid(array_LED_line,   Line_Leds_Amount, CHSV(0, 0, 0));

    Animation::state   = Animation::ERROR_PART_1;
    Animation::counter = Line_Leds_Amount - 1;  
  }

  void ERROR_Func_Part_1()
  {
    if(Animation::counter > -1)
    {
      if(Animation::counter % 2 > 0)
      {
        array_LED_line [Animation::counter] = CHSV(140, 255, 255);
      }

      else
      {
        array_LED_line [Animation::counter] = CHSV(255, 255, 255);
      }
      FastLED.delay(16);
      Animation::counter--;   
    }

    else
    {
      Animation::state = Animation::ERROR_PART_2;
      Animation::counter = 0;
      Animation::timer_millis = millis();
    }
  }

  void ERROR_Func_Part_2()
  {
    if(millis() - Animation::timer_millis > 480)
    {
      if(Animation::counter < 12)
      {
        int k = Animation::counter * 20;
        for(int d = 0; d < 11; d += 10)
        {
          for (int h = 0; h < 10; h++)
          {
            if (h >= 0 && h <= 4)
            {
              array_LED_bra [k+h+d] = CHSV(140, 255, 255);
            }
            else
            {
              array_LED_bra [k+h+d] = CHSV(255, 255, 255);
            }
          }       
        }
        FastLED.delay(20);
        Animation::counter++; 
      }

      else
      {
        Animation::state = Animation::ERROR_PART_3;
        Animation::counter = 0;
        Animation::error_p3_low       = 0;
        Animation::error_p3_high      = 5;
        Animation::error_p3_brightnes = 255;
        delay(100);
      }
      Animation::timer_millis = millis();
    }

  }

  void ERROR_Func_Part_3()
  {
    if(Animation::counter < 27)
    {
      for(int i = 0; i < 12; i++)
      {
        Animation::error_p3_brightnes--;
        Animation::error_p3_brightnes = constrain(Animation::error_p3_brightnes, 0, 255);       // чтобы плавно снизить яркость в 0...
        int k = i * 20;
        for(int d = 0; d < 11; d += 10)
        {
          array_LED_bra [k + d + Animation::error_p3_low] = CHSV(255, 255, Animation::error_p3_brightnes);
          array_LED_bra [k + d + Animation::error_p3_high] = CHSV(140, 255, Animation::error_p3_brightnes);           

        }
        FastLED.delay(30);     
      }
      Animation::error_p3_low++;
      Animation::error_p3_high++;
      if (Animation::error_p3_low > 9)
      {
        Animation::error_p3_low = Animation::error_p3_low % 10;
      }
      if (Animation::error_p3_high > 9)
      {
        Animation::error_p3_high = Animation::error_p3_high % 10;
      }
      Animation::counter++;
    }
    
    else
    {
      Animation::state = Animation::ERROR_PART_4;
      Animation::counter = 0;
      delay(100);
    }
  }

  void ERROR_Func_Part_4()
  {
    if(Animation::counter < Line_Leds_Amount)
    {
      array_LED_line [Animation::counter] = CHSV(0, 0, 0);
      FastLED.delay(20);
      Animation::counter++; 
    }

    else
    {
      Animation::state = Animation::NONE;
    }
  }

  void Led_Animation_Check()
  {
    switch (Animation::state)
    {
      case Animation::UP_PART_0:
      {
        UP_Func_Part_0();
        break;
      }

      case Animation::UP_PART_1:
      {
        UP_Func_Part_1();
        break;
      }

      case Animation::UP_PART_2:
      {
        UP_Func_Part_2();
        break;
      }

      case Animation::DOWN_PART_0:
      {
        DOWN_Func_Part_0();
        break;
      }

      case Animation::DOWN_PART_1:
      {
        DOWN_Func_Part_1();
        break;
      }

      case Animation::DOWN_PART_2:
      {
        DOWN_Func_Part_2();
        break;
      }

      case Animation::CLOCK:
      {
        clock_master();
        break;
      }

      case Animation::ERROR_PART_0:
      {
        ERROR_Func_Part_0();
        break;
      }

      case Animation::ERROR_PART_1:
      {
        ERROR_Func_Part_1();
        break;
      }

      case Animation::ERROR_PART_2:
      {
        ERROR_Func_Part_2();
        break;
      }

      case Animation::ERROR_PART_3:
      {
        ERROR_Func_Part_3();
        break;
      }        

      case Animation::ERROR_PART_4:
      {
        ERROR_Func_Part_4();
        break;
      } 

      default:
      {
        break;
      }
    }
  }

  void LEDhello()                                  // индикация начала обработки команды из телеграма // 
  {
    byte val_HSV_line_curBUF = constrain(val_HSV_line_cur, 4, 255);
    array_LED_line [0] = CHSV(255, 0, (val_HSV_line_curBUF));
    array_LED_line [1] = CHSV(140, 255, (val_HSV_line_curBUF));
    array_LED_line [2] = CHSV(255, 0, (val_HSV_line_curBUF));
    FastLED.delay(30);
  }

  void LEDbye()                                    // индикация конца обработки команды из телеграма // 
  {
    array_LED_line [0] = CHSV(hue_HSV_bra_and_line_day_random, sat_HSV_line_day, val_HSV_line_cur);
    array_LED_line [1] = CHSV(hue_HSV_bra_and_line_day_random, sat_HSV_line_day, val_HSV_line_cur);
    array_LED_line [2] = CHSV(hue_HSV_bra_and_line_day_random, sat_HSV_line_day, val_HSV_line_cur);  
    FastLED.delay(30);
  }


  ///↓↓↓ НОЧНОЙ РЕЖИМ ↓↓↓///

  namespace NightTime
  {
    namespace State
    {
      enum
      {
        OFF        = 1,                 // Отключен
        MANUAL_ON  = 2,                 // Включен до следующего утра
        NEUTRAL    = 3,                 // В нейтральном положение, чтобы сработало сразу при включение
        ON         = 4,                 // Включен
        MANUAL_OFF = 5,                 // Отключен до следующего утра    
      };
    }
    namespace DimState
    {
      enum
      {
        OFF      = 1,
        DECREASE = 2,
        INCREASE = 3,
      };
    }
  }

  class class_NightTime                            // класс ночного режима //
  {
    public:
      class_NightTime()                            // конструктор класса //
      {
        _NightTimeState    = NightTime::State::NEUTRAL;
        _NightTimeDimState = NightTime::DimState::OFF;
      }

      void update_NightTime()                      // метод проверки пора ли переключить ночной режим //
      {
        if(_NightTimeState <= NightTime::State::NEUTRAL)                       // Ночной режим ВКЛ с 21:00 до 6:48 //
        { 
          if (object_TimeDate.get_TimeB() > 210000 || object_TimeDate.get_TimeB() < 64800)
          {
            _NightTimeDimState = NightTime::DimState::DECREASE;     // Уменьшение яркости (займет 10 минут при стандартной яркости) //
            _NightTimeState    = NightTime::State::ON;              // 1 - выкл, 2 - вкл до след утра , 3 - в нейтральном положение, чтобы сработало сразу при включение, 4 -вкл , 5 - выкл след вечера //
          }
        }

        if(_NightTimeState >= NightTime::State::NEUTRAL)                       // Ночной режим ВЫКЛ c 6:50 до 20:58 // 
        {
          if (object_TimeDate.get_TimeB() > 65000 && object_TimeDate.get_TimeB() < 205800)
          {    
            _NightTimeDimState = NightTime::DimState::INCREASE;     // Увеличение яркости (займет 40 минут при стандартной яркости) //
            _NightTimeState    = NightTime::State::OFF;             // 1 - выкл, 2 - вкл до след утра , 3 - в нейтральном положение, чтобы сработало сразу при включение, 4 -вкл , 5 - выкл след вечера //
          }
        }
      }

      void Night_Time_Manual_ON()
      {
        val_HSV_bra_cur = val_HSV_bra_night;
        val_HSV_line_cur = val_HSV_line_night;
        val_HSV_clock_cur = val_HSV_clock_night;
        _NightTimeState    = NightTime::State::MANUAL_ON;
        _NightTimeDimState = NightTime::DimState::OFF;                // необходимо, чтобы прервать плавное изменение яркости если ночной режим был переключен в процессе изменения яркости //
        Animation::state   = Animation::NONE;
        FillSolidMY();
        clock_master();
      }

      void Night_Time_Manual_OFF()
      {
        val_HSV_bra_cur = val_HSV_bra_day;
        val_HSV_line_cur = val_HSV_line_day;
        val_HSV_clock_cur = val_HSV_clock_day;
        _NightTimeState    = NightTime::State::MANUAL_OFF;
        _NightTimeDimState = NightTime::DimState::OFF;                // необходимо, чтобы прервать плавное изменение яркости если ночной режим был переключен в процессе изменения яркости //
        Animation::state   = Animation::NONE;
        random_colour_line_and_bra();
        hue_HSV_clock_day_random = hue_HSV_bra_and_line_day_random;
        FillSolidMY();
        clock_master();
      }

      void Night_Time_Manual_Switch()
      {
        if(_NightTimeState == NightTime::State::MANUAL_ON || _NightTimeState == NightTime::State::ON)
        {
          Night_Time_Manual_OFF();
        }

        else
        {
          Night_Time_Manual_ON();
        }
      }

      void Night_Time_Dim()                              // плавное изменение яркости //
      {
        if (_NightTimeDimState == NightTime::DimState::DECREASE)                    // Уменьшение яркости (займет 10 минут при стандартной яркости) // 
        {
          val_HSV_bra_cur = val_HSV_bra_cur - 8;
          val_HSV_line_cur = val_HSV_line_cur - 4;
          val_HSV_clock_cur = val_HSV_clock_cur - 10;

          if (val_HSV_bra_cur <= val_HSV_bra_night)
          {
            val_HSV_bra_cur = val_HSV_bra_night;
            val_HSV_line_cur = val_HSV_line_night;
            val_HSV_clock_cur = val_HSV_clock_night;
            _NightTimeDimState = NightTime::DimState::OFF;
          }    
        }
          
        else if (_NightTimeDimState == NightTime::DimState::INCREASE)               // Увеличение яркости (займет 40 минут при стандартной яркости) //
        {
          val_HSV_bra_cur = val_HSV_bra_cur + 2;
          val_HSV_line_cur = val_HSV_line_cur + 1;
          val_HSV_clock_cur = val_HSV_clock_cur + 2;
            
          if (val_HSV_bra_cur >= val_HSV_bra_day)
          {
            val_HSV_bra_cur = val_HSV_bra_day;
            val_HSV_line_cur = val_HSV_line_day;
            val_HSV_clock_cur = val_HSV_clock_day;
            _NightTimeDimState = NightTime::DimState::OFF;
          }     
        }
      }

      byte get_NightTimeState()
      {
        return _NightTimeState;
      }

    private:
      byte _NightTimeState;                        // 1 - выкл, 2 - вкл до след утра , 3 - в нейтральном положение, чтобы сработало сразу при включение, 4 -вкл , 5 - выкл до след вечера //   
      byte _NightTimeDimState;                     // Флаг для ночного режима (1 - выкл, 2 - уменьшение яркости, 3 увеличение яркости) //
  };

  class_NightTime object_NightTime;                // создаем экземпляр класса class_NightTime (объект) //

/// ↓↓↓ Адресная лента часы

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

  byte ClockArray_main [Clock_Leds_Amount];                // основной массив для отображения данных (результат сложения 13 цифр + точка, двоеточие, процент) //
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
        byte buf_array [14];

        for (int i = 0; i < 7; i++)                          // по сути, этот цикл является очень примитивным дешифратором, где _key_ID это индекс строки в массиве _key_ARRAY, на которой лежит ключ//
        {    
          jesse_yield_func();

          buf_array [i*2] = _numbers_code_array [_cur_number][_key_ARRAY [_key_ID][i]];
          buf_array [(i*2)+1] = _numbers_code_array [_cur_number][_key_ARRAY [_key_ID][i]];
        }

        for (int i = 0; i < _array_length; i++)              // полученный результат переносим в основной массив ClockArray_main, чтобы при вызове следующих объектов сохранить индекс - храним его в ArrayGlobalCounter //
        {
          jesse_yield_func();
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
    jesse_yield_func();

    byte dividerIndex = SYNCdata.indexOf(',');
    String buf_temp = SYNCdata.substring(0, dividerIndex);
    String buf1 = SYNCdata.substring(dividerIndex + 1);

    byte dividerIndex2 = buf1.indexOf(',');
    String buf_hum = buf1.substring(0, dividerIndex2);
    String buf2 = buf1.substring(dividerIndex2 + 1);

    byte dividerIndex3 = buf2.indexOf(',');
    String buf_co2 = buf2.substring(0, dividerIndex3);

    jesse_yield_func();

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
      if (val_HSV_clock_cur == val_HSV_clock_night)                             // проверяем закончилось ли плавное диммирование яркости и потом выключаем лишние данные //
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
    jesse_yield_func();

    int hue_HSV_clock_day_random_buf = hue_HSV_clock_day_random;                          // буфер переменной для сравнения ниже //

    if (timer_min_hue_clock_target == 60)                         // если интервал 60 минут, то цвет меняется в 00 минут независимо от того, когда включили микроконтроллер //
    {
      if (object_TimeDate.get_MIN() == 0)                                           // в 00 минут меняем цвет //
      {
        hue_HSV_clock_day_random = hue_HSV_bra_and_line_day_random;
      }
    }
    
    else                                                          // в других случаях меняем просто по таймеру //
    {
      timer_min_hue_clock_cur++;                                  // поскольку функция clock_animation вызывается раз в минуту - делаем инкремент счетчика и получаем минутный таймер //
      if (timer_min_hue_clock_cur >= timer_min_hue_clock_target)  // hue_HSV_bra_and_line_day_random меняется каждую минуту. Как только подошло время по таймеру приравниваем к нему hue_HSV_clock_day_random //
      {
        hue_HSV_clock_day_random = hue_HSV_bra_and_line_day_random;
        timer_min_hue_clock_cur = 0;
      }
    }

    if (val_HSV_clock_cur <= val_HSV_clock_night)                               // если яркость меньше или равна ночному режиму, значит сейчас ночной режим. анимации отключена, управление по RGB. //
    {
      for(int i = 0; i < Clock_Leds_Amount; i++)
      {
        array_LED_clock [i] = CRGB((RGB_clock_night [0]*val_HSV_clock_cur*ClockArray_main [i]),(RGB_clock_night [1]*val_HSV_clock_cur*ClockArray_main [i]),(RGB_clock_night [2]*val_HSV_clock_cur*ClockArray_main [i]));

        jesse_yield_func();
      }
      FastLED.delay(50);
    }

    else if (hue_HSV_clock_day_random != hue_HSV_clock_day_random_buf)                    // если яркость поменялась с начала функции - значит нужна анимация по HSV //
    {
      if (object_TimeDate.get_MIN() % 2 > 0)                                        // анимация снизу вверх каждую нечетную минуту
      {
        for(int i = 0; i < Clock_Leds_Amount; i++)
        {
          array_LED_clock [i] = CHSV(hue_HSV_clock_day_random, sat_HSV_clock_day, (val_HSV_clock_cur*ClockArray_main [i]));
          FastLED.delay(20);

          jesse_yield_func();
        }
      }

      else                                                        // анимация сверху вниз каждую четную минуту
      {
        for(int i = Clock_Leds_Amount - 1; i >= 0; i--)
        {
          array_LED_clock [i] = CHSV(hue_HSV_clock_day_random, sat_HSV_clock_day, (val_HSV_clock_cur*ClockArray_main [i]));
          FastLED.delay(20);

          jesse_yield_func();
        }
      }
    }

    else                                                          // во всех других случаях - анимация отключена, режим HSV //
    {
      for(int i = 0; i < Clock_Leds_Amount; i++)
      {
        array_LED_clock [i] = CHSV(hue_HSV_clock_day_random, sat_HSV_clock_day, (val_HSV_clock_cur*ClockArray_main [i]));

        jesse_yield_func();
      }
      FastLED.delay(20);
    }

    Animation::state = Animation::NONE;
  }

  String clock_indication()                        // отображение текущего состояния отображения данных на табло в текстовом виде //
  {
    jesse_yield_func();

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

    jesse_yield_func();

    if (clock_night_indication_co2 == true)
    {
      buf_co2_message = "\n  СО2 - включено ( /11102 ).";    
    }
    else
    {
      buf_co2_message = "\n  СО2 - отключено ( /11102 ).";
    }

    jesse_yield_func();

    if (clock_night_indication_temperature == true)
    {
      buf_temp_message = "\n  Температура - включена ( /11103 ).";     
    }
    else
    {
      buf_temp_message = "\n  Температура - отключена ( /11103 ).";
    }

    jesse_yield_func();

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

/// ↓↓↓ Телеграм

  void Message_command_get_data(String text)       // вызывается из Common CODE файла, из функции Message_from_Telegram_converter() //
  {
    jesse_yield_func();

    switch (object_array_users[users_array_index].get_message_state())
    {
      case 104:                                // яркость //
      {
        int buf_text_int = text.toInt();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        val_HSV_bra_day = constrain(buf_text_int, 0, 255);
        val_HSV_line_day = (val_HSV_bra_day/2);
        val_HSV_bra_cur = val_HSV_bra_day;
        val_HSV_line_cur = val_HSV_line_day;
        FillSolidMY();
        object_array_users[users_array_index].send_message("Яркость бра и линии днём установлена: " + String(val_HSV_bra_day));
        break;            
      }

      case 105:                                // яркость часов днём //
      {
        int buf_text_int = text.toInt();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        val_HSV_clock_day = constrain(buf_text_int, 0, 255);
        val_HSV_clock_cur = val_HSV_clock_day;
        clock_master();
        object_array_users[users_array_index].send_message("Яркость часов днём установлена: " + String(val_HSV_clock_day));     
        break;   
      } 

      case 106:                                // яркость часов ночью //  
      {
        int buf_text_int = text.toInt();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        val_HSV_clock_night = constrain(buf_text_int, 0, 40);
        val_HSV_clock_cur = val_HSV_clock_night;
        clock_master();
        object_array_users[users_array_index].send_message("Яркость часов ночью установлена: " + String(val_HSV_clock_night));       
        break;   
      }

      case 110:                               // интервал смены цвета часов днём //  
      {
        int buf_text_int = text.toInt();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        timer_min_hue_clock_target = constrain(buf_text_int, 1, 10080);
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
        break;
      }
    }
    object_array_users[users_array_index].set_message_state(1);
  }

  void Message_command_send_data(int text_int)      // вызывается из Common CODE файла, из функции Message_from_Telegram_converter() //
  {
    jesse_yield_func();

    switch (text_int)
    {
      case 101:                                                       // запрос лога за текущий день //
      {
        LOG_send_by_command();
        break;
      }

      case 102:                                                       // запрос первой строки excel //
      {
        object_array_users[users_array_index].send_message("Дата и Время, Температура в комнате, Влажность в комнате, СО2, Рекуп. Приток (in), Рекуп. Приток (out), Рекуп. Вытяжка (in), Рекуп. Вытяжка (out), КПД рекуп. на вытяжку (%), КПД рекуп. на приток (%), Реальное КПД на приток с учётом теплопотерь, Воздуховод с улицы (°C), Батарея (°C), Воздуховод с батареи (°C), Объединенный поток после фильтров(°C), Теплопотери от улицы до рекуператора (°C), Теплопотери воздухувода от кровати до рекуп. и нагрев вентилятором (°C), % воздуха c улицы, % воздуха c батареи, Текущее положение заслонок, Шагов за сутки (без учета калибровки)");
        break;
      }

      case 10300:                                                     // отключить ночной режим //
      {
        object_NightTime.Night_Time_Manual_OFF();
        object_array_users[users_array_index].send_message("Ночной режим принудительно ОТКЛЮЧЁН до вечера (Ночью в любом случае не будет появляться анимация ошибки).");
        break;
      }

      case 10301:                                                     // включить ночной режим //
      {
        object_NightTime.Night_Time_Manual_ON();
        object_array_users[users_array_index].send_message("Ночной режим принудительно ВКЛЮЧЕН до утра (Днём перестанет появляться анимация ошибки).");
        break;
      }

      case 10310:                                                     // вызов инлайн меню //
      {
        String buf_id = object_array_users[users_array_index].get_id();
        fb::Message my_Switch_inline_msg("Ночной Режим:", buf_id);

        fb::InlineMenu my_Switch_inline_menu;
        String buf_Command = "/10311@" + This_bot_name;
        my_Switch_inline_menu.addButton("Switch", buf_Command);                // callback data
        my_Switch_inline_msg.setInlineMenu(my_Switch_inline_menu);             // подключить меню

        bot_main.sendMessage(my_Switch_inline_msg);
        break;
      }

      case 10311:                                                     // срабатывание кнопки инлайн меню //
      {
        object_NightTime.Night_Time_Manual_Switch();
        break;
      }

      case 104:                                                       // яркость бра и линии днём //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для изменения яркости бра и линии днём (от 0 до 255):\n\nТекущее значение: " + String(val_HSV_bra_day));
        object_array_users[users_array_index].set_message_state(104);
        break;
      }

      case 105:                                                       // яркость часов днём //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для изменения яркости часов в дневное время (от 0 до 255):\n\nТекущее значение: " + String(val_HSV_clock_day));
        object_array_users[users_array_index].set_message_state(105);
        break;
      }

      case 106:                                                       // яркость часов ночью //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для изменения яркости часов ночью (от 0 до 40):\n\nТекущее значение: " + String(val_HSV_clock_night));
        object_array_users[users_array_index].set_message_state(106);
        break;
      }

      case 107:                                                       // анимация "снизу вверх" //
      {
        Animation::state = Animation::UP_PART_0;
        break;
      }

      case 108:                                                       // анимация "сверху вниз" //
      {
        Animation::state = Animation::DOWN_PART_0;
        break;
      }

      case 109:                                                       // анимация "ошибка" //
      {
        Animation::state = Animation::ERROR_PART_0;
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

      case 190:                                                       // ВКЛ/ВЫКЛ Текстовых уведомлений //
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

      case 390:                                                       // выбор гостевого чата кнопки //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_array_users[users_array_index].send_message("Отправьте ID группы или профиля, чтобы я отправился туда. Я продолжу отвечать на ваши запросы!");
          object_array_users[users_array_index].set_message_state(390);
        }
        break;
      }

      case 666:                                                       // запрос debug отчёта //
      {
        send_debug_repor_by_command();
        break;
      }
    }
  }

/// ↓↓↓ Синхронизация

  void SYNCstart()                                 // получение температур, проверка времени, синхронизация ошибок //
  {
    jesse_yield_func();

    Serial.println(object_TimeDate.get_UTC());                              // отправляем время для синхронизации //
    
    if (Serial.available())                                                 // если что-то есть в серил, то читаем //
    {
      jesse_yield_func();

      String SYNCmessage = Serial.readString();

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

    LOG_write_sync_data();
  }

/// ↓↓↓ Setup и Loop

  void setup()                                     // стандартная функция Ардуино - выполняется один раз в начале //
  {
    ESP.wdtDisable();                              // отключаем software WDT //
    delay(50);
    ESP.wdtEnable(8000);                          // включаем  software WDT с таймером на 10 секунд //
      /*
      https://bigdanzblog.wordpress.com/2019/10/08/watch-dog-timer-wdt-for-esp8266/
      There is a hardware WDT and a software WDT.
      The HW WDT is always running and will reset the MCU after about 6 seconds.
      The SW WDT seems to reset the MCU at 1.5 about seconds.
      You can enable/disable the SW WDT, but not the HW WDT.
    */

    Serial.begin(115200);                          // запускаем Serial Port и определяем его скорость //
    Serial.setTimeout(300);                        // таймаут для .readString (ждет заданное значение на чтение Serial)

    WiFi.setOutputPower(16.00);                    // "When values higher than 19.25 are set, the board resets every time a connection with the AP is established." // https://stackoverflow.com/questions/75712199/esp8266-watchdog-reset-when-using-wifi // 
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);          // подключаемся к Wi-Fi //

    object_TimeDate.set_UTC_time();
    object_TimeDate.update_TimeDate();             // обновляем текущее время //

    setup_telegram_bots();

    FastLED.addLeds<WS2812, Bra_Pin, GRB>(array_LED_bra, Bra_Leds_Amount);   // определяем лед ленту БРА //
    FastLED.addLeds<WS2812, Line_Pin, GRB>(array_LED_line, Line_Leds_Amount);     // определяем лед ленту ЛИНИИ //
    FastLED.addLeds<WS2812, Clock_Pin, GRB>(array_LED_clock, Clock_Leds_Amount);    // определяем лед ленту ЧАСОВ //
    FastLED.setBrightness(255);                                             // задаём глобальную яркость всех лент, как максимальную //

    send_reset_info();
    //send_alert("Я проснулся.");
  }

  void loop()                                                      // основной луп //
  {
    obj_stopwatch_ms_loop.start();                                 // для подсчета времени лупа //

    object_TimeDate.update_TimeDate();                                           // получаем актуальное время с сервера //
  
    object_NightTime.update_NightTime();                                         // обновляем состояние ночного режима //

    bot_tick_and_call_debug();                                                   // update telegram - получение сообщения из телеги и их обработка // внутри вызывается debug и .tick //

    if (global_ERROR_flag == true && Animation::state == Animation::NONE)        // проверят были ли ошибки и если были - делает анимацию //
    {
      if (object_NightTime.get_NightTimeState() == 1)                            // 1 значит - днём можно включить ночной режим, чтобы отключить анимацию ошибки во время отладки // 1 - выкл, 2 - вкл до след утра , 3 - в нейтральном положение, чтобы сработало сразу при включение, 4 -вкл , 5 - выкл до след вечера //
      {    
        Animation::state = Animation::ERROR_PART_0;                              // если были ошибки и сейчас день, без включенного ночного режима - запускаем анимацию ошибки //
      }
      global_ERROR_flag = false;                                                 // в любом случае сбрасываем флаг ошибки, чтобы если ночью была ошибка, то утром первым делом она не захерачила анимацию //     
    }

    Led_Animation_Check();

    if(object_TimeDate.get_MIN() % 2 > 0 && flag_every_minute_timer == false)    // таймер каждую нечетную минуту //
    {
      LOG_send_by_timer(); 
      object_NightTime.Night_Time_Dim();
      Animation::state = Animation::UP_PART_0;
      flag_every_minute_timer = true;
    }

    if(object_TimeDate.get_MIN() % 2 == 0 && flag_every_minute_timer == true)    // таймер каждую четную минуту //
    {
      SYNCdata = "";                                                             // обнуляем стринг с датой, чтобы при ошибках синхронизации на часах не продолжали висеть старые данные //
      SYNCstart();
      object_NightTime.Night_Time_Dim();
      Animation::state = Animation::DOWN_PART_0;
      restart_check();
      flag_every_minute_timer = false;
    }

    obj_stopwatch_ms_loop.stop();
  }







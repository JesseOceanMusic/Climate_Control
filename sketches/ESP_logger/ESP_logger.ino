//Tools > MMU 16KB cache + 48KB IRAM and 2nd HEAP (shared)
//Tools > Stack Protection Enable
//NodeMCU 1.0 (ESP-12E Module)

#define THIS_IS_LOGGER_CODE
#define Jesse_yield_enable                       // delay(0) и yield() одно и тоже... и то и то даёт возможность ESP в эти прерывания обработать wi-fi и внутренний код // https://arduino.stackexchange.com/questions/78590/nodemcu-1-0-resets-automatically-after-sometime //


#include "A:\1 - important\PROJECTS\Arduino\!Climate_Control\! GEN 8\Gen_8_ver_016\Common_CODE.cpp"

String SYNCdata;                                 // стринг для полученных данных из Serial Port //

/// ↓↓↓ Адресная лента бра и линия

  #include "FastLED.h"                             // LED библиотека //   

  #define Bra_Leds_Amount 240                      // Бра //       
  #define Bra_Pin         D1                       // ↑↑↑ //
  CRGB array_LED_bra [Bra_Leds_Amount];            // ↑↑↑ //

  #define Clock_Leds_Amount 191                    // Часы //   
  #define Clock_Pin         D4                     // ↑↑↑ //
  CRGB array_LED_clock [Clock_Leds_Amount];        // ↑↑↑ //

  #define Line_Leds_Amount 60                      // Полоска //   
  #define Line_Pin         D2                      // ↑↑↑ //
  CRGB array_LED_line [Line_Leds_Amount];          // ↑↑↑ //

  byte depth;

  int timer_min_hue_clock_cur = 0;                 // счетчик //
  int timer_min_hue_clock_target = 60;             // интервал смены цвета часов в минутах //

  int hue_HSV_clock_day_random = 175;              // hue (цвет) часов - меняется раз в час, чтобы не разражать. // пусть при запуске будет бирюзовым, раз в час цвет меняется //
  int hue_HSV_bra_and_line_day_random;             // переменная, которая хранит значение последнего случайного hue (цвет) //

  int val_HSV_bra_day     = 80;                    // value (яркость) бра днём // 
  int val_HSV_line_day    = 40;                    // value (яркость) линии днём //
  int val_HSV_clock_day   = 100;                   // value (яркость) часов днём //

  int sat_HSV_bra_day_random;                      // переменная, которая хранит значение последнего случайного saturatuion (насыщенность) //
  int sat_HSV_line_day    = 70;                    // saturation (насыщенность) линии днём //
  int sat_HSV_clock_day   = 130;                   // saturation (насыщенность) часов днём //

  int val_HSV_bra_night    = 0;                     // value (яркость) бра ночью //
  int val_HSV_line_night   = 0;                     // value (яркость) линии ночью //
  int val_HSV_clock_night  = 1;                     // value (яркость) часов ночью //
  byte RGB_clock_night [3] = {0, 0, 1,};           // цвет часов ночью. по умолчанию синий (blue) 0,0,1,

  int val_HSV_bra_cur     = val_HSV_bra_night;     // текущее значение яркости бра // нужно для плавного изменения яркости при изменение ночного режима по расписанию //
  int val_HSV_line_cur    = val_HSV_line_night;    // текущее значение яркости линии // ↑↑↑ //
  int val_HSV_clock_cur   = val_HSV_clock_night;   // текущее значение яркости часов // ↑↑↑ //

  bool global_ERROR_flag = false;                  // Флаг для ошибок (1 - нет ошибок, 2 - в цикле были ошибки) // ps в какой-то момент появилось подозрение, что где-то в биоблетаках уже есть ErrorSTATE и возникают ошибки //
  bool ERROR_animation_manual_off = false;         // Отключение анимации ошибок вручную через команду //

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
      CLOCK_MASTER,
      CLOCK_UP_PART_1,
      CLOCK_UP_PART_2,
      CLOCK_DOWN_PART_1,
      CLOCK_DOWN_PART_2,
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
      Animation::state = Animation::CLOCK_MASTER;
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
      Animation::state = Animation::CLOCK_MASTER;
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

      case Animation::CLOCK_MASTER:
      {
        clock_master();
        break;
      }

      case Animation::CLOCK_UP_PART_1:
      {
        UP_clock_func_part_1();
        break;
      }

      case Animation::CLOCK_UP_PART_2:
      {
        UP_clock_func_part_2();
        break;
      }

      case Animation::CLOCK_DOWN_PART_1:
      {
        DOWN_clock_func_part_1();
        break;
      }

      case Animation::CLOCK_DOWN_PART_2:
      {
        DOWN_clock_func_part_2();
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

  void set_clock_night_color(byte R_input, byte G_input, byte B_input)
  {
    RGB_clock_night [0] = R_input;
    RGB_clock_night [1] = G_input;
    RGB_clock_night [2] = B_input;
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
          if (object_TimeDate.get_TimeB() > 210000 || object_TimeDate.get_TimeB() < 84800)
          {
            _NightTimeDimState = NightTime::DimState::DECREASE;     // Уменьшение яркости (займет 10 минут при стандартной яркости) //
            _NightTimeState    = NightTime::State::ON;              // 1 - выкл, 2 - вкл до след утра , 3 - в нейтральном положение, чтобы сработало сразу при включение, 4 -вкл , 5 - выкл след вечера //
          }
        }

        if(_NightTimeState >= NightTime::State::NEUTRAL)                       // Ночной режим ВЫКЛ c 6:50 до 20:58 // 
        {
          if (object_TimeDate.get_TimeB() > 85000 && object_TimeDate.get_TimeB() < 205800)
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

  /*ОБЩАЯ ЛОГИКА МАССИВОВ
    1 - преобразуем цифру в формат семисегментного отображения
    2 - преобразуем полученный результат с учетом смещения для конкретной цифры на табло (из-за того, что свтодиодная лента расположена змейкой и не одномерна)
    3 - складываем отдельные цифры и знаки "%" "." ":"" в один большой массив из 0 и 1
    4 - отображаем данные где для каждого светодиода Value в HSV умножаем на 0 или 1 для того, чтобы получились цифры на табло.
  */

  /*БАЗОВАЯ ПОСЛЕДОВАТЕЛЬНОСТЬ сегментов используемая в digit_to_segment_converter.
    [0] сегмент - нижний-центральный.
    [1] сегмент - средний-центральный.
    [2] сегмент - верхний-центральный.
    [3] сегмент - нижний-левый.
    [4] сегмент - верхний-левый.
    [5] сегмент - нижний-правый.
    [6] сегмент - верхный-правый.
  
    ┌───[2]───┐
    │         │
    [4]     [6]
    │         │
    ├───[1]───┤
    │         │
    [3]     [5]
    │         │
    └───[0]───┘
  */

  byte ClockArray_main [Clock_Leds_Amount];        // основной массив для отображения данных (результат сложения 13 цифр + точка, двоеточие, процент) //
  bool clock_halfLED_turn_off = true;              // выключиет половину светодиодов в цифрах, чтобы снизить яркость //

  #define CLOCK_EMPTY_NUMBER 10                    // выключает все сегменты.
  #define CLOCK_FILL_NUMBER  8                     // цифра 8 и есть FILL.

  class class_Clock                                // класс часов //
  {
    public:
      class_Clock(byte key, unsigned int array_first_index, byte array_length, bool is_it_symbol)               // конструктор класса //
      {
        _key_ID = key;                                       // ключ смещения цифр относительно описанного определения выше на 15 строк //
        _array_first_index = array_first_index;
        _array_length = array_length;                        // длина массива. для точки будет 1, для цифры 14 итд. необходимо, чтобы потом складывать массивы //
        _is_it_symbol = is_it_symbol;
      }
      
      void set_cur_number(byte cur_number)                   // от 0 до 9 = цифры от 0 до 9  //  10 = EMPTY  //
      {
        _cur_number = constrain(cur_number, 0, 10);          // защита от дурака. проверяет, чтобы точно не выйти за границу массива, когда будем использовать эту перменную в методе update //
      }

      void update()
      { 
        byte buf_array [14];

        for (int i = 0; i < 7; i++)                          // примитивный дешифратор, где _key_ID это индекс строки в массиве _key_ARRAY, на которой лежит ключ//
        {    
          buf_array [i*2]     = _numbers_code_array [_cur_number][_key_ARRAY [_key_ID][i]];                 // ДЕШИФРАТОР - не пытайся разобраться! //
          buf_array [(i*2)+1] = _numbers_code_array [_cur_number][_key_ARRAY [_key_ID][i]];                 // ДЕШИФРАТОР - не пытайся разобраться! //
        }

        if (clock_halfLED_turn_off == true)                  // если включён режим половины светодиодов //
        {
          if (_is_it_symbol == false)                        // если это НЕ символ (двоеточие/процент/точка) //
          {
            if (object_NightTime.get_NightTimeState() == 2 || object_NightTime.get_NightTimeState() == 4)   // если влючен ночной режим //
            {
              for (int i = 0; i < 7; i++)
              {
                buf_array [i*2] = 0;
              }
            }
          }
        }

        for (int i = 0; i < _array_length; i++)              // полученный результат переносим в основной массив ClockArray_main //
        {
          jesse_yield_func();

          if (_array_first_index + i < Clock_Leds_Amount)    // проверка, на всякий случай, чтобы не выйти за границы массива Clock_Leds_Amount=191, последний индекс массива = 190 //
          {
            ClockArray_main [_array_first_index + i] = buf_array [i];
          }
        }
      }

    private:
      byte _key_ID;
      unsigned int _array_first_index;
      byte _array_length;
      bool _is_it_symbol;

      byte _cur_number;

      static constexpr byte _numbers_code_array [11][7] =        // цифры в семисегментном формате //
      {
        {    1, 0, 1, 1, 1, 1, 1,},                          // 0
          /* 0  1  2  3  4  5  6
          ┌───[2]───┐
          │         │
          [4]     [6]
          │         │
          ├─────────┤
          │         │
          [3]     [5]
          │         │
          └───[0]───┘
          */
        {    0, 0, 0, 0, 0, 1, 1,},                          // 1
          /* 0  1  2  3  4  5  6
          ┌─────────┐
          │         │
          │       [6]
          │         │
          ├─────────┤
          │         │
          │       [5]
          │         │
          └─────────┘
          */
        {    1, 1, 1, 1, 0, 0, 1,},                          // 2
          /* 0  1  2  3  4  5  6
          ┌───[2]───┐
          │         │
          │       [6]
          │         │
          ├───[1]───┤
          │         │
          [3]       │
          │         │
          └───[0]───┘
          */
        {    1, 1, 1, 0, 0, 1, 1,},                          // 3
          /* 0  1  2  3  4  5  6
          ┌───[2]───┐
          │         │
          │       [6]
          │         │
          ├───[1]───┤
          │         │
          │       [5]
          │         │
          └───[0]───┘
          */
        {    0, 1, 0, 0, 1, 1, 1,},                          // 4
          /* 0  1  2  3  4  5  6
          ┌─────────┐
          │         │
          [4]     [6]
          │         │
          ├───[1]───┤
          │         │
          │       [5]
          │         │
          └─────────┘
          */
        {    1, 1, 1, 0, 1, 1, 0,},                          // 5
          /* 0  1  2  3  4  5  6
          ┌───[2]───┐
          │         │
          [4]       │
          │         │
          ├───[1]───┤
          │         │
          │       [5]
          │         │
          └───[0]───┘
          */
        {    1, 1, 1, 1, 1, 1, 0,},                          // 6
          /* 0  1  2  3  4  5  6
          ┌───[2]───┐
          │         │
          [4]       │
          │         │
          ├───[1]───┤
          │         │
          [3]     [5]
          │         │
          └───[0]───┘
          */
        {    0, 0, 1, 0, 0, 1, 1,},                          // 7
          /* 0  1  2  3  4  5  6
          ┌───[2]───┐
          │         │
          │       [6]
          │         │
          ├─────────┤
          │         │
          │       [5]
          │         │
          └─────────┘
          */
        {    1, 1, 1, 1, 1, 1, 1,},                          // 8
          /* 0  1  2  3  4  5  6
          ┌───[2]───┐
          │         │
          [4]     [6]
          │         │
          ├───[1]───┤
          │         │
          [3]     [5]
          │         │
          └───[0]───┘
          */
        {    1, 1, 1, 0, 1, 1, 1,},                          // 9
          /* 0  1  2  3  4  5  6
          ┌───[2]───┐
          │         │
          [4]     [6]
          │         │
          ├───[1]───┤
          │         │
          │       [5]
          │         │
          └───[0]───┘
          */
        {    0, 0, 0, 0, 0, 0, 0,},                          // empty
          /* 0  1  2  3  4  5  6
          ┌─────────┐
          │         │
          │         │
          │         │
          ├─────────┤
          │         │
          │         │
          │         │
          └─────────┘
          */
      };

      static constexpr byte _key_ARRAY [6][7] =                  // ключи смещения цифр //
      {
        {    0, 1, 2, 3, 4, 5, 6,},                              // прямая последовательность для точки, двоеточия, процента
          /* ↑  ↑  ↑  ↑  ↑  ↑  ↑
          .  0  1  2  3  4  5  6
          ┌───[2]───┐
          │         │
          [4]     [6]
          │         │
          ├───[1]───┤
          │         │
          [3]     [5]
          │         │
          └───[0]───┘
          */
        {    5, 0, 3, 1, 6, 2, 4,},                              // смещение сегментов (цифры 1, 2)
          /* ↑  ↑  ↑  ↑  ↑  ↑  ↑
          .  0  1  2  3  4  5  6
          ┌───[2→5]───┐
          │           │
          [4→6]   [6→4]
          │           │
          ├───[1→3]───┤
          │           │
          [3→2]   [5→0]
          │           │
          └───[0→1]───┘
          */
        {    1, 5, 0, 3, 4, 2, 6,},                              // смещение сегментов (цифра 3)
          /* ↑  ↑  ↑  ↑  ↑  ↑  ↑
          .  0  1  2  3  4  5  6
          ┌───[2→5]───┐
          │           │
          [4→4]   [6→6]
          │           │
          ├───[1→0]───┤
          │           │
          [3→3]   [5→1]
          │           │
          └───[0→2]───┘
          */
        {    4, 2, 6, 5, 0, 3, 1,},                              // смещение сегментов (цифры 4, 5)
          /* ↑  ↑  ↑  ↑  ↑  ↑  ↑
          .  0  1  2  3  4  5  6
          ┌───[2→1]───┐
          │           │
          [4→0]   [6→2]
          │           │
          ├───[1→6]───┤
          │           │
          [3→5]   [5→3]
          │           │
          └───[0→4]───┘
          */
        {    5, 0, 3, 4, 2, 6, 1,},                              // смещение сегментов (цифры 6, 7, 8, 9)
          /* ↑  ↑  ↑  ↑  ↑  ↑  ↑
          .  0  1  2  3  4  5  6
          ┌───[2→4]───┐
          │           │
          [4→3]   [6→5]
          │           │
          ├───[1→6]───┤
          │           │
          [3→2]   [5→0]
          │           │
          └───[0→1]───┘
          */
        {    4, 2, 6, 5, 0, 3, 1,},                              // смещение сегментов (цифры 10, 11, 12, 13)
          /* ↑  ↑  ↑  ↑  ↑  ↑  ↑
          .  0  1  2  3  4  5  6
          ┌───[2→1]───┐
          │           │
          [4→0]   [6→2]
          │           │
          ├───[1→6]───┤
          │           │
          [3→5]   [5→3]
          │           │
          └───[0→4]───┘
          */
      };
  };

  /* РИСУНОК ЧАСОВ (расположение лед ленты)

    ┌→[9]→[10]→[COLON]→[11]→→→→→[12]→→→
    ↑
    └←[8]←←[7]←←←←←←←←←[6]←←←←←←←[5]←←┐
    .                                 ↑
    ┌→→→→→→[2]→→→→→→→→→[3]→[DOT]→[4]→→┘
    ↑
    └←[1]←←[0]←←←←←←←←←[PERCENTAGE]←←←←

  */

  class_Clock object_clock_PERCENTAGE (0, 0,  6,   true);      // символ процента  // index 0-5
  class_Clock object_clock_0          (1, 6,  14,  false);     // Влажность        // index 6-19
  class_Clock object_clock_1          (1, 20, 14,  false);     // Влажность        // index 20-33
  class_Clock object_clock_2          (2, 34, 14,  false);     // Температура      // index 34-47
  class_Clock object_clock_3          (3, 48, 14,  false);     // Температура      // index 48-61
  class_Clock object_clock_DOT        (0, 62, 1,   true);      // символ точки     // index 62
  class_Clock object_clock_4          (3, 63, 14,  false);     // температура      // index 63-76
  class_Clock object_clock_5          (4, 77, 14,  false);     // СО2              // index 77-90
  class_Clock object_clock_6          (4, 91, 14,  false);     // СО2              // index 91-104
  class_Clock object_clock_7          (4, 105, 14, false);     // СО2              // index 105-118
  class_Clock object_clock_8          (4, 119, 14, false);     // СО2              // index 119-132
  class_Clock object_clock_9          (5, 133, 14, false);     // время            // index 133-146
  class_Clock object_clock_10         (5, 147, 14, false);     // время            // index 147-162
  class_Clock object_clock_COLON      (0, 161, 2,  true);      // символ двоеточия // index 161-162
  class_Clock object_clock_11         (5, 163, 14, false);     // время            // index 163-176
  class_Clock object_clock_12         (5, 177, 14, false);     // время            // index 177-190

  bool clock_night_indication_time        = true;  // ВРЕМЯ       // флаг отображения ночью на табло 
  bool clock_night_indication_co2         = false; // СО2         // ↑↑↑
  bool clock_night_indication_temperature = false; // ТЕМПЕРАТУРА // ↑↑↑
  bool clock_night_indication_humidity    = false; // ВЛАЖНОСТЬ   // ↑↑↑

  void clock_master()                              // мастер функция для часов вызывающая другие функции и методы. //
  {
    clock_string_to_array_converter();

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
      object_clock_PERCENTAGE.set_cur_number(CLOCK_EMPTY_NUMBER); // выключает подсветку % //
      object_clock_0.set_cur_number(CLOCK_EMPTY_NUMBER);
      object_clock_1.set_cur_number(CLOCK_EMPTY_NUMBER);
    }
    else
    {
      object_clock_PERCENTAGE.set_cur_number(CLOCK_FILL_NUMBER);                                      // включает подсветку % //
      object_clock_0.set_cur_number(buf2_hum % 10);
      buf2_hum /= 10;
      object_clock_1.set_cur_number(buf2_hum % 10);
    }

    if (buf2_temp == 0)                                           // если температура при синхронизации не пришла и равна 0, то выключает индикацию температуры //
    {
      object_clock_DOT.set_cur_number(CLOCK_EMPTY_NUMBER);                                              // выключает подсчетку точки //
      object_clock_4.set_cur_number(CLOCK_EMPTY_NUMBER);
      object_clock_3.set_cur_number(CLOCK_EMPTY_NUMBER);
      object_clock_2.set_cur_number(CLOCK_EMPTY_NUMBER);
    }
    else
    {
      object_clock_DOT.set_cur_number(CLOCK_FILL_NUMBER);                                              // включает подсветку точки //
      object_clock_4.set_cur_number(buf2_temp % 10);
      buf2_temp /= 10;
      object_clock_3.set_cur_number(buf2_temp % 10);
      buf2_temp /= 10;
      object_clock_2.set_cur_number(buf2_temp % 10);
    }

    if (buf2_co2 == 0)                                            // если со2 при синхронизации не пришла и равна 0, то выключает индакацию со2 //
    {
      object_clock_5.set_cur_number(CLOCK_EMPTY_NUMBER);
      object_clock_6.set_cur_number(CLOCK_EMPTY_NUMBER);
      object_clock_7.set_cur_number(CLOCK_EMPTY_NUMBER);
      object_clock_8.set_cur_number(CLOCK_EMPTY_NUMBER);
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
        object_clock_8.set_cur_number(CLOCK_EMPTY_NUMBER);
      }
      
      else
      {
        object_clock_8.set_cur_number(buf2_co2 % 10);
      }
    }

    /*
    if (object_TimeDate.get_HOUR() < 10)                                            // если меньше 10 утра, то выключает первую цифру, чтобы получить 8:55 вместо 08:55 //
    {
      object_clock_9.set_cur_number(CLOCK_EMPTY_NUMBER);
    }

    else
    {
      object_clock_9.set_cur_number(object_TimeDate.get_HOUR()/10);
    }
    */

    object_clock_9.set_cur_number(object_TimeDate.get_HOUR()/10);
    object_clock_10.set_cur_number(object_TimeDate.get_HOUR()%10);
    object_clock_COLON.set_cur_number(CLOCK_FILL_NUMBER);
    object_clock_11.set_cur_number(object_TimeDate.get_MIN()/10);
    object_clock_12.set_cur_number(object_TimeDate.get_MIN()%10);

    if (object_NightTime.get_NightTimeState() == 2 || object_NightTime.get_NightTimeState() == 4)               // если влючен ночной режим, то проверяем какие данные выводим //
    {
      if (val_HSV_clock_cur == val_HSV_clock_night)                             // проверяем закончилось ли плавное диммирование яркости и потом выключаем лишние данные //
      {
        if (clock_night_indication_time == false)
        {
          object_clock_COLON.set_cur_number(CLOCK_EMPTY_NUMBER);                                             // выключает подсветку двоеточия //
          object_clock_9.set_cur_number(CLOCK_EMPTY_NUMBER);                  // выключает цифры //
          object_clock_10.set_cur_number(CLOCK_EMPTY_NUMBER);                 // ↑↑↑ //
          object_clock_11.set_cur_number(CLOCK_EMPTY_NUMBER);                 // ↑↑↑ //
          object_clock_12.set_cur_number(CLOCK_EMPTY_NUMBER);                 // ↑↑↑ //
        }

        if (clock_night_indication_co2 == false)
        {
          object_clock_5.set_cur_number(CLOCK_EMPTY_NUMBER);                  // выключает цифры //
          object_clock_6.set_cur_number(CLOCK_EMPTY_NUMBER);                  // ↑↑↑ //
          object_clock_7.set_cur_number(CLOCK_EMPTY_NUMBER);                  // ↑↑↑ //
          object_clock_8.set_cur_number(CLOCK_EMPTY_NUMBER);                  // ↑↑↑ //
        }

        if (clock_night_indication_temperature == false)
        {
          object_clock_DOT.set_cur_number(CLOCK_EMPTY_NUMBER);                                               // выключает подсчетку точки //
          object_clock_4.set_cur_number(CLOCK_EMPTY_NUMBER);                  // выключает цифры //
          object_clock_3.set_cur_number(CLOCK_EMPTY_NUMBER);                  // ↑↑↑ //
          object_clock_2.set_cur_number(CLOCK_EMPTY_NUMBER);                  // ↑↑↑ //
        }

        if (clock_night_indication_humidity == false)
        {
          object_clock_PERCENTAGE.set_cur_number(CLOCK_EMPTY_NUMBER);                                         // выключает подсветку % //
          object_clock_0.set_cur_number(CLOCK_EMPTY_NUMBER);                   // выключает цифры //
          object_clock_1.set_cur_number(CLOCK_EMPTY_NUMBER);                   // ↑↑↑ //
        }
      }
    }
  }

  void clock_animation()                           // анимация часов  //
  {
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

    if (val_HSV_clock_cur <= val_HSV_clock_night)                              // если яркость меньше или равна ночному режиму, значит сейчас ночной режим. анимации отключена, управление по RGB. //
    {
      for(int i = 0; i < Clock_Leds_Amount; i++)
      {
        array_LED_clock [i] = CRGB((RGB_clock_night [0]*val_HSV_clock_cur*ClockArray_main [i]),(RGB_clock_night [1]*val_HSV_clock_cur*ClockArray_main [i]),(RGB_clock_night [2]*val_HSV_clock_cur*ClockArray_main [i]));
      }

      FastLED.delay(30);
      Animation::state = Animation::NONE;
    }

    else if (hue_HSV_clock_day_random != hue_HSV_clock_day_random_buf)         // если цвет поменялся с начала функции - значит нужна анимация по HSV //
    {
      if (object_TimeDate.get_MIN() % 2 > 0)                                   // анимация снизу вверх каждую нечетную минуту
      {
        Animation::state = Animation::CLOCK_UP_PART_1;
      }

      else                                                        // анимация сверху вниз каждую четную минуту
      {
        Animation::state = Animation::CLOCK_DOWN_PART_1;
      }
    }

    else                                                          // во всех других случаях - анимация отключена, режим HSV //
    {
      for(int i = 0; i < Clock_Leds_Amount; i++)
      {
        array_LED_clock [i] = CHSV(hue_HSV_clock_day_random, sat_HSV_clock_day, (val_HSV_clock_cur*ClockArray_main [i]));
      }
      FastLED.delay(30);
      Animation::state = Animation::NONE;
    }
  }

  void UP_clock_func_part_1()
  {
    Animation::state = Animation::CLOCK_UP_PART_2;
    Animation::counter = 0;
  }

  void UP_clock_func_part_2()
  {
    if(Animation::counter < Clock_Leds_Amount)
    {
      array_LED_clock [Animation::counter] = CHSV(hue_HSV_clock_day_random, sat_HSV_clock_day, (val_HSV_clock_cur*ClockArray_main [Animation::counter]));
      FastLED.delay(20);
      Animation::counter++;
    }

    else
    {
      Animation::state = Animation::NONE;
    }
  }

  void DOWN_clock_func_part_1()
  {
    Animation::state = Animation::CLOCK_DOWN_PART_2;
    Animation::counter = Clock_Leds_Amount - 1;
  }

  void DOWN_clock_func_part_2()
  {
    if(Animation::counter >= 0)
    {
      array_LED_clock [Animation::counter] = CHSV(hue_HSV_clock_day_random, sat_HSV_clock_day, (val_HSV_clock_cur*ClockArray_main [Animation::counter]));
      FastLED.delay(20);
      Animation::counter--;
    }

    else
    {
      Animation::state = Animation::NONE;
    }
  }

  String clock_indication()                      // отображение текущего состояния отображения данных на табло в текстовом виде //
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

/// ↓↓↓ SD карта

  #include <SPI.h>                                 // библиотека для SD карты //   
  #include <SD.h>                                  // ↑↑↑ //  

  File myFile;

  void LOG_write_sync_data()                       // запись лога //
  {
    if (!SD.begin(4))
    {
      send_alert("ERROR: Ошибка инициализации SD карты");
    }

    jesse_yield_func();

    myFile = SD.open(object_TimeDate.get_DateFULL() + ".txt", FILE_WRITE);

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

  void check_log_timer()
  {
    static bool flag_every_day_log_timer;                                                   // флаг для отправки лога раз в сутки //
    if (flag_every_day_log_timer == false && object_TimeDate.get_TimeB() > 234500)          // ТАЙМЕР отправки лога в 23:45 //
    {
      log_read_and_send(true);
      flag_every_day_log_timer = true;
    }


    if (flag_every_day_log_timer == true && object_TimeDate.get_TimeB() < 234400)           // Возвращает флаг обратно, чтобы лог отправился на следующий день // 
    {    
      flag_every_day_log_timer = false;
    }
  }

  void log_read_and_send(bool is_it_timer)
  {
    obj_stopwatch_ms_Send_Log.start();
    if (!SD.begin(4))
    {
      send_alert("ERROR: Ошибка инициализации SD карты");
    }

    String file_name = object_TimeDate.get_DateFULL() + ".txt";
    jesse_yield_func();
    myFile = SD.open(file_name);

    if (myFile)
    {
      fb::File f(file_name, fb::File::Type::document, myFile);

      jesse_yield_func();

      if(is_it_timer == true)
      {
        String buf_user_ID = object_array_users[USERS::ME].get_id();
        f.chatID = buf_user_ID;
        bot_second.sendFile(f, false);
      }
      
      else
      {
        String buf_user_ID = object_array_users[users_array_index].get_id();
        f.chatID = buf_user_ID;
        bot_main.sendMessage(fb::Message("Ожидайте...\nОтправляю файл: " + file_name, object_array_users[users_array_index].get_id()));
        delay(20);
        bot_main.sendFile(f, false);
        delay(20);
        bot_main.sendMessage(fb::Message("Отправка файла завершена.", object_array_users[users_array_index].get_id()));
      }

      myFile.close();
    }

    else
    {
      send_alert("ERROR: Ошибка чтения SD карты");
    }
    obj_stopwatch_ms_Send_Log.stop();
  }

/// ↓↓↓ Телеграм

  void Message_command_get_data(String text)     // вызывается из Common CODE файла, из функции Message_from_Telegram_converter() //
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
          object_array_users[USERS::GUEST].set_id(text);
          object_array_users[users_array_index].send_message("Теперь я буду отвечать на входящие запросы с ID: " + text);
          object_array_users[USERS::GUEST].send_message("Я проснулся.");
        }
        break;
      }
    }
    object_array_users[users_array_index].set_message_state(1);
  }

  void Message_command_send_data(int text_int)   // вызывается из Common CODE файла, из функции Message_from_Telegram_converter() //
  {
    jesse_yield_func();

    switch (text_int)
    {
      case 101:                                                       // запрос лога за текущий день //
      {
        log_read_and_send(false);
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
        buf_message += "\n\n/112100 - красный";
        buf_message += "\n/112010 - зелёный";
        buf_message += "\n/112001 - синий";

        buf_message += "\n\n/112110 - жёлтый";
        buf_message += "\n/112011 - лазурный";
        buf_message += "\n/112101 - маджента";
        
        buf_message += "\n\n/112111 - белый";

        buf_message += "\n\n/112211 - пудровый розовый";
        buf_message += "\n/112121 - светло-бирюзовый";
        buf_message += "\n/112112 - светло-васильковый";
        
        buf_message += "\n\n/112221 - светло-жёлтый";
        buf_message += "\n/112122 - небесный светло-голубой";
        buf_message += "\n/112212 - лавандовый розовый";

        buf_message += "\n\n/112123 - небесный голубой";
        buf_message += "\n/112223 - небесный пурпурный";
        buf_message += "\n/112322 - нежный розовый";

        buf_message += "\n\n\nВвести вручную RGB цвет - /11299";
        object_array_users[users_array_index].send_message(buf_message);
        break;
      }

      case 112100:                                                     // ↑↑↑ // Красный //
      {
        set_clock_night_color(1, 0, 0);
        clock_master();
        break;
      }

      case 112010:                                                     // ↑↑↑ // Зеленый //
      {
        set_clock_night_color(0, 1, 0);
        clock_master();
        break;
      }

      case 112001:                                                     // ↑↑↑ // Синий //
      {
        set_clock_night_color(0, 0, 1);
        clock_master();
        break;
      }

      case 112110:                                                     // ↑↑↑ // Желтый //
      {
        set_clock_night_color(1, 1, 0);
        clock_master();
        break;
      }

      case 112011:                                                     // ↑↑↑ // Лазурный //
      {
        set_clock_night_color(0, 1, 1);
        clock_master();
        break;
      }

      case 112101:                                                     // ↑↑↑ // Маджента //
      {
        set_clock_night_color(1, 0, 1);
        clock_master();
        break;
      }

      case 112111:                                                     // ↑↑↑ // Белый //
      {
        set_clock_night_color(1, 1, 1);
        clock_master();
        break;
      }

      case 112211:                                                     // ↑↑↑ // пудровый розовый //
      {
        set_clock_night_color(2, 1, 1);
        clock_master();
        break;
      }

      case 112121:                                                     // ↑↑↑ // светло-бирюзовый //
      {
        set_clock_night_color(1, 2, 1);
        clock_master();
        break;
      }

      case 112112:                                                     // ↑↑↑ // светло-васильковый //
      {
        set_clock_night_color(1, 1, 2);
        clock_master();
        break;
      }

      case 112221:                                                     // ↑↑↑ // светло-жёлтый //
      {
        set_clock_night_color(2, 2, 1);
        clock_master();
        break;
      }

      case 112122:                                                     // ↑↑↑ // небесный светло-голубой //
      {
        set_clock_night_color(1, 2, 2);
        clock_master();
        break;
      }

      case 112212:                                                     // ↑↑↑ // лавандовый розовый //
      {
        set_clock_night_color(2, 1, 2);
        clock_master();
        break;
      }

      case 112123:                                                     // ↑↑↑ // небесный голубой //
      {
        set_clock_night_color(1, 2, 3);
        clock_master();
        break;
      }

      case 112223:                                                     // ↑↑↑ // небесный пурпурный //
      {
        set_clock_night_color(2, 2, 3);
        clock_master();
        break;
      }

      case 112322:                                                     // ↑↑↑ // нежный розовый //
      {
        set_clock_night_color(3, 2, 2);
        clock_master();
        break;
      }

      case 11299:                                                     // ручной выбор цвета часов ночью //
      {
        object_array_users[users_array_index].send_message("Отправьте 3 цифры для установки цвета часов ночью (текущее значение: " + String(RGB_clock_night [0]) + String(RGB_clock_night [1]) + String(RGB_clock_night [2]) + "). Первая цифра RED, вторая BLUE, третья GREEN. \n\n\n*Параметр яркости часов ночью является коэфициентов для RGB цвета.");
        object_array_users[users_array_index].set_message_state(112);
        break;
      }

      case 113:                                                       // ВКЛ/ВЫКЛ режим halfLED //
      {
        clock_halfLED_turn_off = !clock_halfLED_turn_off;

        if (clock_halfLED_turn_off == true)
        {
          object_array_users[users_array_index].send_message("Включён режим halfLED ночью");
        }
        else
        {
          object_array_users[users_array_index].send_message("Отключен режим halfLED ночью");
        }

        clock_master();
        break;
      }

      case 180:                                                       // ВКЛ/ВЫКЛ ERROR анимации //
      {
        ERROR_animation_manual_off = !ERROR_animation_manual_off;
        
        if (ERROR_animation_manual_off == true)
        {
          object_array_users[users_array_index].send_message("Отключена Анимация ошибок днём (ночью всегда отключена)");
        }
        else
        {
          object_array_users[users_array_index].send_message("Включена Анимация ошибок днём (ночью всегда отключена)");
        }

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
    obj_stopwatch_ms_Main_LOOP.start();                                 // для подсчета времени лупа //

    free_heap::tick();

    object_TimeDate.update_TimeDate();                                           // получаем актуальное время с сервера //
  
    object_NightTime.update_NightTime();                                         // обновляем состояние ночного режима //

    bot_tick_and_call_debug();                                                   // update telegram - получение сообщения из телеги и их обработка // внутри вызывается debug и .tick //

    if (global_ERROR_flag == true && Animation::state == Animation::NONE)        // проверят были ли ошибки и если были - делает анимацию //
    {
      if (object_NightTime.get_NightTimeState() == 1 && ERROR_animation_manual_off == false)  // 1 значит - днём можно включить ночной режим, чтобы отключить анимацию ошибки во время отладки // 1 - выкл, 2 - вкл до след утра , 3 - в нейтральном положение, чтобы сработало сразу при включение, 4 -вкл , 5 - выкл до след вечера //
      {    
        Animation::state = Animation::ERROR_PART_0;                              // если были ошибки и сейчас день, без включенного ночного режима - запускаем анимацию ошибки //
      }
      global_ERROR_flag = false;                                                 // в любом случае сбрасываем флаг ошибки, чтобы если ночью была ошибка, то утром первым делом она не захерачила анимацию //     
    }

    Led_Animation_Check();

    if(object_TimeDate.get_MIN() % 2 > 0 && flag_every_minute_timer == false)    // таймер каждую нечетную минуту //
    {
      check_log_timer(); 
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

    obj_stopwatch_ms_Main_LOOP.stop();
  }
//Tools > MMU 16KB cache + 48KB IRAM and 2nd HEAP (shared)
//Tools > Stack Protection Enable
//NodeMCU 1.0 (ESP-12E Module)

#define THIS_IS_CHAT_CODE
#define Jesse_yield_enable                       // delay(0) и yield() одно и тоже... и то и то даёт возможность ESP в эти прерывания обработать wi-fi и внутренний код // https://arduino.stackexchange.com/questions/78590/nodemcu-1-0-resets-automatically-after-sometime //
//#define FB_USE_LOG Serial


#include "A:\1 - important\PROJECTS\Arduino\!Climate_Control\! GEN 8\Gen_8_ver_014\Common_CODE.cpp"

/// ↓↓↓ Синхронизация ошибок

  bool global_ERROR_flag = false;                  // Флаг для ошибок (1 - нет ошибок, 2 - в цикле были ошибки) // ps в какой-то момент появилось подозрение, что где-то в биоблетаках уже есть ErrorSTATE и возникают ошибки //

/// ↓↓↓ Датчики температуры - ds18b20

  #include <OneWire.h>                             // библиотека для датчиков температуры //
  #include <DallasTemperature.h>                   // ↑↑↑ //

  OneWire OneWire(D5);                             // Определяем пин, на котором висят датчики температуры //
  DallasTemperature ds(&OneWire);                  // ↑↑↑ //

  float b23;                                       // Эффективность рекуперации на приток //
  float b45;                                       // Эффективность рекуперации на вытяжку //
  float real_efficiency;

  float b62;                                       // Теплопотери притока (если заслонки от батареи закрыты) //
  float b82;                                       // Теплопотери воздухувода от кровати до рекуператора и нагрев вентилятором //

  float b61;                                       // Процент воздуха с улицы //
  float b71;                                       // Процент воздуха с батареи //

  class class_ds18b20                              // класс датчиков температура ds18b20 //
  {
    public:
      class_ds18b20(bool alert_flag, byte ar0, byte ar1, byte ar2, byte ar3, byte ar4, byte ar5, byte ar6, byte ar7, String name, float crit_temp_high_error)  // конструктор класса //
      {
        _crit_temp_low_alert  = 0.0;
        _crit_temp_low_error  = -45.0;
        _crit_temp_high_error = crit_temp_high_error;

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
        jesse_yield_func();

        _temp = ds.getTempC(_array_address);
        _second_temp_request();                  // Очень редко (раз в несколько дней) какой-то датчик может дать неправильные показания. Возможно эта функция избавит от лишних уведомлений об "ошибках".

        _check_alerts();
        _check_errors();   
      }

      float get_temp()
      {
        return(_temp);
      }
      
      void set_crit_temp_low_alert(String crit_temp_low_alert)
      {
        float buf_crit_temp_low_alert = crit_temp_low_alert.toFloat();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        _crit_temp_low_alert = constrain(buf_crit_temp_low_alert, -55, 55);
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
        if (_alert_flag == true)
        {
          if (_crit_temp_low_alert > _temp)
          {
            send_alert("ALERT: " + _name + String(_temp) + "°C" );
            global_ERROR_flag = true;
          }
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

      void _second_temp_request()                  // если датчик не прислал показания пробуем запросить еще раз. При Ошибке выдаёт -127.00 //
      {
        unsigned long millis_local_timer = millis();
        while(_temp < -120)
        {
          _temp = ds.getTempC(_array_address);
          delay(20);

          if(millis() - millis_local_timer > 5000)
          {
            return;
          }
        }
      }
  };

  class_ds18b20 object_ds18b20_0(true,  0x28, 0x76, 0x6A, 0x39, 0x0, 0x0, 0x0, 0x43, "Рекуператор Приток (in): ", 45);
  class_ds18b20 object_ds18b20_1(false, 0x28, 0x5C, 0x4E, 0x3C, 0x0, 0x0, 0x0, 0x5C, "Рекуператор Приток (out): ", 45);
  class_ds18b20 object_ds18b20_2(false, 0x28, 0xAD, 0x18, 0x3A, 0x0, 0x0, 0x0, 0x19, "Рекуператор Вытяжка (in): ", 45);
  class_ds18b20 object_ds18b20_3(true,  0x28, 0x27, 0xB5, 0x3E, 0x0, 0x0, 0x0, 0xED, "Рекуператор Вытяжка (out): ", 45);
  class_ds18b20 object_ds18b20_4(false, 0x28, 0xB6, 0x76, 0x39, 0x0, 0x0, 0x0, 0x81, "Воздуховод c Улицы: ", 45);
  class_ds18b20 object_ds18b20_5(false, 0x28, 0xF,  0x2A, 0x3A, 0x0, 0x0, 0x0, 0x2C, "Воздуховод с Батареи: ", 45);
  class_ds18b20 object_ds18b20_6(false, 0x28, 0x5E, 0x81, 0x39, 0x0, 0x0, 0x0, 0xFE, "Объединенный поток: ", 45);
  class_ds18b20 object_ds18b20_7(false, 0x28, 0x8C, 0x6B, 0x39, 0x0, 0x0, 0x0, 0x33, "Батарея: ", 105);

/// ↓↓↓ Управление заслонками

  float temp_thermostat_target_for_air_dumpers = 23.0;     // температура термостата для режима заслонки //
  float temp_thermostat_target_for_recuperator =  1.0;     // температура термостата для режима рекуперации (чтобы предотвратить обмерзание) //
  float temp_thermostat_range                  = 0.15;     // чувствительность термостата //
  int   Step_Per_loop                          =   10;     // количество шагов двигателя за цикл запуска термостата //

  bool use_recuperator;                            // флаг режима рекуператор/заслонки //
  #define recuperator_button A0                    // пин выключателя режима //
  #define dir_UP             LOW                   // чтобы не путаться, поскольку low это 0 вольт, а high это 3,3 вольта //
  #define dir_DOWN           HIGH                  // ↑↑↑ //

  const byte pin_DIR        = 16;    //D0          // общий пин для смены direction моторов. HIGH - Вниз , LOW - Вверх //
  const byte pin_knob_LOW   =  0;    //D3          // нижние концевики  //
  const byte pin_knob_HIGH  =  2;    //D4          // верхние концевики //
  const byte pin_step_SREET = 12;    //D6          // пин для управления шагами двигателя заслонки с улицы //
  const byte pin_step_HOME  = 13;    //D7          // пин для управления шагами двигателя заслонки от батареи //

  namespace motor_calibration
  {
    enum
    {
      idle  = 0,
      full  = 1,
      quick = 2,
    }state;
  }

  const int street_LOWEST_position_const = -2912;                                     // НИЖНЯЯ точка плюс отступ (константа для проверки отклонения от первоначальных данных) //
  const int home_LOWEST_position_const   =  3188;                                     // ВЕРХНЯЯ точка плюс отступ (константа для проверки отклонения от первоначальных данных) //

  const int street_low_space     = 70;                                                // отступ, чтобы нельзя было полностью закрыть заслонку с улицы и всегда был минимальный приток //
  int street_LOWEST_position_cur = street_LOWEST_position_const + street_low_space;   // нижняя точка плюс отступ (выставляется после калибровки set_LOW_limit_home) //
  int home_LOWEST_position_cur   = home_LOWEST_position_const;                        // нижняя точка плюс отступ (выставляется после калибровки set_LOW_limit_street) //


  namespace air_dumpers_fast_change_position
  {
    enum
    {
      IDLE         = 0,
      CLOSE_STREET = 1,
      OPEN_STREET  = 2,
    }state = air_dumpers_fast_change_position::IDLE;
  }

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
      class_motor_plus_knobs(byte pin_step, int low_space, int LOWEST_position) : class_motor(pin_step)
      {
        _low_space = low_space;                     // отступ, чтобы нельзя было полностью закрыть заслонку с улицы и всегда был минимальный приток //
        _LOWEST_position = LOWEST_position;
      }

      bool go_to_HIGH_limit()                       // Калибровка заслонки с улицы верхнее положение //
      {
        int steps_counter_limit4000 = 4000;                                      // считаем количество шагов, чтобы не уйти в бесконечный цикл, если заклинит двигатель //
        _position_before_calibration = 0;

        digitalWrite(pin_DIR, dir_UP);
        while (digitalRead(pin_knob_HIGH) == 1 && steps_counter_limit4000 > 0)  // Идём вверх пока не сработает кнопка //
        {
          do_8_microsteps();
          steps_counter_limit4000--;
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

      bool go_to_LOW_limit()                       // Калибровка заслонки с улицы нижнее положение (быстрая калбировка) //
      {
        int steps_counter_limit4000 = 4000;                                      // считаем количество шагов, чтобы не уйти в бесконечный цикл, если заклинит двигатель //
        _position_before_calibration = _LOWEST_position;

        digitalWrite(pin_DIR, dir_DOWN);
        while (digitalRead(pin_knob_LOW) == 1 && steps_counter_limit4000 > 0)   // Идём вниз пока не сработает кнопка //
        {
          do_8_microsteps();
          steps_counter_limit4000--;
          _position_before_calibration--;
        }

        digitalWrite(pin_DIR, dir_UP);
        for (int i = 0; i < _knob_space; i++)                                    // Отступаем обратно //
        {
          do_8_microsteps();
          _position_before_calibration++;
        }

        for (int i = 0; i < _low_space; i++)                                     // отступ, чтобы нельзя было полностью закрыть заслонку с улицы и всегда был минимальный приток //
        {
          do_8_microsteps();
          _position_before_calibration++;
        }

        return(_check_limit4000(steps_counter_limit4000));
      }    

      bool set_LOW_limit()                         // Калибровка заслонки с улицы нижнее положение (полноценная калибровка) //
      {
        _LOWEST_position = 0;
        int steps_counter_limit4000 = 4000;                                      // считаем количество шагов, чтобы не уйти в бесконечный цикл, если заклинит двигатель //
        _position_before_calibration = 0;
    
        digitalWrite(pin_DIR, dir_DOWN);
        while (digitalRead(pin_knob_LOW) == 1 && steps_counter_limit4000 > 0)    // Идём вниз пока не сработает кнопка //
        {
          do_8_microsteps();
          _LOWEST_position++;                                                    // _steps_GLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
          steps_counter_limit4000--;
          _position_before_calibration++;
        }

        digitalWrite(pin_DIR, dir_UP);
        for (int i = 0; i < _knob_space; i++)                                    // Отступаем обратно от концевика //
        {
          do_8_microsteps();
          _LOWEST_position--;                                                    // _steps_GLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
          _position_before_calibration--;
        }

        for (int i = 0; i < _low_space; i++)                                     // отступ, чтобы нельзя было полностью закрыть заслонку с улицы и всегда был минимальный приток //
        {
          do_8_microsteps();
          _LOWEST_position--;                                                    // _steps_GLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
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
      const byte _knob_space = 110;                // отступ от концевиков //
      int _low_space;                              // отступ, чтобы нельзя было полностью закрыть заслонку с улицы и всегда был минимальный приток //

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

  class_motor_plus_knobs object_STREET_motor_plus_knobs(pin_step_SREET, street_low_space, 0 - street_LOWEST_position_cur);
  class_motor_plus_knobs object_HOME_motor_plus_knobs  (pin_step_HOME, 0, home_LOWEST_position_cur);

  class class_motor_main
  {
    public:
      class_motor_main()
      {
      }

      void doXsteps_func(int steps_amount, bool do_i_need_to_count_steps)
      {
        int buf_doXsteps_counter = _doXsteps_counter;

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
          motor_calibration::state = motor_calibration::full;
          global_ERROR_flag = true;
        }

        if (do_i_need_to_count_steps == false)
        {
          _doXsteps_counter = buf_doXsteps_counter;
        }
      }

      void calibrate_test(bool is_it_reboot)            // проверяет нужна ли калибровка и если нужна делает. так же по таймеру, раз в сутки, запускает быструю калибровыку //
      {
        int buf_steps_GLOBAL = _steps_GLOBAL;                          // сохраняем положение заслонок до калибровки
        int street_motor_position_before_calibration;                  // необходимо на случай, если ESP перезагрузилась и после калибровки нужно вернуть заслонки
        int home_motor_position_before_calibration;                    // ↑↑↑
        

        if(motor_calibration::state == 2 && _calibrate_ERROR == false)                                                        // быстрая калибровка //
        {
          object_array_users[USERS::ME].send_message_second_chat("Начинаю процесс быстрой калибровки.");

          if(digitalRead(pin_knob_LOW) == 1 && digitalRead(pin_knob_HIGH) == 1)
          {
            if(use_recuperator == true)
            {
              _calibrate_ERROR                         = object_STREET_motor_plus_knobs.go_to_HIGH_limit();
              street_motor_position_before_calibration = object_STREET_motor_plus_knobs.get_position_before_calibration();
              _calibrate_ERROR                         = object_HOME_motor_plus_knobs.go_to_LOW_limit();
              home_motor_position_before_calibration   = object_HOME_motor_plus_knobs.get_position_before_calibration();
              _steps_GLOBAL = home_LOWEST_position_cur;
            }

            else
            {
              _calibrate_ERROR                         = object_STREET_motor_plus_knobs.go_to_LOW_limit();
              street_motor_position_before_calibration = object_STREET_motor_plus_knobs.get_position_before_calibration();
              _calibrate_ERROR                         = object_HOME_motor_plus_knobs.go_to_HIGH_limit();
              home_motor_position_before_calibration   = object_HOME_motor_plus_knobs.get_position_before_calibration();          
              _steps_GLOBAL = street_LOWEST_position_cur;
            }

            object_array_users[USERS::ME].send_message_second_chat("Быстрая калибровка выполнена.\n\nТекущее положение заслонок: " + String(_steps_GLOBAL));
          }
          
          else
          {
            send_alert("Быстрая калибровка невозможна. Концевик нажат.");
            motor_calibration::state = motor_calibration::full;
          }
        }

        if(motor_calibration::state == motor_calibration::full && _calibrate_ERROR == false)                                  // полноценная калибровка //
        {
          object_array_users[USERS::ME].send_message_second_chat("Начинаю процесс полноценной калибровки.");

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
            object_array_users[USERS::ME].send_message_second_chat("Верхнее положение заслонки с улицы откалибровано.");
          }

          if (_calibrate_ERROR == false)
          {
            _calibrate_ERROR = object_HOME_motor_plus_knobs.go_to_HIGH_limit();    // Производим калибровку второй раз на случай, если была зажата противоположенный концевик! //
            object_array_users[USERS::ME].send_message_second_chat("Верхнее положение заслонки от батареи откалибровано.");
          }
          

          if (_calibrate_ERROR == false)                                              // калибровка нижнего положения STREET //
          {
            _calibrate_ERROR = object_STREET_motor_plus_knobs.set_LOW_limit();

            street_LOWEST_position_cur = object_STREET_motor_plus_knobs.get_LOWEST_position();

            int cal_dif1 = street_LOWEST_position_const - street_LOWEST_position_cur;
            object_array_users[USERS::ME].send_message_second_chat("Нижнее положожение заслонки с улицы откалибровано. Результат: " + String(street_LOWEST_position_cur) + "\n\nОтклонение от константного значения: " + String(cal_dif1));

            _calibrate_ERROR = object_STREET_motor_plus_knobs.go_to_HIGH_limit();          // открываем заслонку обратно //

            object_array_users[USERS::ME].send_message_second_chat("Положение заслонки с улицы возвращено в открытое положение. Текущее положение _steps_GLOBAL: " + String(_steps_GLOBAL));
          }

          if (_calibrate_ERROR == false)                                              // калибровка нижнего положения HOME //
          {
            _calibrate_ERROR = object_HOME_motor_plus_knobs.set_LOW_limit();

            home_LOWEST_position_cur = object_HOME_motor_plus_knobs.get_LOWEST_position();

            _steps_GLOBAL = home_LOWEST_position_cur;

            int cal_dif2 = home_LOWEST_position_const - home_LOWEST_position_cur;
            object_array_users[USERS::ME].send_message_second_chat("Нижнее положожение заслонки от батареи откалибровано. Результат: " + String(home_LOWEST_position_cur) + "\n\nОтклонение от константного значения: " + String(cal_dif2));
          }
        }

        if (motor_calibration::state == motor_calibration::full || motor_calibration::state == motor_calibration::quick)      // завершающий процесс любой калибровки //
        {
          if (_calibrate_ERROR == false)                                         // если нет ошибок - возвращаем положение заслонок в положение до калибровки //
          {
            int buf_steps_amount = 0;

            if(is_it_reboot == false)
            {
              buf_steps_amount = buf_steps_GLOBAL - _steps_GLOBAL;
            }

            else
            {
              buf_steps_amount = 0 - _steps_GLOBAL + home_motor_position_before_calibration - street_motor_position_before_calibration;
            }
            
            doXsteps_func(buf_steps_amount, false);
            motor_calibration::state = motor_calibration::idle;
            object_array_users[USERS::ME].send_message_second_chat("Положение заслонок возвращено в положение до калибровки. Текущее положение _steps_GLOBAL: " + String(_steps_GLOBAL));
          }

          if (_calibrate_ERROR == true)                                          // если есть ошибки - отправляем уведомление о невозможности калибровки //
          {
            global_ERROR_flag = true;
            send_alert("ERROR: Калибровка невозможна - заклинил двигатель или не работают концевики.");
          }
        }

        if (_daily_calibrate_flag == true && object_TimeDate.get_TimeB() > 150101 && object_TimeDate.get_TimeB() < 152929)    // автоматическая калибровка раз в сутки в 14:01 //
        {
          motor_calibration::state = motor_calibration::quick;
          object_array_users[USERS::ME].send_message_second_chat("Отправил запрос на быструю калибровку по таймеру (раз в сутки в период с 15:00 до 15:30).");
          _daily_calibrate_flag = false;
        }

        if (_daily_calibrate_flag == false)                                      // поднимаем флаг калибровки //
        {
          if (object_TimeDate.get_TimeB() < 145959 || object_TimeDate.get_TimeB() > 153131)
          {
            _daily_calibrate_flag = true;
          }
        }

        if (_step_counter_Flag == 1 && object_TimeDate.get_TimeB() > 234500)     // отправка отчета по количеству пройденых шагов за день //
        {
          _step_counter_Flag = 0;
          object_array_users[USERS::ME].send_message_second_chat("Шагов сделано за сегодня (без учета калибровки и ручного управления): " + String(_doXsteps_counter));
          _doXsteps_counter = 0;
        }

        if (_step_counter_Flag == 0 && object_TimeDate.get_TimeB() < 234400)     // возвращает флаг обратно, чтобы отчет отправился на следующий день //
        {
          _step_counter_Flag = 1;
        }
      }

      String getMotorPositions()                        // создает стринг с текстовым описанием положения заслонок как в абсолютном значении, так и в процентах //
      {
        int position_street = 0;
        int position_home = 0;

        if (_steps_GLOBAL < 0)
        {
          position_street = map(_steps_GLOBAL, 0, street_LOWEST_position_cur - street_low_space, 0, 100);
        }

        if (_steps_GLOBAL > 0)
        {
          position_home = map(_steps_GLOBAL, 0, home_LOWEST_position_cur, 0, 100);
        }

        String Message_Motor_Positions =  "\n\nКрайнее нижнее положение заслонки от батареи: "        + String(home_LOWEST_position_const) +\
                                          "\nТекущее положение заслонок(_steps_GLOBAL*): "            + String(_steps_GLOBAL) +\
                                          "\nКрайнее нижнее положение заслонки с улицы: "             + String(street_LOWEST_position_cur) +\
                                          "\n\n*Заслонка с улицы закрыта на "                         + String(position_street) +\
                                          "%.\n*Заслонка от батареи закрыта на "                      + String(position_home) +\
                                          "%.\n\n**Шагов сделано за сегодня (без учета калибровки и ручного управления): " + String(_doXsteps_counter);

        return (Message_Motor_Positions);
      }

      int get_steps_GLOBAL()
      {
        return(_steps_GLOBAL);
      }

      long get_doXsteps_counter()
      {
        return(_doXsteps_counter);
      }
    private:
      bool _calibrate_ERROR;
      bool _daily_calibrate_flag = false;               // флаг для таймера автоматической калибровки раз в сутки // стоит false, чтобы если включить ESP с 14 до 19 - не сработало две калибровки подряд (1 раз при включени и еще одна по таймеру) //
      bool _step_counter_Flag = 1;                      // флаг таймера для отправки отчета по количеству пройденных шагов за сутки // Нужно, чтобы быстро понять, не делают ли заслонки лишних движений летом //
      int _doXsteps_counter = 0;                       // счетчик количества шагов сделанных за день //
      int _steps_GLOBAL = home_LOWEST_position_const;   // одна переменная для определения положения ОБЕИХ заслонок // _steps_GLOBAL - от 0 вверх - "батарея", от 0 вниз = "улица" //
  };

  class_motor_main object_motor_main;

  void recuperator_button_check(bool send_message_anyway)
  {
    static unsigned long millis_timer_button_check;

    if(millis() - millis_timer_button_check > 100)
    {
      if(analogRead(recuperator_button) > 975)       // Режим - ЗАСЛОНКИ
      {
        if(use_recuperator == true || send_message_anyway == true)
        {
          String recuperator_info_message_2  = "\n\n*В режиме Заслонки стоит выставить скорость приточного вентилятора 4 или 5.";
                recuperator_info_message_2 += " Скорость вытяжного вентилятор 0 или 1";

          use_recuperator = false;
          send_alert("Выбран режим вентиляции: Заслонки." + recuperator_info_message_2);
          air_dumpers_fast_change_position::state = air_dumpers_fast_change_position::CLOSE_STREET;
        }
      }

      else if(analogRead(recuperator_button) < 50)        // Режим - РЕКУПЕРАТОР
      {
        if(use_recuperator == false || send_message_anyway == true)
        {
        String recuperator_info_message  = "\n\n*В режиме Рекуператор стоит увеличивать количество приточного воздуха,";
              recuperator_info_message += " чтобы было положительное давление в квартире и пыль/запахи не затягивало из щелей в стенах.\n";
              recuperator_info_message += " Скорость выставлять так, чтобы КПД рекуперации не вытяжку было на 10-20% больше, чем на приток.";

        use_recuperator = true;
        send_alert("Выбран режим вентиляции: Рекуператор." + recuperator_info_message);
        air_dumpers_fast_change_position::state = air_dumpers_fast_change_position::OPEN_STREET;
        }
      }

      millis_timer_button_check = millis();
    }
  }

  void move_air_dumpers_fast()                    // Быстрое закрытие или открытие заслонок при изменение режима вентиляции //
  {
    if(air_dumpers_fast_change_position::state == air_dumpers_fast_change_position::OPEN_STREET)
    {
      if(object_motor_main.get_steps_GLOBAL() != home_LOWEST_position_cur)
      {
        object_motor_main.doXsteps_func(Step_Per_loop, true);                  // положительные значения открываем улицу, закрываем батарею //          
      }

      else
      {
        air_dumpers_fast_change_position::state = air_dumpers_fast_change_position::IDLE;
      }
    }
    
    if(air_dumpers_fast_change_position::state == air_dumpers_fast_change_position::CLOSE_STREET)
    {
      if(object_motor_main.get_steps_GLOBAL() != street_LOWEST_position_cur)
      {
        object_motor_main.doXsteps_func(0 - Step_Per_loop, true);                  // положительные значения открываем улицу, закрываем батарею //          
      }

      else
      {
        air_dumpers_fast_change_position::state = air_dumpers_fast_change_position::IDLE;
      }      
    }
  }

/// ↓↓↓ Датчик температуры и влажности SHT41 + PWM/ШИМ увлажнителя

  #include "Adafruit_SHT4x.h"

  const byte pin_PWM_Humidifier = 15;   // D8 - пин ШИМ/PWM для шагового двигателя увлажнителя.

  Adafruit_SHT4x sht4 = Adafruit_SHT4x();

  float room_humidity_range  =  2.00;              // чувствительность +- включения выключения увлажнителя //
  float room_humidity_target = 40.00;              // желаемая влажность //

  float room_conditioner_temp_range  = 0.3;        // чувствительность +- включения выключения "охладителя" //
  float room_conditioner_temp_target = 23.7;       // желаемая температура летом //
  bool  room_conditioner_flag = true;

  class class_SHT41                                // класс датчика SHT41 температуры и влажности //
  {
    public:
      class_SHT41()                                // конструктор класса //
      {
        _humidity_low_alert = 15;
        _humidity_high_alert = 80;
        _crit_temp_low_error = -45.00;
        _crit_temp_high_error = 45.00;      
      }

      void update()                                // обновление данных с датчика //
      {
        jesse_yield_func();

        sensors_event_t humidity, temp;                // запрашиваем температуру и влажность адафрут //
        sht4.getEvent(&humidity, &temp);               // обновляем показания температуры и влажность //

        _temp = temp.temperature;
        _humidity = humidity.relative_humidity;

        jesse_yield_func();

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
        float buf_humidity_low_alert = humidity_low_alert.toFloat();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        _humidity_low_alert = constrain(buf_humidity_low_alert, 0, 100);
        object_array_users[users_array_index].send_message("Оповещения будут приходить, если влажность в квартире опустится ниже: " + String(_humidity_low_alert) + "%");
      }

      void set_humidity_high_alert (String humidity_high_alert)
      {
        float buf_humidity_high_alert = humidity_high_alert.toFloat();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        _humidity_high_alert = constrain(buf_humidity_high_alert, 0, 100);
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
        if ((14 > int(_temp)) || (int(_temp) > _crit_temp_high_error))                         // Если датчик SHT41 отключен от питание значение будет 0 //
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

  class_SHT41 object_Temp_Humidity_sensor;         // создаем экземпляр класса SHT41 (объект) //

/// ↓↓↓ Термостат

  bool new_mode_thermostat = true;
  unsigned long thermostat_air_dumpers_timer_millis;

  void thermostat()                              // термостат //
  {
    if(use_recuperator == false)                 // режим заслонок //
    {
      if(new_mode_thermostat == false)           // старый режим термостата //
      {
        if (object_ds18b20_0.get_temp() < (temp_thermostat_target_for_air_dumpers - temp_thermostat_range))        // Если рекуператор приток (in) меньше ...
        {
          object_motor_main.doXsteps_func((0 - Step_Per_loop), true);    // отрицательные значения открываем батарею, закрываем улицу //
        }

        else if (object_ds18b20_0.get_temp() > (temp_thermostat_target_for_air_dumpers + temp_thermostat_range))   // Если рекуператор приток (in) больше ...
        {             
          object_motor_main.doXsteps_func(Step_Per_loop, true);          // положительные значения открываем улицу, закрываем батарею //
        }
      }

      else                                       // новый режим термостата //
      {
        if(millis() - thermostat_air_dumpers_timer_millis > 1000 * 60 * 15)
        {
          if (object_Temp_Humidity_sensor.get_temp() < (temp_thermostat_target_for_air_dumpers - temp_thermostat_range))        // Если температура в комнате меньше ...
          {
            object_motor_main.doXsteps_func((0 - Step_Per_loop), true);    // отрицательные значения открываем батарею, закрываем улицу //
          }

          else if (object_Temp_Humidity_sensor.get_temp() > (temp_thermostat_target_for_air_dumpers + temp_thermostat_range))   // Если температура в комнате больше ...
          {             
            object_motor_main.doXsteps_func(Step_Per_loop, true);          // положительные значения открываем улицу, закрываем батарею //
          }
        }
      }
    }

    else                                         // режим рекупетора //
    {
      if (object_ds18b20_0.get_temp() < (temp_thermostat_target_for_recuperator - temp_thermostat_range))        // Если рекуператор приток (in) меньше ...
      {
        object_motor_main.doXsteps_func((0 - Step_Per_loop), true);    // отрицательные значения открываем батарею, закрываем улицу //
      }

      else if (object_ds18b20_0.get_temp() > (temp_thermostat_target_for_recuperator + temp_thermostat_range))   // Если рекуператор приток (in) больше ...
      {             
        object_motor_main.doXsteps_func(Step_Per_loop, true);          // положительные значения открываем улицу, закрываем батарею //
      }
    }
  }
/// ↓↓↓ Увлажнитель и "охладитель"

  void humidifier()                                // увлажнитель //
  {
    if(room_conditioner_flag == false)
    {
      if (object_Temp_Humidity_sensor.get_humidity() < (room_humidity_target - room_humidity_range))
      {
        analogWrite(pin_PWM_Humidifier, 127);                          // от 0 до 255 // при старте увлажнитель выключен //
      }

      else if (object_Temp_Humidity_sensor.get_humidity() > (room_humidity_target + room_humidity_range))
      {
        analogWrite(pin_PWM_Humidifier, 0);                            // от 0 до 255 // при старте увлажнитель выключен //
      }
    }

    else
    {
      if (object_Temp_Humidity_sensor.get_humidity() < (room_humidity_target - room_humidity_range) || object_Temp_Humidity_sensor.get_temp() > (room_conditioner_temp_target + room_conditioner_temp_range))
      {
        analogWrite(pin_PWM_Humidifier, 127);                          // от 0 до 255 // при старте увлажнитель выключен //
      }

      else if (object_Temp_Humidity_sensor.get_humidity() > (room_humidity_target + room_humidity_range) && object_Temp_Humidity_sensor.get_temp() < (room_conditioner_temp_target - room_conditioner_temp_range))
      {
        analogWrite(pin_PWM_Humidifier, 0);                            // от 0 до 255 // при старте увлажнитель выключен //
      }      
    }
  }

/// ↓↓↓ Датчик СО2 - SCD41

  #include <SensirionI2CScd4x.h>
  #include <Wire.h>

  SensirionI2CScd4x scd4x;

  class class_SCD41                                      // класс датчика СО2 //
  {
    public:
      class_SCD41()
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
        object_array_users[USERS::ME].send_message("Начинаю процесс принудительной рекалибровки. Это займет 5 минут.");

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

          jesse_yield_func();

          error_co2 = scd4x.readMeasurement(_CO2, buf_temperature_co2, buf_humidity_co2);
          if (error_co2)
          {
            send_alert("ERROR: не смог получить показания со2.");
            _CO2 = 0;
            global_ERROR_flag = true;
          }

          jesse_yield_func();

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
          send_alert("ERROR уровень со2: " + String(_CO2) + "ppm" + "\n\n\n Если ошибка повторяется - попробуйте перезапуск /35101@" + This_bot_name);
          global_ERROR_flag = true;
        }
      }

      void _check_alerts()
      {
        if (_CO2 > _co2_high_alert)
        {
          send_alert("ALERT уровень со2: " + String(_CO2) + "ppm" +\
                    "\n\nИзменить температуру термостата (если снизить температуру термостата - колчество воздуха с улицы увеличится и со2 снизиться): /104@" + This_bot_name +\
                    "\n\nНастройка оповещений: /103@" + This_bot_name);
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
          object_array_users[USERS::ME].send_message("ОСТАНОВИЛ периодическое измерение в датчике СО2.");
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
          object_array_users[USERS::ME].send_message("Выставил ALTITUDE (высоту над уровнем моря): " + String(_altitude) + "метров.");
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
          object_array_users[USERS::ME].send_message("ЗАПУСТИЛ периодическое измерение в датчике СО2.");
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
          object_array_users[USERS::ME].send_message("Рекалибровка прошла успешно. \n\nРезультаты:\n Текущее значение СО2: " + String(targetCo2Concentration) + "ppm.\n Коррекция: " + String(Correction_in_ppm) + "ppm." + "\n\n frcCorrection: " + String(frcCorrection));
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
          object_array_users[USERS::ME].send_message("Сброс на заводские настройки прошёл успешно.");
        }
      }
  };

  class_SCD41 object_CO2_sensor;                         

/// ↓↓↓ Обновление показаний датчиков

  namespace sensors_update
  {
    enum
    {
      NONE   = 0,
      PART_0 = 1,
      PART_1 = 2,
      PART_2 = 3,
      PART_3 = 4,
      PART_4 = 5,
    } state;

    unsigned long millis_timer;
    int counter = 0;
    unsigned long last_update;
  }

  void update_sensors_data_and_calculations()
  {
    jesse_yield_func();

    switch(sensors_update::state)
    {
      case sensors_update::NONE:
      {
        break;
      }

      case sensors_update::PART_0:
      {
        update_sensors_part_0();
        break;
      }
      
      case sensors_update::PART_1:
      {
        update_sensors_part_1();
        break;
      }

      case sensors_update::PART_2:
      {
        update_sensors_part_2();
        break;
      }

      case sensors_update::PART_3:
      {
        update_sensors_part_3();
        break;
      }

      case sensors_update::PART_4:
      {
        update_sensors_part_4();
        break;
      }            
    }
  }

  void update_sensors_part_0()
  {
    ds.setWaitForConversion(false);       // makes it async
    ds.requestTemperatures();             // запрашиваем температуру со всех датчиков ds18b20 сразу, а дальше только получаем данные //
    ds.setWaitForConversion(true);

    sensors_update::state = sensors_update::PART_1;
  }

  void update_sensors_part_1()
  {
    object_CO2_sensor.update();           // СО2 // хоть и вызываем update, но если не прошло 40 секунд - показание не обновит и выдаст последние значение. //
    object_Temp_Humidity_sensor.update(); // занимает 10ms //

    sensors_update::state = sensors_update::PART_2;
    sensors_update::millis_timer = millis();
  }

  void update_sensors_part_2()
  {
    if(millis() - sensors_update::millis_timer > 1000)
    {
      sensors_update::counter = 0;
      sensors_update::state = sensors_update::PART_3;
    }
  }

  void update_sensors_part_3()
  {
    switch(sensors_update::counter)
    {
      case 0:
        object_ds18b20_0.update();            // Рекуператор Приток (in) //
        sensors_update::counter++;
        break;

      case 1:
        object_ds18b20_1.update();            // Рекуператор Приток (out) //
        sensors_update::counter++;
        break;

      case 2:
        object_ds18b20_2.update();            // Рекуператор Вытяжка (in) //
        sensors_update::counter++;
        break;

      case 3:
        object_ds18b20_3.update();            // Рекуператор Вытяжка (out) //
        sensors_update::counter++;
        break;

      case 4:
        object_ds18b20_4.update();            // Воздуховод c Улицы //
        sensors_update::counter++;
        break;

      case 5:
        object_ds18b20_5.update();            // Воздуховод с Батареи //
        sensors_update::counter++;
        break;

      case 6:
        object_ds18b20_6.update();            // Объединенный поток //
        sensors_update::counter++;
        break;

      case 7:
        object_ds18b20_7.update();            // Батарея //
        sensors_update::counter = 0;
        sensors_update::state = sensors_update::PART_4;
        break;
    }
  }

  void update_sensors_part_4()
  {
    b62 = object_ds18b20_0.get_temp() - object_ds18b20_4.get_temp();                        // Теплопотери притока (если заслонки от батареи закрыты) //
    b82 = object_ds18b20_0.get_temp() - object_ds18b20_6.get_temp();                        // Теплопотери воздухувода от кровати до рекуператора и нагрев вентилятором //

    float buf_c1 = (object_ds18b20_2.get_temp() - object_ds18b20_0.get_temp());

    calculations_b23(buf_c1);            // Эффективность рекуперации на приток //
    calculations_b45(buf_c1);            // Эффективность рекуперации на вытяжку //
    calculations_real_efficiency(buf_c1);// Эффективность рекуперации на приток с учетом теплопотерь //

    calculations_b61_b71();              // Процент воздуха с улицы   и   Процент воздуха с батареи //

    sensors_update::last_update = millis();
    sensors_update::state = sensors_update::NONE;
  }

  void calculations_b23(float buf_c1)              // расчет b23 (Эффективность рекуперации на приток) //
  {
    b23 = 0;

    if (buf_c1 < -1 || buf_c1 > 1)                                          // чтобы не считать КПД при разницах Притока и вытяжки меньше 1°C //
    {
      if (object_ds18b20_2.get_temp() != object_ds18b20_0.get_temp())       // Исключить деление на 0 //
      {
        if (object_ds18b20_1.get_temp() != object_ds18b20_0.get_temp())     // Исключить деление на 0 //
        {
          b23 = 100 / (object_ds18b20_2.get_temp() - object_ds18b20_0.get_temp()) * (object_ds18b20_1.get_temp() - object_ds18b20_0.get_temp());  // ЭФФЕКТИВНОСТЬ РЕКУПЕРАЦИИ НА ПРИТОК //
        }      
      }
    }
  }

  void calculations_b45(float buf_c1)              // расчет b45 (Эффективность рекуперации на вытяжку) // 
  {
    b45 = 0;

    if (buf_c1 < -1 || buf_c1 > 1)                                          //  Чтобы не считать КПД при разницах Притока и вытяжки меньше 1°C //
    {
      if (object_ds18b20_2.get_temp() != object_ds18b20_0.get_temp())       // Исключить деление на 0 //
      {
        if (object_ds18b20_2.get_temp() != object_ds18b20_3.get_temp())     // Исключить деление на 0 //
        {
          b45 = 100 / (object_ds18b20_2.get_temp() - object_ds18b20_0.get_temp()) * (object_ds18b20_2.get_temp() - object_ds18b20_3.get_temp());  // ЭФФЕКТИВНОСТЬ РЕКУПЕРАЦИИ НА ВЫТЯЖКУ //
        }
      }
    }
  }

  void calculations_real_efficiency(float buf_c1)  // КПД притока с учетом теплопотерь //
  {
    real_efficiency = 0;

    if (buf_c1 < -1 || buf_c1 > 1)                                          // чтобы не считать КПД при разницах Притока и вытяжки меньше 1°C //
    {
      float buf_added_heat_loss = b62 / 2;                                                                  // примерные теплопотери от шкафа до вытяжки //
      float buf_delta = object_ds18b20_2.get_temp() - object_ds18b20_4.get_temp() + buf_added_heat_loss;    // Вытяжка (in) - воздуховод с улицы + примерные теплопотери вытяжного канала //
      float buf_heat_recovery = object_ds18b20_1.get_temp() - object_ds18b20_0.get_temp();                  // Приток (out) - Приток (in) //

      if(buf_delta != 0)
      {
        real_efficiency = 100 / buf_delta * buf_heat_recovery;
      }
    }
  }

  void calculations_b61_b71()                      // расчет b61 (Процент воздуха с улицы) и b71 (Процент воздуха с батареи) //
  {
    b61 = 0;
    b71 = 0;

    if(object_motor_main.get_steps_GLOBAL() != home_LOWEST_position_const)            // заслонка от батареи не закрыта
    {
      if(object_ds18b20_6.get_temp() > object_ds18b20_4.get_temp())                   // объединенный поток теплее воздуха с улицы 
      {
        if(object_ds18b20_6.get_temp() < object_ds18b20_5.get_temp())                 // объединенный поток холоднее воздуха с батареи
        {
          if(object_Temp_Humidity_sensor.get_temp() > object_ds18b20_4.get_temp())    // в квартире теплее, чем на улице
          {
            if(object_ds18b20_5.get_temp() != object_ds18b20_4.get_temp())            // исключить деление на 0
            {
              b61 = ((object_ds18b20_5.get_temp() - object_ds18b20_6.get_temp()) / (object_ds18b20_5.get_temp() - object_ds18b20_4.get_temp())) * 100;    // ПРОЦЕНТ ВОЗДУХА С УЛИЦЫ
              b71 = ((object_ds18b20_6.get_temp() - object_ds18b20_4.get_temp()) / (object_ds18b20_5.get_temp() - object_ds18b20_4.get_temp())) * 100;    // ПРОЦЕНТ ВОЗДУХА С БАТАРЕИ
            }
          }
        }
      }
    }
  }

/// ↓↓↓ Вывод показаний в текстовом виде

  String TEMP_text_output()                        // Команда вывода данных в текстовом виде //
  {
    jesse_yield_func();

    int air_exchange_caclulated = 0;
    if(object_CO2_sensor.get_CO2() > 410)                                             // чтобы исключить значение на 0 и не показывать нереалистичные значения //
    {
      air_exchange_caclulated = 18000 / (object_CO2_sensor.get_CO2() - 400);          // этот коэффициент взят из таблички расчета со2 и актуален для одного человека находящегося в покое //
    }
    unsigned long buf_lust_update_in_sec = (millis() - sensors_update::last_update) / 1000;

    String Message = ("Показаний датчиков: " + String(buf_lust_update_in_sec) + " секунд(ы) назад.\n\n" +\
                      "Температура в комнате: " + String(object_Temp_Humidity_sensor.get_temp()) +\
                      "°C\nВлажность в комнате: " + String(object_Temp_Humidity_sensor.get_humidity()) +\
                      "\nСодержание СО2 в воздухе: " + String(object_CO2_sensor.get_CO2()) +\
                      "\nПримерный воздухообмен*: " + String(air_exchange_caclulated) +\
                      "м³\n*Расчет на одного человека, в состояние покоя. Для более-менее правильного расчета нужно несколько часов, из-за инертности вентиляции." +\
                      "\n\nРекуп. Приток (in): " + String(object_ds18b20_0.get_temp()) +\
                      "°C\nPeкyп. Приток (out): " + String(object_ds18b20_1.get_temp()) +\
                      "°C\nPeкyп. Вытяжка (in): " + String(object_ds18b20_2.get_temp()) +\
                      "°C\nPeкyп. Вытяжка (out): " + String(object_ds18b20_3.get_temp()) +\
                      "°C\n\nKПД на вытяжку: " + String(b45) +\
                      "%\nKПД на приток: " + String(b23) +\
                      "%\nРеальное КПД притока с учётом теплопотерь: " + String(real_efficiency) +\
                      "%\n\nВоздуховод с улицы: " + String(object_ds18b20_4.get_temp()) +\
                      "°C\nБатарея: " + String(object_ds18b20_7.get_temp()) +\
                      "°C\nВоздуховод с батареи: " + String(object_ds18b20_5.get_temp()) +\
                      "°C\nОбъeдинeнный поток: " + String(object_ds18b20_6.get_temp()) +\
                      "°C\n\nTeплoпoтepи от кровати до рекуп. и нагрев вентилятором: " + String(b82) +\
                      "°C\nTeплoпoтepи от улицы до рекуператора: " + String(b62) +\
                      "°C\n\n\n\n↓↓↓ТОЛЬКО ДЛЯ ХОЛОДОВ↓↓↓" + "\n\nBoздyxa c улицы: " + String(b61) +\
                      "%\nBoздyxa c батареи: " + String(b71) + "%");

    jesse_yield_func();

    return(Message);
  }

/// ↓↓↓ Вывод показаний excel/CSV

  String TEMP_excel_output()                       // Команда вывода данных для записи на SD карту и формирование лога для экселя(CSV/comma seperated value/значения записанные через запятую) //
  {
    jesse_yield_func();

    String Message = (String(object_Temp_Humidity_sensor.get_temp()) + "," +\
              String(object_Temp_Humidity_sensor.get_humidity()) + "," +\
              String(object_CO2_sensor.get_CO2()) + "," +\
              String(object_ds18b20_0.get_temp()) + "," +\
              String(object_ds18b20_1.get_temp()) + "," +\
              String(object_ds18b20_2.get_temp()) + "," +\
              String(object_ds18b20_3.get_temp()) + "," +\
              String(b45) + "," + String(b23) + "," +\
              String(real_efficiency) + "," +\
              String(object_ds18b20_4.get_temp()) + "," +\
              String(object_ds18b20_7.get_temp()) + "," +\
              String(object_ds18b20_5.get_temp()) + "," +\
              String(object_ds18b20_6.get_temp()) + "," +\
              String(b62) + "," + String(b82) + "," +\
              String(b61) + "," + String(b71) + "," +\
              String(object_motor_main.get_steps_GLOBAL()) + "," + String(object_motor_main.get_doXsteps_counter()));
    return(Message);
  }

/// ↓↓↓ Телеграм

  void Message_command_get_data(String text)     // вызывается из Common CODE файла, из функции Message_from_Telegram_converter() //
  {
    jesse_yield_func();

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

      case 104:                                  // установка температуры термостата для режима заслонок //
      {
        float buf_text_float = text.toFloat();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        temp_thermostat_target_for_air_dumpers = constrain(buf_text_float, 19, 26);
        object_array_users[users_array_index].send_message("Термостат для режима заслоноки установлен на температуру: " + String(temp_thermostat_target_for_air_dumpers) + "°C");
        break;      
      }

      case 105:                                  // установка чувствительности термостата //
      {
        float buf_text_float = text.toFloat();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        temp_thermostat_range = constrain(buf_text_float, 0.1, 6);
        object_array_users[users_array_index].send_message("Термостат включится/выключится при отклонение +- " + String(temp_thermostat_range) + "°C");
        break;   
      }

      case 306:                                  // установка температуры термостата для режима рекуператор //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {  
          float buf_text_float = text.toFloat();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
          temp_thermostat_target_for_recuperator = constrain(buf_text_float, -8, 8);
          object_array_users[users_array_index].send_message("Термостат для режима рекуператор установлен на температуру: " + String(temp_thermostat_target_for_recuperator) + "°C");
        }     
        break;
      }

      case 108:                                  // установка шага заслонок //
      {
        int calculation_buf2 = 999999;
        int buf_text_int = text.toInt();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        Step_Per_loop = constrain(buf_text_int, 0, 1000);  // Ограничивает диапазон возможных значений //
        if(Step_Per_loop != 0)
        {
          float calculation_buf = (home_LOWEST_position_cur - street_LOWEST_position_cur) / Step_Per_loop;
          calculation_buf2 = int(calculation_buf) * 2;
        }

        String buf = "Шаг изменения положения заслонок: " + String(Step_Per_loop) +\
                      "\n\n*Термостат запускается раз в две минуты и при отклонение от заданной температуры меняет положение заслонок. \n\n*Текущее значение количества шагов значит, что обе заслонки могут поменять своё положение на противоположенное за " + String(calculation_buf2) + " минут(ы).";
        object_array_users[users_array_index].send_message(buf);
        break;  
      }

      case 109:                                  // ручное управление заслонками //
      {
        int buf_text_int = text.toInt();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        int buf_steps_amount = constrain(buf_text_int, -10000, 10000);
        object_array_users[users_array_index].send_message("Принял количество шагов: " + String(buf_steps_amount));

        object_motor_main.doXsteps_func(buf_steps_amount, false);
        object_array_users[users_array_index].send_message(object_motor_main.getMotorPositions());
        break;  
      }

      case 111:                                  // ручная установка крайнего нижнего положения заслонки с улицы //
      {
        int buf_text_int = text.toInt();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        street_LOWEST_position_cur = constrain(buf_text_int, street_LOWEST_position_const, 0);
        if (object_motor_main.get_steps_GLOBAL() < street_LOWEST_position_cur)
        {
          object_motor_main.doXsteps_func((street_LOWEST_position_cur - object_motor_main.get_steps_GLOBAL()), false);
        }
        object_array_users[users_array_index].send_message("Установлено крайнее положение заслонки с улицы: " + String(street_LOWEST_position_cur));
        break;  
      }

      case 112:                                  // ручная установка крайнего нижнего положения заслонки от батареи //
      {
        int buf_text_int = text.toInt();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        home_LOWEST_position_cur = constrain(buf_text_int, 0, home_LOWEST_position_const);
        if (object_motor_main.get_steps_GLOBAL() > home_LOWEST_position_cur)
        {
          object_motor_main.doXsteps_func((home_LOWEST_position_cur - object_motor_main.get_steps_GLOBAL()), false);
        }
        object_array_users[users_array_index].send_message("Установлено крайнее положение заслонки от батареи: " + String(home_LOWEST_position_cur));
        break;  
      }

      case 113:                                  // Установка желаемой влажности //
      {
        float buf_text_float = text.toFloat();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        room_humidity_target = constrain(buf_text_float, 5, 70);
        float hum_buf_low = room_humidity_target - room_humidity_range;
        float hum_buf_high = room_humidity_target + room_humidity_range;

        String buf = "Установлена желаемая влажность: " + String(room_humidity_target) +\
                      "\n\n*Увлажнитель включится, когда влажность опуститься до: " + String(hum_buf_low) +\
                      "\nУвлажнитель выключится, когда влажность поднимется до: " +  String(hum_buf_high);
        object_array_users[users_array_index].send_message(buf);
        break;  
      }

      case 114:                                  // Установка чувствительности увлажнителя //
      {
        float buf_text_float = text.toFloat();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        room_humidity_range = constrain(buf_text_float, 1, 20);
        float hum_buf_low = room_humidity_target - room_humidity_range;
        float hum_buf_high = room_humidity_target + room_humidity_range;

        String buf = "Чувствительность увлажнителя установлена: " + String(room_humidity_range) +\
                      "\n\n*Увлажнитель включится, когда влажность опуститься до: " + String(hum_buf_low) +\
                      "\nУвлажнитель выключится, когда влажность поднимется до: " + String(hum_buf_high);
        object_array_users[users_array_index].send_message(buf);
        break;  
      }

      case 116:                                   // установка температуры охладителя //
      {
        float buf_text_float = text.toFloat();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        room_conditioner_temp_target = constrain(buf_text_float, 22, 30);
        object_array_users[users_array_index].send_message("Охладитель установлен на температуру: " + String(room_conditioner_temp_target) + "°C");
        break; 
      }

      case 117:                                   // установка чувствительности охладителя //
      {
        float buf_text_float = text.toFloat();                   // "Because of the way the constrain() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results."  https://www.arduino.cc/reference/en/language/functions/math/constrain/
        room_conditioner_temp_target = constrain(buf_text_float, 0.2, 6);
        object_array_users[users_array_index].send_message("Охладитель включится/выключится при отклонение +- " + String(room_conditioner_temp_target) + "°C");
        break;   
      }

      case 390:                                  // выбор гостевого чата вручную //
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

    switch (text_int)                            // 1** команды доступные всем, 3** команды требующие доступ администратора //
    {
      case 101:                                  // текущие температуры в текстовом виде //
      {
        object_array_users[users_array_index].send_message(TEMP_text_output() + "\n\n\n\n↓↓↓ЗАСЛОНКИ↓↓↓" + object_motor_main.getMotorPositions());
        break;
      }
      
      case 102:                                  // текущие температуры в формате excel //
      {
        object_array_users[users_array_index].send_message(TEMP_excel_output() + "\n\n*Дата и время добавляется LOG ботом, по этому её здесь нет.");
        break;
      }

      case 103:                                  // установка оповещений (ALERT) //
      { 
        String buf = "Установка оповещений: \n\n Температура нижняя граница для рекуператора (/10301@" + This_bot_name + "). Текущее значение: " + String(object_ds18b20_0.get_crit_temp_low_alert()) +\
                      "\n Влажность нижняя граница (/10302@" + This_bot_name + "). Текущее значение: " + String(object_Temp_Humidity_sensor.get_humidity_low_alert()) +\
                      "\n Влажность верхняя граница (/10303@" + This_bot_name + "). Текущее значение: " + String(object_Temp_Humidity_sensor.get_humidity_high_alert()) +\
                      "\n СО2 верхняя граница (/10304@" + This_bot_name + "). Текущее значение: " + String(object_CO2_sensor.get_co2_high_alert());
        object_array_users[users_array_index].send_message(buf);
        break;
      }

      case 10301:                                // ↑↑↑ // температура // 
      {
        String buf = "Отправьте сообщение для установки температуры, НИЖЕ которой будут приходить уведомления (ALERT) (от -55 до 55):\n\n   Текущее значение Рекуператор Приток (in): " + String(object_ds18b20_0.get_crit_temp_low_alert()) +\
                      "\n   Текущее значение Рекуператор Вытяжка (out): " + String(object_ds18b20_3.get_crit_temp_low_alert());
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(10301);
        break;
      }

      case 10302:                                // ↑↑↑ // влажность нижняя граница //      
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для установки влажности, НИЖЕ которой будут приходить уведомления (ALERT) (от 0 до 100):\n\nТекущее значение: " + String(object_Temp_Humidity_sensor.get_humidity_low_alert()));
        object_array_users[users_array_index].set_message_state(10302);
        break;            
      }

      case 10303:                                // ↑↑↑ // влажность верхняя граница //      
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для установки влажности, ВЫШЕ которой будут приходить уведомления (ALERT) (от 0 до 100):\n\nТекущее значение: " + String(object_Temp_Humidity_sensor.get_humidity_high_alert()));
        object_array_users[users_array_index].set_message_state(10303);
        break;            
      }

      case 10304:                                // ↑↑↑ // со2 // 
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение для установки СО2, ВЫШЕ котрого будут приходить уведомления (ALERT) (от 0 до 2500):\n\nТекущее значение: " + String(object_CO2_sensor.get_co2_high_alert()));
        object_array_users[users_array_index].set_message_state(10304);
        break;            
      }

      case 104:                                  // установка температуры термостата для режима заслонок //
      {
        String buf =  "Отправьте сообщение для установки температуры термостата для режима заслонки в °C (от 19 до 26):\n\nТекущее значение: " + String(temp_thermostat_target_for_air_dumpers) +\
                      "\n\n*Термостат в режиме заслонки управляет температурой по датчику температуры в комнате.";
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(104);
        break;
      }

      case 105:                                  // установка чувствительности термостата //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение дла установки чувствительности термостата в °C (от 0.1 до 6):\n\nТекущее значение: " + String(temp_thermostat_range));
        object_array_users[users_array_index].set_message_state(105);
        break;
      }

      case 306:                                  // установка температуры термостата для режима рекуператор //
      {
        String buf =  "Отправьте сообщение для установки температуры термостата для режима рекуператор в °C (от -8 до 8):\n\nТекущее значение: " + String(temp_thermostat_target_for_recuperator) +\
                      "\n\n*Термостат в режиме рекуператор управляет температурой по датчику Рекуператор Приток(in). \n\nЭта настройка нужна для предотвращения обмерзания рекуператора зимой.";
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(306);
        break;
      }      

      case 108:                                  // установка шага заслонок //
      {
        int calculation_buf2 = 999999;

        if(Step_Per_loop != 0)
        {
          float calculation_buf = (home_LOWEST_position_cur - street_LOWEST_position_cur) / Step_Per_loop;
          calculation_buf2 = int(calculation_buf) * 2;
        }

        String buf = "Отправьте сообщение для установки шага изменения положения заслонок (от 0 до 1000):\n\nТекущее значение: " + String(Step_Per_loop) +\
                      "\n\n*Термостат запускается раз в две минуты и при отклонение от заданной температуры меняет положение заслонок. \n\n*Текущее значение количество шагов значит, что обе заслонки могут поменять своё положение на противоположенное за " + String(calculation_buf2) + " минут(ы).";
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(108);
        break;
      }

      case 109:                                  // ручное управление заслонками //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение с количеством шагов от -10000 до 10000:" + object_motor_main.getMotorPositions());
        object_array_users[users_array_index].set_message_state(109);
        break;
      }

      case 110:                                  // ручная инициализация быстрой калибровки заслонок //
      {
        motor_calibration::state = motor_calibration::quick;
        object_array_users[users_array_index].send_message("Принял запрос на быструю калиброку.\n\n*Быстрая калибровка не сбрасывает крайние положения заслонок выставленные вручную.");
        object_motor_main.calibrate_test(false);
        break;
      }

      case 310:                                  // ручная инициализация полноценной калибровки заслонок //
      {
        object_array_users[users_array_index].send_message("Полноценная калибровка займет несколько минут. \n\n*Полноценная калибровка сбрасывает крайние положения заслонок выставленные вручную.\n\n\nПродолжить? - /31001@" + This_bot_name);
        break;
      }

      case 31001:                                // ручная инициализация полноценной калибровки заслонок // подтверждение //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          motor_calibration::state = motor_calibration::full;
          object_array_users[users_array_index].send_message("Принял запрос на полноценную калибровку.");
          object_motor_main.calibrate_test(false);
        }
        break;
      }

      case 111:                                  // ручная установка крайнего нижнего положения заслонки с улицы //
      {
        String buf = "Отправьте сообщение для установки крайнего нижнего положения заслонки с улицы (во избежание ошибок его невозможно выставить меньше, чем значение после калибровки).\n\nЗначение крайнего нижнего положения заслонки с улицы после калибровки: " + String(street_LOWEST_position_const) +\
                      "\nЗначение выставленное вручную: " + String(street_LOWEST_position_cur);
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(111);
        break;
      }

      case 112:                                  // ручная установка крайнего нижнего положения заслонки от батареи //
      {
        String buf = "Отправьте сообщение для установки крайнего нижнего положения заслонки от батареи (во избежание ошибок его невозможно выставить меньше, чем значение после калибровки).\n\nЗначение крайнего нижнего положения заслонки от батареи после калибровки: " + String(home_LOWEST_position_const) +\
                      "\nЗначение выставленное вручную: " + String(home_LOWEST_position_cur);
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(112);
        break;
      }

      case 113:                                  // Установка желаемой влажности //
      {
        String buf =  "Отправьте сообщение для установки желаемой влажности(от 5 до 70).\n\nТекущее значение: "    + String(room_humidity_target);
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(113);
        break;
      }

      case 114:                                  // Установка чувствительности увлажнителя //
      {
        String buf =  "Отправьте сообщение для установки чувствительности увлажнителя (от 1 до 20).\n\nТекущее значение: " + String(room_humidity_range);
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(114);
        break;
      }

      case 115:                                   // вкл/откл охладителя //
      {
        room_conditioner_flag = !room_conditioner_flag;
        if (room_conditioner_flag == true)
        {
          object_array_users[users_array_index].send_message("Охладитель включен.");
        }
        else
        {
          object_array_users[users_array_index].send_message("Охладитель отключен.");
        }
        break;
      }

      case 116:                                   // установка температуры охладителя //
      {
        String buf =  "Отправьте сообщение для установки температуры охладителя в °C (от 22 до 30):\n\nТекущее значение: " + String(room_conditioner_temp_target) +\
                      "\n\n*Охладитель управляет увлажнителем. (во время испарения воды (адиабатическое охлаждение/увлажнение) происходит незначительное охлаждение, может помочь немного снизить температуру летом)";
        object_array_users[users_array_index].send_message(buf);
        object_array_users[users_array_index].set_message_state(116);
        break;
      }

      case 117:                                   // установка чувствительности охладителя //
      {
        object_array_users[users_array_index].send_message("Отправьте сообщение дла установки чувствительности охладителя в °C (от 0.2 до 6):\n\nТекущее значение: " + String(room_conditioner_temp_range));
        object_array_users[users_array_index].set_message_state(117);
        break;
      }

      case 190:                                  // ВКЛ/ВЫКЛ Текстовых уведомлений //
      {
        object_array_users[users_array_index].set_alert_flag();
        break;
      }

      case 350:                                  // установка разрешения ds12b20 на 12bit //
      {
        object_array_users[users_array_index].send_message("Эта комана необходима в случае, если настройки датчиков температуры сбились и показывают значения с разрешением 9бит вместо 12бит (разрешение 9бит - 0,5, разрешение 12бит - 0.0625)\n\n\nПродолжить? - /35001@" + This_bot_name);
        break;
      }

      case 35001:                                // ↑↑↑ // Подтверждение //
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
            object_ds18b20_7.set_res_to_12_bit();
        }
        break;
      }

      case 351:                                  // перезапуск датчика со2 //
      {
        object_array_users[users_array_index].send_message("Если датчик СО2 перестал отдавать показания или глючит - можно попробовать его перезапустить.\n\n\nПродолжить? - /35101@" + This_bot_name);
        break;
      }

      case 35101:                                // ↑↑↑ // Подтверждение //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_CO2_sensor.restart();
        }
        break;
      }

      case 352:                                  // рекалибровка со2 //
      {
        object_array_users[users_array_index].send_message("Рекалибровка датчика СО2 нужна в случае, если даже при открытых окнах он не показывает значение около 400 и нужно его рекалибровать.\n\nПри рекалибровке необходимо максимально проветрить помещение, чтобы воздух был близок к уличному.\n\n\nПродолжить? - /35201@" + This_bot_name);
        break;      
      }

      case 35201:                                // ↑↑↑ // Подтверждение //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_CO2_sensor.recalibration();
        }

        break;
      }

      case 353:                                  // сброс на заводские настройки со2 //
      {
        object_array_users[users_array_index].send_message("Сброс на заводские настройки сбрасывает все настройки, в том числе параметры калбировки датчика.\n\n\nПродолжить? - /35301@" + This_bot_name);
        break;      
      }

      case 35301:                                // ↑↑↑ // Подтверждение //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_CO2_sensor.factory_reset();
        }
        break;
      }

      case 370:                                  // Перезагрузка ESP //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_array_users[users_array_index].send_message("Поднял флаг для перезагрузки. Сработает через ~2 минуты.");
          esp_restart_flag = true;
        }
        break;
      }

      case 380:                                  // отключить всех остальных пользователей от управления //
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

      case 390:                                  // выбор гостевого чата //
      {
        if (object_array_users[users_array_index].get_admin_flag() == true)
        {
          object_array_users[users_array_index].send_message("Отправьте ID группы или профиля, чтобы определить гостевой чат (гостевой чат может быть только 1).");
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
  void SYNCstart()                                 // Отправка температур и проверка времени на синхронизацию //
  {
    jesse_yield_func();

    Serial.print(object_TimeDate.get_UTC());
    Serial.print(";");
    Serial.print(TEMP_excel_output());
    Serial.print(";");
    Serial.print(global_ERROR_flag);

    global_ERROR_flag = false;
    
    if (Serial.available())                                                 // если что-то есть в серил, то читаем //
    {
      jesse_yield_func();

      String SYNCmessage = Serial.readString();

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
    //Serial.setTimeout(300);                        // таймаут для .readString (ждет заданное значение на чтение Serial)

    WiFi.setOutputPower(16.00);                    // "When values higher than 19.25 are set, the board resets every time a connection with the AP is established." // https://stackoverflow.com/questions/75712199/esp8266-watchdog-reset-when-using-wifi // 
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);          // подключаемся к Wi-Fi //

    object_TimeDate.set_UTC_time();
    object_TimeDate.update_TimeDate();             // обновляем текущее время //

    setup_telegram_bots();

    pinMode(pin_knob_LOW,  INPUT_PULLUP);          // определяем пины // нижние концевики //
    pinMode(pin_knob_HIGH, INPUT_PULLUP);          // ↑↑↑ // верхние концевики //

    pinMode(pin_step_SREET, OUTPUT);               // ↑↑↑ // шаговый двигатель заслонки с улицы //
    pinMode(pin_step_HOME,  OUTPUT);               // ↑↑↑ // шаговый двигатель заслонки от батареи //
    pinMode(pin_DIR,        OUTPUT);               // ↑↑↑ // общий пин direction (направление вращения двигателей) //

    analogWrite(pin_PWM_Humidifier, 0);            // от 0 до 255 // при старте увлажнитель выключен //

    ds.begin();                                    // инициализация датчиков температуры по шине OneWire //

    if (!sht4.begin())                             // инициализация адафрут датчика по шине i2c //
    {
      send_alert("ERROR: Ошибка инициализации датчика температуры и влажности.");
      global_ERROR_flag = true;
    }
    sht4.setPrecision(SHT4X_HIGH_PRECISION);       // Какие-то настройки в адафрут библиотеке отвечающие за точность //
    sht4.setHeater(SHT4X_NO_HEATER);               // Какие-то настройки в адафрут библиотеке отвечающие за "прогрев". Видимо, это необходимо при использование датчика в экстремальных температурах, хз. По умолчанию выключено. //

    scd4x.begin(Wire);                             // инициализация датчика СО2 по шине i2c //

    send_reset_info();                             // причина перезагрузки //
    //send_alert("Я проснулся.");

    recuperator_button_check(true);                // текущий режим вентиляции рекуператор или заслонки //
    motor_calibration::state = motor_calibration::quick;     // выставление стейта быстрой калибровки //
    object_motor_main.calibrate_test(true);        // быстрая калибровка //
  }

  void loop()                                      // основной луп //
  {
    obj_stopwatch_ms_Main_LOOP.start();                                               // для подсчета времени лупа //

    free_heap::tick();

    object_TimeDate.update_TimeDate();                                                // обновляем текущее время //

    recuperator_button_check(false);                                                  // проверяем положение выключателя //

    move_air_dumpers_fast();                                                          // меняем быстро положение заслонок, если включили режим рекуперации //

    bot_tick_and_call_debug();                                                        // update telegram - получение сообщения из телеги и их обработка // внутри вызывается debug и .tick //

    update_sensors_data_and_calculations();                                           // если появился стейт на обновление показание - начнет non-blocking обновление //

    if(object_TimeDate.get_MIN() % 2 > 0 && flag_every_minute_timer == false)         // таймер каждую нечетную минуту //
    {
      SYNCstart();
      humidifier();
      thermostat();

      flag_every_minute_timer = true;
    }

    if(object_TimeDate.get_MIN() % 2 == 0 && flag_every_minute_timer == true)         // таймер каждую четную минуту //
    {
      restart_check();                                                                // проверяет была ли команда на перезагрузку //
      object_motor_main.calibrate_test(false);                                        // проверяет нужна ли калибровка //
      sensors_update::state = sensors_update::PART_0;                                 // раз в две минуты обновляем показания датчиков //

      flag_every_minute_timer = false;
    }

    obj_stopwatch_ms_Main_LOOP.stop();
  }
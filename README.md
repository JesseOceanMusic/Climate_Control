# Настройки в ардуино перед прошивкой

*  Tools > MMU 16KB cache + 48KB IRAM and 2nd HEAP (shared)
*  Tools > Stack Protection Enable 

# Версии Библиотек 

* NTPClient                3.2.1
* Adafruit BusIO           1.16.1
* Adafruit GFX Lubrart     1.11.10
* Adafruit SH110X          2.1.10
* Adafruit SHT4x Library   1.0.4    Хотя в коде подгружается лишь эта библиотека, она подтягивает еще несколько, возможно не все.
* Adafruit SSD1306         2.5.10
* Adafruit United Sensor   1.1.14
* Arduinojson              7.1.0
* DallasTemperature        3.9.0
* FastLED                  3.7.0
* OneWire                  2.3.8
* Sensirion Core           0.7.1
* Sensirion I2C SCD 4x     0.4.0
* UniversalTelegramBot     1.3.0    ***

***Все библиотеки стандартные, кроме библиотеки телеграма. в неё я добавлял два блока кода из обсуждений на гитхабе. Один блок кода позволил отправлять текстовые документы боту. Второй блок кода пофиксил баг. баг возникал, когда боту приходило слишком длинное сообщение, которое ESP не могла загрузить в память и уходила в бесконечную ошибку. суть доп кода в том, что он взаимодействует с функцией api "getupdetes" и удаляет сообщения длиннее ??? символов.


# Используемые компоненты на 2024-09-18

* 2 микроконтроллера "NodeMCU Lua v3" (на базе esp8266)                                    // 
* 2 шилда для nodemcu. Искать "nodemcu breadboard".                                        // единственная маркировка, которая на них есть "NodeMCU Base v1.0"
* 2 драйвера "TMC2209".                                                                    // 
* 2 шилда для драйверов. Искать "expansion board for A4988"                                // Вообще они для драйверов a4988, но оказались вполне себе совместимы с TMC2209.
* 2 шаговых двигателя nema 17 "US-17HS4401"                                                // Под замену скорее всего подойдет любая nema 17 с таким же количеством шагов на оборот, питанием и потреблением.
* 4 концевика "KW12-lun".                                                                  // Обычные концевики с возможностью выбора нормально-закрытый или нормально-открытый.
* шаговый винт с параметрами диаметр T8, pitch 2mm, lead 8mm.                              // Вряд ли с ним что-то произойдет. А если произойдет, то придется менять латунную гайку. Но, скорее всего, просто люфт потихоньку будет расти, а в данном случае это вообще ни на что не влияет.
* 7 датчиков температуры по протоколу oneWire "ds18B20" не путать с ds1820 и ds18S20.      // ds1820 имеет разрешение в 9бит, а ds18b20 12бит. По этому у первого шаг показаний 0,5 градуса, а у второго 0,0625 градуса.
* датчик температуры и влажности по протоколу i2c "SHT41".                                 // Довольно точный датчик влажности. Пришлось добавлять вторую шину, так как датчики влажности на шине OneWire стоят неадекватных денег.
* датчик СО2 SCD41                                                                         // Было два датчика, у обоих дрифт +-50 в режиме SingleShot и практически его отутствие в режиме PeriodicMeasurement и LowPowerPeriodicMeasurement .
* SD card reader.                                                                          // Самый обычный кард ридер для ардуино.
* 2 блока питания 220av/5dc.                                                               // 2 китайских бп на 5в. То есть, реализовано независимое питание микроконтроллеров. Может если один бп умрет, то я таки получу оповещение от второго микроконтроллера. Но не факт.
* 2 блока питания 220ac/12dc.                                                              // 2 китайских бп на 12в. Один питае шаговые двигатели, второй лед ленту.
* адресная лента "5v WS2812B 60 светодиодов на метр"                                       // 
* твердотельное реле "FQFER SSR-40DA"                                                      // Китайское реле, но на целых 40 китайских ампер. Всё-таки твердотельные реле довольно надежные и на ней всего-лишь будет висеть увлажнитель с незначительным потреблением в 0,5-2 ампера.


# Комментарии

* Хотя функция NightTime работает корректно, но использование одного стейта NightTimeFlag для определения состояния, как придутельного, так и автоматического ночного режима - оказалось визуально сложно воспринимаемым. Можно подумать, как реализовать эту же функцию на двух bool, но читаемее. если в какой-то момент будет неодходимость переписывать функциюю NightTime, возможно, заодно стоит и поменять логику на более понятную человеку.

* В то же время функции управления и калибровки шагового двигателя, хоть и сложные, но это связанно со сложностью управлением шагового двигателя. Не думаю, что их можно упрастить значительно. При необходимости в них придется вникать и страдать.

* Немного о шаговых двигателях и функциях stepsSTREET8 и stepsHOME8. Дело в том, что шаговый двигатель имеет определенное физическое количество шагов на один оборот. для nema 17 - это 200 шагов на один оборот вала. Но сами по себе шаговые двигатели довольно шумные. по этому, используется драйвер шаговыго двигателя. драйвер добавляет промежуточные шаги (микрошаги). Конктретно этот драйвер я настроил на деление каждого шага на 8 микрошагов. можно было выбрать и делитель выше, вплодь до 64, но на 8 микрошагах он уже бесшумен. Дело еще в том, что ESP имеет несколько другую архитектуру, чем ардуино. ESP не поддерживает задержки в микросекундах, по этому в функции используется delay(1), как минимально возможное. Использование delayMicroseconds - крэшит программу. Так как через команду мы делаем 1 микрошаг, то 8 микрошагов составят 1 реальный шаг двигателя. По этому, было принято решение считать и останавливаться именно на реальных шагах двигателя, а не между ними. Полагаю, что это уменьшит относительныз дрифт (смещение) в процессе работы и добавит стабильности. Избыточно? Возможно. Но зато ощущается правильно. В любом случае, ежедневная калибровка призвана как раз избавить от проблемы смещения со временем из-за возможных пропусков шагов. И как бонус - проверить, что концевики работают (ну по крайней мере 2 из 4 =0).

* Переменные для температур оставлены в буквенно-цифровом варианте из-за того, что названия были бы слишком длинные. Проще использовать короткие переменные и смотреть описание в комментарях чуть выше. a2-a8 это показания с датчиков. перменные b** это результаты расчетов.

# Принципиальная схема

Сделано с помощью EasyEDA.com

![Чертеж принципиальной схемы](images/Schematic_2024_10_01.png)

# Фотографии

![Фотография заслонок](images/Air_Dumpers.jpg)
![Фотография часов](images/Clock.jpg)
![Фотография ESP1](images/Wiring1.jpg)
![Фотография ESP2](images/Wiring2.jpg)

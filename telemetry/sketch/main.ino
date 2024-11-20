#include <SPI.h>
#include <SD.h>
#include <Wire.h>  
#include <SFE_BMP180.h> // Библиотека для работы с BMP180

SFE_BMP180 bmp; // Объект датчика BMP180

int ADXL345 = 0x53; // Адрес акселерометра

float X_out, Y_out, Z_out;  // Выходы акселерометра

const int PIN_CHIP_SELECT = 4;

double P0; // Переменная для хранения среднего значения давления

void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  pinMode(PIN_CHIP_SELECT, OUTPUT);

  Wire.begin(); // Инициализация шины I2C

  // Настройка режима измерения для ADXL345
  Wire.beginTransmission(ADXL345); 
  Wire.write(0x2D); 
  Wire.write(8); 
  Wire.endTransmission();

  if (!bmp.begin()) { // Проверяем подключение датчика BMP180
    Serial.println("Could not find a valid BMP180 sensor!");
    while (1) {}
  }

  if (!SD.begin(PIN_CHIP_SELECT)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");

  // Измеряем давление 10 раз и находим среднее значение
  double totalPressure = 0.0;
  for (int i = 0; i < 10; ++i) {
    totalPressure += getPressure();
    delay(500); // Ждем полсекунды между измерениями
  }
  P0 = totalPressure / 10.0;
  Serial.print("Среднее значение давления: ");
  Serial.println(P0);
}

void loop() {
  int time = millis();
  float pressure = getPressure();     // Текущее давление
  float temperature = getTemperature(); // Температура
  float alt = get_alt(P0, pressure);
  float acceleration = getAcceleration();// Ускорение
  readXYZAxes(&X_out, &Y_out, &Z_out);   // Оси X, Y, Z
  write_on_SDcard(String(time), alt, pressure, temperature, acceleration, X_out, Y_out, Z_out);
}

// Функция для получения давления
float getPressure() {
  char status;
  double P, T;

  // Сначала получаем температуру
  status = bmp.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = bmp.getTemperature(T);
    if (status != 0)
    {
      // Затем получаем давление
      status = bmp.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = bmp.getPressure(P, T);
        if (status != 0)
        {
          return (float)(P / 100.0F); // Преобразуем паскали в гектопаскали
        }
      }
    }
  }
  return 0.0; // Если произошла ошибка, возвращаем 0.0
}

// Функция для получения температуры
float getTemperature() {
  char status;
  double T;

  // Получаем температуру
  status = bmp.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = bmp.getTemperature(T);
    if (status != 0)
    {
      return (float)T;
    }
  }
  return 0.0; // Если произошла ошибка, возвращаем 0.0
}

// Функция для получения ускорения
float getAcceleration() {
  // Здесь может быть ваша реализация получения ускорения
  return 9.81; // Пока возвращаем фиктивное значение
}

void readXYZAxes(float* X_out, float* Y_out, float* Z_out) {
  // Чтение данных акселерометра
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); 
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); 

  *X_out = ( Wire.read() | Wire.read() << 8 );
  *X_out = *X_out / 256;

  *Y_out = ( Wire.read() | Wire.read() << 8 );
  *Y_out = *Y_out / 256;

  *Z_out = ( Wire.read() | Wire.read() << 8 );
  *Z_out = *Z_out / 256;

  /*Serial.print("Xa= ");
  Serial.print(*X_out);
  Serial.print("   Ya= ");
  Serial.print(*Y_out);
  Serial.print("   Za= ");
  Serial.println(*Z_out);*/
}

void write_on_SDcard(String time, float alt, float pressure, float temp, float accel, float x, float y, float z) {
  String logStringData = "Виктория;";
  logStringData += time + ";";
  logStringData += String(alt) + ";";
  logStringData += String(pressure) + ";"; // Изменено на запись давления
  logStringData += String(temp) + ";";
  logStringData += String(accel) + ";";
  logStringData += String(x) + ";";
  logStringData += String(y) + ";";
  logStringData += String(z) + ";";

  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(logStringData);
    dataFile.close();
    Serial.println(logStringData);
  } else {
    Serial.println("error opening datalog.txt");
  }
}

// Функция для расчета высоты относительно уровня моря
float get_alt(double p0, float pressure) {
    return 44330 * (1 - pow((pressure / p0), 0.1903));
}
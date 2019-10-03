#include <LGPS.h>
#include <LDHT.h>

#define DHTPIN 8          // what pin we're connected to
#define DHTTYPE DHT11     // using DHT11 sensor

gpsSentenceInfoStruct info; //needed to get GPS data
LDHT dht(DHTPIN,DHTTYPE);

float tempC=0.0,Humi=0.0;

double latitude = 0.00;
double longitude = 0.00;
int hour = 0, minute = 0, second = 0;
int sat_num = 0;    //number of visible satellites
int day = 0, month = 0, year = 0;
String time_format = "00:00:00", date_format = "00:00:0000";
String lat_format = "0.00000", lon_format = "0.00000";
int pause = 3000;   //time in milliseconds between two logs

void setup()
{
  Serial.begin(115200);
  LGPS.powerOn();
  dht.begin(); 
  Serial.print(DHTTYPE);
  Serial.println("GPS started.");
}

void loop()
{
  Serial.println("*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*");
  if (getData(&info) > 3)   //Start retriving data if no of satellites being tracked are greater than 3
  {
    String str = "|";
    str += date_format;
    str += ",";
    str += time_format;
    str += ",";
    str += lat_format;
    str += ",";
    str += lon_format;
    str += ",";
    str += sat_num;
    str += "|";
//    Serial.println(str);
  }
  else{
    Serial.println("Less then 4 satelites.");
  }
  if(dht.read())
    {
        tempC = dht.readTemperature();
        Humi = dht.readHumidity();


        Serial.println("------------------------------");
        Serial.print("Temperature Celcius = ");
        Serial.print(dht.readTemperature());
        Serial.println("C");

        Serial.print("Temperature Fahrenheit = ");
        Serial.print(dht.readTemperature(false));
        Serial.println("F");

        Serial.print("Humidity = ");
        Serial.print(dht.readHumidity());
        Serial.println("%");

        Serial.print("HeatIndex = ");
        Serial.print(dht.readHeatIndex(tempC,Humi));
        Serial.println("C");

        Serial.print("DewPoint = ");
        Serial.print(dht.readDewPoint(tempC,Humi));
        Serial.println("C");
    }
  delay(pause);
}

/**
*Converts degrees from (d)ddmm.mmmm to (d)dd.mmmmmm
*@param str the string rappresentation of the angle in (d)ddmm.mmmm format
*@param dir if true the direction is south, and the angle is negative.
*@return the given angle in dd.mmmmmm format.
*/
float convert(String str, boolean dir)
{
  double mm, dd;
  int point = str.indexOf('.');
  dd = str.substring(0, (point - 2)).toFloat();
  mm = str.substring(point - 2).toFloat() / 60.00;
  return (dir ? -1 : 1) * (dd + mm);
}

/**
*Gets gps informations
*@param info gpsSentenceInfoStruct is a struct containing NMEA sentence infomation
*@return the number of hooked satellites, or 0 if there was an error getting informations
*/
int getData(gpsSentenceInfoStruct* info)
{
  Serial.println("Collecting GPS data.");
  LGPS.getData(info);
//  Serial.println((char*)info->GPGGA);
  if (info->GPGGA[0] == '$')
  {
    Serial.println("Parsing GGA data....");
    String str = (char*)(info->GPGGA);
    str = str.substring(str.indexOf(',') + 1);
    hour = str.substring(0, 2).toInt();
    minute = str.substring(2, 4).toInt();
    second = str.substring(4, 6).toInt();
    time_format = "";
    time_format += hour;
    time_format += ":";
    time_format += minute;
    time_format += ":";
    time_format += second;
    str = str.substring(str.indexOf(',') + 1);
    latitude = convert(str.substring(0, str.indexOf(',')), str.charAt(str.indexOf(',') + 1) == 'S');
    int val = latitude * 1000000;
    String s = String(val);
    lat_format = s.substring(0, (abs(latitude) < 100) ? 2 : 3);
    lat_format += '.';
    lat_format += s.substring((abs(latitude) < 100) ? 2 : 3);
    str = str.substring(str.indexOf(',') + 3);
    longitude = convert(str.substring(0, str.indexOf(',')), str.charAt(str.indexOf(',') + 1) == 'W');
    val = longitude * 1000000;
    s = String(val);
    lon_format = s.substring(0, (abs(longitude) < 100) ? 2 : 3);
    lon_format += '.';
    lon_format += s.substring((abs(longitude) < 100) ? 2 : 3);

    sat_num = str.substring(0, 2).toInt();

    String Latitude = String(latitude, 5);
    String Longitude = String(longitude, 4);
    String Sat_num = String(sat_num);
    
    Serial.println("Data comming in......");
    Serial.print("Latitude :");
    Serial.println(Latitude);
    Serial.print("Longitude :");
    Serial.println(longitude);
    Serial.print("No of satellites being tracked :");
    Serial.println(Sat_num);
    Serial.println("done.");
  
    
    if (info->GPRMC[0] == '$')
    {
      Serial.print("Parsing RMC data....");
      str = (char*)(info->GPRMC);
      int comma = 0;
      for (int i = 0; i < 60; ++i)
      {
        if (info->GPRMC[i] == ',')
        {
          comma++;
          if (comma == 7)
          {
            comma = i + 1;
            break;
          }
        }
      }

      day = str.substring(0, 2).toInt();
      month = str.substring(2, 4).toInt();
      year = str.substring(4, 6).toInt();
      date_format = "20";
      date_format += year;
      date_format += "-";
      date_format += month;
      date_format += "-";
      date_format += day;
      Serial.println("done.");
      return sat_num;
    }
  }
  else
  {
    Serial.println("No GGA data");
  }
  return 0;
}

// dep lib
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// customized headers
#include "config.h"
#include "litncov.h"

#include <Crypto.h>
#include <SHA256.h>

#define HASH_SIZE 32

WiFiClient client;
HTTPClient http; //Declare an object of class HTTPClient

WiFiUDP ntpUDP;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 3600 * 8); //UTC+8

/*
 *  Determine if the device is automatically connected to wifi via cache
 *  Return a Boolean value
 */
bool autoConfig()
{
  WiFi.begin();
  for (int i = 0; i < CONFIG_WIFI_RETRY_TIME; i++)
  {
    int wstatus = WiFi.status();
    if (wstatus == WL_CONNECTED)
    {
      Serial.println("AutoConfig Success");
      // wifi info
      Serial.println("WiFi Connected.");
      Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
      Serial.printf("Password: %s\n", WiFi.psk().c_str());
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      WiFi.printDiag(Serial);
      return true;
      //break;
    }
    else
    {
      Serial.println("AutoConfig Waiting......");
      Serial.println("WIFI STATUS: " + wstatus);
      delay(1000);
    }
  }
  Serial.println("AutoConfig Faild!");
  return false;
  WiFi.printDiag(Serial);
}

/*
 *  SmartConfig function
 *  Return void
 */
void smartConfig()
{
  WiFi.mode(WIFI_STA);
  Serial.println("\r\nWait for Smartconfig");
  WiFi.beginSmartConfig();
  while (1)
  {
    Serial.print(".");
    if (WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success\n");
      WiFi.setAutoConnect(true); // set auto connect when device boot
      break;
    }
    delay(500); // It's important to add delay to this place, otherwise it's extremely easy to crash and restart.
  }
}

char *btoh(char *dest, uint8_t *src, int len)
{
  char *d = dest;
  while (len--)
    sprintf(d, "%02x", (unsigned char)*src++), d += 2;
  return dest;
}

char *crypto_password(char *str)
{
  char hex[256];
  char *msg = str;

  uint8_t result[HASH_SIZE];
  SHA256 sha256Hash;

  sha256Hash.reset();
  sha256Hash.update(msg, strlen(msg));
  sha256Hash.finalize(result, HASH_SIZE);

  return btoh(hex, result, HASH_SIZE);
}

String nowDateTime()
{
  http.begin(client, "http://quan.suning.com/getSysTime.do");
  int httpCode = http.GET();

  String now;

  if (httpCode > 0)
  {
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, http.getString());

    now = doc["sysTime2"].as<char *>();
  }

  http.end(); //Close connection

  return now;
}

litUserInfo litLogin(char *user, char *psw)
{
  litUserInfo rte;
  StaticJsonDocument<200> loginData;
  char buffer[256];

  loginData["cardNo"] = user;
  loginData["password"] = crypto_password(psw);

  serializeJson(loginData, buffer);

  http.begin(client, CONFIG_LIT_ENDPOINT_LOGIN); //Specify request destination
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(buffer); //Send the request

  if (httpCode > 0)
  { //Check the returning code

    String payload = http.getString(); //Get the request response payload
    Serial.println(payload);           //Print the response payload

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    Serial.println(doc["code"].as<int>());

    if (doc["code"].as<int>() == 200)
    {

      
      rte.userId = doc["data"]["userId"].as<int>();
      rte.cardNo = doc["data"]["cardNo"].as<char *>();
      rte.name = doc["data"]["name"].as<char *>();
      rte.teamId = doc["data"]["teamId"].as<int>();
      rte.token = doc["data"]["token"].as<char *>();
      rte.expireTime = doc["data"]["expireTime"].as<char *>();
      rte.lastUpdateTime = doc["data"]["lastUpdateTime"].as<char *>();
      rte.sex = doc["data"]["sex"].as<int>();
      rte.age = doc["data"]["age"].as<int>();
      rte.nativePlaceProvince = doc["data"]["nativePlaceProvince"].as<char *>();
      rte.nativePlaceCity = doc["data"]["nativePlaceCity"].as<char *>();
      rte.nativePlaceDistrict = doc["data"]["nativePlaceDistrict"].as<char *>();
      rte.nativePlaceAddress = doc["data"]["nativePlaceAddress"].as<char *>();
      rte.teamName = doc["data"]["teamName"].as<char *>();
      rte.teamProvince = doc["data"]["teamProvince"].as<char *>();
      rte.teamCity = doc["data"]["teamCity"].as<char *>();
      rte.mobile = doc["data"]["mobile"].as<char *>();
      rte.organizationName = doc["data"]["organizationName"].as<char *>();
      rte.identity = doc["data"]["identity"].as<int>();
      rte.isAdmin = doc["data"]["isAdmin"].as<int>();
      rte.logoUrl = doc["data"]["logoUrl"].as<char *>();
      rte.isTwoTemperature = doc["data"]["isTwoTemperature"].as<int>();
      rte.isApprover = doc["data"]["isApprover"].as<char *>();
      rte.isGeneralAdmin = doc["data"]["isGeneralAdmin"].as<int>();
      rte.isReportAdmin = doc["data"]["isReportAdmin"].as<int>();
      rte.teamNo = doc["data"]["teamNo"].as<char *>();
      rte.localAddress = doc["data"]["localAddress"].as<char *>();
      rte.userOrganizationId = doc["data"]["userOrganizationId"].as<int>();
      rte.isReturnSchoolApprover = doc["data"]["isReturnSchoolApprover"].as<int>();
    }
  }

  Serial.println(rte.token);

  http.end(); //Close connection

  return rte;
}

litLastRecordInfo litLastRecord(litUserInfo lui)
{
  litLastRecordInfo rte;
  String eurl = CONFIG_LIT_ENDPOINT_LASTRECORD;
  eurl += "?teamId=";
  eurl += lui.teamId;
  eurl += "&userId=";
  eurl += lui.userId;

  http.begin(client, eurl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("token", lui.token);

  Serial.println(lui.token);

  int httpCode = http.GET();

  if (httpCode > 0)
  { //Check the returning code

    String payload = http.getString(); //Get the request response payload
    Serial.println(payload);           //Print the response payload

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    if (doc["code"].as<int>() == 200)
    {
      rte.id = doc["data"]["id"].as<int>();
      rte.userId = doc["data"]["userId"].as<int>();
      rte.teamId = doc["data"]["teamId"].as<int>();
      rte.createTime = doc["data"]["createTime"].as<char *>();
      rte.currentProvince = doc["data"]["currentProvince"].as<char *>();
      rte.currentProvince = doc["data"]["currentCity"].as<char *>();
      rte.currentDistrict = doc["data"]["currentDistrict"].as<char *>();
      rte.currentAddress = doc["data"]["currentAddress"].as<char *>();
      rte.isInTeamCity = doc["data"]["isInTeamCity"].as<int>();
      rte.healthyStatus = doc["data"]["healthyStatus"].as<int>();
      rte.temperatureNormal = doc["data"]["temperatureNormal"].as<int>();
      rte.temperature = doc["data"]["temperature"].as<float>();
      rte.temperatureTwo = doc["data"]["temperatureTwo"].as<float>();
      rte.temperatureThree = doc["data"]["temperatureThree"].as<float>();
      rte.selfHealthy = doc["data"]["selfHealthy"].as<int>();
      rte.selfHealthyInfo = doc["data"]["selfHealthyInfo"].as<char *>();
      rte.selfHealthyTime = doc["data"]["selfHealthyTime"].as<char *>();
      rte.friendHealthy = doc["data"]["friendHealthy"].as<int>();
      rte.travelPatient = doc["data"]["travelPatient"].as<char *>();
      rte.contactPatient = doc["data"]["contactPatient"].as<char *>();
      rte.isolation = doc["data"]["isolation"].as<int>();
      rte.seekMedical = doc["data"]["seekMedical"].as<int>();
      rte.seekMedicalInfo = doc["data"]["seekMedicalInfo"].as<char *>();
      rte.exceptionalCase = doc["data"]["exceptionalCase"].as<int>();
      rte.exceptionalCaseInfo = doc["data"]["exceptionalCaseInfo"].as<char *>();
      rte.reportDate = doc["data"]["reportDate"].as<char *>();
      rte.currentStatus = doc["data"]["currentStatus"].as<char *>();
      rte.isolation = doc["data"]["villageIsCase"].as<int>();
      rte.reportDate = doc["data"]["caseAddress"].as<char *>();
      rte.peerIsCase = doc["data"]["peerIsCase"].as<int>();
      rte.peerAddress = doc["data"]["peerAddress"].as<char *>();
      rte.goHuBeiCity = doc["data"]["goHuBeiCity"].as<char *>();
      rte.goHuBeiTime = doc["data"]["goHuBeiTime"].as<char *>();
      rte.contactProvince = doc["data"]["contactProvince"].as<char *>();
      rte.contactCity = doc["data"]["contactCity"].as<char *>();
      rte.contactDistrict = doc["data"]["contactDistrict"].as<char *>();
      rte.contactAddress = doc["data"]["contactAddress"].as<char *>();
      rte.contactTime = doc["data"]["contactTime"].as<char *>();
      rte.diagnosisTime = doc["data"]["diagnosisTime"].as<char *>();
      rte.treatmentHospitalAddress = doc["data"]["treatmentHospitalAddress"].as<char *>();
      rte.cureTime = doc["data"]["cureTime"].as<char *>();
      rte.abroadInfo = doc["data"]["abroadInfo"].as<char *>();
      rte.isAbroad = doc["data"]["isAbroad"].as<int>();
    }
  }

  http.end();

  return rte;
}

bool litFisrtRecord(char *user, char *psw, float temperature = 36.6, float temperatureTwo = 0.00, float temperatureThree = 0.00)
{
  // try to login
  litUserInfo lui = litLogin(user, psw);
  // check status


  litLastRecordInfo llr = litLastRecord(lui);

  // Serial.println(llr.temperatureTwo);

  char buffer[256];

  StaticJsonDocument<200> recordData;

  /* From: http//:<host>/web/#/healthForm Date: 2021-02-11 17:45:00
  mobile: '',
  age:'',
  sex:'',
  nativePlaceProvince:'',
  nativePlaceCity:'',
  nativePlaceDistrict:'',
  nativePlaceAddress:'',
  localAddress:'',

  currentProvince: '',//目前所在地省
  currentCity: '',//目前所在地市
  currentDistrict:null,//目前所在地区
  currentLocation: '',//目前所在地
  //
    requestFlag === 1，今日已提交,较昨日无变化回显时，获取最新数据
    if(res.data.currentDistrict){
    this.formData.currentLocation=areaJson.province_list[res.data.currentProvince]+'-'+areaJson.city_list[res.data.currentCity]+'-'+areaJson.county_list[res.data.currentDistrict];
    }else{
    this.formData.currentLocation=areaJson.province_list[res.data.currentProvince]+'-'+areaJson.city_list[res.data.currentCity];
    //回显信息是否是海外
    this.isSelectOverseas=(res.data.currentProvince==='900000');
  //
  currentAddress: '',//目前所在地详细地址

  villageIsCase:'0',//所在小区或者村是否有确诊病例
  caseAddress:'',//病例地址
  peerIsCase:'0',//同住人是否有确诊病例
  peerAddress:'',//共同居主人地址

  isInTeamCity: '',
  temperatureNormal: '0',
  temperature: '',
  selfHealthy: '0',
  selfHealthyInfo:'',
  selfHealthyTime:null,
  friendHealthy: '0',
  isolation: '0',

  currentStatus:'1000705',//当前所属状态
  diagnosisTime:null,//选择已治愈时确诊时间
  treatmentHospitalAddress:'',//选择已治愈时治疗医院地址
  cureTime:null,//选择已治愈时治愈时间

  travelPatient:'1000803',//疫情旅行史
  goHuBeiCity:'',//去过的湖北城市
  goHuBeiTime:null,//去湖北城市时间

  contactPatient: '1000904',//接触情况
  contactTime:null,//接触时间
  contactProvince: '',//接触地点省
  contactCity: '',//接触地点市
  contactDistrict:'',//接触地点区
  contactLocation:'',//接触地点所在地
  contactAddress:'',//接触地点所在地详细地址

  isAbroad:'',//是否去过国外
  abroadInfo:'',//去过的国外地区详细信息

  seekMedical: '0',
  seekMedicalInfo: '',
  exceptionalCase: '0',
  exceptionalCaseInfo: '',
  isTrip: '0',
  */

  recordData["mobile"] = lui.mobile;
  recordData["nativePlaceProvince"] = lui.nativePlaceProvince;
  recordData["nativePlaceCity"] = lui.nativePlaceCity;
  recordData["nativePlaceDistrict"] = lui.nativePlaceDistrict;
  recordData["nativePlaceAddress"] = lui.nativePlaceAddress;
  recordData["localAddress"] = lui.localAddress;

  recordData["currentProvince"] = llr.currentProvince;
  recordData["currentCity"] = llr.currentCity;
  recordData["currentDistrict"] = llr.currentDistrict;
  recordData["currentLocation"] = ""; // unnecessary
  recordData["currentAddress"] = llr.currentAddress;

  recordData["currentAddress"] = llr.villageIsCase;
  recordData["caseAddress"] = llr.caseAddress;
  recordData["peerIsCase"] = llr.peerIsCase;
  recordData["peerAddress"] = llr.peerAddress;
 
  recordData["isInTeamCity"] = llr.isInTeamCity;

  recordData["temperatureNormal"] = llr.temperatureNormal;
  recordData["temperature"] = temperature;
  recordData["selfHealthy"] = llr.selfHealthy;
  recordData["selfHealthyInfo"] = llr.selfHealthyInfo;
  recordData["selfHealthyTime"] = llr.selfHealthyTime;
  recordData["friendHealthy"] = llr.friendHealthy;
  recordData["isolation"] = llr.isolation;

  recordData["currentStatus"] = llr.currentStatus;
  recordData["diagnosisTime"] = llr.diagnosisTime;
  recordData["treatmentHospitalAddress"] = llr.treatmentHospitalAddress;
  recordData["cureTime"] = llr.cureTime;

  recordData["travelPatient"] = llr.travelPatient;
  recordData["goHuBeiCity"] = llr.goHuBeiCity;
  recordData["goHuBeiTime"] = llr.goHuBeiTime;

  recordData["contactPatient"] = llr.contactPatient;
  recordData["contactTime"] = llr.contactTime;
  recordData["contactProvince"] = llr.contactProvince;
  recordData["contactCity"] = llr.contactCity;
  recordData["contactDistrict"] = llr.contactDistrict;
  recordData["contactLocation"] = ""; // unnecessary
  recordData["contactAddress"] = llr.contactAddress;

  recordData["isAbroad"] = llr.isAbroad;
  recordData["abroadInfo"] = llr.abroadInfo;

  recordData["seekMedical"] = llr.seekMedical;
  recordData["seekMedicalInfo"] = llr.seekMedicalInfo;
  recordData["exceptionalCase"] = llr.exceptionalCase;
  recordData["exceptionalCaseInfo"] = llr.exceptionalCaseInfo;
  recordData["isTrip"] = llr.isAbroad;

  recordData["userId"] = lui.userId;
  recordData["teamId"] = lui.teamId;
  recordData["healthyStatus"] = llr.healthyStatus;

  recordData["temperatureTwo"] = temperatureTwo;
  recordData["temperatureThree"] = temperatureThree;

  // String now = nowDateTime();
  // Serial.println(now);
  // // if (now.isEmpty())
  // // {
  // //   return false;
  // // }

  // recordData["reportDate"] = now;

  serializeJson(recordData, buffer);

  http.begin(client, CONFIG_LIT_ENDPOINT_ADDRECORD); //Specify request destination
  http.addHeader("Content-Type", "application/json");
  http.addHeader("token", lui.token);  

  int httpCode = http.POST(buffer); //Send the request

  Serial.println("准备上报");

  if (httpCode > 0)
  { //Check the returning code

    String payload = http.getString(); //Get the request response payload
    Serial.println(payload);

    Serial.println("成功");

  }

  Serial.println("结束");

  http.end();


}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  // wifi config
  if (!autoConfig())
  {
    Serial.println("Start module");
    smartConfig();
  }

  //
  char litUser[] = "";
  char litPWD[] = "";

  litFisrtRecord(litUser, litPWD);
  // Serial.println(timeClient.getDay());
}

void loop()
{
  timeClient.update();
  // Serial.println(timeClient.getFormattedTime());

  // Serial.println(nowDateTime());
  delay(1000);
}

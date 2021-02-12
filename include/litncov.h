// LIT(Luoyang Institute of Science and Technology) Health Platform
#define CONFIG_LIT_ENDPOINT_LOGIN "http://hmgr.sec.lit.edu.cn/wms/healthyLogin"
#define CONFIG_LIT_ENDPOINT_LASTRECORD "http://hmgr.sec.lit.edu.cn/wms/lastHealthyRecord"
#define CONFIG_LIT_ENDPOINT_ADDRECORD "http://hmgr.sec.lit.edu.cn/wms/addHealthyRecord"

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
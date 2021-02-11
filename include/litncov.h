// LIT(Luoyang Institute of Science and Technology) Health Platform
#define CONFIG_LIT_ENDPOINT_LOGIN "http://hmgr.sec.lit.edu.cn/wms/healthyLogin"
#define CONFIG_LIT_ENDPOINT_LASTRECORD "http://hmgr.sec.lit.edu.cn/wms/lastHealthyRecord"
#define CONFIG_LIT_ENDPOINT_ADDRECORD "http://hmgr.sec.lit.edu.cn/wms/addHealthyRecord"

// User base info
struct litUserInfo
{
    int userId;
    const char *cardNo;
    const char *name;
    int teamId;
    const char *token = NULL;
    const char *expireTime;
    const char *lastUpdateTime;
    int sex;
    int age;
    const char *nativePlaceProvince;
    const char *nativePlaceCity;
    const char *nativePlaceDistrict;
    const char *nativePlaceAddress;
    const char *teamName;
    const char *teamProvince;
    const char *teamCity;
    const char *mobile;
    const char *organizationName;
    int identity;
    int isAdmin;
    const char *logoUrl;
    int isTwoTemperature;
    const char *isApprover;
    int isGeneralAdmin;
    int isReportAdmin;
    const char *teamNo;
    const char *localAddress;
    int userOrganizationId;
    int isReturnSchoolApprover;
};

// the last record info
struct litLastRecordInfo
{
    int id;
    int userId;
    int teamId;
    const char *createTime;
    const char *currentProvince;
    const char *currentCity;
    const char *currentDistrict;
    const char *currentAddress;
    int isInTeamCity;
    int healthyStatus;
    int temperatureNormal;
    float temperature;
    float temperatureTwo;
    float temperatureThree;
    int selfHealthy;
    const char *selfHealthyInfo;
    const char *selfHealthyTime;
    int friendHealthy;
    const char *travelPatient;
    const char *contactPatient;
    int isolation;
    int seekMedical;
    const char *seekMedicalInfo;
    int exceptionalCase;
    const char *exceptionalCaseInfo;
    const char *reportDate;
    const char *currentStatus;
    int villageIsCase;
    const char *caseAddress;
    int peerIsCase;
    const char *peerAddress;
    const char *goHuBeiCity;
    const char *goHuBeiTime;
    const char *contactProvince;
    const char *contactCity;
    const char *contactDistrict;
    const char *contactAddress;
    const char *contactTime;
    const char *diagnosisTime;
    const char *treatmentHospitalAddress;
    const char *cureTime;
    const char *abroadInfo;
    int isAbroad;
};

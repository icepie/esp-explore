# esp-explore (esp8266-arduino-litncov)
> 该分支实现了 [lit-ncov-report](https://github.com/icepie/lit-ncov-report) 项目的功能, 是本人一次对 `esp8266` 的探索性的开发. 

用IOT(物联网)的方式实现健康状况管控平台(洛阳理工学院)的每日定时上报


## 配置

> 用户配置文件在 `include/config.h`

```cpp

// Default
#define CONFIG_PROJECT_CODE "litncov"
#define CONFIG_DEVICE_FW 0.3
#define CONFIG_WIFI_RETRY_TIME 10

// USER
#define CONFIG_LIT_HEALTH_USER "" //用户名
#define CONFIG_LIT_HEALTH_PWD ""  //密码
#define CONFIG_SERVERCHAN_SCKEY "" //server酱推送密钥

// TASK
#define CONFIG_LIT_REPORT_TIME "00:02:00" //(UTC+8)

```

## 构建

> 请确保 [platformio](https://platformio.org) 等依赖已正确安装

### 克隆

```
$ git clone https://github.com/icepie/esp-explore -b esp8266-arduino-litncov
```

### 编译

``` bash
$ make all
```

### 烧录

 ```bash
 $ make upload
 ```
 
### 串口监视
 
 ```bash
 $ make monitor
 ```
 
## 使用
 
1. 该固件配网方式为 `SmartConfig` , 可使用微信配网等方式让设备联网

2. 配置正确即会在预设时间发送上报结果


# TODO

待补充

## 相关

[实战-打造一个帮你健康打卡的物联网小硬件-计划](https://icepie.vercel.app/2021/02/09/esp8266-litncov-00/)





setlocal

set TYPE=android-app
set APPID=org.doubango.ultimateMrz.videorecognizer
set APPSTORE=com.android.vending,com.google.android.feedback,com.huawei.appmarket,com.amazon
set APPSIGN=75:2D:C2:5F:20:18:EE:1F:BC:04:A2:FF:B3:7E:01:8D:3F:68:9A:B1:F4:A9:41:18:84:16:A7:25:67:19:32:2D

runtimeKey.exe ^
    --json true ^
    --assets ../../../assets ^
    --type %TYPE% --appid %APPID% --appstore %APPSTORE% --appsign %APPSIGN%

endlocal
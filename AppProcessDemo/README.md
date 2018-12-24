# Android端的项目
Android端当作Server，发送录制屏幕的编码数据和接受按键。

## 使用
运行 gradle task
1. adb_push任务，就可以将编译好的dex push到手机中。
2. adb_forward 任务.可以进行接口转发
3. adb_exec任务，可以直接将demo在手机上运行起来，等待Client端连接。


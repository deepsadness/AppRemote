# AppRemote

Android ADB Simple Remote Screen
通过ADB连接，在mac上进行Android屏幕简单的控制

- 参考项目：
https://github.com/Genymobile/scrcpy 

- 博文地址:
1. [Android PC投屏简单尝试—最终章1](https://www.jianshu.com/p/06f4b8919991)
2. [Android PC投屏简单尝试—最终章2](https://www.jianshu.com/p/c2da5174d5f7)

## 使用
*因为使用了Clion，目前只能在mac上运行。还未配置VSStudio*

1. Android手机通过USB连接电脑，开启USB调试模式
2. 打开[Android项目](https://github.com/deepsadness/AppRemote/tree/master/AppProcessDemo)
在AndroidStudio 的项目中。运行gradle中的 adb_forward 和adb_push 任务。
3. 进入adb shell 运行app_process
```
   adb shell CLASSPATH=/data/local/tmp/class.jar app_process /data/local/tmp com.cry.cry.appprocessdemo.HelloWorld
```
4.运行这个项目

## 介绍
- ADB
使用 adb forward 进行接口转发。通过Socket进行连接
- FFMPEG
接受Android端发送的H264 Naul，解码成YUV
- SDL2
接受解码后的YUV。提供屏幕渲染
接受键盘和点击事件。并通过socket 进行发送

## 线程模型
- client端(PC)
    - event_loop
        SDL的EventLoop。复制渲染上屏和分发事件
    - event_sender(Socket send)
        接受SDL分发的事件。并把对应的事件通过Socket分发给Android手机。
    - screen_receiver(Socket recv)
        通过Socket接受的 H264 Naul,使用FFmpeg进行解码。

- server端(Android)
    - screen record (Socket InputStream)
        使用SurfaceControl和MediaCodec进行屏幕录制，录制的结果通过Socket发送
    - event_loop (Socket OutputStream)
        接受Socket发送过来的事件。并调用对应的API进行事件的注入(InputManager)

### 线程通信
- frames
两块缓存区域。
   - decode_frame
        解码放置的frame
   - render_frame
        渲染需要的frame.使用该frame 进行render
数据流动
   - 生产的过程
     screen_receiver 负责生产。
   - 消费的过程
     event_loop 负责消费。将两块缓存区域进行交换，并把render_frame上屏

- event
一个event_queue队列来接受。可以使用链表
数据流动
   - 生产的过程
     event_loop 负责生产。并把数据送入队列当中
   - 消费的过程
     event_sender 负责消费。如果队列不为空，则进行发送

## 设计类
- client端
    SDL_Screen 负责SDL的显示和事件的接受
    FFmpegDecoder 负责数据的解码。和送入缓存
    EventSender 发送事件

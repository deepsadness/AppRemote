package com.cry.cry.appprocessdemo;

import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.os.SystemClock;
import android.system.ErrnoException;
import android.system.Os;
import android.system.OsConstants;
import android.text.TextUtils;
import android.util.Log;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;

import com.cry.cry.appprocessdemo.bean.DisplayInfo;
import com.cry.cry.appprocessdemo.bean.Size;
import com.cry.cry.appprocessdemo.refect.DisplayManager;
import com.cry.cry.appprocessdemo.refect.InputManager;
import com.cry.cry.appprocessdemo.refect.ServiceManager;

import java.io.FileDescriptor;
import java.io.IOException;
import java.io.InterruptedIOException;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.security.KeyFactory;

public class HelloWorld {
    public static void main(String[] args) {
        ServiceManager serviceManager = new ServiceManager();
        DisplayManager displayManager = serviceManager.getDisplayManager();
        //获取屏幕的信息
        DisplayInfo displayInfo = displayManager.getDisplayInfo();
        final Size size = displayInfo.getSize();
        System.out.println("getDisplayInfo display=" + displayInfo);

        //判断是否亮屏
        boolean screenOn = serviceManager.getPowerManager().isScreenOn();
        System.out.println("is screen on=" + screenOn);
        //注入点击事件
        if (!screenOn) {    //点亮屏幕
            clickPowerKey(serviceManager);
        }

//        模拟点击
        //开始连接Socket
        LocalServerSocket serverSocket = null;
        ByteBuffer buffer = ByteBuffer.allocate(1024);
        StartRd startRd = null;
        Thread thread = null;
        try {
            serverSocket = new LocalServerSocket("recorder");
            LocalSocket accept = serverSocket.accept();
            Log.d("zzx", "连接成功！！");
            System.out.println("连接成功！！");

            final FileDescriptor fileDescriptor = accept.getFileDescriptor();
            sendScreenInfo(size, buffer, fileDescriptor);

            //连接成功，就开始屏幕录制
            startRd = new StartRd(size, fileDescriptor);
            thread = new Thread(startRd);
            thread.start();
            //开启socket的事件监听
            boolean eof = false;
            startRd = event_loop(serviceManager, size, buffer, startRd, fileDescriptor, eof);
//            while (!eof) {
//                int read = Os.read(fileDescriptor, buffer);
//                System.out.println("read = " + read);
//                if (read == -1) {
//                    continue;
//                } else {
//                    buffer.flip();
//                    String readContent = new String(buffer.array(), buffer.position(), buffer.limit(), "UTF-8");
//                    buffer.clear();
//                    if (TextUtils.isEmpty(readContent)) {
//                        continue;
//                    }
//                    System.out.println("rev msg= " + readContent);
//                    if ("end".equals(readContent)) {
//                        eof = true;
////                        break;
//                    }
//                }
//            }
            System.out.println("break loop");

            //这个时候也要中断屏幕录制
            if (startRd != null) {
                startRd.requestStop();
            }

        } catch (IOException e) {
            e.printStackTrace();
        } catch (ErrnoException e) {
            e.printStackTrace();
        } finally {
            if (serverSocket != null) {
                try {
                    serverSocket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

    }

    private static StartRd event_loop(ServiceManager serviceManager, Size size, ByteBuffer buffer, StartRd startRd, FileDescriptor fileDescriptor, boolean eof) throws ErrnoException, InterruptedIOException, UnsupportedEncodingException {
        Thread thread;
        do {
            //因为读到的数据，可能不是一次读完。所以需要保存读取的状态
            int read = Os.read(fileDescriptor, buffer);
            System.out.println("read=" + read + ",position=" + buffer.position() + "," +
                    "limit=" + buffer.limit() + ",remaining " + buffer.remaining());

            if (read == -1 || read == 0) {
                //如果这个时候read 0 的话。就结束
                break;
//                    continue;
            } else {
                buffer.flip();
                byte b = buffer.get(0);
                if (b == 0 && read > 1) { //如果是0 的话，就当作是Action
                    byte type = buffer.get(1);
                    System.out.println("type=" + type + ",position=" + buffer.position() + "," +
                            "limit=" + buffer.limit() + ",remaining " + buffer.remaining());

                    if (type < 2 && read == 6) {//action down 1 down 0 up
                        System.out.println("enter key event");
                        //判断当前读取的数据是否够，不够还需要继续读取
                        buffer.position(1);
//                        int x = buffer.getShort() & 0xffff;
//                        int y = buffer.getShort() & 0xffff;
                        int x = buffer.get(2) << 8 | buffer.get(3) & 0xff;
                        int y = buffer.get(4) << 8 | buffer.get(5) & 0xff;

                        System.out.println("x=" + x);
                        System.out.println("y=" + y);
                        //接受到事件进行处理

                        boolean key = createKey(serviceManager, type, x, y);
                        System.out.println("enter key result = " + key);
                        buffer.clear();
                    } else if (type == 2 && read == 14) { //滚动事件
                        System.out.println("remaining" + buffer.remaining());
                        buffer.position(1);
//                        int x = buffer.getShort() & 0xffff;
//                        int y = buffer.getShort() & 0xffff;
                        int x = buffer.get(2) << 8 | buffer.get(3) & 0xff;
                        int y = buffer.get(4) << 8 | buffer.get(5) & 0xff;

                        System.out.println("position = " + buffer.position());
                        System.out.println("remaining" + buffer.remaining());

                        int hs = buffer.get(6) << 24 | buffer.get(7) << 16 | buffer.get(8) <<
                                8 | buffer.get(9);

                        int vs = buffer.get(10) << 24 | buffer.get(11) << 16 | buffer.get(12) <<
                                8 | buffer.get(13);
                        System.out.println("x=" + x);
                        System.out.println("y=" + y);
                        System.out.println("hs=" + hs);
                        System.out.println("vs=" + vs);
                        //接受到事件进行处理
                        buffer.clear();
                        boolean b1 = injectScroll(serviceManager, x, y, hs, vs);
                        System.out.println("scroll result = " + b1);
                    } else if (type == 3) { //接受按键事件
                        System.out.println("enter key code event");
                        int action = buffer.get(2) == 1 ? KeyEvent.ACTION_DOWN : KeyEvent.ACTION_UP;
                        int keyCode = buffer.get(3);
                        boolean key = injectKeyEvent(serviceManager, action, keyCode);
                        System.out.println("enter key result = " + key);
                        buffer.clear();
                    }
                } else {
                    String readContent = new String(buffer.array(), buffer.position(), buffer.limit(), "UTF-8");
                    buffer.clear();
                    if (TextUtils.isEmpty(readContent)) {
                        continue;
                    }
                    System.out.println("rev msg= " + readContent + ", msg length=" + readContent.length());
                    String trimMsg = readContent.trim();
                    System.out.println("rev trim msg= " + trimMsg + ", msg length=" + trimMsg.length());
                    if ("start".equals(trimMsg)) {
                        //开启线程。进行录制
                        startRd = new StartRd(size, fileDescriptor);
                        thread = new Thread(startRd);
                        thread.start();
                    } else if ("end".equals(trimMsg)) {
                        System.out.println("enter end");
                        eof = true;
                        if (startRd != null) {
                            startRd.requestStop();
//                            thread.interrupt();

                        }
//                        break;
                    } else if (trimMsg.startsWith("click")) {
                        System.out.println("Click");
                        int kIndex = trimMsg.indexOf("k");
                        int xIndex = trimMsg.indexOf("x");
                        int x = Integer.parseInt(trimMsg.substring(kIndex + 1, xIndex));
                        int y = Integer.parseInt(trimMsg.substring(xIndex + 1));

                        long now = SystemClock.uptimeMillis();
                        MotionEvent event = MotionEvent.obtain(now, now, MotionEvent.ACTION_DOWN,
                                x, y, 0);
                        boolean downResult = serviceManager.getInputManager().injectInputEvent(event, InputManager
                                .INJECT_INPUT_EVENT_MODE_ASYNC);

                        System.out.println("click down result = " + downResult);

                        long end = SystemClock.uptimeMillis();
                        MotionEvent event2 = MotionEvent.obtain(now, end, MotionEvent
                                        .ACTION_UP,
                                x, y, 0);
                        boolean b1 = serviceManager.getInputManager().injectInputEvent(event2, InputManager
                                .INJECT_INPUT_EVENT_MODE_ASYNC);

                        System.out.println("click up result = " + b1);
                    }
                }
            }
        } while (!eof);
        return startRd;
    }

    private static void sendScreenInfo(Size size, ByteBuffer buffer, FileDescriptor fileDescriptor) throws IOException {
        //将尺寸数据先发送过去
        int width = size.getWidth();
        int height = size.getHeight();
        byte wHigh = (byte) (width >> 8);
        byte wLow = (byte) (width & 0xff);

        byte hHigh = (byte) (height >> 8);
        byte hLow = (byte) (height & 0xff);

        buffer.put(wHigh);
        buffer.put(wLow);

        buffer.put(hHigh);
        buffer.put(hLow);

//            System.out.println("发送尺寸 size result = " + write);
//            int write = Os.write(fileDescriptor, buffer);
        byte[] buffer_size = new byte[4];
        buffer_size[0] = (byte) (width >> 8);
        buffer_size[1] = (byte) (width & 0xff);
        buffer_size[2] = (byte) (height >> 8);
        buffer_size[3] = (byte) (height & 0xff);
        writeFully(fileDescriptor, buffer_size, 0, buffer_size.length);
        System.out.println("发送尺寸 size result ");
        buffer.clear();
    }

    private static boolean injectKeyEvent(ServiceManager serviceManager, int action, int keyCode) {
        KeyEvent keyEvent = EventFactory.keyEvent(action, keyCode, 0, 0);
        return serviceManager.getInputManager().injectInputEvent(keyEvent, InputManager
                .INJECT_INPUT_EVENT_MODE_ASYNC);
    }


    private static boolean createKey(ServiceManager serviceManager, int type, int x, int y) {
        MotionEvent event = EventFactory.createMotionEvent(type, x, y);

        return serviceManager.getInputManager().injectInputEvent(event, InputManager
                .INJECT_INPUT_EVENT_MODE_ASYNC);
    }


    private static boolean injectScroll(ServiceManager serviceManager, int x, int y, int hScroll,
                                        int vScroll) {
        MotionEvent event = EventFactory.createScrollEvent(x, y, hScroll, vScroll);
        return serviceManager.getInputManager().injectInputEvent(event, InputManager
                .INJECT_INPUT_EVENT_MODE_ASYNC);
    }

    private static void clickPowerKey(ServiceManager serviceManager) {
        System.out.println("enter screen KEYCODE_POWER");
        KeyEvent[] keyEvents = EventFactory.clickEvent(KeyEvent.KEYCODE_POWER);
        boolean result = true;
        for (KeyEvent keyEvent : keyEvents) {
            result = result & serviceManager.getInputManager().injectInputEvent(keyEvent, InputManager.INJECT_INPUT_EVENT_MODE_ASYNC);
        }
        System.out.println("key KEYCODE_POWER result = " + result);
    }

    private static class StartRd implements Runnable {
        ScreenRecorder screenRecorder;
        private Size size;
        private FileDescriptor fileDescriptor;

        public StartRd(Size size, FileDescriptor fileDescriptor) {
            this.screenRecorder = new ScreenRecorder();
            this.size = size;
            this.fileDescriptor = fileDescriptor;
        }

        @Override
        public void run() {
            System.out.println("开始ScreenRecorder");
            screenRecorder = new ScreenRecorder();
            //开始将数据不断的写入
            screenRecorder.record(size.getWidth() / 2, size.getHeight() / 2, fileDescriptor);
            System.out.println("结束ScreenRecorder");
        }

        public void requestStop() {
            screenRecorder.setStop(true);
        }
    }


    public static void writeFully(FileDescriptor fd, ByteBuffer from) throws IOException {
        // ByteBuffer position is not updated as expected by Os.write() on old Android versions, so
        // count the remaining bytes manually.
        // See <https://github.com/Genymobile/scrcpy/issues/291>.
        int remaining = from.remaining();
        while (remaining > 0) {
            try {
                int w = Os.write(fd, from);
                if (BuildConfig.DEBUG && w < 0) {
                    // w should not be negative, since an exception is thrown on error
                    throw new AssertionError("Os.write() returned a negative value (" + w + ")");
                }
                remaining -= w;
            } catch (ErrnoException e) {
                if (e.errno != OsConstants.EINTR) {
                    throw new IOException(e);
                }
            }
        }
    }

    public static void writeFully(FileDescriptor fd, byte[] buffer, int offset, int len) throws IOException {
        writeFully(fd, ByteBuffer.wrap(buffer, offset, len));
    }
}

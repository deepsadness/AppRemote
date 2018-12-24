package com.cry.cry.appprocessdemo;

import android.os.SystemClock;
import android.view.InputDevice;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class EventFactory {
    /*
    创建一个KeyEvent
     */
    public static KeyEvent keyEvent(int action, int keyCode, int repeat, int metaState) {
        long now = SystemClock.uptimeMillis();
        /**
         * 1. 点击的时间 The time (in {@link android.os.SystemClock#uptimeMillis}) at which this key code originally went down.
         * 2. 事件发生的时间 The time (in {@link android.os.SystemClock#uptimeMillis}) at which this event happened.
         * 3. UP DOWN MULTIPLE 中的一个: either {@link #ACTION_DOWN},{@link #ACTION_UP}, or {@link #ACTION_MULTIPLE}.
         * 4. code The key code. 输入的键盘事件
         * 5. 重复的事件次数。点出次数？ A repeat count for down events (> 0 if this is after the initial down) or event count for multiple events.
         * 6. metaState Flags indicating which meta keys are currently pressed.  暂时不知道什么意思
         * 7. The device ID that generated the key event.
         * 8. Raw device scan code of the event. 暂时不知道什么意思
         * 9. The flags for this key event 暂时不知道什么意思
         * 10. The input source such as {@link InputDevice#SOURCE_KEYBOARD}.
         */
        KeyEvent event = new KeyEvent(now, now, action, keyCode, repeat, metaState,
                KeyCharacterMap.VIRTUAL_KEYBOARD,
                0,
                0,
                InputDevice.SOURCE_KEYBOARD);
        return event;
    }

    /*
    通过送入一个ACTION_DOWN 和ACTION_UP 来模拟一次点击的事件
     */
    public static KeyEvent[] clickEvent(int keyCode) {
        return new KeyEvent[]{keyEvent(KeyEvent.ACTION_DOWN, keyCode, 0, 0)
                , keyEvent(KeyEvent.ACTION_UP, keyCode, 0, 0)};
    }


    private static long lastMouseDown;
    private static final MotionEvent.PointerCoords[] pointerCoords = {new MotionEvent.PointerCoords()};
    private static final MotionEvent.PointerProperties[] pointerProperties = {new MotionEvent
            .PointerProperties()};

    public static MotionEvent createMotionEvent(int type, int x, int y) {
        long now = SystemClock.uptimeMillis();
        int action;
        if (type == 1) {
            lastMouseDown = now;
            action = MotionEvent.ACTION_DOWN;
        } else {
            action = MotionEvent.ACTION_UP;
        }
        MotionEvent.PointerCoords[] pointerCoords = {new MotionEvent.PointerCoords()};
        MotionEvent.PointerCoords coords = pointerCoords[0];
        coords.x = 2 * x;
        coords.y = 2 * y;
        MotionEvent.PointerProperties[] pointerProperties = {new MotionEvent
                .PointerProperties()};
        MotionEvent.PointerProperties props = pointerProperties[0];
        props.id = 0;
        props.toolType = MotionEvent.TOOL_TYPE_FINGER;

        coords = pointerCoords[0];
        coords.orientation = 0;
        coords.pressure = 1;
        coords.size = 1;

        return MotionEvent.obtain(
                lastMouseDown, now,
                action,
                1, pointerProperties, pointerCoords,
                0, 1,
                1f, 1f,
                0, 0,
                InputDevice.SOURCE_TOUCHSCREEN, 0);
    }

    public static MotionEvent createScrollEvent(int x, int y, int hScroll, int vScroll) {
        long now = SystemClock.uptimeMillis();

        MotionEvent.PointerCoords[] pointerCoords = {new MotionEvent.PointerCoords()};
        MotionEvent.PointerCoords coords = pointerCoords[0];
        coords.x = 2 * x;
        coords.y = 2 * y;
        MotionEvent.PointerProperties[] pointerProperties = {new MotionEvent
                .PointerProperties()};
        MotionEvent.PointerProperties props = pointerProperties[0];
        props.id = 0;
        props.toolType = MotionEvent.TOOL_TYPE_FINGER;

        coords = pointerCoords[0];
        coords.orientation = 0;
        coords.pressure = 1;
        coords.size = 1;
        coords.setAxisValue(MotionEvent.AXIS_HSCROLL, hScroll);
        coords.setAxisValue(MotionEvent.AXIS_VSCROLL, vScroll);
        return MotionEvent.obtain(lastMouseDown, now, MotionEvent.ACTION_SCROLL, 1, pointerProperties, pointerCoords, 0, 0, 1f, 1f, 0,
                0, InputDevice.SOURCE_MOUSE, 0);
    }
}

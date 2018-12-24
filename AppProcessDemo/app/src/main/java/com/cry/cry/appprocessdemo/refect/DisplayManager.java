package com.cry.cry.appprocessdemo.refect;

import android.graphics.Point;
import android.os.IInterface;

import com.cry.cry.appprocessdemo.bean.DisplayInfo;
import com.cry.cry.appprocessdemo.bean.Size;

import java.lang.reflect.Method;

/**
 *
 */
public class DisplayManager {
    /**
     * 这个service 对应 final class BinderService extends IDisplayManager.Stub
     */
    private final IInterface service;

    public DisplayManager(IInterface service) {
        this.service = service;
    }

    public DisplayInfo getDisplayInfo() {
        try {
            Object displayInfo = service.getClass().getMethod("getDisplayInfo", int.class)
                    .invoke(service, 0);
            Class<?> cls = displayInfo.getClass();
            // width and height already take the rotation into account
            int width = cls.getDeclaredField("logicalWidth").getInt(displayInfo);
            int height = cls.getDeclaredField("logicalHeight").getInt(displayInfo);
            int rotation = cls.getDeclaredField("rotation").getInt(displayInfo);
            return new DisplayInfo(new Size(width, height), rotation);
        } catch (Exception e) {
            e.printStackTrace();
            throw new AssertionError(e);
        }
    }

    /*
    这方法是在DisplayManager里面有，但是DisplayManagerService内，没有。所以没法调用

    public DisplayInfo getDisplay() {
        try {
            Object display = service.getClass().getMethod("getDisplay", int.class)
                    .invoke(service, 0);
            Point point = new Point();
            Method getSize = display.getClass().getMethod("getSize", Point.class);
            Method getRotation = display.getClass().getMethod("getRotation");
            getSize.invoke(display, point);
            int rotation = (int) getRotation.invoke(display);
            return new DisplayInfo(new Size(point.x, point.y), rotation);
        } catch (Exception e) {
            e.printStackTrace();
            throw new AssertionError(e);
        }
    }
      */
}

package com.cry.cry.appprocessdemo.refect;

import android.annotation.SuppressLint;
import android.os.Build;
import android.os.IInterface;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class PowerManager {
    private final IInterface service;
    private final Method isScreenOnMethod;

    public PowerManager(IInterface service) {
        this.service = service;
        try {
            @SuppressLint("ObsoleteSdkInt") // we may lower minSdkVersion in the future
                    String methodName = Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT_WATCH ? "isInteractive" : "isScreenOn";
            isScreenOnMethod = service.getClass().getMethod(methodName);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
            throw new AssertionError(e);
        }
    }

    public boolean isScreenOn() {
        try {
            return (Boolean) isScreenOnMethod.invoke(service);
        } catch (InvocationTargetException | IllegalAccessException e) {
            throw new AssertionError(e);
        }
    }
}

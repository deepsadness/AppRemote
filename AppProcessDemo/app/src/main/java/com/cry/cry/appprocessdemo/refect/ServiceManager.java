package com.cry.cry.appprocessdemo.refect;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.IBinder;
import android.os.IInterface;

import java.lang.reflect.Method;

@SuppressLint("PrivateApi")
public final class ServiceManager {

    private final Method getServiceMethod;
    private DisplayManager displayManager;
    private PowerManager powerManager;
    private InputManager inputManager;

    public ServiceManager() {
        try {
            getServiceMethod = Class.forName("android.os.ServiceManager").getDeclaredMethod("getService", String.class);
        } catch (Exception e) {
            e.printStackTrace();
            throw new AssertionError(e);
        }
    }

    private IInterface getService(String service, String type) {
        try {
            IBinder binder = (IBinder) getServiceMethod.invoke(null, service);
            Method asInterface = Class.forName(type + "$Stub").getMethod("asInterface", IBinder.class);
            return (IInterface) asInterface.invoke(null, binder);
        } catch (Exception e) {
            e.printStackTrace();
            throw new AssertionError(e);
        }

    }

    public DisplayManager getDisplayManager() {
        if (displayManager == null) {
            IInterface service = getService(Context.DISPLAY_SERVICE, "android.hardware.display.IDisplayManager");
            displayManager = new DisplayManager(service);
        }
        return displayManager;
    }

    public PowerManager getPowerManager() {
        if (powerManager == null) {
            IInterface service = getService(Context.POWER_SERVICE, "android.os.IPowerManager");
            powerManager = new PowerManager(service);
        }
        return powerManager;
    }

    public InputManager getInputManager() {
        if (inputManager == null) {
            IInterface service = getService(Context.INPUT_SERVICE, "android.hardware.input.IInputManager");
            inputManager = new InputManager(service);
        }
        return inputManager;
    }

}

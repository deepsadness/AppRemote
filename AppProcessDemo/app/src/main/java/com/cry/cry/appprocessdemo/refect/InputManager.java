package com.cry.cry.appprocessdemo.refect;

import android.os.IInterface;
import android.view.InputEvent;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class InputManager {

    public static final int INJECT_INPUT_EVENT_MODE_ASYNC = 0;
    public static final int INJECT_INPUT_EVENT_MODE_WAIT_FOR_RESULT = 1;
    public static final int INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH = 2;


    private final IInterface service;
    private final Method injectInputEventMethod;

    public InputManager(IInterface service) {
        this.service = service;
        try {
            injectInputEventMethod = service.getClass().getMethod("injectInputEvent", InputEvent.class, int.class);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
            throw new AssertionError(e);
        }
    }


    public boolean injectInputEvent(InputEvent inputEvent, int mode) {
        try {
            return (Boolean) injectInputEventMethod.invoke(service, inputEvent, mode);
        } catch (InvocationTargetException | IllegalAccessException e) {
            e.printStackTrace();
            throw new AssertionError(e);
        }
    }
}

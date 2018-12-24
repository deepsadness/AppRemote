package com.cry.cry.appprocessdemo.bean;

public class DisplayInfo {
    private final Size size;
    private final int rotation;

    public DisplayInfo(Size size, int rotation) {
        this.size = size;
        this.rotation = rotation;
    }

    public Size getSize() {
        return size;
    }

    public int getRotation() {
        return rotation;
    }

    @Override
    public String toString() {
        return "DisplayInfo{" +
                "size=" + size +
                ", rotation=" + rotation +
                '}';
    }
}

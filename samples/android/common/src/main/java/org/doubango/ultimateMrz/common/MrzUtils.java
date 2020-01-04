/*
 * Copyright (C) 2016-2019 Doubango AI <https://www.doubango.org>
 * License: For non-commercial use only
 * Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
 * WebSite: https://www.doubango.org/webapps/mrz/
 */
package org.doubango.ultimateMrz.common;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.graphics.PointF;
import android.util.Log;

import org.doubango.ultimateMrz.Sdk.UltMrzSdkResult;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.util.LinkedList;
import java.util.List;

/**
 * Utility class
 */
public class MrzUtils {
    static final String TAG = MrzUtils.class.getCanonicalName();
    /**
     * Translation and scaling information used to map image pixels to screen pixels.
     */
    public static class MrzTransformationInfo {
        final int mXOffset;
        final int mYOffset;
        final float mRatio;
        final int mWidth;
        final int mHeight;
        public MrzTransformationInfo(final int imageWidth, final int imageHeight, final int canvasWidth, final int canvasHeight) {
            final float xRatio = (float)canvasWidth / (float)imageWidth;
            final float yRatio =  (float)canvasHeight / (float)imageHeight;
            mRatio = Math.min( xRatio, yRatio );
            mWidth = (int)(imageWidth * mRatio);
            mHeight = (int)(imageHeight * mRatio);
            mXOffset = (canvasWidth - mWidth) >> 1;
            mYOffset = (canvasHeight - mHeight) >> 1;
        }
        public float transformX(final float x) { return x * mRatio + mXOffset; }
        public float transformY(final float y) { return y * mRatio + mYOffset; }
        public PointF transform(final PointF p) { return new PointF(transformX(p.x), transformY(p.y)); }
        public int getXOffset() { return mXOffset; }
        public int getYOffset() { return mYOffset; }
        public float getRatio() { return mRatio; }
        public int getWidth() { return mWidth; }
        public int getHeight() { return mHeight; }
    }

    /**
     * MRZ single line
     */
    static class Line {
        private String mText;
        private float mConfidence;
        private float mWarpedBox[];

        public String getText() { return mText; }
        public float getConfidence() { return mConfidence; }
        public float[] getWarpedBox() { return mWarpedBox; }
    }

    /**
     * MRZ zone.
     * A zone have one or multiple lines.
     */
    static class Zone {
        private List<Line> mLines;
        private float mWarpedBox[];

        public List<Line> getLines() { return mLines; }
        public float[] getWarpedBox() { return mWarpedBox; }
    }

    /**
     * Extracts MRZ zones from a result returned by the engine.
     * @param result The result holding returned by the MRZ engine and containing the zones to extract.
     * @return A list of zones.
     */
    static public final List<Zone> extractZones(final UltMrzSdkResult result) {
        final List<Zone> zones = new LinkedList<>();
        if (!result.isOK() || result.numZones() == 0) {
            return zones;
        }
        final String jsonString = result.json();
        //final String jsonString = "{\"duration\":11523,\"frame_id\":0,\"zones\":[{\"lines\":[{\"confidence\":92.0,\"text\":\"P<CZESPECIMEN<<VZOR<<<<<<<<<<<<<<<<<<<<<<<<<\",\"warpedBox\":[74.88424,733.0735,1235.959,731.3058,1236.07,768.6389,75.00539,773.809]},{\"confidence\":90.0,\"text\":\"99006000<8CZE1102299F16090641152291111<<<<24\",\"warpedBox\":[74.9899,803.8572,1233.987,805.0367,1234.007,839.5286,75.0125,842.2074]}],\"warpedBox\":[75,732,1236,732,1236,840,75,840]}]}";
        if (jsonString == null) { // No zone
            return zones;
        }

        try {
            final JSONObject jObject = new JSONObject(jsonString);
            if (jObject.has("zones")) {
                final JSONArray jZones = jObject.getJSONArray("zones");
                for (int i = 0; i < jZones.length(); ++i) {
                    final JSONObject jZone = jZones.getJSONObject(i);
                    final JSONArray jWarpedBoxZone = jZone.getJSONArray("warpedBox");
                    final Zone zone = new Zone();
                    zone.mLines = new LinkedList<>();
                    zone.mWarpedBox = new float[8];
                    for (int m = 0; m < 8; ++m) {
                        zone.mWarpedBox[m] = (float) jWarpedBoxZone.getDouble(m);
                    }
                    if (jZone.has("lines")) {
                        final JSONArray jLines = jZone.getJSONArray("lines");
                        for (int j = 0; j < jLines.length(); ++j) {
                            final JSONObject jLine = jLines.getJSONObject(j);
                            Line line = new Line();
                            line.mConfidence = (float) jLine.getDouble("confidence");
                            line.mText = jLine.getString("text");
                            line.mWarpedBox = new float[8];
                            final JSONArray jWarpedBoxLine = jLine.getJSONArray("warpedBox");
                            for (int k = 0; k < 8; ++k) {
                                line.mWarpedBox[k] = (float) jWarpedBoxLine.getDouble(k);
                            }
                            zone.mLines.add(line);
                        }
                    }
                    zones.add(zone);
                }
            }
        }
        catch (JSONException e) {
            e.printStackTrace();
            Log.e(TAG, e.toString());
        }
        return zones;
    }

    /**
     * Checks if the returned result is success. An assertion will be raised if it's not the case.
     * In production you should catch the exception and perform the appropriate action.
     * @param result The result to check
     * @return The same result
     */
    static public final UltMrzSdkResult assertIsOk(final UltMrzSdkResult result) {
        if (!result.isOK()) {
            throw new AssertionError("Operation failed: " + result.phrase());
        }
        return result;
    }

    /**
     * Converts the result to String.
     * @param result
     * @return
     */
    static public final String resultToString(final UltMrzSdkResult result) {
        return "code: " + result.code() + ", phrase: " + result.phrase() + ", numZones: " + result.numZones() + ", json: " + result.json();
    }

    /**
     *
     * @param fileName
     * @return Must close the returned object
     */
    static public FileChannel readFileFromAssets(final AssetManager assets, final String fileName) {
        FileInputStream inputStream = null;
        try {
            AssetFileDescriptor fileDescriptor = assets.openFd(fileName);
            inputStream = new FileInputStream(fileDescriptor.getFileDescriptor());
            return inputStream.getChannel();
            // To return DirectByteBuffer: fileChannel.map(FileChannel.MapMode.READ_ONLY, fileDescriptor.getStartOffset(), fileDescriptor.getDeclaredLength());
        } catch (IOException e) {
            e.printStackTrace();
            Log.e(TAG, e.toString());
            return null;
        }
    }
}
/*
 * Copyright (C) 2016-2019 Doubango AI <https://www.doubango.org>
 * License: For non-commercial use only
 * Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
 * WebSite: https://www.doubango.org/webapps/mrz/
 */
package org.doubango.ultimateMrz.benchmark;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.ExifInterface;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.SystemClock;
import android.util.Log;
import android.view.WindowManager;
import android.widget.TextView;

import org.doubango.ultimateMrz.Sdk.ULTMRZ_SDK_IMAGE_TYPE;
import org.doubango.ultimateMrz.Sdk.UltMrzSdkEngine;
import org.doubango.ultimateMrz.Sdk.UltMrzSdkResult;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

/**
 * More info about the Benchmark at https://www.doubango.org/SDKs/mrz/docs/Benchmark.html
 */
public class MrzBenchmarkActivity extends AppCompatActivity {

    /**
     * TAG used for the debug logs.
     */
    static final String TAG = MrzBenchmarkActivity.class.toString();

    /**
     * Defines the debug level to output on the console. You should use "verbose" for diagnostic, "info" in development stage and "warn" on production.
     * JSON name: "debug_level"
     * Default: "info"
     * type: string
     * pattern: "verbose" | "info" | "warn" | "error" | "fatal"
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#debug-level
     */
    static final String CONFIG_DEBUG_LEVEL = "info";

    /**
     * Whether to write the transformed input image to the disk. This could be useful for debugging.
     * JSON name: "debug_write_input_image_enabled"
     * Default: false
     * type: bool
     * pattern: true | false
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#debug-write-input-image-enabled
     */
    static final boolean CONFIG_DEBUG_WRITE_INPUT_IMAGE = false; // must be false unless you're debugging the code

    /**
     * Defines the maximum number of threads to use.
     * You should not change this value unless you know what you’re doing. Set to -1 to let the SDK choose the right value.
     * The right value the SDK will choose will likely be equal to the number of virtual cores.
     * For example, on an octa-core device the maximum number of threads will be 8.
     * JSON name: "num_threads"
     * Default: -1
     * type: int
     * pattern: [-inf, +inf]
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#num-threads
     */
    static final int CONFIG_NUM_THREADS = -1;

    /**
     * Whether to enable GPGPU computing. This will enable or disable GPGPU computing on the computer vision and deep learning libraries.
     * On ARM devices this flag will be ignored when fixed-point (integer) math implementation exist for a well-defined function.
     * For example, this function will be disabled for the bilinear scaling as we have a fixed-point SIMD accelerated implementation.
     * Same for many deep learning parts as we’re using QINT8 quantized inference.
     * JSON name: "gpgpu_enabled"
     * Default: true
     * type: bool
     * pattern: true | false
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#gpgpu-enabled
     */
    static final boolean CONFIG_GPGPU_ENABLED = true;

    /**
     * A device contains a CPU and a GPU. Both can be used for math operations.
     * This option allows using both units. On some devices the CPU is faster and on other it's slower.
     * When the application starts, the work (math operations to perform) is equally divided: 50% for the CPU and 50% for the GPU.
     * Our code contains a profiler to determine which unit is faster and how fast (percentage) it is. The profiler will change how
     * the work is divided based on the time each unit takes to complete. This is why this configuration entry is named "workload balancing".
     * JSON name: "gpgpu_workload_balancing_enabled"
     * Default: false for x86 and true for ARM
     * type: bool
     * pattern: true | false
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#gpgpu-workload-balancing-enabled
     */
    static final boolean CONFIG_GPGPU_WORKLOAD_BALANCING_ENABLED = (System.getProperty("os.arch") == "armv71" || System.getProperty("os.arch") == "aarch64");

    /**
     * Before calling the classifier to determine whether a zone contains a MRZ line we need to segment the text using multi-layer segmenter followed by clustering.
     * The multi-layer segmenter uses hysteresis for the voting process using a [min, max] double thresholding values. This configuration entry defines how low the
     * thresholding values should be. Lower the values are, higher the number of fragments will be and higher the recall will be. High number of fragments means more
     * data to process which means more CPU usage and higher processing time.
     * JSON name: "segmenter_accuracy"
     * Default: high
     * type: string
     * pattern: "veryhigh" | "high" | "medium" | "low" | "verylow"
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#segmenter-accuracy
     */
    static final String CONFIG_SEGMENTER_ACCURACY = "high";

    /**
     * Defines the interpolation method to use when pixels are scaled, deskewed or deslanted. bicubic offers the best quality but is slow as there
     * is no SIMD or GPU acceleration yet. bilinear and nearest interpolations are multithreaded and SIMD accelerated. For most scenarios bilinear
     * interpolation is good enough to provide high accuracy/precision results while the code still runs very fast.
     * JSON name: "interpolation"
     * Default: bilinear
     * type: string
     * pattern: "nearest" | "bilinear" | "bicubic"
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#interpolation
     */
    static final String CONFIG_INTERPOLATION = "bilinear";

    /**
     * Defines the minimum number of MRZ lines needed to form a valid zone. For example, this value must be 2 for passports (TD3 format) and visas (MRVA and MRVB formats).
     * JSON name: "min_num_lines"
     * Default: 2
     * type: int
     * pattern: [1, inf]
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#min-num-lines
     */
    static final int CONFIG_MIN_NUM_LINES = 2;

    /**
     * Defines the Region Of Interest (ROI) for the detector. Any pixels outside region of interest will be ignored by the detector.
     * Defining an WxH region of interest instead of resizing the image at WxH is very important as you'll keep the same quality when you define a ROI while you'll lose in quality when using the later.
     * JSON name: "roi"
     * Default: [0.f, 0.f, 0.f, 0.f]
     * type: float[4]
     * pattern: [left, width, top, height]
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#roi
     */
    static final List<Float> CONFIG_ROI = Arrays.asList(0.f, 0.f, 0.f, 0.f);

    /**
     * Defines a threshold for the recognition score/confidence. Any recognition with a score below that threshold will be ignored/removed.
     * This value could be used to filter the false-positives and improve the precision. Low value will lead to high recall and low precision
     * while a high value means the opposite.
     * Default: 0
     * type: float
     * pattern: [0.f, 1.f]
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#min-score
     */
    static final double CONFIG_MIN_SCORE = 0.0; // 0%

    /**
     * Number of times to try.
     * This number must be high enough (> 10) to make sure the noise is relatively small.
     */
    static final int NUM_LOOPS = 100;

    /**
     * The percentage of images with an MRZ-zone. Within [0.0, 1.0] interval.
     * When a user is using his camera to look for MRZ zones most of the time there is no zone at all.
     * In fact, only 0.08% of the images will likely be positive and the application will stops
     * as soon as a zone is found. So, the application has to run very fast when there is no zone
     * to offer nice user experience (no freezing).
     * In this benchmark we consider 20% of the images have a MRZ zone which is a very high guess.
     */
    static final float PERCENT_POSITIVES = .2f; // 20%

    /**
     * Path to a file with MRZ lines. Used to evaluate the recognizer.
     */
    static final String FILE_NAME_POSITIVE = "Passport-Australia_1280x720.jpg";
    /**
     * Path to a file without MRZ lines. Used to evaluate the decoder.
     */
    static final String FILE_NAME_NEGATIVE = "Passport-France_1200x864.jpg";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Check some values
        if (PERCENT_POSITIVES < 0 || PERCENT_POSITIVES > 1) {
            throw new AssertionError("PERCENT_POSITIVES must be within [0, 1]");
        }
        if (NUM_LOOPS <= 0) {
            throw new AssertionError("NUM_LOOPS must be > 0");
        }

        // Initialize the engine
        // https://www.doubango.org/SDKs/mrz/docs/cpp-api.html#_CPPv4N14ultimateMrzSdk15UltMrzSdkEngine4initEPKc
        UltMrzSdkResult result = assertIsOk(UltMrzSdkEngine.init(
                getAssets(),
                getConfig()
        ));
    }

    @Override
    protected void onResume() {
        Log.i(TAG, "onResume");
        super.onResume();

        final TextView textView = findViewById(R.id.textView);
        textView.setText("*** Preparing... ***");

        // Create image indices
        List<Integer> indices = new ArrayList<>(NUM_LOOPS);
        final int numPositives = (int)(NUM_LOOPS * PERCENT_POSITIVES);
        for (int i = 0; i < numPositives; ++i) {
            indices.add(1); // positive index
        }
        for (int i = numPositives; i < NUM_LOOPS; ++i) {
            indices.add(0); // negative index
        }
        Collections.shuffle(indices); // make the indices random

        // Read the images
        final MrzImage images[] = new MrzImage[2];
        images[0] = readFile(FILE_NAME_NEGATIVE);
        if (images[0] == null) {
            throw new AssertionError("Failed to read file");
        }
        images[1] = readFile(FILE_NAME_POSITIVE);
        if (images[1] == null) {
            throw new AssertionError("Failed to read file");
        }

        textView.setText("*** Started timing... ***");

        // Processing
        Log.i(TAG, "*** Started timing... ***");
        final long startTimeInMillis = SystemClock.uptimeMillis();
        UltMrzSdkResult result = null, resultPositive = null;
        for (Integer i : indices) {
            final MrzImage image = images[i];
            // Processing RGB data: https://www.doubango.org/SDKs/mrz/docs/cpp-api.html#_CPPv4N14ultimateMrzSdk15UltMrzSdkEngine7processEK21ULTMRZ_SDK_IMAGE_TYPEPKvK6size_tK6size_tK6size_tKi
            // Processing YUV data: https://www.doubango.org/SDKs/mrz/docs/cpp-api.html#_CPPv4N14ultimateMrzSdk15UltMrzSdkEngine7processEK21ULTMRZ_SDK_IMAGE_TYPEPKvPKvPKvK6size_tK6size_tK6size_tK6size_tK6size_tK6size_tKi
            assertIsOk((result = UltMrzSdkEngine.process(
                    image.mType,
                    image.mBuffer,
                    image.mWidth,
                    image.mHeight,
                    image.mWidth, // stride
                    image.mJpegOrient
            )));
            if (i == 1) {
                resultPositive = result;
            }
        }
        final long endTimeInMillis = SystemClock.uptimeMillis();
        final long elapsedTime = (endTimeInMillis - startTimeInMillis);
        final float estimatedFps = 1000.f / (elapsedTime / (float)NUM_LOOPS);

        Log.i(TAG, "Result:" + resultToString(result));
        Log.i(TAG, "Elapsed time: " + elapsedTime + " millis, FrameRate: " + estimatedFps);

        textView.setText("Elapsed time: " + (endTimeInMillis - startTimeInMillis) + " millis" + ", Frame rate: " + estimatedFps + "\n\n\n" + resultToString(resultPositive));
    }

    @Override
    public void onDestroy() {
        Log.i(TAG, "onDestroy");
        // DeInit the MRZ engine
        // https://www.doubango.org/SDKs/mrz/docs/cpp-api.html#_CPPv4N14ultimateMrzSdk15UltMrzSdkEngine6deInitEv
        final UltMrzSdkResult result = assertIsOk(UltMrzSdkEngine.deInit());

        super.onDestroy();
    }

    MrzImage readFile(final String name) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        Bitmap bitmap;
        InputStream inputStream;
        try {
            inputStream = getAssets().open(name);
            bitmap = BitmapFactory.decodeStream(inputStream, null, options);
        }
        catch(IOException e) {
            e.printStackTrace();
            Log.e(TAG, e.toString());
            return null;
        }
        if (bitmap.getRowBytes() < bitmap.getWidth() << 2) {
            throw new AssertionError("Not ARGB");
        }

        int orientation = 1;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            ExifInterface exif = null;
            try {
                inputStream.reset();
                exif = new ExifInterface(inputStream);
                final int orientation_ = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION,
                        ExifInterface.ORIENTATION_UNDEFINED);
                if (orientation_ >= 1 && orientation_ <= 8) { // any invalid valid will cause abort in the processing function
                    orientation = orientation_;
                }
            } catch (IOException e) {
                e.printStackTrace();
                Log.e(TAG, "Failed to read EXIF info: " + e.toString());
            }
        }
        else {
            Log.w(TAG, "Cannot extract EXIF information");
        }

        final int widthInBytes = bitmap.getRowBytes();
        final int width = bitmap.getWidth();
        final int height = bitmap.getHeight();
        final ByteBuffer nativeBuffer = ByteBuffer.allocateDirect(widthInBytes * height);
        bitmap.copyPixelsToBuffer(nativeBuffer);
        nativeBuffer.rewind();

        // RGBA format: https://www.doubango.org/SDKs/mrz/docs/cpp-api.html#_CPPv4N14ultimateMrzSdk28ULTMRZ_SDK_IMAGE_TYPE_RGBA32E
        return new MrzImage(ULTMRZ_SDK_IMAGE_TYPE.ULTMRZ_SDK_IMAGE_TYPE_RGBA32, nativeBuffer, width, height,orientation);
    }

    final String getConfig() {
        // More information on the JSON config at https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html
        JSONObject config = new JSONObject();
        try {
            config.put("debug_level", CONFIG_DEBUG_LEVEL);
            config.put("debug_write_input_image_enabled", CONFIG_DEBUG_WRITE_INPUT_IMAGE);
            if (CONFIG_DEBUG_WRITE_INPUT_IMAGE) {
                // Create folder to dump input images for debugging
                File dummyFile = new File(getExternalFilesDir(null), "dummyFile");
                if (!dummyFile.getParentFile().exists() && !dummyFile.getParentFile().mkdirs()) {
                    Log.e(TAG, "mkdir failed: " + dummyFile.getParentFile().getAbsolutePath());
                }
                final String debugInternalDataPath = dummyFile.getParentFile().exists() ? dummyFile.getParent() : Environment.getExternalStorageDirectory().getAbsolutePath();
                dummyFile.delete();
                config.put("debug_internal_data_path", debugInternalDataPath);
            }

            config.put("num_threads", CONFIG_NUM_THREADS);
            config.put("gpgpu_enabled", CONFIG_GPGPU_ENABLED);
            config.put("gpgpu_workload_balancing_enabled", CONFIG_GPGPU_WORKLOAD_BALANCING_ENABLED);


            config.put("segmenter_accuracy", CONFIG_SEGMENTER_ACCURACY);
            config.put("interpolation", CONFIG_INTERPOLATION);
            config.put("min_num_lines", CONFIG_MIN_NUM_LINES);
            config.put("roi", new JSONArray(CONFIG_ROI));
            config.put("min_score", CONFIG_MIN_SCORE);
        }
        catch (JSONException e) {
            e.printStackTrace();
        }
        return config.toString();
    }

    static class MrzImage {
        final ULTMRZ_SDK_IMAGE_TYPE mType;
        final ByteBuffer mBuffer;
        final int mWidth;
        final int mHeight;
        final int mJpegOrient;

        MrzImage(final ULTMRZ_SDK_IMAGE_TYPE type, final ByteBuffer buffer, final int width, final int height, final int jpegOrient) {
            mType = type;
            mBuffer = buffer;
            mWidth = width;
            mHeight = height;
            mJpegOrient = jpegOrient;
        }
    }

    /**
     * Checks if the result is success. Raise an exception if the result is failure.
     * @param result the result to check
     * @return the same result received in param.
     */
    static final UltMrzSdkResult assertIsOk(final UltMrzSdkResult result) {
        if (!result.isOK()) {
            throw new AssertionError("Operation failed: " + result.phrase());
        }
        return result;
    }

    /**
     * Converts the result to String for display.
     * @param result the result to convert
     * @return the String representing the result
     */
    static final String resultToString(final UltMrzSdkResult result) {
        return (result == null)
                ? "null"
                : "code: " + result.code() + ", phrase: " + result.phrase() + ", numZones: " + result.numZones() + ", json: " + result.json();
    }

}

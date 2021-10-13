/*
 * Copyright (C) 2016-2019 Doubango AI <https://www.doubango.org>
 * License: For non-commercial use only
 * Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
 * WebSite: https://www.doubango.org/webapps/mrz/
 */
package org.doubango.ultimateMrz.videorecognizer;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.util.Size;

import org.doubango.ultimateMrz.common.MrzActivity;
import org.doubango.ultimateMrz.common.MrzCameraFragment;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.util.Arrays;
import java.util.List;

/**
 * Main activity
 */
public class MrzVideoRecognizerActivity extends MrzActivity {

    /**
     * TAG used for the debug logs.
     */
    static final String TAG = MrzVideoRecognizerActivity.class.getCanonicalName();

    /**
     * Preferred size for the video stream. Will select the
     * closest size from the camera capabilities.
     */
    static final Size PREFERRED_SIZE = new Size(1280, 720);

    /**
     * The server url used to activate the license. Please contact us to get the real URL.
     * e.g. https://localhost:3600
     */
    static final String ACTIVATION_SERVER_URL = "";

    /**
     * The master or slave key to use for the activation.
     * You MUST NEVER include your master key in the code or share it with the end user.
     * The master key should be used to generate slaves (one-time activation keys).
     * More information about master/slave keys at https://www.doubango.org/SDKs/LicenseManager/docs/Jargon.html.
     */
    static final String ACTIVATION_MASTER_OR_SLAVE_KEY = "";

    /**
     * Base64 string representing the license token.
     * JSON name: "license_token_data"
     * Default: null
     * type: string
     * pattern: base64
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#license-token-data
     */
    static final String LICENSE_TOKEN_DATA = "";

    /**
     * Path to the file containing the license token.
     * JSON name: "license_token_file"
     * Default: null
     * type: string
     * pattern: path
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#license-token-file
     */
    static final String LICENSE_TOKEN_FILE = "";

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
     * Whether to enable backpropagation to detect the MRZ lines.
     * Technical description at https://www.doubango.org/SDKs/mrz/docs/Detection_techniques.html#backpropagation.
     * JSON name: "backpropagation_enabled"
     * Default: true for x86 CPUs and false for ARM CPUs.
     * type: bool
     * pattern: true | false
     * Available since: 2.5.2
     */
    static final boolean CONFIG_BACKPROPAGATION_ENABLED = System.getProperty("os.arch").equals("amd64");

    /**
     * Whether to enable vertical check to detect +/-90deg rotated images. We recommend not enabling this features as it adds significant latency.
     * Technical description at https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#vertical-check-enabled.
     * JSON name: "vertical_check_enabled"
     * Default: true for x86 CPUs and false for ARM CPUs.
     * type: bool
     * pattern: true | false
     * Available since: 2.8.1
     */
    static final boolean CONFIG_VCHECK_ENABLED = System.getProperty("os.arch").equals("amd64");;

    /**
     * Whether to enable Image Enhancement for Low Contrast Document (IELCD).
     * Technical description at https://www.doubango.org/SDKs/mrz/docs/IELCD.html#ielcd.
     * JSON name: "ielcd_enabled"
     * Default: true for x86 CPUs and false for ARM CPUs.
     * type: bool
     * pattern: true | false
     * Available since: 2.6.0
     * More info: https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#ielcd-enabled
     */
    static final boolean CONFIG_IELCD_ENABLED = System.getProperty("os.arch").equals("amd64");

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
     * pattern: [left, right, top, bottom]
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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate " + this);
        super.onCreate(savedInstanceState);

        // At this step, the base class (MrzActivity) already initialized the engine (thanks to "super.onCreate()").
        // Do not try to create the parallel delivery callback in this method. Do it
        // in the constructor or at the declaration (see above). If the engine is initialized without
        // a parallel delivery callback, then it'll run in sequential mode.

        // Add camera fragment to the layout
        getSupportFragmentManager().beginTransaction()
                .replace(R.id.container, MrzCameraFragment.newInstance(PREFERRED_SIZE, this))
                .commit();
    }

    @Override
    public void onResume() {
        super.onResume();

    }

    @Override
    public void onDestroy() {
        Log.i(TAG, "onDestroy " + this);
        super.onDestroy();

        // At this step, the engine is already deInitialized (thanks to "super.onDestroy()").
        // Any call using the engine (UltMrzSdkEngine.*) will miserably fail.
    }

    @Override
    protected int getLayoutResId() {
        return R.layout.activity_main;
    }

    @Override
    protected JSONObject getJsonConfig() {
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
            config.put("backpropagation_enabled", CONFIG_BACKPROPAGATION_ENABLED);
            config.put("vertical_check_enabled", CONFIG_VCHECK_ENABLED);
            config.put("ielcd_enabled", CONFIG_IELCD_ENABLED);

            config.put("segmenter_accuracy", CONFIG_SEGMENTER_ACCURACY);
            config.put("interpolation", CONFIG_INTERPOLATION);
            config.put("min_num_lines", CONFIG_MIN_NUM_LINES);
            config.put("roi", new JSONArray(CONFIG_ROI));
            config.put("min_score", CONFIG_MIN_SCORE);
        }
        catch (JSONException e) {
            e.printStackTrace();
        }
        return config;
    }

    @Override
    protected List<Float> getDetectROI() { return CONFIG_ROI; }

    @Override
    protected String getActivationServerUrl() { return ACTIVATION_SERVER_URL; }

    @Override
    protected String getLicenseTokenData() { return LICENSE_TOKEN_DATA; }

    @Override
    protected String getLicenseTokenFile() { return LICENSE_TOKEN_FILE; }

    @Override
    protected String getActivationMasterOrSlaveKey() { return ACTIVATION_MASTER_OR_SLAVE_KEY; }
}

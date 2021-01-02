'''
    * Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
    * File author: Mamadou DIOP (Doubango Telecom, France).
    * License: For non commercial use only.
    * Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
    * WebSite: https://www.doubango.org/webapps/mrz/


    https://github.com/DoubangoTelecom/ultimateMRZ/blob/master/SDK_dist/samples/c++/recognizer/README.md
	Usage: 
		recognizer.py \
			--image <path-to-image-with-mrzdata-to-recognize> \
			[--assets <path-to-assets-folder>] \
            [--backprop <whether-to-enable-backpropagation:true/false>] \
            [--ielcd <whether-to-enable-IELCD:true/false>] \
			[--tokenfile <path-to-license-token-file>] \
			[--tokendata <base64-license-token-data>]
	Example:
		recognizer.py \
			--image C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dist/assets/images/Czech_passport_2005_MRZ_orient1_1300x1002.jpg \
			--assets C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dist/assets \
            --backprop True \
			--tokenfile C:/Projects/GitHub/ultimate/ultimateMRZ/SDK_dev/tokens/windows-iMac.lic
'''

import ultimateMrzSdk
import sys
import argparse
import json
import platform
import os.path
from PIL import Image, ExifTags

# EXIF orientation TAG
ORIENTATION_TAG = [orient for orient in ExifTags.TAGS.keys() if ExifTags.TAGS[orient] == 'Orientation']

# Defines the default JSON configuration. More information at https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html
JSON_CONFIG = {
    "debug_level": "info",
    "debug_write_input_image_enabled": False,
    "debug_internal_data_path": ".",
        
    "num_threads": -1,
    "gpgpu_enabled": True,
    "gpgpu_workload_balancing_enabled": False,
        
    "segmenter_accuracy": "high",
    "gamma": -1,
    "interpolation": "bilinear",
    "min_num_lines": 2,
    "roi": [0, 0, 0, 0],
    "min_score": 0.0
}

TAG = "[PythonRecognizer] "

# Check result
def checkResult(operation, result):
    if not result.isOK():
        print(TAG + operation + ": failed -> " + result.phrase())
        assert False
    else:
        print(TAG + operation + ": OK -> " + result.json())

# Entry point
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="""
    This is the recognizer sample using python language
    """)

    parser.add_argument("--image", required=True, help="Path to the image with MRZ data to recognize")
    parser.add_argument("--assets", required=False, default="../../../assets", help="Path to the assets folder")
    parser.add_argument("--backprop", required=False, default=True, help="Whether to enable backpropagation to detect the MRZ lines. Technical description at https://www.doubango.org/SDKs/mrz/docs/Detection_techniques.html#backpropagation.")
    parser.add_argument("--ielcd", required=False, default=platform.processor()=='i386', help="Whether to enable Image Enhancement for Low Contrast Document (IELCD). More information at https://www.doubango.org/SDKs/mrz/docs/IELCD.html. Default: true for x86 CPUs and false for ARM CPUs.")    
    parser.add_argument("--tokenfile", required=False, default="", help="Path to license token file")
    parser.add_argument("--tokendata", required=False, default="", help="Base64 license token data")

    args = parser.parse_args()
    IMAGE = args.image
    ASSETS = args.assets
    TOKEN_FILE = args.tokenfile
    TOKEN_DATA = args.tokendata
    

    # Check if image exist
    if not os.path.isfile(IMAGE):
        print(TAG + "File doesn't exist: %s" % IMAGE)
        assert False

    # Decode the image
    image = Image.open(IMAGE)
    width, height = image.size
    if image.mode == "RGB":
        format = ultimateMrzSdk.ULTMRZ_SDK_IMAGE_TYPE_RGB24
    elif image.mode == "RGBA":
        format = ultimateMrzSdk.ULTMRZ_SDK_IMAGE_TYPE_RGBA32
    elif image.mode == "L":
        format = ultimateMrzSdk.ULTMRZ_SDK_IMAGE_TYPE_Y
    else:
        print(TAG + "Invalid mode: %s" % image.mode)
        assert False

    # Read the EXIF orientation value
    exif = image._getexif()
    exifOrientation = exif[ORIENTATION_TAG[0]] if len(ORIENTATION_TAG) == 1 and exif != None else 1

    # Update JSON options using values from the command args
    if ASSETS:
        JSON_CONFIG["assets_folder"] = ASSETS
    if TOKEN_FILE:
        JSON_CONFIG["license_token_file"] = TOKEN_FILE
    if TOKEN_DATA:
        JSON_CONFIG["license_token_data"] = TOKEN_DATA

    JSON_CONFIG["backpropagation_enabled"] = (args.backprop == "True")
    JSON_CONFIG["ielcd"] = (args.ielcd == "True")


    # Initialize the engine
    checkResult("Init", 
                ultimateMrzSdk.UltMrzSdkEngine_init(json.dumps(JSON_CONFIG))
               )

    # Recognize/Process
    # Please note that the first time you call this function all deep learning models will be loaded 
    # and initialized which means it will be slow. In your application you've to initialize the engine
    # once and do all the recognitions you need then, deinitialize it.
    checkResult("Process",
                ultimateMrzSdk.UltMrzSdkEngine_process(
                    format,
                    image.tobytes(), # type(x) == bytes
                    width,
                    height,
                    0, # stride
                    exifOrientation
                    )
        )

    # Press any key to exit
    input("\nPress Enter to exit...\n") 

    # DeInit the engine
    checkResult("DeInit", 
                ultimateMrzSdk.UltMrzSdkEngine_deInit()
               )
    
    
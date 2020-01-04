/* Copyright (C) 2011-2019 Doubango Telecom <https://www.doubango.org>
 * File author: Mamadou DIOP (Doubango Telecom, France).
 * License: For non commercial use only.
 * Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
 * WebSite: https://www.doubango.org/webapps/mrz/
 */

#import "ViewController.h"
#import <ultimate_mrz-sdk/ultimateMRZ-SDK-API-PUBLIC.h> /* Public header for MRZ/MRTD framework */

#import <vector>
#import <chrono>
#import <vector>
#import <algorithm>
#import <random>

// More info about the Benchmark at https://www.doubango.org/SDKs/mrz/docs/Benchmark.html

using namespace ultimateMrzSdk; /* MRZ/MRTD engine C++ namespace */

#define kLoopCount          100

#define kPercentPositives  0.2 // 20%

#define kFileNegativeName   @"Passport-France_1200x864"

#define kFilePositiveName   @"Passport-Australia_1280x720"

@interface ViewController ()

@property (weak, nonatomic) IBOutlet UILabel *label;

@end

@implementation ViewController
CGImageRef imageNegative;
CGImageRef imagePositive;
@synthesize label;

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // The MRZ/MRTD engine is initialized in "AppDelegate.mm" when the application is initialized
    // You must make sure the engine is correctly initialized before calling UltMrzSdkEngine::process()
    
    label.text = @"Benchmark sample application";
    label.numberOfLines = 0;
    
    // Load negative image
    imageNegative = CGImageCreateWithJPEGDataProvider(CGDataProviderCreateWithCFData((CFDataRef)[NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:kFileNegativeName ofType:@"jpg"]]), NULL, true, kCGRenderingIntentDefault);
    if (!imageNegative) {
        label.text = [NSString stringWithFormat:@"Failed to load file: %@", kFileNegativeName];
        ULTMRZ_SDK_PRINT_ERROR("Failed to load file: %s", [kFileNegativeName UTF8String]);
        return;
    }
    
    // Load positive image
    imagePositive = CGImageCreateWithJPEGDataProvider(CGDataProviderCreateWithCFData((CFDataRef)[NSData dataWithContentsOfFile:[[NSBundle mainBundle] pathForResource:kFilePositiveName ofType:@"jpg"]]), NULL, true, kCGRenderingIntentDefault);
    if (!imagePositive) {
        label.text = [NSString stringWithFormat:@"Failed to load file: %@", kFilePositiveName];
        ULTMRZ_SDK_PRINT_ERROR("Failed to load file: %s", [kFilePositiveName UTF8String]);
        return;
    }
}

- (void)viewDidAppear:(BOOL)animated{
    [super viewDidAppear: animated];
    
    // Make sure images are loaded
    if (!imageNegative || !imagePositive) {
        return;
    }
    
    label.text = @"Staring benchmark application...";
    
    // Create image indices
    std::vector<size_t> indices(kLoopCount, 0);
    const int numPositives = (int)std::max(kLoopCount * kPercentPositives, 1.);
    for (int i = 0; i < numPositives; ++i) {
        indices[i] = 1; // positive index
    }
    std::shuffle(std::begin(indices), std::end(indices), std::default_random_engine{}); // make the indices random
    
    // Extract image pointers and size
    // Important: The JPEG images have component stored with R first (lowest address) follow by G
    // then B and A last (highest address). If you're using the camera and requesting kCVPixelFormatType_32BGRA
    // format then R and B are swapped which means the right format to use is ULTMRZ_SDK_IMAGE_TYPE_BGRA32
    // instead of ULTMRZ_SDK_IMAGE_TYPE_RGBA32. See VideoRecognizer sample for more information.
    CFDataRef dataNegative = CGDataProviderCopyData(CGImageGetDataProvider(imageNegative));
    CFDataRef dataPositive = CGDataProviderCopyData(CGImageGetDataProvider(imagePositive));
    const size_t dataBitsPerPixel[] = { CGImageGetBitsPerPixel(imageNegative), CGImageGetBitsPerPixel(imagePositive)};
    const ULTMRZ_SDK_IMAGE_TYPE dataType[] = {
        dataBitsPerPixel[0] == 32 ? ULTMRZ_SDK_IMAGE_TYPE_RGBA32 : ULTMRZ_SDK_IMAGE_TYPE_RGB24,
        dataBitsPerPixel[1] == 32 ? ULTMRZ_SDK_IMAGE_TYPE_RGBA32 : ULTMRZ_SDK_IMAGE_TYPE_RGB24
    };
    const UInt8* dataPtr[2] = { CFDataGetBytePtr(dataNegative), CFDataGetBytePtr(dataPositive) };
    const size_t dataSize[2][3] = {
        { CGImageGetWidth(imageNegative), CGImageGetHeight(imageNegative), CGImageGetBytesPerRow(imageNegative) / (dataBitsPerPixel[0] >> 3)  },
        { CGImageGetWidth(imagePositive), CGImageGetHeight(imagePositive), CGImageGetBytesPerRow(imagePositive) / (dataBitsPerPixel[1] >> 3) }
    };
    
    // Recognize/Process
    const std::chrono::high_resolution_clock::time_point timeStart = std::chrono::high_resolution_clock::now();
    UltMrzSdkResult resultNegative(0, "OK", "{}");
    UltMrzSdkResult resultPositive(0, "OK", "{}");;
    for (const auto& indice : indices) {
        UltMrzSdkResult* result_ = indice ? &resultPositive : &resultNegative;
		// Processing RGB data: https://www.doubango.org/SDKs/mrz/docs/cpp-api.html#_CPPv4N14ultimateMrzSdk15UltMrzSdkEngine7processEK21ULTMRZ_SDK_IMAGE_TYPEPKvK6size_tK6size_tK6size_tKi
        // Processing YUV data: https://www.doubango.org/SDKs/mrz/docs/cpp-api.html#_CPPv4N14ultimateMrzSdk15UltMrzSdkEngine7processEK21ULTMRZ_SDK_IMAGE_TYPEPKvPKvPKvK6size_tK6size_tK6size_tK6size_tK6size_tK6size_tKi
        ULTMRZ_SDK_ASSERT((*result_ = UltMrzSdkEngine::process(
            dataType[indice], // Raw format
            dataPtr[indice], // uncompressed data
            dataSize[indice][0], // width
            dataSize[indice][1], // height
            dataSize[indice][2] // stride
        )).isOK());
    }
    const std::chrono::high_resolution_clock::time_point timeEnd = std::chrono::high_resolution_clock::now();
    const double elapsedTimeInMillis = std::chrono::duration_cast<std::chrono::duration<double >>(timeEnd - timeStart).count() * 1000.0;
    ULTMRZ_SDK_PRINT_INFO("Elapsed time (MRZ) = [[[ %lf millis ]]]", elapsedTimeInMillis);
    
    // Print one of the positive results
    ULTMRZ_SDK_PRINT_INFO("result: %s", resultPositive.json());
    
    // Print estimated frame rate
    const double estimatedFps = 1000.f / (elapsedTimeInMillis / (double)kLoopCount);
    ULTMRZ_SDK_PRINT_INFO("*** elapsedTimeInMillis: %lf, estimatedFps: %lf ***", elapsedTimeInMillis, estimatedFps);
    label.text = [NSString stringWithFormat:@"%@ \n\n\nelapsedTimeInMillis: %lf, estimatedFps: %lf", [NSString stringWithUTF8String:resultPositive.json()], elapsedTimeInMillis, estimatedFps];
    
    // Free images
    CGImageRelease(imageNegative), imageNegative = nil;
    CGImageRelease(imagePositive), imagePositive = nil;
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end

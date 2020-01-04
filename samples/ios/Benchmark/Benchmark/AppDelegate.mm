/* Copyright (C) 2011-2019 Doubango Telecom <https://www.doubango.org>
 * File author: Mamadou DIOP (Doubango Telecom, France).
 * License: For non commercial use only.
 * Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
 * WebSite: https://www.doubango.org/webapps/mrz/
 */

#import "AppDelegate.h"
#import <ultimate_mrz-sdk/ultimateMRZ-SDK-API-PUBLIC.h> /* Public header for MRZ/MRTD framework */

using namespace ultimateMrzSdk; /* MRZ/MRTD engine C++ namespace */

// Configuration for MRZ/MRTD deep learning engine
static const char* kUltimateMrzConfig =
"{"
"\"debug_level\": \"info\","
"\"debug_write_input_image_enabled\": false,"
"\"debug_internal_data_path\": \".\","
""
"\"num_threads\": -1,"
"\"gpgpu_enabled\": true,"
#if defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARMT) || defined(__arm) || defined(__aarch64__)
"\"gpgpu_workload_balancing_enabled\": true,"
#else // Disable GPGPU/CPU work balancing on x86 and enable it on ARM -> https://devtalk.nvidia.com/default/topic/494659/execute-kernels-without-100-cpu-busy-wait-/
"\"gpgpu_workload_balancing_enabled\": false,"
#endif
""
"\"segmenter_accuracy\": \"high\","
"\"interpolation\": \"bilinear\","
"\"min_num_lines\": 2,"
"\"roi\": [0, 0, 0, 0],"
"\"min_score\": 0.0"
"}";

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Initialize the MRZ/MRTD engine
	// https://www.doubango.org/SDKs/mrz/docs/cpp-api.html#_CPPv4N14ultimateMrzSdk15UltMrzSdkEngine4initEPKc
    ULTMRZ_SDK_PRINT_INFO("Initializing the MRZ/MRTD engine...");
    ULTMRZ_SDK_ASSERT(UltMrzSdkEngine::init(kUltimateMrzConfig).isOK());
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // DeInitialize the MRZ/MRTD engine
	// https://www.doubango.org/SDKs/mrz/docs/cpp-api.html#_CPPv4N14ultimateMrzSdk15UltMrzSdkEngine6deInitEv
    ULTMRZ_SDK_PRINT_INFO("DeInitializing the MRZ/MRTD engine...");
    ULTMRZ_SDK_ASSERT(UltMrzSdkEngine::deInit().isOK());
    
}


@end

/* Copyright (C) 2011-2019 Doubango Telecom <https://www.doubango.org>
 * File author: Mamadou DIOP (Doubango Telecom, France).
 * License: For non commercial use only.
 * Source code: https://github.com/DoubangoTelecom/ultimateMRZ-SDK
 * WebSite: https://www.doubango.org/webapps/mrz/
 */

#import "ViewController.h"
#import <ultimate_mrz-sdk/ultimateMRZ-SDK-API-PUBLIC.h> /* Public header for MRZ/MRTD framework */

using namespace ultimateMrzSdk; /* MRZ/MRTD engine C++ namespace */

// OCR-B Font size
#define kFontSize       12.f

// Stroke with in DPI for drawing
#define kStrokenWidth   3.0f

// This is for testing only. Please use validation to make sure the MRZ fields are correct
// https://www.doubango.org/SDKs/mrz/docs/Data_validation.html
#define kMinConfidenceToNominateAsOK    30.f

// Video preset/size
#define kVideoPreset    AVCaptureSessionPreset1280x720

@interface ViewController ()
@property (nonatomic, strong) AVCaptureSession *session;
@property (nonatomic, strong) AVCaptureVideoPreviewLayer *previewLayer;
@property (nonatomic, strong) CAShapeLayer* shapeLayer;
@property (nonatomic, strong) UIFont* fontOCRB;
@property (nonatomic, strong) NSDictionary* textAttributes;
@property (atomic, strong) NSDictionary* latestResult;
@property int exifOrientation;
@property CGFloat imageWidth;
@property CGFloat imageHeight;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Set default EXIF orientation to "Normal"
    self.exifOrientation = 1;
    
    // https://developer.apple.com/documentation/uikit/text_display_and_fonts/adding_a_custom_font_to_your_app
    // family:'OcrB' font:'OcrB-Regular'
    self.fontOCRB = [UIFont fontWithName:@"OcrB-Regular" size: kFontSize];
    NSAssert(self.fontOCRB != nil, @"Failed to load OCR-B font");
    self.textAttributes = @{NSFontAttributeName: self.fontOCRB, NSForegroundColorAttributeName:[UIColor blackColor] };
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void) viewWillAppear:(BOOL)animated {
    [super viewWillAppear: animated];
    
    [self setupCaptureSession];
    
    [self updateOrientation];
}

-(void) viewDidAppear:(BOOL)animated {
    [super viewDidAppear: animated];
}

- (void) viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    
    [self updateOrientation];
}

// Update video orientation info based on device orientation
// This function must be called on UI-thread
- (void) updateOrientation {
    AVCaptureConnection* connection  = self.previewLayer.connection;
    if (connection.supportsVideoOrientation) {
        switch([[UIApplication sharedApplication] statusBarOrientation]) { // FIXME(dmi): deprecated
            case UIInterfaceOrientationUnknown:
                ULTMRZ_SDK_PRINT_INFO("Orientation: UIInterfaceOrientationUnknown");
                break;
            case UIInterfaceOrientationPortrait:
                connection.videoOrientation = AVCaptureVideoOrientationPortrait;
                ULTMRZ_SDK_PRINT_INFO("Orientation: UIInterfaceOrientationPortrait");
                break;
            case UIInterfaceOrientationPortraitUpsideDown:
                connection.videoOrientation = AVCaptureVideoOrientationPortrait; // On purpose to make sure preview is really upsidedown (No EXIF equivalent)
                ULTMRZ_SDK_PRINT_INFO("Orientation: UIInterfaceOrientationPortraitUpsideDown");
                break;
            case UIInterfaceOrientationLandscapeLeft:
                connection.videoOrientation = AVCaptureVideoOrientationLandscapeLeft;
                ULTMRZ_SDK_PRINT_INFO("Orientation: UIInterfaceOrientationLandscapeLeft");
                break;
            case UIInterfaceOrientationLandscapeRight:
                connection.videoOrientation = AVCaptureVideoOrientationLandscapeRight;
                ULTMRZ_SDK_PRINT_INFO("Orientation: UIInterfaceOrientationLandscapeRight");
                break;
        }
    }
    
    self.previewLayer.frame = self.view.bounds;
    self.shapeLayer.frame = self.view.bounds;
}

// Transform a point from image coord. sys. to the preview layer coord. sys.
-(CGPoint) transformPoint: (CGPoint)point {
    // The transformed point
    CGPoint pointT = point;
    
    // 1: Normal - AVCaptureVideoOrientationLandscapeRight
    // 3: Rotate 180 CW - AVCaptureVideoOrientationLandscapeLeft
    // 5: Mirror horizontal and rotate 270 CW - AVCaptureVideoOrientationPortraitUpsideDown
    // 6: Rotate 90 CW - AVCaptureVideoOrientationPortrait
    if (self.exifOrientation == 5 || self.exifOrientation == 6) { // Portrait Mode
        if (self.exifOrientation == 6) {
            // Code in "ultimate_mrz_sdk_exif.cxx" function "__UltMrzSdkExifProcessFast6"
            pointT.x = (self.imageHeight - pointT.x); // In portrait mode width and height are swapped, this is why we're substracting height instead of width.
        }
        // In portrait mode width and heigth are swapped
        const CGFloat sx = 1.f / (float)self.imageHeight;
        const CGFloat sy = 1.f / (float)self.imageWidth;
        pointT = [self.previewLayer pointForCaptureDevicePointOfInterest:
                            CGPointMake(pointT.y*sy, pointT.x*sx)];
    }
    else { // Landscape Mode
        const CGFloat sx = 1.f / (float)self.imageWidth;
        const CGFloat sy = 1.f / (float)self.imageHeight;
        if (self.exifOrientation == 3) {
            // Code in "ultimate_mrz_sdk_exif.cxx" function "__UltMrzSdkExifProcessFast3"
            pointT.x = (self.imageWidth - pointT.x);
            pointT.y = (self.imageHeight - pointT.y);
        }
        pointT = [self.previewLayer pointForCaptureDevicePointOfInterest:
                            CGPointMake(pointT.x*sx, pointT.y*sy)];
    }
    
    return pointT;
}

// Implements "CALayerDelegate"
- (void)drawLayer:(CALayer*)layer inContext:(CGContextRef)ctx {
    // Check if the JSON result is empty. If empty
    // then, clear the layer and do nothing
    NSDictionary* jsonObject = [self.latestResult copy];
    if (jsonObject == nil || jsonObject.count == 0) {
        CGContextClearRect(ctx, self.shapeLayer.bounds);
        return;
    }
    
    // Push the context as current
    UIGraphicsPushContext(ctx);
    
    // Draw duration (aka Inference time)
    if ([[jsonObject allKeys] containsObject:@"duration"]) {
        NSString* duration = [NSString stringWithFormat:@"Inference time: %@", [jsonObject objectForKey:@"duration"]];
        CGSize textSize = [duration sizeWithAttributes: self.textAttributes];
        CGPoint leftCorner = CGPointMake(0.f, 0.f);
        
        CGContextSetFillColorWithColor(ctx, [UIColor whiteColor].CGColor);
        CGContextFillRect(ctx, CGRectMake(leftCorner.x, leftCorner.y, textSize.width, textSize.height));
        [duration drawAtPoint:leftCorner withAttributes: self.textAttributes];
    }
    
    // Loop through the JSON entries and draw the result to the screen
    if ([[jsonObject allKeys] containsObject:@"zones"]) {
        // X-Offset = half the stroke width
        const CGFloat offsetX = (kStrokenWidth * 0.5f /** [[UIScreen mainScreen] scale]*/);
        const CGFloat offsetNice = offsetX * 5; // To give the zone border a nice room
        // JSON zones
        NSMutableArray* jZones = [jsonObject objectForKey:@"zones"];
        for (int i = 0; i < jZones.count; ++i) {
            // JSON entries for the Zone
            NSDictionary* jZone = [jZones objectAtIndex: i];
            NSMutableArray* jWarpedBoxZone = [jZone objectForKey: @"warpedBox"];
            
            // The next code to check the MRZ lines isn't correct. You have
            // to use the validation process as explained at https://www.doubango.org/SDKs/mrz/docs/Data_validation.html to check
            // the validity.
            bool looksOK = true;
            NSString* confidences = @"";
            if ([[jZone allKeys] containsObject:@"lines"]) {
                NSMutableArray* jLines = [jZone objectForKey: @"lines"];
                const NSUInteger expectTextLength =  [[[jLines objectAtIndex: 0] objectForKey:@"text"] length];
                for (int j = 0; j < jLines.count; ++j) {
                    NSDictionary* jLine = [jLines objectAtIndex: j];
                    NSString* text = [jLine objectForKey:@"text"];
                    NSNumber* confidence = [jLine objectForKey:@"confidence"];
                    looksOK &= ([confidence floatValue] >= kMinConfidenceToNominateAsOK)
                        && [text length] == expectTextLength;
                    confidences = [NSString stringWithFormat:@"%@%@%.2f%%", confidences, (!confidences.length ? @"" : @" - "), [confidence floatValue]];
                }
            }
            
            // Defines the drawing color base on the validity
            CGColorRef defaultDrawingColor = looksOK ? [UIColor greenColor].CGColor : [UIColor redColor].CGColor;
            
            // WarpedBox for the entire zone (contains the MRZ lines)
            const CGPoint warpedBoxZone[] = {
                [self transformPoint: CGPointMake([[jWarpedBoxZone objectAtIndex: 0] floatValue] - offsetNice, [[jWarpedBoxZone objectAtIndex: 1] floatValue] - offsetNice)],
                [self transformPoint: CGPointMake([[jWarpedBoxZone objectAtIndex: 2] floatValue] + offsetNice, [[jWarpedBoxZone objectAtIndex: 3] floatValue] - offsetNice)],
                [self transformPoint: CGPointMake([[jWarpedBoxZone objectAtIndex: 4] floatValue] + offsetNice, [[jWarpedBoxZone objectAtIndex: 5] floatValue] + offsetNice)],
                [self transformPoint: CGPointMake([[jWarpedBoxZone objectAtIndex: 6] floatValue] - offsetNice, [[jWarpedBoxZone objectAtIndex: 7] floatValue] + offsetNice)],
                [self transformPoint: CGPointMake([[jWarpedBoxZone objectAtIndex: 0] floatValue] - offsetNice, [[jWarpedBoxZone objectAtIndex: 1] floatValue] - offsetNice)]
            };
            
            // Draw Zone's warped box
            CGContextSetStrokeColorWithColor(ctx, defaultDrawingColor);
            CGContextSetLineWidth(ctx, kStrokenWidth);
            CGContextAddLines(ctx, warpedBoxZone, sizeof(warpedBoxZone)/sizeof(warpedBoxZone[0]));
            CGContextStrokePath(ctx);
            
            // Draw confidences
            CGPoint leftCorner = CGPointMake(warpedBoxZone[3].x - offsetX, warpedBoxZone[3].y);
            CGSize textSize = [confidences sizeWithAttributes: self.textAttributes];
            CGContextSetFillColorWithColor(ctx, defaultDrawingColor);
            CGContextFillRect(ctx, CGRectMake(leftCorner.x, leftCorner.y, textSize.width, textSize.height));
            [confidences drawAtPoint:leftCorner withAttributes: self.textAttributes];
            
            // Unpack lines in the zone and draw them
            if ([[jZone allKeys] containsObject:@"lines"]) {
                NSMutableArray* jLines = [jZone objectForKey: @"lines"];
                leftCorner = CGPointMake(warpedBoxZone[0].x - offsetX, warpedBoxZone[0].y);
                for (int j = (int)(jLines.count - 1); j >= 0; --j) {
                    NSDictionary* jLine = [jLines objectAtIndex: j];
                    // Text
                    NSString* text = [jLine objectForKey:@"text"];
                    textSize = [text sizeWithAttributes: self.textAttributes];
                    
                    // Padding to draw top instead of mid
                    leftCorner.y -= (textSize.height * 1.f);
                    
                    // Actual drawing for the Text
                    CGContextSetFillColorWithColor(ctx, defaultDrawingColor);
                    CGContextFillRect(ctx, CGRectMake(leftCorner.x, leftCorner.y, textSize.width, textSize.height));
                    [text drawAtPoint:leftCorner withAttributes: self.textAttributes];
                    
                    // Draw the baselines for the MRZ lines
                    NSMutableArray* jWarpedBoxZoneLine = [jLine objectForKey: @"warpedBox"];
                    if ([[jWarpedBoxZoneLine objectAtIndex: 0] floatValue] != -1.f) { // -1.f means trial version
                        const CGPoint baseLine[] = {
                            [self transformPoint: CGPointMake([[jWarpedBoxZoneLine objectAtIndex: 6] floatValue], [[jWarpedBoxZoneLine objectAtIndex: 7] floatValue])],
                            [self transformPoint: CGPointMake([[jWarpedBoxZoneLine objectAtIndex: 4] floatValue], [[jWarpedBoxZoneLine objectAtIndex: 5] floatValue])]
                        };
                        CGContextSetStrokeColorWithColor(ctx, [UIColor yellowColor].CGColor);
                        CGContextSetLineWidth(ctx, 1);
                        CGContextAddLines(ctx, baseLine, sizeof(baseLine)/sizeof(baseLine[0]));
                        CGContextStrokePath(ctx);
                    }
                }
            }
        }
    }
    
    // Pop context
    UIGraphicsPopContext();
}

- (void) processResult: (NSString *)json {
    
#if 0
    NSString* jsonString = @"{\"duration\":11523,\"frame_id\":0,\"zones\":[{\"lines\":[{\"confidence\":92.0,\"text\":\"P<CZESPECIMEN<<VZOR<<<<<<<<<<<<<<<<<<<<<<<<<\",\"warpedBox\":[74.88424,733.0735,1235.959,731.3058,1236.07,768.6389,75.00539,773.809]},{\"confidence\":90.0,\"text\":\"99006000<8CZE1102299F16090641152291111<<<<24\",\"warpedBox\":[74.9899,803.8572,1233.987,805.0367,1234.007,839.5286,75.0125,842.2074]}],\"warpedBox\":[75,732,1236,732,1236,840,75,840]}]}";
#else
    NSString* jsonString = json;
#endif
    
    if (jsonString == nil) {
        self.latestResult = nil;
    }
    else {
        NSData* jsonData = [jsonString dataUsingEncoding: NSUTF8StringEncoding];
        NSError *jsonError = nil;
        self.latestResult = [NSJSONSerialization JSONObjectWithData:jsonData options:kNilOptions error:&jsonError];
        
        if (jsonError != nil) {
             NSLog(@"JSON parsing failed: %@", [jsonError localizedDescription]);
        }
    }
    
    [self.shapeLayer setNeedsDisplay];
}

// Create and configure a capture session and start it running
- (void) setupCaptureSession {
    NSError *error = nil;
    
    // Create the session
    AVCaptureSession *session = [[AVCaptureSession alloc] init];
    
    // Configure the session to produce lower resolution video frames, if your
    // processing algorithm can cope. We'll specify medium quality for the
    // chosen device.
    session.sessionPreset = kVideoPreset;
    
    // Find a suitable AVCaptureDevice
    AVCaptureDevice *device = [AVCaptureDevice
                               defaultDeviceWithMediaType:AVMediaTypeVideo];
    
    // Create a device input with the device and add it to the session.
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];
    
    // Check error
    if (error) {
         NSLog(@"Failed to find input device: %@", [error localizedDescription]);
    }
    // Must not use assertion in your app, this is sample app
    NSAssert(input != nil && error == nil, @"Failed to find input device");
    
    // Add input to the session
    [session addInput:input];
    
    // Create a VideoDataOutput and add it to the session
    AVCaptureVideoDataOutput *output = [[AVCaptureVideoDataOutput alloc] init];
    [session addOutput:output];
    
    // Configure your output.
    dispatch_queue_t queue = dispatch_queue_create("videoCaptureQueue", NULL);
    [output setSampleBufferDelegate:self queue:queue];
    
    output.alwaysDiscardsLateVideoFrames = YES;
    
    // Specify the pixel format
    output.videoSettings =
    [NSDictionary dictionaryWithObject:
     [NSNumber numberWithInt:kCVPixelFormatType_32BGRA]
                                forKey:(id)kCVPixelBufferPixelFormatTypeKey];
    
    // Setup preview layer
    [self setPreviewLayer: [AVCaptureVideoPreviewLayer layerWithSession: session]];
    [self.previewLayer setVideoGravity: AVLayerVideoGravityResizeAspectFill];
    [[self.view layer] addSublayer:self.previewLayer];
    
    // Setup Overlay
    self.shapeLayer = [CAShapeLayer layer];
    self.shapeLayer.frame = self.view.frame;
    self.shapeLayer.delegate = self;
    [self.previewLayer addSublayer:self.shapeLayer];
    
    // Start capturing
    [session startRunning];
    
    // Save session
    [self setSession:session];
}

// Implements "AVCaptureVideoDataOutputSampleBufferDelegate"
- (void)captureOutput:(AVCaptureOutput *)captureOutput
        didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
        fromConnection:(AVCaptureConnection *)connection
{
    // Exif orientation
    switch (self.previewLayer.connection.videoOrientation) {
        case AVCaptureVideoOrientationPortraitUpsideDown:
            self.exifOrientation = 5; // Mirror horizontal and rotate 270 CW
            break;
        case AVCaptureVideoOrientationPortrait:
            self.exifOrientation = 6; // Rotate 90 CW
            break;
        case AVCaptureVideoOrientationLandscapeLeft:
            self.exifOrientation = 3; // Rotate 180 CW
            break;
        case AVCaptureVideoOrientationLandscapeRight:
        default:
            self.exifOrientation = 1; // Normal
            break;
    }
    
    // Get a CMSampleBuffer's Core Video image buffer for the media data
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    
    // Make sure we have RGBA frames as requested
    // Must not use assertion in your app, this is sample app
    NSAssert(CVPixelBufferGetPixelFormatType(imageBuffer) == kCVPixelFormatType_32BGRA, @"Not RGBA format");
    
    // Lock the base address of the pixel buffer
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    
    // Compute current width/height used in the drawing process
    // to convert from camera coord sys. to layer sys.
    self.imageWidth = CVPixelBufferGetWidth(imageBuffer);
    self.imageHeight = CVPixelBufferGetHeight(imageBuffer);
    
    // -- MRZ/MRTD Processing --
    // Very Important: Before calling this function make sure the engine is initialized.
    // In our case the initialization is done in "AppDelegate.mm". You can move it to this
    // file and do it in "viewDidLoad"
	// Processing RGB data: https://www.doubango.org/SDKs/mrz/docs/cpp-api.html#_CPPv4N14ultimateMrzSdk15UltMrzSdkEngine7processEK21ULTMRZ_SDK_IMAGE_TYPEPKvK6size_tK6size_tK6size_tKi
    // Processing YUV data: https://www.doubango.org/SDKs/mrz/docs/cpp-api.html#_CPPv4N14ultimateMrzSdk15UltMrzSdkEngine7processEK21ULTMRZ_SDK_IMAGE_TYPEPKvPKvPKvK6size_tK6size_tK6size_tK6size_tK6size_tK6size_tKi
    const UltMrzSdkResult result = UltMrzSdkEngine::process(
        ULTMRZ_SDK_IMAGE_TYPE_BGRA32, // Raw format: kCVPixelFormatType_32BGRA
        CVPixelBufferGetBaseAddress(imageBuffer), // uncompressed data
        self.imageWidth, // width
        self.imageHeight, // height
        CVPixelBufferGetBytesPerRow(imageBuffer) >> 2, // stride
        self.exifOrientation
    );
    
    // Request redraw using the new result.
    [self performSelectorOnMainThread: @selector(processResult:)
                        withObject: result.isOK() ? [NSString stringWithUTF8String: result.json()] : nil
                        waitUntilDone: NO]; // do not wait until done

    // Print result to the console
    // In the final application you should remove the next lines
    // to avoid logging the MRZ data to the device logs
    if (!result.isOK()) {
         ULTMRZ_SDK_PRINT_ERROR("Result NOK: %s", result.json());
    }
    else if (result.isOK()) {
        ULTMRZ_SDK_PRINT_INFO("Result OK: %s", result.json());
    }
    
    // Unlock memory address
    CVPixelBufferUnlockBaseAddress(imageBuffer,0);
}

@end

- [Android](#android)
  - [License format](#android-license-format)
    - [The application identifier](#android-license-format-appid)
    - [List of installers](#android-license-format-installers)
    - [Signing certificate's fingerprint (SHA256)](#android-license-format-fingerprint)
  - [License generation](#android-license-generation)
  - [License deployment](#android-license-deployment)
- [iOS](#ios)

<hr />

Starting version **v2.10.0** we have added support for application-wide licenses. This kind of license is attached to an application identifier rather than a device.
This is suitable for applications distributed via Google Play or any other store with no limitation on the number of devices.

<a name="android"></a>
# Android #
Everything related to the license is done in C++ to make it harder to decompile.

<a name="android-license-format"></a>
## License format ##
The license is a base64 encrypted string containing 3 elements:
  - The application identifier
  - The list of allowed installers (stores)
  - The signing certificate's fingerprint
 
At runtime we'll check that:
  - the application identifier matches the one in the license string `check #1`
  - the application was signed using the same signature stored in the license string `check #2`
  - the application was installed from a store listed in the license string (Google Play, Amazon, Huawei...) `check #3`
 
The above checks means you'll not be able to test a license while developing your app unless the app is deployed and installed from a store. You'll be able to pass `check #1` and `check #2` but not `check #3` if the license is correctly configured. We offer (free) #3 device-based licenses to help you test your app.

<a name="android-license-format-appid"></a>
### The application identifier (SHA256) ###
The application identifier is in your `build.gradle` at `android {` -> `defaultConfig {` -> `applicationId "<< your appid here >>"`. **The license will no longer work if you change this identifier**. This identifier must remain the same for the application's lifetime.
Example -> https://github.com/DoubangoTelecom/ultimateMRZ-SDK/blob/68c998b3d8ed4f5e0a71fdac3bc04f3f79352224/samples/android/videorecognizer/build.gradle#L8.

<a name="android-license-format-installers"></a>
### List of installers ###
The license will only work with applications installed from a registered store. By default, we support **Google Play, Amazon and Huawei** stores. New stores can be added on request.

<a name="android-license-format-fingerprint"></a>
### Signing certificate's fingerprint ###
An application must be signed before deploying it on a store. The signing keys must remain the same for the application's lifetime. By default these keys are [valid for at least 25 years](https://developer.android.com/studio/publish/app-signing#:~:text=Your%20key%20should%20be%20valid,about%20yourself%20for%20your%20certificate.).

Please note that the dev (debug) keys used to sign an application are not valid for the app stores. On Windows, the dev keystore is at `C:\Users\<<name>>\.android\debug.keystore`. Please do not use that keystore for license generation.

We'll need the certificate's fingerprint (**SHA256**) to generate your license. The fingerprint could be generated using the following command: `keytool -list -v -keystore <keystore path> -alias <key alias> -storepass <store password> -keypass <key password>`. Example: `keytool -list -v -keystore "C:\Projects\AndroidKeystore\android_keystore.jks" -alias myalias -storepass mypass1 -keypass mypass2`.

For information on how to retrive the certificate's fingerprint: https://help.short.io/en/articles/4171170-where-to-find-android-app-package-name-and-sha256-fingerprint-certificate

<a name="android-license-generation"></a>
## License generation ##
In order to generate a license for your application we'll need 2 elements:
  - The application identifier
  - The certificate's fingerprint (SHA-256)

<a name="android-license-deployment"></a>
## License deployment ##
The license is a base64 encrypted string. It could be stored in a file and loaded at runtime or hard-coded. JSON configuration entry [license_token_data](https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html#license-token-data) is used to pass the license to the MRZ engine in order to unlock the SDK.

<a name="ios"></a>
# iOS #
Coming soon...

#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <array>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include "LibCamera.h"

using namespace cv;
using namespace std;
int main() {
    time_t start_time = time(0);
    int frame_count = 0;
    float lens_position = 100;
    float focus_step = 50;
    LibCamera cam;
    uint32_t width = 640;
    uint32_t height = 480;
    uint32_t stride;
    char key;
    int ret = cam.initCamera();
    cam.configureStill(width, height, formats::RGB888, 4, 0);
    ControlList controls_;
    int64_t frame_time = 1000000 / 30;
    // Set frame rate
	controls_.set(controls::FrameDurationLimits, libcamera::Span<const int64_t, 2>({ frame_time, frame_time }));
    // Adjust the brightness of the output images, in the range -1.0 to 1.0
    controls_.set(controls::Brightness, 0.5);
    // Adjust the contrast of the output image, where 1.0 = normal contrast
    controls_.set(controls::Contrast, 1.5);
    // Set the exposure time
    controls_.set(controls::ExposureTime, 10000);
    cam.set(controls_);
    int32_t User_Exposure_Time = 10000; 
    if (!ret) {
        bool flag;
        LibcameraOutData frameData;
        cam.startCamera();
        while (true) {
            flag = cam.readFrame(&frameData);
            if (!flag)
                continue;
            Mat im(height, width, CV_8UC3, frameData.imageData);

            imshow("libcamera-demo", im);
            key = waitKey(1);
            if (key == 'q') {
                break;
            } else if (key == 'f') {
                controls_.set(controls::AfMode, controls::AfModeAuto);
                controls_.set(controls::AfTrigger, 0);
                cam.set(controls_);
            } else if (key == 'a' || key == 'A') {
                lens_position += focus_step;
            } else if (key == 'd' || key == 'D') {
                lens_position -= focus_step;
            } else if (key =='t' || key == 'T'){
                User_Exposure_Time = User_Exposure_Time + 1000;
                // Set the exposure time
                controls_.set(controls::ExposureTime, User_Exposure_Time);
                cam.set(controls_);
            } else if (key =='y' || key == 'Y'){
                User_Exposure_Time = User_Exposure_Time - 1000;
                // Set the exposure time
                controls_.set(controls::ExposureTime, User_Exposure_Time);
                cam.set(controls_);
            }else if (key =='s' || key == 'S'){
                imwrite("Test.bmp", im);
            }
            // To use the manual focus function, libcamera-dev needs to be updated to version 0.0.10 and above.
            if (key == 'a' || key == 'A' || key == 'd' || key == 'D') {

                controls_.set(controls::AfMode, controls::AfModeManual);
				controls_.set(controls::LensPosition, lens_position);
                cam.set(controls_);
            }

            frame_count++;
            if ((time(0) - start_time) >= 1){
           
                auto ExpT =controls_.get<int32_t>(controls::ExposureTime);
                printf("fps:%d_ExpT:%dus\n", frame_count,ExpT.value());
                
                frame_count = 0;
                start_time = time(0);
            }
            cam.returnFrameBuffer(frameData);
        }
        destroyAllWindows();
        cam.stopCamera();
    }
    cam.closeCamera();
    return 0;
}

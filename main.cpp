#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <array>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "LibCamera.h"

using namespace cv;
using namespace std;
int main() {
    time_t start_time = time(0);
    int frame_count = 0;
    float lens_position = 100;
    float focus_step = 50;
    LibCamera cam;
    uint32_t width = 1280;
    uint32_t height = 960;
    uint32_t stride;
    char key;
    int ret = cam.initCamera();
    cam.configureStill(width, height, formats::RGB888, 4, 180);
    ControlList controls_;
    int64_t frame_time = 1000 / 10;
    float User_Brightness = 0.0;
    float User_Contrast = 1.0;
    int32_t User_BlackLevel = 1000;
    float User_AnalogGain = 1;
    float User_DigitalGain;
    float User_SensorTemperature;
    int32_t User_Exposure_Time = 1000;
    int32_t ImageSet_White_Test[40] = {100, 1000, 1500,2000	, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000,20000, 30000,40000,50000,60000,70000,80000,90000,
            100000,150000,200000,250000,300000,350000,400000,450000,500000,550000,600000,650000,700000,750000,800000,850000,900000,950000,1000000 };
    int32_t ImageSet_HDR[3] = {1000, 5000, 20000};
    int index =0;
    //libcamera::Span<const int32_t, 4> BLs =controls_.get(controls::SensorBlackLevels);
    auto ALgain =controls_.get<float>(controls::AnalogueGain);
    auto DLgain =controls_.get<float>(controls::DigitalGain);
    auto temp =controls_.get<float>(controls::SensorTemperature);
    
    printf("_%.2f_%.2f_%.2f",*ALgain,*DLgain,*temp );
    //impossible to set Sensor Black level. only for inquiring parameter
    //controls_.set(controls::SensorBlackLevels, libcamera::Span<const int32_t, 4>({ User_BlackLevel, User_BlackLevel,User_BlackLevel,User_BlackLevel }));
    // Set frame rate
	controls_.set(controls::FrameDurationLimits, libcamera::Span<const int64_t, 2>({ frame_time, frame_time }));
    // Adjust the brightness of the output images, in the range -1.0 to 1.0
    controls_.set(controls::Brightness, User_Brightness);
    // Adjust the contrast of the output image, where 1.0 = normal contrast
    controls_.set(controls::Contrast, User_Contrast);
    // Set the exposure time
    controls_.set(controls::ExposureTime, 100000);
    //controls_.set(controls::SensorBlackLevels,User_BlackLevel);
    controls_.set(controls::AnalogueGain,2);
    
    cam.set(controls_);
    int SaveCount = 0;
    int roi_OffsetX =0;
    int roi_OffsetY =0;
    int roi_Width =width;
    int roi_Height =height;
    int scale = 1;
    int hMove = 1;
    int vMove = 1;
    double min, max;
    bool b_HDRmode = 0;
    cv::Point min_loc, max_loc;
    if (!ret) {
        bool flag;
        LibcameraOutData frameData;
        cam.startCamera();
        while (true) {
            flag = cam.readFrame(&frameData);
            if (!flag)
                continue;
            Mat im(height, width, CV_8UC3, frameData.imageData);
            printf("%d_%d",width, height);
            Mat rgb[3];
            split(im,rgb);
            minMaxLoc(rgb[1],&min,&max,&min_loc,&max_loc);
            if(b_HDRmode){
                
                
            }

            Rect r(roi_OffsetX,roi_OffsetY,roi_Width,roi_Height);
            Mat resized_im = im(r);
            Mat new_resized_im;
            resize(resized_im,new_resized_im,cv::Size(width,height));
            imshow("libcamera-demo", new_resized_im);
            //imshow("libcamera-demo", im);
            key = waitKey(1);
            if (key == 'p') {
                break;
            } else if (key == 'f') {
                controls_.set(controls::AfMode, controls::AfModeAuto);
                controls_.set(controls::AfTrigger, 0);
                //cam.set(controls_);
            } else if (key == 'w' || key == 'W') {
                vMove++;
                if(roi_OffsetY - roi_Height >0) roi_OffsetY = roi_OffsetY - roi_Height;
            } else if (key == 's' || key == 'S') {
                vMove--;
                if(roi_OffsetY + roi_Height >0) roi_OffsetY = roi_OffsetY + roi_Height;
            } else if (key == 'a' || key == 'A') {
                hMove++;
                if(roi_OffsetX - roi_Width >0) roi_OffsetX = roi_OffsetX - roi_Width;
            } else if (key == 'd' || key == 'D') {
                hMove--;
                if(roi_OffsetX + roi_Width >0) roi_OffsetX = roi_OffsetX + roi_Width;
            } else if (key == 'z' || key == 'Z') {
                scale = scale*2;
                roi_Width = int(roi_Width/scale);
                roi_OffsetX = int((width-roi_Width)/2);
                roi_Height = int(roi_Height/scale);
                roi_OffsetY = int((height-roi_Height)/2); 
                printf("%d_%d_%d_%d",roi_Width,roi_OffsetX,roi_Height,roi_OffsetY );
            } else if (key == 'x' || key == 'X') {
                scale = scale/2;
                roi_Width = int(roi_Width*scale);
                roi_OffsetX = int((width-roi_Width)/2);
                roi_Height = int(roi_Height*scale);
                roi_OffsetY = int((height-roi_Height)/2); 
                printf("%d_%d_%d_%d",roi_Width,roi_OffsetX,roi_Height,roi_OffsetY );
            } else if (key =='e' || key == 'E'){
                User_Exposure_Time = User_Exposure_Time + 1000;
                // Set the exposure time
                controls_.set(controls::ExposureTime, User_Exposure_Time);
                
            } else if (key =='q' || key == 'Q'){
                User_Exposure_Time = User_Exposure_Time - 1000;
                // Set the exposure time
                controls_.set(controls::ExposureTime, User_Exposure_Time);
                //cam.set(controls_);
            } else if (key =='g' || key == 'G'){
                //User_Brightness = User_Brightness + 0.1;
                // Adjust the brightness of the output images, in the range -1.0 to 1.0
                //controls_.set(controls::Brightness, User_Brightness);
                User_AnalogGain = User_AnalogGain*2;
                controls_.set(controls::AnalogueGain,User_AnalogGain);
            } else if (key =='h' || key == 'H'){
                //User_Brightness = User_Brightness - 0.1;
                // Adjust the brightness of the output images, in the range -1.0 to 1.0
                User_AnalogGain = User_AnalogGain/2;
                controls_.set(controls::AnalogueGain, User_AnalogGain);
            } else if (key =='b' || key == 'B'){
                scale = 1;
                roi_OffsetX =0;
                roi_OffsetY =0;
                roi_Width =width;
                roi_Height =height;
            } else if (key =='n' || key == 'N'){
                b_HDRmode = true;
            }else if (key =='1'){
                roi_OffsetX = 0;
                roi_OffsetY = 0;
                roi_Width = 640;
                roi_Height = 480;
            }else if (key =='2'){
                roi_Width = 640;
                roi_Height = 480;
                roi_OffsetX = 640;
                roi_OffsetY = 0;
                
            }else if (key =='3'){
                roi_Width = int(width*0.5);
                roi_OffsetX = 0;
                roi_Height = int(height*0.5);
                roi_OffsetY = int(height*0.5);
            }else if (key =='4'){
                roi_Width = int(width*0.5);
                roi_OffsetX = int(width*0.5);
                roi_Height = int(height*0.5);
                roi_OffsetY = int(height*0.5);
            }else if (key =='h' || key == 'H'){

                string FileName = "Bit8_Arducam_IMX477_ExpT_";
                string str_TimeUnit = "us_";
                string FileExtension = "_.bmp";
                auto ExpT =controls_.get<int32_t>(controls::ExposureTime);
                string str_ExpT = to_string(*ExpT);
                string str_SaveCount = to_string(SaveCount);
                FileName.append(str_ExpT);
                FileName.append(str_TimeUnit);
                FileName.append(str_SaveCount);
                FileName.append(FileExtension);
                
                std::cout<<FileName<<std::endl;
                imwrite(FileName, im);
                SaveCount++;
                if (SaveCount == 10) SaveCount =0;
            }
            
            cam.set(controls_);
            frame_count++;
            if ((time(0) - start_time) >= 1){
                auto ExpT =controls_.get<int32_t>(controls::ExposureTime);
                // For Image testing
                /*
                string FileName = "Bit8_Arducam_IMX477_ExpT_";
                string str_TimeUnit = "us_";
                string FileExtension = "_.bmp";
                
                string str_ExpT = to_string(*ExpT);
                string str_SaveCount = to_string(SaveCount);
                FileName.append(str_ExpT);
                FileName.append(str_TimeUnit);
                FileName.append(str_SaveCount);
                FileName.append(FileExtension);
                
                std::cout<<FileName<<std::endl;
                imwrite(FileName, rgb[1]);
                SaveCount++;
                if (SaveCount == 10){
                    //User_Exposure_Time = User_Exposure_Time + 1000;
                    // Set the exposure time
                    index++;
                    if(index == 40) break;
                    controls_.set(controls::ExposureTime, ImageSet_White_Test[index]);
                    SaveCount =0;
                } 
                */
                cv::Scalar tempVal = cv::mean( rgb[1] );
                float myMAtMean = tempVal.val[0];
                printf("fps:%d_ExpT:%dus_MIN:%.0f_MAX:%.0f_AVE:%.2f\n", frame_count,ExpT.value(),min,max,myMAtMean);


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

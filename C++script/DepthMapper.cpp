#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <cstdio>

using namespace std;

int main()
{
  bool enable_rgb = false;
  bool enable_depth =false;
  //libfreenect2::setGlobalLogger(libfreenect2::createConsoleLogger(libfreenect2::Logger::Debug));


  libfreenect2::Freenect2 freenect2;
  libfreenect2::Freenect2Device *dev = 0;
  libfreenect2::PacketPipeline *pipeline = 0;
  libfreenect2::Freenect2Device::Config config;
  config.MinDepth = 0.5f;
  config.MaxDepth = 8.0f;

  string serial = freenect2.getDefaultDeviceSerialNumber();
  if(freenect2.enumerateDevices() == 0)
  {
    cout << "no device connected!" << endl;
    return -1;
  }
  if (serial == "")
  {
    serial = freenect2.getDefaultDeviceSerialNumber();
  }
  printf("%s\n",serial.c_str());
  dev = freenect2.openDevice(serial);
  int types = 0;
  puts("TEST1");
  if (enable_rgb)
    types |= libfreenect2::Frame::Color;
    if (enable_depth)
      types |= libfreenect2::Frame::Ir | libfreenect2::Frame::Depth;
    libfreenect2::SyncMultiFrameListener listener(types);
    libfreenect2::FrameMap frames;
    dev->setConfiguration(config);
    dev->setColorFrameListener(&listener);
    dev->setIrAndDepthFrameListener(&listener);
    if (enable_rgb && enable_depth)
  {
    if (!dev->start()){
      puts("TEST1");
      return -1;
    }

  }
  else
  {
    if (!dev->startStreams(enable_rgb, enable_depth))
      return -1;
  }
  cout << "device serial: " << dev->getSerialNumber() << endl;
  cout << "device firmware: " << dev->getFirmwareVersion() << endl;
  libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
  libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4), depth2rgb(1920, 1080 + 2, 4);;
  cv::Mat depthmatUndistorted, rgbd, rgbd2;
  while(true){
    static int i = 1;
    ostringstream oss;
    oss << setfill( '0' ) << setw( 10 ) << i++;

    //画像フォルダがあるパスを指定する部分
    cv::Mat rgbMat ;
    cv::Mat rgbtest ;
    cv::Mat depthtest ;
    cv::Mat depthMat ;
    cv::Mat Wrgbd ;
    //cv::imread( "/home/kei/document/experiments/2019.01.15/10jpg/" + oss.str() + ".jpg" ).convertTo(rgbMat, CV_8UC3);
    //rgbtest = cv::imread( "/home/kei/document/experiments/2019.01.15/10jpg/" + oss.str() + ".jpg");
    //rgbtest = cv::imread( "/home/kei/document/C++script/KinectOneStream/jpg/" + oss.str() + ".jpg");
    //cv::imread( "/home/kei/document/experiments/2019.01.15/10jpg/" + oss.str() + ".jpg" ).convertTo(rgbMat, CV_8UC4);
    //rgbtest = cv::imread( "/home/kei/document/experiments/2019.02.02/motion/1.squat/jpg/" + oss.str() + ".jpg" );
    // rgbtest = cv::imread( "/home/kei/document/experiments/Hamano/H5_1/color2/" + oss.str() + ".jpg" );
    rgbtest = cv::imread( "/home/shoda/Documents/mitsu/color/" + oss.str() + ".jpg" );
    
    printf("%d\n", rgbtest.type());
    cv::cvtColor(rgbtest, rgbMat, cv::COLOR_BGR2BGRA);
    //cv::imread( "/home/kei/document/C++script/KinectOneStream/jpg/" + oss.str() + ".jpg" ).convertTo(rgbMat, CV_8UC4);

    depthtest =  cv::imread( "C++script/16pngtest.png",2); //cleanなレジストレーション画像を作る用
    // depthtest =  cv::imread( "/home/kei/document/experiments/Hamano/H5_1/depth/" + oss.str() + ".png",2);
    // depthtest =  cv::imread( "/home/shoda/Documents/mitsu/depth/" + oss.str() + ".tiff", 2);
    depthtest.convertTo(depthMat, CV_32FC1);
    //depthMat = depthMat * 4096.0f;
    puts("color");
    //
    /*string filename = "/home/kei/document/experiments/2019.01.15/depth/" + oss.str() + ".tiff";
    printf("hello\n");
    //string filename = "/home/kei/document/C++script/KinectOneStream/depth/" + oss.str() + ".tiff";
    ifstream ifs (filename.c_str(),ios::in|ios::binary);
    if (!ifs)
    {
      cerr << "Can't open the file\n";
      return -1;
    }
    int len;
    ifs.seekg (0, ios::end);
    len = ifs.tellg ();
    ifs.seekg (0, ios::beg);
    char* data= new char [len];
    ifs.read (data, len);

    cv::Mat depthMat( 1, len, CV_16UC1, (void*)data );
    //cv::cvtColor(rawData, depthMat, cv::COLOR_GRAY2BGRA);
    //depthMat = imdecode( rawData, cv::IMREAD_ANYDEPTH|cv::IMREAD_ANYCOLOR );
    //cv::cvtColor(rawData, depthMat, cv::COLOR_GRAY2BGRA);*/
    //cv::imshow("depth", depthMat / 4096.0f);
    //cv::imwrite("/home/kei/document/C++script/test1.tiff",depthMat);
    //cv::Mat depthMat = imdecode( rawData, cv::IMREAD_ANYDEPTH|cv::IMREAD_ANYCOLOR );
    printf("hello%i\n", depthMat.depth());
    //depthtest = cv::imread( "/home/kei/document/experiments/2019.01.15/depth/" + oss.str() + ".tiff" );
    //cv::cvtColor(depthtest, depthMat, cv::COLOR_BGR2BGRA);

    // Build frames
    libfreenect2::Frame rgb(rgbMat.cols, rgbMat.rows, 4, rgbMat.data);
    //depthMat = depthMat * 255.0f;
    libfreenect2::Frame depth(512, 424, 4, depthMat.data);
    //rgb.data = rgbMat.data;
    //depth.data = depthMat.data;

    //画像が無くなったらループを抜ける
    if( rgbMat.empty()){
      break;
    }
    // Register
    registration->apply(&rgb, &depth, &undistorted, &registered, true, &depth2rgb);
    registration->apply(&rgb, &depth, &undistorted, &registered);
    cv::Mat(undistorted.height, undistorted.width, CV_32FC1, undistorted.data).copyTo(depthmatUndistorted);
    cv::Mat(registered.height, registered.width, CV_8UC4, registered.data).copyTo(rgbd);
    cv::Mat(depth2rgb.height, depth2rgb.width, CV_32FC1, depth2rgb.data).copyTo(rgbd2);
    //cv::imshow("RGB", rgbMat);
    //"/home/kei/document/experiments/2019.01.15/10jpg/" + oss.str() + ".jpg"
    //cv::imshow("undistorted", depthmatUndistorted / 4096.0f);
    //cv::imshow("registered", rgbd);
    //cv::imshow("depth2RGB", rgbd2 / 4096.0f);
    //cv::imwrite("/home/kei/document/C++script/undistorted/" + oss.str() + ".png",depthmatUndistorted / 256.0f);
    // cv::imwrite("/home/kei/document/experiments/Hamano/H5_1/regi2/" + oss.str() + ".jpg",rgbd);
    cv::imwrite("/home/shoda/Documents/mitsu/regi_clean/" + oss.str() + ".jpg",rgbd);

    //rgbd2 = rgbd2 / 256.0f;
    rgbd2.convertTo(Wrgbd, CV_16UC1);
    //cv::imwrite("/home/kei/document/experiments/2019.02.02/切り出し/1.squat/registered/" + oss.str() + ".tiff",Wrgbd);
    printf("データ型は%d\n", Wrgbd.type());
    //cv::imshow( "depth", depth );
    //cv::imshow( "image", image );
    /*
        if( cv::waitKey( 0 ) >= 0 ){
            break;
        }*/
    //cv::waitKey( 0 );
  }
  cv::destroyAllWindows();
  return 0;
}

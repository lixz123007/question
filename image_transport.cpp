#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sstream>
#include <sensor_msgs/image_encodings.h>  
#include <opencv2/imgproc/imgproc.hpp> 
#include <iostream>
#include <std_msgs/Int8.h>
#include "std_msgs/String.h"

static const std::string OPENCV_WINDOW = "Image window";
 namespace enc = sensor_msgs::image_encodings;
 
class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  
public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/kinect2/qhd/image_depth_rect", 1, 
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);
 
    cv::namedWindow(OPENCV_WINDOW);
  }
 
  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }



  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImageConstPtr cv_ptr;
    try
    {
      if (enc::isColor(msg->encoding))
      cv_ptr = cv_bridge::toCvShare(msg, enc::BGR8);
    else
      cv_ptr = cv_bridge::toCvShare(msg, enc::MONO16);

    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }
 
    // Draw an example circle on the video stream
    if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
      cv::circle(cv_ptr->image, cv::Point(50, 50), 10, CV_RGB(255,0,0));
 
    // Update GUI Window
    cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);
    
    // Output modified video stream
    image_pub_.publish(cv_ptr->toImageMsg());
  }
};
 
int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}

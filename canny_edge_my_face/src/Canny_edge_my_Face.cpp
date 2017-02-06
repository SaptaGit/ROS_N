#include <ros/ros.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/Image.h>
#include <gtest/gtest.h>
#include <cv_bridge/CvBridge.h>
#include "image_transport/image_transport.h"

sensor_msgs::Image g_image;

void ImageCallback(const sensor_msgs::Image::ConstPtr& image)
{
	//Converting the ROS image format to OpenCV format.
	IplImage *incomingImage = NULL ;
    sensor_msgs::CvBridge bridge_;
	try
      {
        incomingImage = bridge_.imgMsgToCv(image, "bgr8");
      }
    catch (sensor_msgs::CvBridgeException error)
     {
       ROS_ERROR("error getting image");
     }
  
	//Converting to gray scale and applying Canny Edge detection.
	Mat grayscale(incomingImage), edge(incomingImage);
    cvtColor(incomingImage,grayscale,CV_BGR2GRAY);
	Canny( grayscale, edge, 50, 150, 3);
	edge.convertTo(draw, CV_8U);
	g_image = bridge_.cvToImgMsg(cv_image, "bgr8");
	// Displaying the Image
	imshow( "contour",edge);  
}

// Class to publish the Generated Image
class PublishContourImage
 {
 private: 
  ros::Publisher img_pub;
  public: 
   PublishContourImage(ros::NodeHandle &n)
   {
      image_transport::Publisher image_pub_;
      image_pub_ = n.advertise<sensor_msgs::Image>("/Image/Contour", 1);
   }

   void pub_image(ros::NodeHandle &n)
  {
   while(n.ok())    
   {
   ros::spinOnce(); 
   image_pub_.publish(g_image);
   r.sleep();
   } 
 }
};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "Canny_edge_my_Face");
  ros::NodeHandle nh;
  cvNamedWindow("contour",WINDOW_NORMAL);
  cvStartWindowThread();
  ros::Subscriber sub1 = nh.subscribe("/cv_camera_node/image_raw", 1, imageCallback);
  PublishContourImage pci(nh);
  pci.pub_image(nh);
  ros::spin();
}

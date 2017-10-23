//
// Created by nima on 4/12/17.
//

#ifndef bardCalibratedCamera_h
#define bardCalibratedCamera_h

#include <vtkOpenGLCamera.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>
#include <vtkSetGet.h>

#include <ros/ros.h>
#include <kdl/frames.hpp>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
#include "opencv2/highgui/highgui.hpp"
#include <image_transport/image_transport.h>
#include <geometry_msgs/PoseStamped.h>
#include <vtkImageImport.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>

/**
 * \class CalibratedCamera
 * \brief This is a Subclass of vtkCamera augmented with intrinsic and extrinsic
 * camera information.
 * Changing the size of the rendering window is taken into consideration to get
 * a correct rendering view (still need to add).
 */

class CalibratedCamera
{
public:



    CalibratedCamera(ros::NodeHandle *n,
                     const std::string cam_name="");

    ~CalibratedCamera() {}

    //static CalibratedCamera *New();
    //vtkTypeMacro(CalibratedCamera, vtkOpenGLCamera);

    //    /**
    //     * \brief Set the window size currently used
    //     * \param width in pixels.
    //     * \param height in pixels.
    //     */
    //    void SetActualWindowSize(const int& width, const int& height);

//    /**
//     * \brief Set the intrinsic parameters from camera calibration.
//     */
//    void SetIntrinsicParameters(const double& fx, const double& fy,
//                                const double& cx, const double& cy);

    /**
    * \brief Update the view angle of the virtual Camera according to window size
     * Note that the windows is the opengl window here,
    */
    void UpdateVirtualView(const double &width, const double &height);

    // Set up the background scene_camera to fill the renderer with the image

    void SetRealCameraToFaceImage(const int *window_size);

    bool IsImageNew();

    void ConfigureBackgroundImage(cv::Mat img);

    void UpdateBackgroundImage(cv::Mat  img);

    /**
     * \brief FaceImage
     */
    void SetCemraToFaceImage(const int *window_siz,
                             const int imageSize[], const double spacing[],
                             const double origin[]);

    void ReadCameraParameters(const std::string file_path);

    void ImageCallback(const sensor_msgs::ImageConstPtr &msg);

    void PoseCallback(const geometry_msgs::PoseStampedConstPtr &msg);

    void LockAndGetImage(cv::Mat &image);

    /**
     * \brief Sets the pose of the camera with respect to world (task frame)
     */
    void SetWorldToCameraTransform(const cv::Vec3d cam_rvec,
                                   const cv::Vec3d cam_tvec);

public:

    vtkSmartPointer<vtkCamera> camera_virtual;
    vtkSmartPointer<vtkCamera> camera_real;
    vtkSmartPointer<vtkImageActor>          image_actor_;

private:
    image_transport::ImageTransport *it;
    image_transport::Subscriber sub_image;

    ros::Subscriber sub_pose;
    CalibratedCamera(const CalibratedCamera&);  // Purposefully not implemented.
    void operator=(const CalibratedCamera&);  // Purposefully not implemented.

    vtkSmartPointer<vtkImageImport>         image_importer_;
    vtkSmartPointer<vtkImageData>           camera_image_;


    vtkSmartPointer<vtkMatrix4x4> intrinsic_matrix;
    //    int m_WindowWidthInPixels;
    //    int m_WindowHeightInPixels;
    cv::Mat image_from_ros;
    bool new_image = false;
    bool new_cam_pose = false;
    bool is_initialized = false;
    // TODO stop using rvec/tvec representation?
    //    KDL::Frame world_to_cam_pose;
    cv::Vec3d cam_rvec_;
    cv::Vec3d cam_tvec_;

    double image_width_;
    double image_height_;
    double fx_;
    double fy_;
    double cx_;
    double cy_;


};


#endif

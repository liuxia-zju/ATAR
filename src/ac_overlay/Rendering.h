//
// Created by charm on 4/12/17.
//

#ifndef TELEOP_VISION_RENDERING_H
#define TELEOP_VISION_RENDERING_H
#include "opencv2/core/core.hpp"
#include "bardCalibratedCamera.h"
#include <vtkImageImport.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkImageActor.h>
#include <vtkRenderWindow.h>


class Rendering {
public:

//    vtkTypeMacro(Rendering, vtkRenderWindow);
//    static Rendering *New();

    Rendering();
    ~Rendering();
    void SetWorldToCameraTransform(const cv::Vec3d &cam_rvec, const cv::Vec3d &cam_tvec);

    void SetEnableImage(bool isEnabled);

    void SetupBackgroundImage(cv::Mat &);

    void UpdateBackgroundImage(cv::Mat &);

    void UpdateWindowSizeRelatedViews();

    void SetCameraIntrinsics(const cv::Matx33d& intrinsics);

    void AddActorToScene(vtkSmartPointer<vtkProp>);

    vtkSmartPointer<vtkRenderWindow>    renderWindow;

private:


//    void SetCamExtrinsics(vtkSmartPointer<vtkCamera> camera);

    void SetImageCameraToFaceImage();

//    void SetCameraToWorldTransform(const cv::Vec3d &cam_rvec, const cv::Vec3d &cam_tvec);

//    bool fromIpl2Vtk( cv::Mat _src, vtkImageData* _dest );

    vtkSmartPointer<CalibratedCamera>    BackgroundCamera;
    vtkSmartPointer<CalibratedCamera>    SceneCamera;
    vtkSmartPointer<vtkImageImport>      ImageImporter;
    vtkSmartPointer<vtkImageActor>       ImageActor;
    vtkSmartPointer<vtkRenderer>         backgroundRenderer;
    vtkSmartPointer<vtkRenderer>         sceneRenderer;
    vtkSmartPointer<vtkMatrix4x4>        WorldToCameraTransform;
    vtkSmartPointer<vtkMatrix4x4>        CameraToWorldTransform;
    vtkSmartPointer<vtkImageData>        camera_image;
};


#endif //TELEOP_VISION_RENDERING_H

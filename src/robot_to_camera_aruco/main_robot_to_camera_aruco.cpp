/*
 * main_robot_to_camera_aruco.cpp
 *
 *  Created on: Jan 16, 2017
 *      Author: nearlab
 */



#include "utils/Colors.hpp"
#include "utils/Drawings.hpp"
#include "utils/Conversions.hpp"
#include "RobotToCameraAruco.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>



/**
 * Enum used to track the progress of the calibration procedure.
 */
enum class CalibProgress { Idle, Ready, Calibrating, Finished };





int main(int argc, char *argv[]) {


	std::string ros_node_name("robot_to_camera_aruco");

	ros::init(argc, argv, ros_node_name);

	RobotToCameraAruco r(ros_node_name);






	//	Eigen::MatrixXf A = Eigen::MatrixXf::Random(3, 1);
	std::vector<Eigen::Vector3d> A = {
			Eigen::Vector3d(0.0100, -0.0010,    0.000),
			Eigen::Vector3d(0.0200 ,  0.0010,    0.001),
			Eigen::Vector3d(0.0400 , 0.0000,    0.000),
			Eigen::Vector3d( 0.000,  0.0200,    0.0010),
			Eigen::Vector3d(-0.001,  0.040,    -0.0010),
			Eigen::Vector3d( 0.001,  0.0500,   -0.0000)};

	std::cout <<"A.size()  " << A.size() << std::endl;
	std::cout <<"A.[0]  " << A[0] << std::endl;
	auto a_pair = FitLineToPoints(A);
	std::cout <<"a_pair.first:  \n" << a_pair.first << std::endl;
	std::cout <<"a_pair.second:  \n" << a_pair.second << std::endl;



	cv::Matx33d rotm_;
	cv::Vec3d br_tvec_;

	r.CalculateTransformationN(A, rotm_, br_tvec_);




	//-----------------------------------------------------------------------------------
	// Construct the drawing object
	//-----------------------------------------------------------------------------------
	Drawings drawings(
			r.camera_intrinsics.camMatrix,
			r.camera_intrinsics.distCoeffs,
			r.board_to_robot_frame,
			//        r.Board.MarkerLength_px / r.Board.MarkerLength);
			1);

	//	drawings.setSquare(Point3d(500,350,0),  Point2d(500,100));
	//	drawings.createEllipse(525,350,220,150,400);

	// Create the window in which to render the video feed
	cvNamedWindow("PSM to Board Calib", CV_WINDOW_AUTOSIZE);
	//    cvSetWindowProperty("teleop", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

	// This is the image that will be rendered at every loop. We'll be drawing the hints on it.
	cv::Mat back_buffer;
	cv::String instructions = "Waiting ...";

	ros::Rate loop_rate(r.ros_freq);

	CalibProgress progress = CalibProgress::Ready;

	while (ros::ok()) {

		back_buffer = r.Image(ros::Duration(1));


		//todo add a way of checking if the board is seen
		//        if (progress == CalibProgress::Idle) {
		//            if (boardDetector.Ready()) {
//		progress = CalibProgress::Ready;
		//            }
		//            continue;
		//        }

		instructions = "Press k to begin calibration.";
		char key = (char) cv::waitKey(1);
		if (key == 27) // Esc
			ros::shutdown();

		if (key == 'k') { // Start calibration
			r.Reset();
			progress = CalibProgress::Calibrating;
		}

		if (progress == CalibProgress::Calibrating) {

//			r.DrawToolTarget(instructions, back_buffer);
			r.DrawCalibrationAxis(instructions, back_buffer);

			if (key == ' ') {
				// save the position of the end effector
				r.SaveCalibrationPoint(r.robot_pose.pose.position);

				// was adding the point enough to complete the calibration?
				if (r.IsCalibrated()) {
					progress = CalibProgress::Finished;

					// get the calibration data
					r.GetTr();
					drawings.update_board_to_robot_frame(r.board_to_robot_frame);
				}
			}
		}

		if (progress == CalibProgress::Finished){
			instructions = "Calibration finished. Press 'Esc' to exit";

			//            if (boardDetector.Ready()) {
			drawings.update_cam_2_board_ref(r.board_to_cam_rvec, r.board_to_cam_tvec);

			// draw the tool tip point
			drawings.drawToolTip(
					back_buffer, r.robot_pose.pose.position, Colors::Redish);

			//-----------------------------------------------------------------------------------
			// publish camera to robot pose
			//-----------------------------------------------------------------------------------
			//                KDL::Frame cam_to_robot = board_to_psm_frame * board_to_cam_frame.Inverse();
			//
			//                geometry_msgs::Pose cr_pose_msg;
			//                // convert pixel to meters
			//                //cam_to_robot.p = cam_to_robot.p / drawings.m_to_px;
			//                tf::poseKDLToMsg(cam_to_robot, cr_pose_msg);
			//
			//                r.pub_board_to_robot_pose.publish(cr_pose_msg);
			//                r.pub_board_to_robot_pose.publish(board_to_psm_pose_msg);
			//            }

		}

		drawings.showTransientNotification(back_buffer);

		cv::Point textOrigin(10, 20);
		auto text_color = (!r.IsCalibrated()) ? Colors::Red : Colors::Green;
		cv::putText(back_buffer, instructions, textOrigin, 1, 1, text_color);

		cv::imshow("PSM to Board Calib", back_buffer);

		ros::spinOnce();
		loop_rate.sleep();
	}

	ROS_INFO("Ending Session...\n");
	ros::shutdown();

	return 0;
}




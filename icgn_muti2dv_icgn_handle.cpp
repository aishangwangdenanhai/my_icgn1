#include <omp.h>
#include "icgn_muti2dv_icgn_handle.h"

namespace dic
{
	ICGN2D1_HANDLE::ICGN2D1_HANDLE()
	{

	};

	ICGN2D1_HANDLE::~ICGN2D1_HANDLE()
	{

	};

	//改到这
	std::vector<cv::Rect> ICGN2D1_HANDLE::process_frame(vector<Mat>fncc_frame1, vector<Mat>fncc_temp, vector<Image2D>ref_img, vector<Image2D>tar_img,
		int& cpu, Mat& frame, vector<Rect>ROI2, vector<Rect>box_tar, int box_radius_, int box_width_height, int subset_radius_x1,
		int subset_radius_y1, int max_iteration1, float max_deformation_norm1,int ii,ofstream& outfile)
	{
		namedWindow("fncc", WINDOW_GUI_NORMAL);
		vector<Rect> box_tar_;
		vector<vector<float>>result1;

		double timer_1 = omp_get_wtime();

#pragma omp parallel for
		for (int i = 0; i < fncc_frame1.size(); i++)
		{
			
			cvtColor(fncc_frame1[i], fncc_frame1[i], COLOR_BGR2GRAY);
			Mat integral_out = Mat(fncc_frame1[i].size(), fncc_frame1[i].type(), Scalar(0, 0, 0));
			Mat integral_out1 = Mat(fncc_frame1[i].size(), fncc_frame1[i].type(), Scalar(0, 0, 0));
			integral(fncc_frame1[i], integral_out, integral_out1);
			
			// iii 表示第iii帧，
			result1.push_back(g(&fncc_frame1[i], &fncc_temp[i], integral_out, integral_out1,i,ii,fncc_frame1.size()));
			rectangle(frame, Rect((int)result1[i][0] + ROI2[i].x, (int)result1[i][1] + ROI2[i].y, (int)result1[i][2], (int)result1[i][3]), Scalar(255, 0, 0), 5, 8);
			float x0 = (float)result1[i][0] + ROI2[i].x + fncc_temp[i].cols / 2;
			float y0 = (float)result1[i][1] + ROI2[i].y + fncc_temp[i].rows / 2;
			circle(frame, Point(x0, y0), 2, Scalar(0, 255, 0), 3, 8);
			
			float u0 = (float)box_tar[i].x + box_radius_ - x0;
			float v0 = (float)box_tar[i].y + box_radius_ - y0;		
			POI2D poi1(box_radius_, box_radius_);//局部坐标

			//初值猜想
			poi1.deformation.u = u0;
			poi1.deformation.v = v0;
			ICGN2D1* icgn1 = new ICGN2D1(subset_radius_x, subset_radius_y, max_deformation_norm, max_iteration, cpu);
	
			icgn1->setImages(ref_img[i], tar_img[i]);
			icgn1->prepareRef();
			icgn1->prepareTar();
			icgn1->compute(&poi1);

			//***********************************************************************
			//写入操作
			 outfile << fixed;
			outfile.precision(6);
			outfile.setf(ios_base::showpos);
			//outfile<<setw(8) << setfill("0");

			cout.setf(ios_base::fixed, ios_base::floatfield);
			outfile<< "    x:" << poi1.deformation.u << "    " << "y:" << poi1.deformation.v << "    ";
			//***********************************************************************
			
			//double x1 = (double)box_tar[i].x + (double)poi1.x + (double)poi1.deformation.u;
			//double y1 = (double)box_tar[i].y + (double)poi1.y + (double)poi1.deformation.v;
				
			//cout << "icgn_u:" << poi1.deformation.u << "    " << "icgn_v:" << poi1.deformation.v << endl;
			//cout << "x11:" << box_tar[i].x + (float)poi1.x << "   " << "y11:" << box_tar[i].y + (float)poi1.y << endl;
			//cout << "x1:" << x1 << " " << "y1:" << y1 << " " << endl;
			delete icgn1;
			box_tar_.push_back(Rect((int)result1[i][0] + ROI2[i].x, (int)result1[i][1] + ROI2[i].y, (int)result1[i][2], (int)result1[i][3]));
		}
		outfile << endl;

		double timer_12 = omp_get_wtime();
		double consumed_1 = timer_12 - timer_1;
		cout << "ICGN1: " << consumed_1 * 1000 << " ms" << endl;

		imshow("fncc", frame);
		return box_tar_;
		//全局poi坐标
	}

	vector<cv::Rect> ICGN2D1_HANDLE::boxes(vector<Rect> ROI1, int fncc_ROI2_zen_radius_, int fncc_ROI2_width_height)
	{
		vector<cv::Rect> ROI;
		for (int i = 0; i < ROI1.size(); i++)
		{
			ROI.push_back(Rect((ROI1[i].x - float(fncc_ROI2_zen_radius_)), ROI1[i].y - float(fncc_ROI2_zen_radius_), fncc_ROI2_width_height, fncc_ROI2_width_height));
		}
		//cout << "ROI.X:" << ROI[0].x << endl;
		return ROI;
	}

	//多线程计算
	void ICGN2D1_HANDLE::icgn_muti_apart(std::vector<cv::Rect>& ROI_queue, cv::Mat first_frame, cv::VideoCapture capture1, int cpu_thread_number, int box_radius1, int box_width_height1,
		int subset_radius_x1, int subset_radius_y1, int max_iteration1, float max_deformation_norm1,string filePath,int fncc_ROI2_zen_radius)
	{
		ICGN2D1_HANDLE instance_;

		instance_.fncc_ROI2_zen_radius = fncc_ROI2_zen_radius;
		instance_.fncc_ROI2_width_height = 2*box_radius1 + 1 + 2*instance_.fncc_ROI2_zen_radius;
		instance_.box_radius = box_radius1;//15
		instance_.box_width_height = box_width_height1;//31


		//set DIC parameters(一次）
		instance_.subset_radius_x = subset_radius_x1;
		instance_.subset_radius_y = subset_radius_y1;
		instance_.max_iteration = max_iteration1;
		instance_.max_deformation_norm = max_deformation_norm1;

		//数据写入
		ofstream outfile;
		outfile.open(filePath);//打开文件
		if (!outfile)
		{
			cout << "文件写入失败" << endl;
			exit(1);
		}



		Mat frame;
		Mat first_frame1;
		VideoCapture capture;
		capture = capture1;
		capture >> frame;
		//cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
		frame.copyTo(first_frame1);
		cv::cvtColor(first_frame1, first_frame1, COLOR_BGR2GRAY);

		vector<Rect> box_tar;//icgn匹配的box

		//整像素
		vector<cv::Rect> ROI1;
		vector<cv::Rect> ROI2;

		vector<cv::Mat> fncc_frame1;
		vector<cv::Mat> fncc_temp;
		vector<cv::Mat> ref_box;
		vector<Image2D>ref_img;
		vector<cv::Mat> tar_box;
		int ii = 0;

		double timer_tic = omp_get_wtime();
		
//#pragma omp parallel for 
		for (int i = 0; i < ROI_queue.size(); i++) 
		{//想要多线程计算这个for循环

			
		    //入栈
			fncc_temp.push_back(first_frame1(ROI_queue[i]));//
			f(&fncc_temp[i],i);//这里可能也需要装	
			ref_box.push_back(first_frame1(ROI_queue[i]));
			ref_img.push_back(ref_box[i]);


			int dj = 1;
			
			if (i == ROI_queue.size() - 1)
			{
				
				while (1)
				{
					if (dj == 1)
					{
						//存值
						for (int i = 0; i < ROI_queue.size(); i++)
						{
							box_tar.push_back(ROI_queue[i]);//
							
							ROI2.push_back(Rect(ROI_queue[i].x - instance_.fncc_ROI2_zen_radius, ROI_queue[i].y - instance_.fncc_ROI2_zen_radius,
								instance_.fncc_ROI2_width_height, instance_.fncc_ROI2_width_height));//
						}
						
					}
					bool ret = capture.read(frame);
					if (!ret)
					{
						cout << "capture stop" << endl;
						break;
					};
									
					vector<Image2D> tar_img;
					
					if (dj == 1)
					{
						for (int i = 0; i < ROI_queue.size(); i++)
						{

							fncc_frame1.push_back(frame(ROI2[i]));
							tar_box.push_back(frame(box_tar[i]));
							cvtColor(tar_box[i], tar_box[i], COLOR_BGR2GRAY);
							tar_img.push_back(tar_box[i]);//装货

	
						}
					}else
					{
						for (int i = 0; i < ROI_queue.size(); i++)
						{
							fncc_frame1[i] = frame(ROI2[i]);
							tar_box[i] = frame(box_tar[i]);
							cvtColor(tar_box[i], tar_box[i], COLOR_BGR2GRAY);
							tar_img.push_back(tar_box[i]);//装货
						};
					}

					

					box_tar = instance_.process_frame(fncc_frame1, fncc_temp, ref_img, tar_img, cpu_thread_number,
						frame, ROI2, box_tar, instance_.box_radius, instance_.box_width_height, subset_radius_x1, subset_radius_y1, max_iteration1, max_deformation_norm1,ii, outfile);
					
					
					//cout << endl << endl;
					ii += 1;
					//这一步货就要清空了				
					ROI2 = instance_.boxes(box_tar, instance_.fncc_ROI2_zen_radius, instance_.fncc_ROI2_width_height);
					
					char c = cv::waitKey(1);
					if (c == 27)
					{
						break;
					}
					dj = 2;
				}
			}
			//cout << "i:" << i << endl;
		}

		double timer_toc = omp_get_wtime();
		double consumed_time = timer_toc - timer_tic;
		cout << "ICGN: " << consumed_time * 1000 << " ms" << endl;
		//display the information
		

		    outfile.close();
			capture.release();
			cv::waitKey(0);
		    cv::destroyAllWindows();  
	}
	
}
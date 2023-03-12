#include "icgn_muti2dv_dic.h"
#include <omp.h>

using namespace dic;
using namespace cv;

bool drawing_box = false;
bool gotBox = false;
Rect box;//模板标定
Point downPoint;

void mouseRectHandler(int event, int x, int y, int flags, void* param);
void darw_roi(Mat input);

Mat diplay;
Mat first_frame;
Mat frame;

int fncc_ROI2_zen_radius = 5;
int box_radius = 15;
int box_width_height = 2 * box_radius + 1;
int subset_radius_x = box_radius-2;
int subset_radius_y = box_radius-2;
int max_iteration = 10;
float max_deformation_norm = 0.001f;
std::vector<Rect>ROI_queue;

//数据写入路径
string filePath = "E:/taine_data/data.txt";

int main()
{
	int cpu_thread_number = omp_get_num_procs() - 1;
	omp_set_num_threads(cpu_thread_number);

	VideoCapture capture("E:/CV/images/DIC三同心圆视频.MOV");
	if (!capture.isOpened())
	{
		cout << "未找到视频" << endl;
		return -1;
	}

	capture >> frame;
	frame.copyTo(first_frame);
	darw_roi(first_frame);
	//cout << "queue size:" << ROI_queue.size() << endl;
	if (ROI_queue.size() > 200)
	{
		cout << "超过数量" << endl;
		return -1;
	}

	//ICGN
	ICGN2D1_HANDLE* icgn1 = new ICGN2D1_HANDLE();
	icgn1->icgn_muti_apart(ROI_queue, first_frame, capture, cpu_thread_number,box_radius,box_width_height,subset_radius_x,subset_radius_y,max_iteration,max_deformation_norm, filePath,fncc_ROI2_zen_radius);
	
	cv::waitKey(0);
	destroyAllWindows();
	return 0;
}

void mouseRectHandler(int event, int x, int y, int flags, void* param)
{
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		//按下鼠标，代表可以可以开始画矩形
		//记录起点
		
		downPoint = Point(x, y);
		box = Rect(downPoint.x - box_radius, downPoint.y - box_radius, box_width_height, box_width_height);
		cout << "xx0:" << downPoint.x << "   " << "yy0:" << downPoint.y << endl;
		ROI_queue.push_back(box);
		rectangle(diplay, box, Scalar(0, 0, 255), 6);
		circle(diplay, Point(downPoint.x, downPoint.y), 2, Scalar(0, 0, 255), 4);
		namedWindow("Muti_play", 0);
		imshow("Muti_play", diplay);

		break;

	case EVENT_LBUTTONUP:

		//松开鼠标，代表结束画矩形
		drawing_box = false;
		gotBox = true;
		break;
	default:
		break;
	}
}

void darw_roi(Mat input)
{
	input.copyTo(diplay);
	namedWindow("Muti_play",WINDOW_GUI_NORMAL);
	imshow("Muti_play", input);
	setMouseCallback("Muti_play", mouseRectHandler, 0);

	int c = cv::waitKey(0);
	if (c == 27)
	{
		return;
	}
}

#include"icgn_muti2dv_fncc.h"

//f_C1计算、fm_的计算
void f(const Mat* const temp,int ii)//考虑传进来的Mat是单通道还是三通道
{
	int tempWeith_f = temp->cols;
	int tempHeight_f = temp->rows;
	int dotCount_f = tempHeight_f * tempWeith_f;

	double f_ = 0.0;
	int i, j, k, l;
	for (i = 0; i < tempHeight_f; i++)
	{
		for (j = 0; j < tempWeith_f; j++)
		{
			f_ += temp->ptr <uchar>(i)[j];//▼
		}
	}
	fm_.push_back(0);
	fm_[ii] = f_ / dotCount_f;//这里储存的值
	//f_fm = f_ - fm_*dotCount_f;//▼：三通道还是单通道，未定，我猜是单通道


	f_C1.push_back(0);
	for (i = 0; i < tempHeight_f; i++)
	{
		for (j = 0; j < tempWeith_f; j++)
		{
			f_C1[ii] += pow((float)temp->ptr<uchar>(i)[j] - fm_[ii], 2);//▼
		}
	}

	f_C1[ii] = (sqrt(f_C1[ii]));//这里储存的值

	
}

//积分表的计算
void integral(Mat src, Mat& integral_out, Mat& integral_out1)
{
	Mat tmp(src.size(), CV_64FC1, 0.0);
	Mat tmp1(src.size(), CV_64FC1, 0.0);

	tmp.ptr<double>(0)[0] = (double)src.ptr<uchar>(0)[0];
	//tmp1.ptr<double>(0)[0] = pow((double)src.ptr<uchar>(0)[0],2);//
	tmp1.ptr<double>(0)[0] = (double)src.ptr<uchar>(0)[0] * (double)src.ptr<uchar>(0)[0];
	for (int i = 1; i < src.cols; i++)//第一行的处理
	{
		tmp.ptr<double>(0)[i] = tmp.ptr<double>(0)[i - 1] + src.ptr<uchar>(0)[i];
		tmp1.ptr<double>(0)[i] = (tmp1.ptr<double>(0)[i - 1] + src.ptr<uchar>(0)[i] * src.ptr<uchar>(0)[i]);
	}

	for (int i = 1; i < src.rows; i++)//第一列的处理
	{
		tmp.ptr<double>(i)[0] = tmp.ptr<double>(i - 1)[0] + src.ptr<uchar>(i)[0];
		tmp1.ptr<double>(i)[0] = (tmp1.ptr<double>(i - 1)[0] + src.ptr<uchar>(i)[0] * src.ptr<uchar>(i)[0]);
	}

	for (int i = 1; i < src.rows; i++)
	{
		for (int j = 1; j < src.cols; j++)
		{
			tmp.ptr<double>(i)[j] = tmp.ptr<double>(i)[j - 1] + tmp.ptr<double>(i - 1)[j]
				- tmp.ptr<double>(i - 1)[j - 1] + src.ptr<uchar>(i)[j];

			tmp1.ptr<double>(i)[j] = (tmp1.ptr<double>(i)[j - 1] + tmp1.ptr<double>(i - 1)[j]
				- tmp1.ptr<double>(i - 1)[j - 1] + src.ptr<uchar>(i)[j] * src.ptr<uchar>(i)[j]);
		}
	}

	tmp.copyTo(integral_out);//这里储存的值
	tmp1.copyTo(integral_out1);//这里储存的值
}


vector<float> g(const Mat* cur_image, const Mat* temp, Mat sum1, Mat sum2,int ii,int iii,int n_size)
{
	//当前图像子区左上角点的位置---->>搜索区域的问题
	int cur_image_H = cur_image->rows;
	int cur_image_W = cur_image->cols;
	int temp_H = temp->rows;
	int temp_W = temp->cols;

	float* p_uchar = new float[cur_image_H * cur_image_W];
	float* t_uchar = new float[temp_H * temp_W];
	for (int i = 0; i < cur_image_H; i++)
	{
		for (int j = 0; j < cur_image_W; j++)
		{
			coordinate.push_back(Point(j, i));//把每个点的位置信息存入coordinate数组中
			p_uchar[i * cur_image_W + j] = cur_image->ptr<uchar>(i)[j];
		}
	}

	for (int i = 0; i < temp_H; i++)
	{
		for (int j = 0; j < temp_W; j++)
		{
			t_uchar[i * temp_W + j] = temp->ptr<uchar>(i)[j];
		}
	}
	
	pStartPoint = &coordinate[0];//指向(0,0)这个像素
	//cout << "pStartPoint:" << (float)cur_image->ptr<uchar>(coordinate[0].y)[coordinate[0].x] << endl;
	int lastPoint = (cur_image_H - temp_H) * cur_image_W + (cur_image_W - temp_W);
	//cout << "lastPoint:" << lastPoint << endl;
	pLastLastPoint = &coordinate[lastPoint];//指向搜索区域最后一个像素
	//cout << "lastPoint:" << (float)cur_image->ptr<uchar>(coordinate[lastPoint].y)[coordinate[lastPoint].x] << endl;

	//子区左上点遍历的区域
	searchLength_x = coordinate[lastPoint].x - coordinate[0].x + 1;
	searchLength_y = coordinate[lastPoint].y - coordinate[0].y + 1;

	//做一个搜索区域的点集
	for (int i = 0; i < searchLength_y; i++)
	{
		for (int j = 0; j < searchLength_x; j++)
		{
			coordinate_search.push_back(Point(j, i));//这好像就对应上了吧
		}
	}
	//cout << "coordinate_search: " << coordinate_search.size() << endl;
	double* gm_ = new double[searchLength_x * searchLength_y];//做一个一维数组
	double* C2_ = new double[searchLength_x * searchLength_y];
	double* gm2_ = new double[searchLength_x * searchLength_y];//做一个一维数组

	//计算积分表灰度值
	double a, a2, b, b2, c, c2, d, d2;
	for (int y = 0; y < searchLength_y; y++)//行
	{
		for (int x = 0; x < searchLength_x; x++)//列
		{
			a = sum1.ptr<double>(y + temp_H - 1)[x + temp_W - 1];
			a2 = sum2.ptr<double>(y + temp_H - 1)[x + temp_W - 1];
			if (y == 0 || x == 0)
			{
				d = 0;
				d2 = 0;
				if (x == 0)
				{
					b = 0;
					b2 = 0;
				}
				else
				{

					b = sum1.ptr<double>(y + temp_H - 1)[x - 1];
					b2 = sum2.ptr<double>(y + temp_H - 1)[x - 1];
				}

				if (y == 0)
				{
					c = 0;
					c2 = 0;
				}
				else
				{
					c = sum1.ptr<double>(y - 1)[x + temp_W - 1];
					c2 = sum2.ptr<double>(y - 1)[x + temp_W - 1];
				}
			}
			else
			{
				b = sum1.ptr<double>(y + temp_H - 1)[x - 1];
				b2 = sum2.ptr<double>(y + temp_H - 1)[x - 1];
				c = sum1.ptr<double>(y - 1)[x + temp_W - 1];
				c2 = sum2.ptr<double>(y - 1)[x + temp_W - 1];
				d = sum1.ptr<double>(y - 1)[x - 1];
				d2 = sum2.ptr<double>(y - 1)[x - 1];
			}


			gm_[y * searchLength_x + x] = (a - b - c + d) / (float)(temp_H * temp_W);
			gm2_[y * searchLength_x + x] = gm_[y * searchLength_x + x] * gm_[y * searchLength_x + x];
			C2_[y * searchLength_x + x] = (a2 - b2 - c2 + d2);
			C2_[y * searchLength_x + x] = sqrt(C2_[y * searchLength_x + x] - temp_H * temp_W * gm2_[y * searchLength_x + x]);
		}
	}
	
	//ZNCC计算
	long double R;
	long double Rmin;//最小的响应值

	//最小响应值位置信息
	double Rmin_x = 0;
	double Rmin_y = 0;
	Rmin = 0;
	//cout << 35 << endl;
	float* sum_ = new float[searchLength_x * searchLength_y]();

	
	for (int k = 0; k < (searchLength_x * searchLength_y); k++)
	{
		//已经假定在第k个子区中了
		int i = 0;
		i += k;
		int j = 0;
		for (; i < temp_H + k; i++)
		{
			j += k;
			for (; j < temp_W + k; j++)
			{
				float a = ((float)t_uchar[(i - k) * temp_W + (j - k)] - fm_[ii])
					* ((float)p_uchar[(i - k) * cur_image_W + (k / searchLength_x) * (cur_image_W - searchLength_x) + j] - gm_[k]);
				sum_[k] += a;
			}

			j = 0;
		}
	}
	
	//cout << "fm_[ii]:" << fm_[ii] << endl;
	//cout << "f_C1[ii]:" << f_C1[ii] << endl;

	for (int i = 0; i < (searchLength_x * searchLength_y); i++)
	{
		float b = 0;
		b = abs(C2_[i]);
		R = sum_[i] / (b * f_C1[ii]);

		if (R > Rmin)
		{
			Rmin = R;
			Rmin_y = coordinate_search[i].y;
			Rmin_x = coordinate_search[i].x;
			//cout << "Rmin: " << R << endl;

		}
	}

	//if (Rmin > 0.8)
	{
		//这里有问题
	
		vector<float> re = {float(Rmin_x),float(Rmin_y),float(temp_W),float(temp_H)};
		//cout << "g:" << re[0] << endl;
		result.push_back(re);
	}
	//cout << "Rmin:" << Rmin << endl;
	
	delete[] gm_;
	delete[] C2_;
	delete[] p_uchar;
	delete[] t_uchar;
	//cout << "result[iii]:" << result[n_size *iii+ ii][0] << endl;
	//cout << "ii:" << ii << endl;
	//cout << "iii:" << n_size *iii << endl;
	//cout << "result.size:" << result.size() << endl;
	return result[n_size *iii + ii];//这个储存值
}

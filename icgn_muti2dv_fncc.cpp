#include"icgn_muti2dv_fncc.h"

//f_C1���㡢fm_�ļ���
void f(const Mat* const temp,int ii)//���Ǵ�������Mat�ǵ�ͨ��������ͨ��
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
			f_ += temp->ptr <uchar>(i)[j];//��
		}
	}
	fm_.push_back(0);
	fm_[ii] = f_ / dotCount_f;//���ﴢ���ֵ
	//f_fm = f_ - fm_*dotCount_f;//������ͨ�����ǵ�ͨ����δ�����Ҳ��ǵ�ͨ��


	f_C1.push_back(0);
	for (i = 0; i < tempHeight_f; i++)
	{
		for (j = 0; j < tempWeith_f; j++)
		{
			f_C1[ii] += pow((float)temp->ptr<uchar>(i)[j] - fm_[ii], 2);//��
		}
	}

	f_C1[ii] = (sqrt(f_C1[ii]));//���ﴢ���ֵ

	
}

//���ֱ�ļ���
void integral(Mat src, Mat& integral_out, Mat& integral_out1)
{
	Mat tmp(src.size(), CV_64FC1, 0.0);
	Mat tmp1(src.size(), CV_64FC1, 0.0);

	tmp.ptr<double>(0)[0] = (double)src.ptr<uchar>(0)[0];
	//tmp1.ptr<double>(0)[0] = pow((double)src.ptr<uchar>(0)[0],2);//
	tmp1.ptr<double>(0)[0] = (double)src.ptr<uchar>(0)[0] * (double)src.ptr<uchar>(0)[0];
	for (int i = 1; i < src.cols; i++)//��һ�еĴ���
	{
		tmp.ptr<double>(0)[i] = tmp.ptr<double>(0)[i - 1] + src.ptr<uchar>(0)[i];
		tmp1.ptr<double>(0)[i] = (tmp1.ptr<double>(0)[i - 1] + src.ptr<uchar>(0)[i] * src.ptr<uchar>(0)[i]);
	}

	for (int i = 1; i < src.rows; i++)//��һ�еĴ���
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

	tmp.copyTo(integral_out);//���ﴢ���ֵ
	tmp1.copyTo(integral_out1);//���ﴢ���ֵ
}


vector<float> g(const Mat* cur_image, const Mat* temp, Mat sum1, Mat sum2,int ii,int iii,int n_size)
{
	//��ǰͼ���������Ͻǵ��λ��---->>�������������
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
			coordinate.push_back(Point(j, i));//��ÿ�����λ����Ϣ����coordinate������
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
	
	pStartPoint = &coordinate[0];//ָ��(0,0)�������
	//cout << "pStartPoint:" << (float)cur_image->ptr<uchar>(coordinate[0].y)[coordinate[0].x] << endl;
	int lastPoint = (cur_image_H - temp_H) * cur_image_W + (cur_image_W - temp_W);
	//cout << "lastPoint:" << lastPoint << endl;
	pLastLastPoint = &coordinate[lastPoint];//ָ�������������һ������
	//cout << "lastPoint:" << (float)cur_image->ptr<uchar>(coordinate[lastPoint].y)[coordinate[lastPoint].x] << endl;

	//�������ϵ����������
	searchLength_x = coordinate[lastPoint].x - coordinate[0].x + 1;
	searchLength_y = coordinate[lastPoint].y - coordinate[0].y + 1;

	//��һ����������ĵ㼯
	for (int i = 0; i < searchLength_y; i++)
	{
		for (int j = 0; j < searchLength_x; j++)
		{
			coordinate_search.push_back(Point(j, i));//�����Ͷ�Ӧ���˰�
		}
	}
	//cout << "coordinate_search: " << coordinate_search.size() << endl;
	double* gm_ = new double[searchLength_x * searchLength_y];//��һ��һά����
	double* C2_ = new double[searchLength_x * searchLength_y];
	double* gm2_ = new double[searchLength_x * searchLength_y];//��һ��һά����

	//������ֱ�Ҷ�ֵ
	double a, a2, b, b2, c, c2, d, d2;
	for (int y = 0; y < searchLength_y; y++)//��
	{
		for (int x = 0; x < searchLength_x; x++)//��
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
	
	//ZNCC����
	long double R;
	long double Rmin;//��С����Ӧֵ

	//��С��Ӧֵλ����Ϣ
	double Rmin_x = 0;
	double Rmin_y = 0;
	Rmin = 0;
	//cout << 35 << endl;
	float* sum_ = new float[searchLength_x * searchLength_y]();

	
	for (int k = 0; k < (searchLength_x * searchLength_y); k++)
	{
		//�Ѿ��ٶ��ڵ�k����������
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
		//����������
	
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
	return result[n_size *iii + ii];//�������ֵ
}

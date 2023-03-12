#include <math.h>
#include <omp.h>

#include "icgn_muti2dv_icgn.h"

namespace dic
{
	//DIC, the 1st order shape function--------------------------------------------
	ICGN2D1_* ICGN2D1_::allocate(int subset_radius_x, int subset_radius_y) {
		int subset_width = 2 * subset_radius_x + 1;
		int subset_height = 2 * subset_radius_y + 1;
		Point2D subset_center(0, 0);

		ICGN2D1_* ICGN_instance = new ICGN2D1_;
		ICGN_instance->ref_subset = new Subset2D(subset_center, subset_radius_x, subset_radius_y);
		ICGN_instance->tar_subset = new Subset2D(subset_center, subset_radius_x, subset_radius_y);
		ICGN_instance->error_img = Eigen::MatrixXf::Zero(subset_height, subset_width);//��һ��ƫ��ͼ����
		ICGN_instance->sd_img = new3D(subset_height, subset_width, 6);//�����½�ͼ

		return ICGN_instance;
	}

	void ICGN2D1_::release(ICGN2D1_* instance) {
		delete instance->ref_subset;
		delete instance->tar_subset;
		delete3D(instance->sd_img);//�ͷŶ�̬�ڴ�
	}

	//���������û�и㶮��ʲô
	ICGN2D1_* ICGN2D1::getInstance(int tid)//tid�ǵ�ǰ�̵߳ı��
	{
		if (tid >= (int)instance_pool.size()) {
			throw std::string("CPU thread ID over limit");
		}
		//std::cout <<"�߳�:" << tid << std::endl;
		return instance_pool[tid];
	}
	
	ICGN2D1::ICGN2D1(int subset_radius_x, int subset_radius_y, float conv_criterion, float stop_condition, int thread_number)
		: ref_gradient(nullptr), tar_interp(nullptr) {
		this->subset_radius_x = subset_radius_x;
		this->subset_radius_y = subset_radius_y;
		this->conv_criterion = conv_criterion;
		this->stop_condition = stop_condition;
		this->thread_number = thread_number;

		//����������Ƕ��̼߳��㣬���poi���м���
		for (int i = 0; i < thread_number; i++) {
			ICGN2D1_* instance = ICGN2D1_::allocate(subset_radius_x, subset_radius_y);
			instance_pool.push_back(instance);//instance_pool��һ������ icgn���гأ���
		}
	}

	ICGN2D1::~ICGN2D1() {
		delete ref_gradient;
		delete tar_interp;

		for (auto& instance : instance_pool) {
			ICGN2D1_::release(instance);
			delete instance;
		}
		instance_pool.clear();//�ͷ�instance_pool�������
	}


	//���������оݡ�������ֹ����
	void ICGN2D1::setIteration(float conv_criterion, float stop_condition) {
		this->conv_criterion = conv_criterion;
		this->stop_condition = stop_condition;
	}


	void ICGN2D1::setIteration(POI2D* poi) {
		conv_criterion = poi->result.convergence;
		stop_condition = (int)poi->result.iteration;
	}

	//����ο�ͼ�ݶ�ͼ
	void ICGN2D1::prepareRef() {
		if (ref_gradient != nullptr) {
			delete ref_gradient;
			ref_gradient = nullptr;
		}

		ref_gradient = new Gradient2D4(*ref_img);
		ref_gradient->getGradientX();
		ref_gradient->getGradientY();//����ο�ͼ����ݶȣ�����ȫ�ֱ�����
	}

	//����Ŀ��ͼȫ�ֲ�ֵϵ����
	void ICGN2D1::prepareTar() {
		if (tar_interp != nullptr) {
			delete tar_interp;
			tar_interp = nullptr;
		}

		tar_interp = new BicubicBspline(*tar_img);//����ǰͼ����в�ֵϵ��
		tar_interp->prepare();//������������ݶȺͲ�ֵ������
	}

	//����ο�ͼ�ݶ�ͼ������Ŀ��ͼȫ�ֲ�ֵϵ����
	void ICGN2D1::prepare() {
		//prepareRef();
		//prepareTar();
	}

	//��������������POI��һ�ױ���
	void ICGN2D1::compute(POI2D* poi) {
		//set instance w.r.t. thread id 
		ICGN2D1_* cur_instance = getInstance(omp_get_thread_num());//��ȡ��ǰ�߳�

		if (poi->y - subset_radius_y < 0 || poi->x - subset_radius_x < 0
			|| poi->y + subset_radius_y > ref_img->height - 1 || poi->x + subset_radius_x > ref_img->width - 1) {
			std::cerr << "POI is too close to boundary" << poi->x << ", " << poi->y << std::endl;
			poi->result.zncc = 0;
		}
		else {
			int subset_width = 2 * subset_radius_x + 1;
			int subset_height = 2 * subset_radius_y + 1;

			//set reference subset�����òο�������
			cur_instance->ref_subset->center = (Point2D)*poi;//�ο�������poi
			cur_instance->ref_subset->fill(ref_img);//��image�ж�ȡ��Ӧλ�õĻҶ�������������
			float ref_mean_norm = cur_instance->ref_subset->zeroMeanNorm();//��������f��ֵ

			//build the hessian matrix����ɭ����
			cur_instance->hessian.setZero();
			for (int r = 0; r < subset_height; r++) {
				for (int c = 0; c < subset_width; c++) {
					int x_local = c - subset_radius_x;
					int y_local = r - subset_radius_y;
					int x_global = (int)poi->x + x_local;
					int y_global = (int)poi->y + y_local;
					float ref_gradient_x = ref_gradient->gradient_x(y_global, x_global);
					float ref_gradient_y = ref_gradient->gradient_y(y_global, x_global);


					cur_instance->sd_img[r][c][0] = ref_gradient_x;
					cur_instance->sd_img[r][c][1] = ref_gradient_x * x_local;
					cur_instance->sd_img[r][c][2] = ref_gradient_x * y_local;
					cur_instance->sd_img[r][c][3] = ref_gradient_y;
					cur_instance->sd_img[r][c][4] = ref_gradient_y * x_local;
					cur_instance->sd_img[r][c][5] = ref_gradient_y * y_local;

					for (int i = 0; i < 6; i++) {
						float sum = 0;
						for (int j = 0; j < 6; j++) {
							sum = cur_instance->hessian(i, j) + cur_instance->sd_img[r][c][i] * cur_instance->sd_img[r][c][j];
							cur_instance->hessian(i, j) = sum;
						}
					}
				}
			}

			//compute inversed hessian matrix
			cur_instance->inv_hessian = cur_instance->hessian.inverse();

			//set target subset������Ŀ��������
			cur_instance->tar_subset->center = (Point2D)*poi;


			//get initial guess�����ó�ʼ������ֵ�������صı���ֵ��u��v)
			Deformation2D1 p_initial(poi->deformation.u, poi->deformation.ux, poi->deformation.uy,
				poi->deformation.v, poi->deformation.vx, poi->deformation.vy);

			//IC-GN iteration
			int iteration = 0; //initialize iteration counter
			Deformation2D1 p_current, p_increment;//P�͡�P
			p_current.setDeformation(p_initial);//�������س�ֵ�������� P

			float dp_norm_max, znssd;
			Point2D local_coor, warped_coor, global_coor;
			do {
				iteration++;
				//reconstruct target subset  �ؽ�Ŀ������
				for (int r = 0; r < subset_height; r++) {
					for (int c = 0; c < subset_width; c++) {
						//�ֲ�����
						int x_local = c - subset_radius_x;//�����뾶Ϊ���� -> local_coorҲ��һ�������ص�
						int y_local = r - subset_radius_y;

						local_coor.x = x_local;
						local_coor.y = y_local;
						warped_coor = p_current.warp(local_coor);//�õ�W(��;p)
						global_coor = (Point2D)*poi + warped_coor;//��һ����g(x + W(��;p))
						cur_instance->tar_subset->eg_mat(r, c) = tar_interp->compute(global_coor);//����Ŀ������ÿ����Ĳ�ֵ����
					}
				}
				float tar_mean_norm = cur_instance->tar_subset->zeroMeanNorm();//���Ӧ����g�ľ�ֵ

				//compute error image ƫ��ͼ������һ���֣���
				cur_instance->error_img = cur_instance->tar_subset->eg_mat * (ref_mean_norm / tar_mean_norm)
					- (cur_instance->ref_subset->eg_mat);

				//calculate ZNSSD  ������
				znssd = cur_instance->error_img.squaredNorm() / (ref_mean_norm * ref_mean_norm);

				//compute numerator
				float numerator[6] = { 0 };
				for (int r = 0; r < subset_height; r++) {
					for (int c = 0; c < subset_width; c++) {
						for (int i = 0; i < 6; i++) {
							numerator[i] += cur_instance->sd_img[r][c][i] * cur_instance->error_img(r, c);
						}
					}
				}

				//compute dp
				float dp[6] = { 0 };
				for (int i = 0; i < 6; i++) {
					for (int j = 0; j < 6; j++) {
						dp[i] += cur_instance->inv_hessian(i, j) * numerator[j];//�����p������������һά����
					}
				}
				p_increment.setDeformation(dp);//�����p�ľ�����ʽ

				//update warp �����κ���
				p_current.warp_matrix = p_current.warp_matrix * p_increment.warp_matrix.inverse();//W(��;p) = W(��;p) * W^-1(��;��p)

				//update p
				p_current.setDeformation();//����p����������

				//check convergence
				int subset_radius_x2 = subset_radius_x * subset_radius_x;//??
				int subset_radius_y2 = subset_radius_y * subset_radius_y;

				dp_norm_max = 0.f;
				dp_norm_max += p_increment.u * p_increment.u;
				dp_norm_max += p_increment.ux * p_increment.ux * subset_radius_x2;
				dp_norm_max += p_increment.uy * p_increment.uy * subset_radius_y2;
				dp_norm_max += p_increment.v * p_increment.v;
				dp_norm_max += p_increment.vx * p_increment.vx * subset_radius_x2;
				dp_norm_max += p_increment.vy * p_increment.vy * subset_radius_y2;

				dp_norm_max = sqrtf(dp_norm_max);//���������о�

			} while (iteration < stop_condition && dp_norm_max >= conv_criterion);

			//store the final result
			poi->deformation.u = p_current.u;
			poi->deformation.ux = p_current.ux;
			poi->deformation.uy = p_current.uy;
			poi->deformation.v = p_current.v;
			poi->deformation.vx = p_current.vx;
			poi->deformation.vy = p_current.vy;

			//�õ��ġ�p��ֵ
			//std::cout << 111111111 << std::endl;
			
			//std::cout << poi->deformation.uy << std::endl;
			//std::cout << poi->deformation.v << std::endl;
			//std::cout << poi->deformation.vx << std::endl;
			//std::cout << poi->deformation.vy << std::endl;

			//save the results for output
			poi->result.u0 = p_initial.u;
			poi->result.v0 = p_initial.v;
			poi->result.zncc = 0.5f * (2 - znssd);

			//����������Ŷ�
			//if (poi->result.zncc > 0.85)
			{
				//std::cout << "icgn.u:" << poi->deformation.u << "  ";
				//std::cout << "icgn.v:" << poi->deformation.v << "  ";
				std::cout << "icgn.zncc:" << poi->result.zncc << std::endl;
			}
			
			//std::cout << poi->x << std::endl;
			poi->result.iteration = (float)iteration;
			poi->result.convergence = dp_norm_max;

			//��ͼ
		}
	}

	//����poi���еĺ���
	void ICGN2D1::compute(std::vector<POI2D>& poi_queue) {
#pragma omp parallel for
		for (int i = 0; i < poi_queue.size(); ++i) {
			compute(&poi_queue[i]);
		}
	}

}//namespace dic
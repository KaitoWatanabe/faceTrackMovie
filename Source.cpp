#include <iostream>
#include <stdio.h> 
#include <windows.h> 
#include <time.h>
#include <string>
#ifdef __cplusplus
#include <opencv2/opencv.hpp>
#endif
using namespace std;

class Dir
{
public:
	// �R���X�g���N�^
	Dir(void){}
	// �f�X�g���N�^
	virtual ~Dir(void){}

	// �t�@�C���ꗗ�擾
	// folder : �t�H���_�̐�΃p�X����͂Ƃ��� 
	// �� : "D:\\Users\\Pictures\\"
	static vector<string> read(string folder){
		// �錾
		vector<string> fileList;
		HANDLE hFind;
		WIN32_FIND_DATA fd;

		// �t�@�C���������̂��߂Ƀ��C���h�J�[�h�ǉ�
		// �� : "D:\\Users\\Pictures\\*.*"
		stringstream ss;
		ss << folder;
		string::iterator itr = folder.end();
		itr--;
		if (*itr != '\\') ss << '\\';
		ss << "*.*";

		// �t�@�C���T��
		// FindFirstFile(�t�@�C����, &fd);
		hFind = FindFirstFile(ss.str().c_str(), &fd);

		// �������s
		if (hFind == INVALID_HANDLE_VALUE){
			std::cout << "�t�@�C���ꗗ���擾�ł��܂���ł���" << std::endl;
			exit(1); // �G���[�I��
		}

		// �t�@�C���������X�g�Ɋi�[���邽�߂̃��[�v
		do{
			// �t�H���_�͏���
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				&& !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				//�t�@�C���������X�g�Ɋi�[
				char *file = fd.cFileName;
				string str = file;
				fileList.push_back(str);
			}
		} while (FindNextFile(hFind, &fd)); //���̃t�@�C����T��

		// hFind�̃N���[�Y
		FindClose(hFind);

		return fileList;
	}
};

void createMovie(){

	//img�t�H���_���̃t�@�C�����̎擾
	std::vector<std::string> backfilelist = Dir::read("img�t�H���_�̐�΃p�X  ConsoleApplication1\\img");


	cv::Mat img, frame;
	char load_name[100];

	cv::namedWindow("OpenCV Test", CV_WINDOW_AUTOSIZE);

	//�摜�̃T�C�Y���w��
	cv::Size outputSize = cv::Size(1280,720);

	//�t���[�������w��
	double fps = 1;

	//�o�̓t�@�C�������w��
	cv::VideoWriter wrt("./movie.avi", CV_FOURCC_DEFAULT, fps, outputSize);

	if (!wrt.isOpened()){
		//�J���Ȃ������ꍇ�̏���
		
	}

	//�t�H���_���̎ʐ^�̐�
	int image_count = backfilelist.size();

	//����̏�������
	for (int i = 0; i < image_count; i++){
		//int img_number = rand() % image_count;
		sprintf_s(load_name, "img/%d.jpg", i);

		printf(load_name);
		printf("\n");
		
		img = cv::imread(load_name);
		wrt << img;
	}
}


int main(int argc, char *argv[])
{
	//�ۑ����ꂽ�摜��
	int i = 0;

	cv::Mat images[10];
	
	//���Ԍv��
	clock_t oldTime, newTime;

	oldTime = clock();

	cv::Mat img, gray;
	// �J��������̃r�f�I�L���v�`��������������
	cv::VideoCapture cap(0);

	//�L���v�`���摜��RGB�Ŏ擾
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);

	//�J�������I�[�v���ł��Ȃ��ꍇ�I��
	if (!cap.isOpened())
	{
		return -1;
	}

	// �E�B���h�E���쐬����
	char windowName[] = "camera";
	cv::namedWindow(windowName, CV_WINDOW_AUTOSIZE);

	// ���ފ�̓ǂݍ���(2��ނ��邩��D���ȕ���)
	std::string cascadeName = "/Users/kaito/Desktop/opencv-master/data/haarcascades/haarcascade_frontalface_alt.xml";
	//std::string cascadeName = "/Users/yusuke/Desktop/data/lbpcascades/lbpcascade_frontalface.xml";
	cv::CascadeClassifier cascade;
	if (!cascade.load(cascadeName))
		return -1;

	

	//scale�̒l��p���Č��摜���k���A�����Ȃ�8�r�b�g�����^�C1�`�����l��(���m�N��)�̉摜���i�[����z����쐬
	double scale = 4.0;

	// �����L�[�����������܂ŁA���[�v������Ԃ�
	while (true)
	{
		//�L�[�������ꂽ�瓮�悪����
		if (cvWaitKey(1) != -1){
			createMovie();
			break;
		}
		
		cap >> img;

		// �O���[�X�P�[���摜�ɕϊ�
		cv::cvtColor(img, gray, CV_BGR2GRAY);
		cv::Mat smallImg(cv::saturate_cast<int>(img.rows / scale), cv::saturate_cast<int>(img.cols / scale), CV_8UC1);
		// �������ԒZ�k�̂��߂ɉ摜���k��
		cv::resize(gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);
		cv::equalizeHist(smallImg, smallImg);

		std::vector<cv::Rect> faces;
		/// �}���`�X�P�[���i��j�T��xo
		// �摜�C�o�͋�`�C�k���X�P�[���C�Œ��`���C�i�t���O�j�C�ŏ���`
		cascade.detectMultiScale(smallImg, faces, 1.1, 2, CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
		//cascade.detectMultiScale(smallImg, faces);

		// ���ʂ̕`��
		std::vector<cv::Rect>::const_iterator r = faces.begin();
		for (; r != faces.end(); ++r)
		{

			cv::Point center;
			int radius;
			center.x = cv::saturate_cast<int>((r->x + r->width*0.5)*scale);
			center.y = cv::saturate_cast<int>((r->y + r->height*0.5)*scale);
			radius = cv::saturate_cast<int>((r->width + r->height)*0.25*scale);

			newTime = clock();

			//���̎ʐ^���B��܂ł̑҂�����(�~���b)
			if ((float)newTime - (float)oldTime > 1000){

				char str[32];
				sprintf_s(str, "%d", i);
				string dirName("./img/");
				string fileName(str);
				string str2(".jpg");


				dirName = dirName + fileName + str2;   
				cv::imwrite(dirName, img);
				i++;
				oldTime = newTime;
			}
			//cv::circle(img, center, radius, cv::Scalar(80, 80, 255), 3, 8, 0);
			
		}

		

		//cv::namedWindow("result", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
		cv::imshow(windowName, img);
		//cv::waitKey(0);
	}
	
}


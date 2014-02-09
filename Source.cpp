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
	// コンストラクタ
	Dir(void){}
	// デストラクタ
	virtual ~Dir(void){}

	// ファイル一覧取得
	// folder : フォルダの絶対パスを入力とする 
	// 例 : "D:\\Users\\Pictures\\"
	static vector<string> read(string folder){
		// 宣言
		vector<string> fileList;
		HANDLE hFind;
		WIN32_FIND_DATA fd;

		// ファイル名検索のためにワイルドカード追加
		// 例 : "D:\\Users\\Pictures\\*.*"
		stringstream ss;
		ss << folder;
		string::iterator itr = folder.end();
		itr--;
		if (*itr != '\\') ss << '\\';
		ss << "*.*";

		// ファイル探索
		// FindFirstFile(ファイル名, &fd);
		hFind = FindFirstFile(ss.str().c_str(), &fd);

		// 検索失敗
		if (hFind == INVALID_HANDLE_VALUE){
			std::cout << "ファイル一覧を取得できませんでした" << std::endl;
			exit(1); // エラー終了
		}

		// ファイル名をリストに格納するためのループ
		do{
			// フォルダは除く
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				&& !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				//ファイル名をリストに格納
				char *file = fd.cFileName;
				string str = file;
				fileList.push_back(str);
			}
		} while (FindNextFile(hFind, &fd)); //次のファイルを探索

		// hFindのクローズ
		FindClose(hFind);

		return fileList;
	}
};

void createMovie(){

	//imgフォルダ内のファイル情報の取得
	std::vector<std::string> backfilelist = Dir::read("imgフォルダの絶対パス  ConsoleApplication1\\img");


	cv::Mat img, frame;
	char load_name[100];

	cv::namedWindow("OpenCV Test", CV_WINDOW_AUTOSIZE);

	//画像のサイズを指定
	cv::Size outputSize = cv::Size(1280,720);

	//フレーム数を指定
	double fps = 1;

	//出力ファイル名を指定
	cv::VideoWriter wrt("./movie.avi", CV_FOURCC_DEFAULT, fps, outputSize);

	if (!wrt.isOpened()){
		//開けなかった場合の処理
		
	}

	//フォルダ内の写真の数
	int image_count = backfilelist.size();

	//動画の書き込み
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
	//保存された画像数
	int i = 0;

	cv::Mat images[10];
	
	//時間計測
	clock_t oldTime, newTime;

	oldTime = clock();

	cv::Mat img, gray;
	// カメラからのビデオキャプチャを初期化する
	cv::VideoCapture cap(0);

	//キャプチャ画像をRGBで取得
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);

	//カメラがオープンできない場合終了
	if (!cap.isOpened())
	{
		return -1;
	}

	// ウィンドウを作成する
	char windowName[] = "camera";
	cv::namedWindow(windowName, CV_WINDOW_AUTOSIZE);

	// 分類器の読み込み(2種類あるから好きな方を)
	std::string cascadeName = "/Users/kaito/Desktop/opencv-master/data/haarcascades/haarcascade_frontalface_alt.xml";
	//std::string cascadeName = "/Users/yusuke/Desktop/data/lbpcascades/lbpcascade_frontalface.xml";
	cv::CascadeClassifier cascade;
	if (!cascade.load(cascadeName))
		return -1;

	

	//scaleの値を用いて元画像を縮小、符号なし8ビット整数型，1チャンネル(モノクロ)の画像を格納する配列を作成
	double scale = 4.0;

	// 何かキーが押下されるまで、ループをくり返す
	while (true)
	{
		//キーが押されたら動画が生成
		if (cvWaitKey(1) != -1){
			createMovie();
			break;
		}
		
		cap >> img;

		// グレースケール画像に変換
		cv::cvtColor(img, gray, CV_BGR2GRAY);
		cv::Mat smallImg(cv::saturate_cast<int>(img.rows / scale), cv::saturate_cast<int>(img.cols / scale), CV_8UC1);
		// 処理時間短縮のために画像を縮小
		cv::resize(gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);
		cv::equalizeHist(smallImg, smallImg);

		std::vector<cv::Rect> faces;
		/// マルチスケール（顔）探索xo
		// 画像，出力矩形，縮小スケール，最低矩形数，（フラグ），最小矩形
		cascade.detectMultiScale(smallImg, faces, 1.1, 2, CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
		//cascade.detectMultiScale(smallImg, faces);

		// 結果の描画
		std::vector<cv::Rect>::const_iterator r = faces.begin();
		for (; r != faces.end(); ++r)
		{

			cv::Point center;
			int radius;
			center.x = cv::saturate_cast<int>((r->x + r->width*0.5)*scale);
			center.y = cv::saturate_cast<int>((r->y + r->height*0.5)*scale);
			radius = cv::saturate_cast<int>((r->width + r->height)*0.25*scale);

			newTime = clock();

			//次の写真を撮るまでの待ち時間(ミリ秒)
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


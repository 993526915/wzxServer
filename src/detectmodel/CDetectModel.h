#ifndef DETECTFACE_H_
#define DETECTFACE_H_

#include "arcsoft_face_sdk.h"
#include "amcomdef.h"
#include "asvloffscreen.h"
#include "merror.h"
#include <iostream>  
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include"co_params.h"
#include"DBpool.h"
#include<jsoncpp/json/json.h>
#include<opencv4/opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class CDetectModel 
{ 
public:
	static CDetectModel& GetInstance() 
	{ 
		static CDetectModel instance;
		return instance; 
	} 
    
    tuple<string,string> detectOneFace(Mat detectSrc,string classnum,string account);
    int add(Json::Value &json_result,Mat image,string classnum,string account);
    int groupGetlist(Json::Value &json_result);
    int groupGetusers(Json::Value &json_result, string classnum);
    int groupDelete(Json::Value &json_result,string classnun) ;
    int userDelete(Json::Value &json_result,string classnum,string account);

private:
    int _readBinary(char *filename, char *buffer);
    int _WriteBinary(char *filename, char *buffer, int length);
    MFloat _CompareImage(Mat src, Mat dst);
    ASF_FaceFeature _GetFaceFeature(Mat src);
    std::string _Mat2Base64(const cv::Mat img, std::string imgType);
    void _TimestampToTime(char* timeStamp, char* dateTime, int dateTimeSize);
    //图像颜色格式转换
    int _ColorSpaceConversion(MInt32 format, cv::Mat img, ASVLOFFSCREEN&offscreen);
    // void _Init();
	CDetectModel();
	~CDetectModel(){}

    MHandle m_handle;
};






#endif

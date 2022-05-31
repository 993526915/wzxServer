#include"CDetectModel.h"
int CDetectModel::_readBinary(char *filename, char *buffer)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("fopen failed\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    int size = fread(buffer, 1, length, fp);
    if (size != length) {
        printf("fread faile\n");
        return size;
    }
    fclose(fp);
    return size;
}

int CDetectModel::_WriteBinary(char *filename, char *buffer, int length) 
{
    FILE *fp = fopen(filename, "wb+");
    if (fp == NULL) {
        printf("fopen failed\n");
        return -1;
    }
    int size = fwrite(buffer, 1, length, fp);
    if (size != length) {
        printf("fread faile\n");
        return size;
    }
    fclose(fp);
    return size;
}

void CDetectModel::_TimestampToTime(char* timeStamp, char* dateTime, int dateTimeSize)
{
    time_t tTimeStamp = atoll(timeStamp);
    struct tm* pTm = gmtime(&tTimeStamp);
    strftime(dateTime, dateTimeSize, "%Y-%m-%d %H:%M:%S", pTm);
}
tuple<string,string> CDetectModel::detectOneFace(Mat detectSrc,string classnum,string account)
{
    tuple<string,string> t = make_tuple("","");
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    char strSql[256];
    sprintf(strSql,"select image,width,height,image_type,image_length from user_table where classnum = '%s' and account = '%s';",
            classnum.c_str(),account.c_str());
    //strSql = "select account,image,width,height,image_type,image_length from user_table where classnum = '"+classnum+"';";
    CResultSet *res = newConnect->ExecuteQuery(strSql);
    while(res->Next())
    {

        int width = res->GetInt("width");
        int height = res->GetInt("height");
        int image_type = res->GetInt("image_type");
        int image_length = res->GetInt("image_length");
//        char picture[1024*1024];
//        memset(picture,0,sizeof(picture));
//        memcpy(picture,res->GetString("image"),image_length);

        Mat pic(height,width,image_type,res->GetString("image"));
        MFloat res = _CompareImage(detectSrc,pic);
        if(res > 0.8)
        {
            t = make_tuple(classnum,account);
            cout << "compare res :" << res << endl;
            break;
        }
        else if(res == 0)
        {
            t = make_tuple("noFace","noFace");
            cout << "noFace" << endl;
            break;
        }
    }
    CDBPool::GetInstance().RelDBConn(newConnect);
    return t;
}
int CDetectModel::add(Json::Value &json_result,Mat image,string classnum,string account)
{
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn(10);
    string strSql = "select classnum,account from user_table where classnum = '"+classnum+"' and account = '"+account + "';"; 
    CResultSet *res =  newConnect->ExecuteQuery(strSql.c_str());
    if(res->Next())
    {
        cout << "user exist" << endl;
        strSql = "update user_table set image=?,width=?,height =?,image_type=?,image_length =? where classnum = '"+ classnum + "' and account = '"+account+"';";
        cout << strSql << endl;
        if(newConnect->StartTransaction()) //启动事务操作
        {
            //char buffer[1024*1024];
            int length = image.cols * image.rows * image.elemSize();
            //memset(buffer,0,sizeof(buffer));
            //memcpy(buffer,image.data,length);
            CPrepareStatement *stmt = new CPrepareStatement();
            if (stmt->Init(newConnect->GetMysql(),strSql))
            {
                int index = 0;
                int type = image.type();
                stmt->SetParam(index++, (char *)image.data);
                stmt->SetParam(index++, image.cols);
                stmt->SetParam(index++, image.rows);
                stmt->SetParam(index++, type);
                stmt->SetParam(index++, length);
                int bRet = stmt->ExecuteUpdate((char *)image.data,0,length);
                if (!bRet)
                {
                    json_result["error_code"] = 1;
                    cout << "sql ExecuteUpdate error!" <<  endl;
                    CDBPool::GetInstance().RelDBConn(newConnect);
                    newConnect->Rollback();
                    return -1;
                }
                else
                {         
                    json_result["error_code"] = 0;
                    CDBPool::GetInstance().RelDBConn(newConnect);
                    newConnect->Commit();
                }
            }
        }
    }
    else
    {
        if(newConnect->StartTransaction()) //启动事务操作
        {
//            char buffer[1024*1024];
            int length = image.cols * image.rows * image.elemSize();
//            memset(buffer,0,sizeof(buffer));
//            memcpy(buffer,image.data,length);

            strSql = "insert into user_table (classnum,account,image,width,height,image_type,image_length) values('"+classnum+"',"+"'"+account+"'" +",?,?,?,?,?);";
            CPrepareStatement *stmt = new CPrepareStatement();
            if (stmt->Init(newConnect->GetMysql(),strSql))
            {
                int index = 0;
                int type = image.type();
                stmt->SetParam(index++, (char *)image.data);
                stmt->SetParam(index++, image.cols);
                stmt->SetParam(index++, image.rows);
                stmt->SetParam(index++, type);
                stmt->SetParam(index++, length);
                int bRet = stmt->ExecuteUpdate((char *)image.data,0,length);
                if (!bRet)
                {
                    json_result["error_code"] = 1;
                    cout << "sql ExecuteUpdate error!" <<  endl;
                    CDBPool::GetInstance().RelDBConn(newConnect);
                    newConnect->Rollback();
                    return -1;
                }
                else
                {
                    
                    json_result["error_code"] = 0;
                    CDBPool::GetInstance().RelDBConn(newConnect);
                    newConnect->Commit();
                }
            }
        }
    }
    return 0;

}
// {
//         "cached" : 0,
//         "error_code" : 0,
//         "error_msg" : "SUCCESS",
//         "log_id" : 2977704542,
//         "result" : 
//         {
//                 "face_token" : "c1daa91cec6f4cef4141465aad785554",
//                 "location" : 
//                 {
//                         "height" : 187,
//                         "left" : 35.07,
//                         "rotation" : -3,
//                         "top" : 168.06,
//                         "width" : 191
//                 }
//         },
//         "timestamp" : 1649861377
// }
int CDetectModel::groupGetlist(Json::Value &json_result)
{
    // {
    //     "cached" : 0,
    //     "error_code" : 0,
    //     "error_msg" : "SUCCESS",
    //     "log_id" : 1214856556,
    //     "result" : 
    //     {
    //             "group_id_list" : 
    //             [
    //                     "number1",
    //                     "123",
    //                     "111",
    //                     "11",
    //                     "1",
    //                     "asa",
    //                     "2",
    //                     "22",
    //                     "21",
    //                     "3",
    //                     "4"
    //             ]
    //     },
    //     "timestamp" : 1649902814
    // }
    Json::Value classnum;
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    string strSql;
    strSql = "select distinct classnum from user_table;";
    CResultSet *res = newConnect->ExecuteQuery(strSql.c_str());
    while(res->Next())
    {
        cout << res->GetString("classnum") << endl;
        classnum["group_id_list"].append(Json::Value(res->GetString("classnum")));
    }
    json_result["result"] = classnum;
    CDBPool::GetInstance().RelDBConn(newConnect);
    return 0;
}
int CDetectModel::groupGetusers(Json::Value &json_result, string classnum)
{
    // {
    //         "cached" : 0,
    //         "error_code" : 0,
    //         "error_msg" : "SUCCESS",
    //         "log_id" : 1013914341,
    //         "result" : 
    //         {
    //                 "user_id_list" : 
    //                 [
    //                         "www",
    //                         "123"
    //                 ]
    //         },
    //         "timestamp" : 1649902613
    // }
    Json::Value classNumber;
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    string strSql;
    strSql = "select account,touxiang from user_table where classnum = '" + classnum +"';";
    CResultSet *res = newConnect->ExecuteQuery(strSql.c_str());
    while(res->Next())
    {
        cout << res->GetString("account") << endl;
        Json::Value userInfo;
        string account = res->GetString("account");
        string touxiang = res->GetString("touxiang");
        userInfo["account"] = account;
        userInfo["touxiang"]  =  touxiang;
        classNumber["user_id_list"].append(userInfo);
    }
    json_result["result"] = classNumber;
    CDBPool::GetInstance().RelDBConn(newConnect);
    return 0;
}
int CDetectModel::groupDelete(Json::Value &json_result,string group_id) 
{

}
int CDetectModel::userDelete(Json::Value &json_result,string group_id,string user_id)
{

}

CDetectModel::CDetectModel()
{
    printf("\n************* ArcFace SDK Info *****************\n");
	MRESULT res = MOK;
	ASF_ActiveFileInfo activeFileInfo = { 0 };
	res = ASFGetActiveFileInfo(&activeFileInfo);
	if (res != MOK)
	{
		printf("ASFGetActiveFileInfo fail: %d\n", res);
	}
	else
	{
		//这里仅获取了有效期时间，还需要其他信息直接打印即可
		char startDateTime[32];
		_TimestampToTime(activeFileInfo.startTime, startDateTime, 32);
		printf("startTime: %s\n", startDateTime);
		char endDateTime[32];
		_TimestampToTime(activeFileInfo.endTime, endDateTime, 32);
		printf("endTime: %s\n", endDateTime);
	}

	//SDK版本信息
	const ASF_VERSION version = ASFGetVersion();
	printf("\nVersion:%s\n", version.Version);
	printf("BuildDate:%s\n", version.BuildDate);
	printf("CopyRight:%s\n", version.CopyRight);
    printf("\n************* Face Recognition *****************\n");
	
    shared_ptr<arcSoftParams> arcSoft = initParams::GetInstance().getArcSoftParams();
    
	res = ASFOnlineActivation((MPChar)arcSoft->appID.c_str(), (MPChar)arcSoft->SDKKey.c_str());
	if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res)
		printf("ASFOnlineActivation fail: %d\n", res);
	else
		printf("ASFOnlineActivation sucess: %d\n", res);

	//初始化引擎
    m_handle = NULL;
	MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION  | ASF_LIVENESS ;
	res = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_0_ONLY, arcSoft->NScale, arcSoft->FaceNum, mask, &m_handle);
    cout << "----------------------ssssssssssssssss-------------------"<<endl;
	if (res != MOK)
		printf("ASFInitEngine fail: %d\n", res);
	else
		printf("ASFInitEngine sucess: %d\n", res);
}

int CDetectModel::_ColorSpaceConversion(MInt32 format, cv::Mat img, ASVLOFFSCREEN&offscreen)
{
    switch (format)   //原始图像颜色格式
    {
    case ASVL_PAF_I420:
        offscreen.u32PixelArrayFormat= (unsigned int)format;
        offscreen.i32Width = img.cols;
        offscreen.i32Height =img.rows;
        offscreen.pi32Pitch[0] =img.step;
        offscreen.pi32Pitch[1] =offscreen.pi32Pitch[0] >> 1;
        offscreen.pi32Pitch[2] =offscreen.pi32Pitch[0] >> 1;
        offscreen.ppu8Plane[0] =img.data;
        offscreen.ppu8Plane[1] =offscreen.ppu8Plane[0] + offscreen.i32Height * offscreen.pi32Pitch[0];
        offscreen.ppu8Plane[2] =offscreen.ppu8Plane[0] + offscreen.i32Height * offscreen.pi32Pitch[0] * 5 / 4;
        break;
    case ASVL_PAF_YUYV:
        offscreen.u32PixelArrayFormat= (unsigned int)format;
        offscreen.i32Width = img.cols;
        offscreen.i32Height = img.rows;
        offscreen.pi32Pitch[0] =img.step;
        offscreen.ppu8Plane[0] =img.data;;
        break;
    case ASVL_PAF_NV12:
        offscreen.u32PixelArrayFormat= (unsigned int)format;
        offscreen.i32Width = img.cols;
        offscreen.i32Height =img.rows;
        offscreen.pi32Pitch[0] =img.step;
        offscreen.pi32Pitch[1] =offscreen.pi32Pitch[0];
        offscreen.ppu8Plane[0] =img.data;
        offscreen.ppu8Plane[1] =offscreen.ppu8Plane[0] + offscreen.pi32Pitch[0] * offscreen.i32Height;
        break;
    case ASVL_PAF_NV21:
        offscreen.u32PixelArrayFormat= (unsigned int)format;
        offscreen.i32Width = img.cols;
        offscreen.i32Height =img.rows;
        offscreen.pi32Pitch[0] =img.step;
        offscreen.pi32Pitch[1] =offscreen.pi32Pitch[0];
        offscreen.ppu8Plane[0] =img.data;
        offscreen.ppu8Plane[1] =offscreen.ppu8Plane[0] + offscreen.pi32Pitch[0] * offscreen.i32Height;
        break;
    case ASVL_PAF_RGB24_B8G8R8:
        offscreen.u32PixelArrayFormat= (unsigned int)format;
        offscreen.i32Width = img.cols;
        offscreen.i32Height =img.rows;
        offscreen.pi32Pitch[0] =img.step;
        offscreen.ppu8Plane[0] =img.data;
        break;
    case ASVL_PAF_DEPTH_U16:
        offscreen.u32PixelArrayFormat= (unsigned int)format;
        offscreen.i32Width = img.cols;
        offscreen.i32Height =img.rows;
        offscreen.pi32Pitch[0] =img.step;
        offscreen.ppu8Plane[0] =img.data;
        break;
    case ASVL_PAF_GRAY:
        offscreen.u32PixelArrayFormat= (unsigned int)format;
        offscreen.i32Width = img.cols;
        offscreen.i32Height =img.rows;
        offscreen.pi32Pitch[0] =img.step;
        offscreen.ppu8Plane[0] =img.data;
        break;
    default:
        return 0;
    }
    return 1;
}


MFloat CDetectModel::_CompareImage(Mat src, Mat dst)
{
    ASF_FaceFeature copyfeature1 = {0};
    ASF_FaceFeature feature1 = _GetFaceFeature(src);
    if(feature1.featureSize==0) return 0;
        //拷贝feature，否则第二次进行特征提取，会覆盖第一次特征提取的数据，导致比对的结果为1
    copyfeature1.featureSize = feature1.featureSize;
    copyfeature1.feature = (MByte *)malloc(feature1.featureSize);
    memset(copyfeature1.feature, 0, feature1.featureSize);
    memcpy(copyfeature1.feature, feature1.feature, feature1.featureSize);
    ASF_FaceFeature feature2 = _GetFaceFeature(dst);
    if(feature2.featureSize==0) return 0;
    // 单人脸特征比对
    MFloat confidenceLevel = 0;
    MRESULT res;
    res = ASFFaceFeatureCompare(m_handle, &copyfeature1, &feature2, &confidenceLevel);
    if (res != MOK)
        printf("ASFFaceFeatureCompare fail: %d\n", res);
    else
        printf("ASFFaceFeatureCompare sucess: %lf\n", confidenceLevel);

    return confidenceLevel;
}



ASF_FaceFeature CDetectModel::_GetFaceFeature(Mat src)
{
    ASVLOFFSCREEN arcPic = { 0 };
    _ColorSpaceConversion(ASVL_PAF_RGB24_B8G8R8,src,arcPic);

    ASF_MultiFaceInfo detectedFaces1 = { 0 };
    ASF_SingleFaceInfo SingleDetectedFaces = { 0 };
    ASF_FaceFeature feature = { 0 };
    MRESULT res = MOK;
    res = ASFDetectFacesEx(m_handle, &arcPic, &detectedFaces1);
    if (res != MOK && detectedFaces1.faceNum <= 0)
    {
        printf("ASFDetectFaces fail: %d\n", res);
    }
    else
    {
        SingleDetectedFaces.faceRect.left = detectedFaces1.faceRect[0].left;
        SingleDetectedFaces.faceRect.top = detectedFaces1.faceRect[0].top;
        SingleDetectedFaces.faceRect.right = detectedFaces1.faceRect[0].right;
        SingleDetectedFaces.faceRect.bottom = detectedFaces1.faceRect[0].bottom;
        SingleDetectedFaces.faceOrient = detectedFaces1.faceOrient[0];

        // 单人脸特征提取
        res = ASFFaceFeatureExtractEx(m_handle, &arcPic, &SingleDetectedFaces, &feature);
        if (res != MOK)
        {
            printf("ASFFaceFeatureExtractEx 1 fail: %d\n", res);
        }
    }
    return feature;
}

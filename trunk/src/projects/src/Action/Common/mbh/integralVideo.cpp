/********************************************************************************

Filename     : integralVideo.cpp

Description  : check file "integralVideo.h"

Author       : FengShi@Discover lab, Apr, 2010
Version No   : 1.00


*********************************************************************************/

#include "integralVideo.h"
#include "waitKeySeconds.h"
#include "formatBinaryStream.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/cudaoptflow.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/video/tracking.hpp>

#include <iostream>

#define normThreshold  0.001

IntegralVideo::IntegralVideo(const std::shared_ptr<sequence::IStreamedVideo>& video, int bins, bool fullOri, cv::Point3f rt2ps,
	float reSzRatio, int stFrame, int endFrame) :_nbins(bins), _fullOri(fullOri)
{
	_fullAng = _fullOri ? 360 : 180;
	_anglePerBin = (float)_fullAng / _nbins;
	_hasIv = 0;
#ifdef INTEGRAL_VIDEO_DEBUG
	if (!filed.is_open())
		filed.open("IntegralVideoError.txt",std::ios::out);
#endif
	computeIntegVideo(video, rt2ps, reSzRatio, stFrame, endFrame);

}

//cvIsInf();
static bool isFlowCorrect(float u) {
	return !cvIsNaN(u) && (fabs(u) < 1e9);
}

static bool isFlowNotCorrect(double u) {
	return cvIsNaN(u) || (fabs(u) > 1e9);
}


bool IntegralVideo::computeIntegVideo(const std::shared_ptr<sequence::IStreamedVideo>& video, cv::Point3f rt2ps, float reSzRatio, int stFrame, int endFrame)
{
	int imT = video->getNumFrames();
	if (imT <= 0)
	{
		_hasIv = 0;
		return false;
	}

	if (endFrame <= 0 || endFrame > imT)
		endFrame = imT;
	if (stFrame < 0)
		stFrame = 0;
	if (stFrame >= endFrame)
	{
		std::cerr << "Class: IntegralVideo:: imT = cap.get(CV_CAP_PROP_FRAME_COUNT) result is " << imT << "\n";
		_hasIv = 0;
		return false;
	}

	_ivPs[0].clear(); _ivPs[1].clear();
	_ivRt[0].clear(); _ivRt[1].clear();

	//skip frames for computing root iv. 
	int skipIm;
	if (fabs(rt2ps.z - 0.5) < 10E-7)  //if rt2ps.z == 0.5, choose every odd input frames(frames 1, 3, 5..) for computing root integral video
		skipIm = 2;
	else if (fabs(rt2ps.z - 1. / 3.) < 10E-7) //if rt2ps.z == 1/3, skip 2 out of 3 frames (choosing frames 1, 4, 7..) for computing root integral video
		skipIm = 3;
	else
		skipIm = 1;  //no skip, choosing all input frames to compute root iv 

	cv::Mat  im, imPs1, imPs2;
	cv::Mat oflow;
	std::vector<cv::Mat> oFlows(2), rootOFs(2), ofTmp(2);

	video->setPosition(stFrame);
	video >> im;

	if (im.channels() >= 3)
		cvtColor(im, imPs1, CV_BGR2GRAY);
	else
		imPs1 = im.clone();

	cv::Size partSz;
	//GaussianBlur(imPs1, imPs1, Size(9,9),2);
	//set the spatial size for parts
	if (fabs(reSzRatio - 0.5) < 10E-5)
	{
		partSz.height = im.rows / 2;
		partSz.width = im.cols / 2;
		pyrDown(imPs1, imPs1, partSz);
	}
	else if (fabs(reSzRatio - 1) < 10E-5)
	{
		partSz.height = im.rows;
		partSz.width = im.cols;

	}
	else
	{
		partSz.height = cvRound(reSzRatio*im.rows);
		partSz.width = cvRound(reSzRatio*im.cols);
		resize(imPs1, imPs1, partSz, 0, 0, CV_INTER_AREA);
	}

	cv::Size rootSz;
	//set the spatial size for root
	if (fabs(rt2ps.x - 0.5) < 10E-5 && fabs(rt2ps.y - 0.5) < 10E-5)
	{
		rootSz.height = partSz.height / 2;
		rootSz.width = partSz.width / 2;
	}
	else
	{
		rootSz.height = cvRound(rt2ps.y*partSz.height);
		rootSz.width = cvRound(rt2ps.x*partSz.width);
	}

	_ivWps = partSz.width + 1;  //part integral video size [im.Width+1, im.Height+1, T]
	_ivHps = partSz.height + 1;
	//_ivBps = 1;

	_ivWrt = rootSz.width + 1;  //root integral video size [im.Width+1, im.Height+1, T]
	_ivHrt = rootSz.height + 1;

	for (int i = 0; i < 2; i++)  //i=0 for u and i=1 for v
	{
		_ivPs[i].push_back(cv::Mat(_ivHps, _ivWps*_nbins, CV_32FC1, cv::Scalar(0))); //first line of integral video (value is 0)
		_ivRt[i].push_back(cv::Mat(_ivHrt, _ivWrt*_nbins, CV_32FC1, cv::Scalar(0))); //first line of integral video (value is 0)
	}
	_ivBps = 0;
	_ivBrt = 0;

	////////////////// CUDA /////////////////////////

	static cv::cuda::GpuMat imTmp1(imPs1.size(), CV_32FC1);
	static cv::cuda::GpuMat imTmp2(imPs1.size(), CV_32FC1);
	static cv::cuda::GpuMat d_flow(imPs1.size(), CV_32FC2);
	static cv::cuda::GpuMat d_Temp[2];

	static cv::Ptr<cv::cuda::FarnebackOpticalFlow> optFlow = cv::cuda::FarnebackOpticalFlow::create();
	//static cv::Ptr<cv::cuda::FarnebackOpticalFlow> optFlow = cv::cuda::FarnebackOpticalFlow::create(5, sqrt(2.f) / 2.0, false, 10, 7, 7, 1.5, cv::OPTFLOW_FARNEBACK_GAUSSIAN);
	//static cv::Ptr<cv::cuda::DensePyrLKOpticalFlow> optFlow = cv::cuda::DensePyrLKOpticalFlow::create(cv::Size(7, 7));
	//static cv::Ptr<cv::cuda::OpticalFlowDual_TVL1> optFlow = cv::cuda::OpticalFlowDual_TVL1::create();

	////////////////// CUDA /////////////////////////

#if 0
#ifdef  _USE_OCL_OPTICAL_FLOW_
	cv::ocl::oclMat d_flowx, d_flowy, imTmp1, imTmp2;
	d_flowx.create(imPs1.size(), CV_32FC1);
	d_flowy.create(imPs1.size(), CV_32FC1);
	imTmp1.create(imPs1.size(), imPs1.type());
	imTmp2.create(imPs1.size(), imPs1.type());

#ifdef 	_USE_DUAL_L1_OF_
	cv::ocl::OpticalFlowDual_TVL1_OCL d_alg;
#else
	cv::ocl::FarnebackOpticalFlow farn;
	farn.numLevels = 5;
	farn.winSize = 10;
	//farn.numLevels = 3;
	//farn.winSize = 7;
	farn.numIters = 7;
	farn.fastPyramids = false;
	farn.pyrScale = sqrt(2.f)/2.0;
	//farn.pyrScale = 0.5;
	farn.polyN = 7;
	farn.polySigma = 1.5;
	farn.flags = 0;
	//farn.pyrScale  = sqrt(2.f)/2.;
	//cout<<"farn: "<<farn.numLevels<<endl;
#endif  //_USE_DUAL_L1_OF_
#else
#ifdef 	_USE_DUAL_L1_OF_
	cv::Ptr<cv::DenseOpticalFlow> alg = cv::createOptFlow_DualTVL1();
#endif  //_USE_DUAL_L1_OF_
#endif  // _USE_OCL_OPTICAL_FLOW_
#endif // 0


	video >> im;
	while (!(im.empty()) && _ivBps <= MAX_VIDEO_BUFFER && _ivBps < endFrame - stFrame)
	{
		//processing image with grey gradient
		if (im.channels() >= 3)
			cvtColor(im, imPs2, CV_BGR2GRAY);
		else
			imPs2 = im.clone();
		//GaussianBlur(imPs2, imPs2, Size(9,9),2);

		if (fabs(reSzRatio - 0.5) < 10E-5)
			pyrDown(imPs2, imPs2, partSz);
		else if (fabs(reSzRatio - 1) > 10E-5)
			resize(imPs2, imPs2, partSz, 0, 0, CV_INTER_AREA);
		//comupte optical flow

		////////////////// CUDA /////////////////////////

		imTmp1.upload(imPs1);
		imTmp2.upload(imPs2);

		optFlow->calc(imTmp1, imTmp2, d_flow);

		cv::cuda::split(d_flow, d_Temp);
		d_Temp[0].download(oFlows[0]);
		d_Temp[1].download(oFlows[1]);

		////////////////// CUDA /////////////////////////

#if 0

#ifdef  _USE_OCL_OPTICAL_FLOW_	
		imTmp1.upload(imPs1);
		imTmp2.upload(imPs2);

#ifdef 	_USE_DUAL_L1_OF_
		d_alg(imTmp1, imTmp2, d_flowx, d_flowy);
#else
		farn(imTmp1, imTmp2, d_flowx, d_flowy);
#endif  //_USE_DUAL_L1_OF_	

		d_flowx.download(oFlows[0]);
		d_flowy.download(oFlows[1]);

#else
#ifdef 	_USE_DUAL_L1_OF_
		alg->calc(imPs1, imPs2, oflow);
#else
		calcOpticalFlowFarneback( imPs1, imPs2, oflow, sqrt(2.f)/2.0, 5, 10, 2, 7, 1.5, cv::OPTFLOW_FARNEBACK_GAUSSIAN );
		//oflow *= 100;  //not work, only multiply to first channel
#endif  //_USE_DUAL_L1_OF_
		split(oflow, oFlows);

#endif  // _USE_OCL_OPTICAL_FLOW_
#endif // 0



		//remove noise optical flow values by threshold those absolute values large than 100 to zero
		//if not using threshold, the results could be better 1% improvements on HMDB51, however, sometimes it is unstable due to the infinite values of wrong optical flow
		//this will avoid unstable computation due to the wrong very large optical flow values, such as 10E7 or -10E7
		/*
			threshold(oFlows[0],oFlows[0],100,10000,THRESH_TOZERO_INV);    // if oFlows[0]>100, set the value to zeros
			threshold(oFlows[0],oFlows[0],-100,10000,THRESH_TOZERO);  // if oFlows[0]<-100, set the value to zeros
			threshold(oFlows[1],oFlows[1],100,10000,THRESH_TOZERO_INV);
			threshold(oFlows[1],oFlows[1],-100,10000,THRESH_TOZERO);
			*/
		if (!checkRange(oFlows[0], 1, NULL, -1e9, 1e9) || !checkRange(oFlows[1], 1, NULL, -1e9, 1e9))
		{
			threshold(oFlows[0], oFlows[0], 1e9, 10000, cv::THRESH_TOZERO_INV);
			threshold(oFlows[0], oFlows[0], -1e9, 10000, cv::THRESH_TOZERO);
			threshold(oFlows[1], oFlows[1], 1e9, 10000, cv::THRESH_TOZERO_INV);
			threshold(oFlows[1], oFlows[1], -1e9, 10000, cv::THRESH_TOZERO);
		}

		cv::Mat tmpPs, tmp0;
		for (int i0 = 0; i0 < 2; i0++)
		{
			integralHist(oFlows[i0], tmpPs);  //compute integral image 
			tmp0 = cv::Mat(_ivHps, _ivWps*_nbins, CV_32FC1);
			add(tmpPs, _ivPs[i0][_ivBps], tmp0);  //add current integral image into last integral image
			_ivPs[i0].push_back(tmp0);

		}
		_ivBps++;

		//if no skip current frame, compute root iv
		if (!(_ivBps % skipIm))
		{
			cv::Mat imRt, tmpRt, tmp1;
			for (int i0 = 0; i0 < 2; i0++)
			{
				//reuse the optical flows computed from Parts
				if (fabs(rt2ps.x - 0.5) < 10E-5 && fabs(rt2ps.y - 0.5) < 10E-5)
					pyrDown(oFlows[i0], rootOFs[i0], rootSz);
				else
					resize(oFlows[i0], rootOFs[i0], rootSz, 0, 0, CV_INTER_AREA);  //resize part image 1 into root image 1 with root to part ratio
				//CV_Assert(imRt.type() == CV_32FC1 && imRt.size() == rootSz);
				integralHist(rootOFs[i0], tmpRt);
				//checkRange(imRt,0);checkRange(tmpRt,0);
				tmp1 = cv::Mat(_ivHrt, _ivWrt*_nbins, CV_32FC1);
				//tmp1 = Mat(_ivHrt,_ivWrt*_nbins,DataType<float>::type);
				add(tmpRt, _ivRt[i0][_ivBrt], tmp1);
				_ivRt[i0].push_back(tmp1);
			}
			_ivBrt++;
		}

		video >> im;

		imPs1 = imPs2;  //set part image 2 as part image 1
		imPs2 = cv::Mat();  //imPs2 must point to other head. otherwise, if imPs2 is changed somewhere else, the imPs1 will also be changed.			
	}
	_ivBps = _ivPs[0].size();
	_ivBrt = _ivRt[0].size();
	_hasIv = 1;
	
	return true;
}


void IntegralVideo::integralHist(const cv::Mat& src, cv::Mat& hist) const
{
	cv::Mat derv0, derv1, magnitude, phase;
	//derv0 = Mat(src.rows, src.cols, src.type());
	//derv1 = Mat(src.rows, src.cols, src.type());
	//Sobel(src, derv0, -1, 1, 0, 1, 1, 0, IPL_BORDER_REPLICATE); //compute dx with [-1, 0 ,1] kernel, and save into _derv[0] with "IPL_BORDER_REPLICATE"
	//Sobel(src, derv1, -1, 0, 1, 1, 1, 0, IPL_BORDER_REPLICATE); //compute dy with [-1, 0 ,1] kernel, and save into _derv[1]
	Sobel(src, derv0, CV_32F, 1, 0, 1, 1, 0, IPL_BORDER_REPLICATE); //compute dx with [-1, 0 ,1] kernel, and save into _derv[0] with "IPL_BORDER_REPLICATE"
	Sobel(src, derv1, CV_32F, 0, 1, 1, 1, 0, IPL_BORDER_REPLICATE); //compute dy with [-1, 0 ,1] kernel, and save into _derv[1]
	//magnitude(derv0, derv1, magnitude);
	//phase(derv0, derv1, phase, true);
	cartToPolar(derv0, derv1, magnitude, phase, true);

	int cols = src.cols;
	int rows = src.rows;

	int iCols = (cols + 1)*_nbins;
	hist = cv::Mat(rows + 1, iCols, CV_32FC1, cv::Scalar(0.));

	for (int iy = 0; iy < rows; iy++)
	{
		const float *pMag = magnitude.ptr<float>(iy);
		const float *pPhase = phase.ptr<float>(iy);
		const float *pHist0 = hist.ptr<float>(iy);//for integral image, first rows and first cols are zero
		float *pHist = hist.ptr<float>(iy + 1); //for integral image, first rows and first cols are zero
		std::vector<float>histSum(_nbins);
		for (int i = 0; i < _nbins; i++) histSum[i] = 0.f;
		for (int ix = 0; ix < cols; ix++)
		{
			float bin, weight0, weight1, magnitude0, magnitude1, angle;
			angle = pPhase[ix] >= _fullAng ? pPhase[ix] - _fullAng : pPhase[ix];
			int bin0, bin1;
			bin = angle / _anglePerBin;

			bin0 = (int)floorf(bin);
			bin1 = (bin0 + 1) % _nbins;

			//CV_Assert(bin0<_nbins && bin0>=0 && bin1<_nbins && bin1>=0);

			//split the magnitude into two adjacent bins
			weight1 = (bin - bin0);
			weight0 = 1 - weight1;
			magnitude0 = pMag[ix] * weight0;
			magnitude1 = pMag[ix] * weight1;
			histSum[bin0] += magnitude0;
			histSum[bin1] += magnitude1;
			for (int n = 0; n < _nbins; n++)
			{
				pHist[(ix + 1)*_nbins + n] = pHist0[(ix + 1)*_nbins + n] + histSum[n];
			}
		}
	}
	//std::cout<<"done iH. cols ="<<cols<<" rows ="<<rows<<"\n";
}


void IntegralVideo::getDesc(const std::vector<cv::Mat>& iv, const cv::Point3i& tlp, const cv::Point3i& whl, std::vector<float>& dst, bool normByArea) const
{
	for (int i = 0; i < _nbins; i++) dst[i] = 0.f;
	int x = tlp.x*_nbins, y = tlp.y, t = tlp.z;
	int w = whl.x*_nbins, h = whl.y, l = whl.z;
	float area;
	if (normByArea)
		area = (whl.x*h*l) / 100.f; //divided by 100 to compensate for float accuracy (in case of the value is too small)
	float t0, t1;
	for (int i = 0; i < _nbins; i++)
	{
		t1 = iv[t + l].at<float>(y + h, x + w + i) - iv[t + l].at<float>(y + h, x + i) - \
			iv[t + l].at<float>(y, x + w + i) + iv[t + l].at<float>(y, x + i);
		t0 = iv[t].at<float>(y + h, x + w + i) - iv[t].at<float>(y + h, x + i) - \
			iv[t].at<float>(y, x + w + i) + iv[t].at<float>(y, x + i);

		if (normByArea)
			dst[i] = (t1 - t0) / area;
		else
			dst[i] = t1 - t0;
	}
}


#ifdef normThreshold
#undef normThreshold
#endif


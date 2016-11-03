/********************************************************************************

Filename     : integralVideo.cpp

Description  : check file "integralVideo.h"

Author       : FengShi@Discover lab, Apr, 2010
Version No   : 1.00 


*********************************************************************************/

#include "integralVideo.h"
#include "waitKeySeconds.h"
#include "formatBinaryStream.h"

#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#define normThreshold  0.05
//#define _NORM_GRAD_IM_  //If defined, normalize the computed gradients with max (abs) gradient value
const float _gaussSd = 2;

void dominantColor(const cv::Mat& colorSrc, cv::Mat& grayDst)
{
	std::vector<cv::Mat> gray;
	split(colorSrc, gray);
	grayDst = gray[0];
	for(int i = 1; i < gray.size(); i++)
		grayDst = cv::max(grayDst, gray[i]);
}

void computeMaxDxDy(const cv::Mat& src, cv::Mat& dx, cv::Mat& dy, bool findMax = 1)
{
	cv::Mat dx0, dy0;
	cv::Sobel(src, dx0, CV_32F, 1, 0, 1, 1, 0, IPL_BORDER_REPLICATE);
	cv::Sobel(src, dy0, CV_32F, 0, 1, 1, 1, 0, IPL_BORDER_REPLICATE); //compute dy with [-1, 0 ,1] kernel, and save into derv[1]
	if(findMax && src.channels() >= 3)
	{
		dominantColor(dx0, dx);
		dominantColor(dy0, dy);
	}
	else 
	{
		dx = dx0;
		dy = dy0;
	}
}

void computeMaxColorDxDy(const cv::Mat& src, cv::Mat& dx, cv::Mat& dy, bool findMax = 1)
{
	cv::Mat  grey;
	if(findMax && src.channels() >= 3)
		dominantColor(src, grey);
	else 
		grey = src;
	
	Sobel(grey, dx, CV_32F, 1, 0, 1, 1, 0, IPL_BORDER_REPLICATE);
	Sobel(grey, dy, CV_32F, 0, 1, 1, 1, 0, IPL_BORDER_REPLICATE); //compute dy with [-1, 0 ,1] kernel, and save into derv[1]
	
}

inline void computeMaxDt_xy(const cv::Mat& der1, const cv::Mat& der2, cv::Mat& dt, bool findMax = 1)
{
	cv::Mat dt0;
	subtract(der2, der1, dt0, cv::noArray(), CV_32F);

	if(findMax)
		dominantColor(dt0, dt);
	else
		dt = dt0;
}

IntegralVideo::IntegralVideo(const std::shared_ptr<sequence::IStreamedVideo>& video, int bins, bool fullOri, cv::Point3f rt2ps,
							 float reSzRatio, bool chnl2, int stFrame, int endFrame, int dscTp):_nbins(bins), _fullOri(fullOri), _2chnls(chnl2)
{ 
	if (dscTp)
		_descTp = _GBH;
	else
		_descTp = _HOG;
	
	_fullAng = _fullOri ? 360:180;
	_anglePerBin = (float)_fullAng/_nbins;
	_hasIv = 0;
	#ifdef INTEGRAL_VIDEO_DEBUG
		if (!filed.is_open())
			filed.open("IntegralVideoError.txt",std::ios::out);
	#endif
	computeIntegVideo(video, rt2ps, reSzRatio,  stFrame, endFrame);
}


bool IntegralVideo::computeIntegVideo(const std::shared_ptr<sequence::IStreamedVideo>& video, cv::Point3f rt2ps, float reSzRatio,
									  int stFrame, int endFrame)
{ 
	int imT = video->getNumFrames();
	if (imT <= 0)
	{
		_hasIv = 0;
		return false;
	}

	if (endFrame <= 0 || endFrame >imT)
		endFrame = imT;
	if (stFrame < 0)
		stFrame = 0;
	if (stFrame >= endFrame)
	{
		std::cerr<<"Class: IntegralVideo:: imT = getNumFrames() result is "<<imT<<"\n";
		_hasIv = 0;
		return false;
	}

	_ivPs[0].clear();_ivPs[1].clear();
	_ivRt[0].clear();_ivRt[1].clear();

	//skip frames for computing root iv. 
	int skipIm;   
	if (abs(rt2ps.z - 0.5) < 10E-7)  //if rt2ps.z == 0.5, choose every odd input frames(frames 1, 3, 5..) for computing root integral video
		skipIm = 2;
	else if (abs(rt2ps.z - 1./3.) < 10E-7) //if rt2ps.z == 1/3, skip 2 out of 3 frames (choosing frames 1, 4, 7..) for computing root integral video
		skipIm = 3;
	else 
		skipIm = 1;  //no skip, choosing all input frames to compute root iv 

	cv::Mat  im, imPs1, imPs2, derXbuf[3], derYbuf[3], derx, oflow, dery;
	std::vector<cv::Mat> oFlows(2), rootOFs(2), oFlows0(2);
	
	video->setPosition(stFrame);
	video>>im;

	imPs1 = im;

	cv::Size partSz;

	if(_descTp == _GBH)
	{
		GaussianBlur(imPs1, imPs1, cv::Size(9, 9), _gaussSd);
		//cout<<"gaussian blur\n";
	}
	if (abs(reSzRatio - 0.5) < 10E-5)
	{
		partSz.height = im.rows/2;
		partSz.width = im.cols/2;
		pyrDown(imPs1, imPs1, partSz);
	}
	else if (abs(reSzRatio - 1) < 10E-5)
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
	if(abs(rt2ps.x - 0.5) < 10E-5 && abs(rt2ps.y - 0.5) < 10E-5)
	{
		rootSz.height = partSz.height/2;
		rootSz.width = partSz.width/2;
	}
	else
	{
		rootSz.height = cvRound(rt2ps.y*partSz.height);
		rootSz.width = cvRound(rt2ps.x*partSz.width);
	}
		
	_ivWps = partSz.width + 1;  //integral video size [im.Width+1, im.Height+1, T]
	_ivHps = partSz.height + 1;
	//_ivBps = 1;

	_ivWrt = rootSz.width + 1;  //integral video size [im.Width+1, im.Height+1, T]
	_ivHrt = rootSz.height + 1;
	
	int ch = _2chnls ? 2:1;
	for(int i = 0; i < ch; i++)  //i=0 for u and i=1 for v
	{
		_ivPs[i].push_back(cv::Mat(_ivHps, _ivWps*_nbins, CV_32FC1, cv::Scalar(0))); //first line of integral video
		_ivRt[i].push_back(cv::Mat(_ivHrt, _ivWrt*_nbins, CV_32FC1, cv::Scalar(0))); //first line of integral video
	}
	_ivBps = 0;
	_ivBrt = 0;

	//computeMaxDxDy(imPs1, derXbuf[0], derYbuf[0], 1);
	computeMaxColorDxDy(imPs1, derXbuf[0], derYbuf[0], 1);
	
	video>>im;	
	while(!(im.empty()) && _ivBps <= MAX_VIDEO_BUFFER && _ivBps < endFrame - stFrame)
	{
		//processing image with grey gradient
		imPs2 = im;
		if(_descTp == _GBH)
			GaussianBlur(imPs2, imPs2, cv::Size(9, 9), _gaussSd);

		if (abs(reSzRatio - 0.5) < 10E-5)
			pyrDown(imPs2, imPs2, partSz);
		else if (abs(reSzRatio - 1) > 10E-5)
			resize(imPs2, imPs2, partSz, 0, 0, CV_INTER_AREA);	
	
		//computeMaxDxDy(imPs2, derXbuf[2], derYbuf[2], 1);
		computeMaxColorDxDy(imPs2, derXbuf[2], derYbuf[2], 1);

		cv::Mat tmpPs, tmp0;

		if(_2chnls)
		{
			//for HOG
			integralHist(derXbuf[0], derYbuf[0], tmpPs);
			tmp0 = cv::Mat(_ivHps, _ivWps*_nbins, CV_32FC1);
			add(tmpPs, _ivPs[0][_ivBps], tmp0);
			_ivPs[0].push_back(tmp0);
			//for GBH
			subtract(derXbuf[2], derXbuf[0], oFlows[0], cv::noArray(), CV_32F);
			subtract(derYbuf[2], derYbuf[0], oFlows[1], cv::noArray(), CV_32F);
			//computeMaxDt_xy(derXbuf[0], derXbuf[2], oFlows[0], 1);
			//computeMaxDt_xy(derYbuf[0], derYbuf[2], oFlows[1], 1);
			//oFlows[0] *= 100;
			//oFlows[1] *= 100;
#ifdef _NORM_GRAD_IM_
			double max0, min0;
			float max1;
			minMaxLoc(oFlows[0], &min0, &max0);
			max1 = max0 > abs(min0) ? max0 : abs(min0);
			if(max1>1e-7)
				oFlows[0] /= max1;

			minMaxLoc(oFlows[1], &min0, &max0);
			max1 = max0 > abs(min0) ? max0 : abs(min0);
			if(max1>1e-7)
				oFlows[1] /= max1;
#endif
		
			integralHist(oFlows[0], oFlows[1], tmpPs);
			tmp0 = cv::Mat(_ivHps, _ivWps*_nbins, CV_32FC1);
			add(tmpPs, _ivPs[1][_ivBps], tmp0);
			_ivPs[1].push_back(tmp0);
		}
		else if( _descTp == _GBH)
		{
			//for GBH
			subtract(derXbuf[2], derXbuf[0], oFlows[0], cv::noArray(), CV_32F);
			subtract(derYbuf[2], derYbuf[0], oFlows[1], cv::noArray(), CV_32F);

			//computeMaxDt_xy(derXbuf[0], derXbuf[2], oFlows[0], 1);
			//computeMaxDt_xy(derYbuf[0], derYbuf[2], oFlows[1], 1);
			//oFlows0[0] *= 100;
			//oFlows0[1] *= 100;

#ifdef _NORM_GRAD_IM_
			double max0, min0;
			float max1;
			minMaxLoc(oFlows[0], &min0, &max0);
			max1 = max0 > abs(min0) ? max0 : abs(min0);
			if(max1>1e-7)
				oFlows[0] /= max1;

			minMaxLoc(oFlows[1], &min0, &max0);
			max1 = max0 > abs(min0) ? max0 : abs(min0);
			if(max1>1e-7)
				oFlows[1] /= max1;
#endif			
			integralHist(oFlows[0], oFlows[1], tmpPs);
			tmp0 = cv::Mat(_ivHps, _ivWps*_nbins, CV_32FC1);
			add(tmpPs, _ivPs[0][_ivBps], tmp0);
			_ivPs[0].push_back(tmp0);
		}
		else
		{
			//for hog
			integralHist(derXbuf[0], derYbuf[0], tmpPs);
			tmp0 = cv::Mat(_ivHps, _ivWps*_nbins, CV_32FC1);
			add(tmpPs, _ivPs[0][_ivBps], tmp0);
			_ivPs[0].push_back(tmp0);
		}
		_ivBps++;

		//if no skip current frame, compute root iv
		if (!(_ivBps % skipIm))
		{
			cv::Mat imRt, tmpRt, tmp1;
			if(abs(rt2ps.x - 0.5) < 10E-5 && abs(rt2ps.y - 0.5) < 10E-5)
			{
				if(_2chnls)
				{
					pyrDown(oFlows[0], rootOFs[0], rootSz);
					pyrDown(oFlows[1], rootOFs[1], rootSz);
					pyrDown(derXbuf[0], derx, rootSz);
					pyrDown(derYbuf[0], dery, rootSz);
				}
				else if( _descTp == _GBH)
				{
					pyrDown(oFlows[0], rootOFs[0], rootSz);
					pyrDown(oFlows[1], rootOFs[1], rootSz);
					
				}
				else
				{
				
					pyrDown(derXbuf[0], derx, rootSz);
					pyrDown(derYbuf[0], dery, rootSz);
				}
			}
			else
			{
				if(_2chnls)
				{
					resize(oFlows[0], rootOFs[0], rootSz, 0, 0, CV_INTER_AREA);  //resize part image 1 into root image 1 with root to part ratio
					resize(oFlows[1], rootOFs[1], rootSz, 0, 0, CV_INTER_AREA); 
					resize(derXbuf[0], derx, rootSz, 0, 0, CV_INTER_AREA);
					resize(derYbuf[0], dery, rootSz, 0, 0, CV_INTER_AREA);
				}
				else if( _descTp == _GBH)
				{				
					resize(oFlows[0], rootOFs[0], rootSz, 0, 0, CV_INTER_AREA);  //resize part image 1 into root image 1 with root to part ratio
					resize(oFlows[1], rootOFs[1], rootSz, 0, 0, CV_INTER_AREA); 
				}
				else
				{
					resize(derXbuf[0], derx, rootSz, 0, 0, CV_INTER_AREA);
					resize(derYbuf[0], dery, rootSz, 0, 0, CV_INTER_AREA);
				}
			}
			
			if(_2chnls)
			{
				//for hog
				integralHist(derx, dery, tmpRt);
				//checkRange(imRt,0);checkRange(tmpRt,0);
				tmp1 = cv::Mat(_ivHrt, _ivWrt*_nbins, CV_32FC1);
				//tmp1 = Mat(_ivHrt,_ivWrt*_nbins,DataType<float>::type);
				add(tmpRt, _ivRt[0][_ivBrt], tmp1);
				_ivRt[0].push_back(tmp1);
				//for GBH
				integralHist(rootOFs[0], rootOFs[1], tmpRt);
				//checkRange(imRt,0);checkRange(tmpRt,0);
				tmp1 = cv::Mat(_ivHrt, _ivWrt*_nbins, CV_32FC1);
				//tmp1 = Mat(_ivHrt,_ivWrt*_nbins,DataType<float>::type);
				add(tmpRt, _ivRt[1][_ivBrt], tmp1);
				_ivRt[1].push_back(tmp1);
			}
			else if( _descTp == _GBH)
			{
				integralHist(rootOFs[0], rootOFs[1], tmpRt);
				//checkRange(imRt,0);checkRange(tmpRt,0);
				tmp1 = cv::Mat(_ivHrt, _ivWrt*_nbins, CV_32FC1);
				//tmp1 = Mat(_ivHrt,_ivWrt*_nbins,DataType<float>::type);
				add(tmpRt, _ivRt[0][_ivBrt], tmp1);
				_ivRt[0].push_back(tmp1);
			}
			else
			{
				//for hog
				integralHist(derx, dery, tmpRt);
				//checkRange(imRt,0);checkRange(tmpRt,0);
				tmp1 = cv::Mat(_ivHrt, _ivWrt*_nbins, CV_32FC1);
				//tmp1 = Mat(_ivHrt,_ivWrt*_nbins,DataType<float>::type);
				add(tmpRt, _ivRt[0][_ivBrt], tmp1);
				_ivRt[0].push_back(tmp1);
			}
			
			_ivBrt++;
		}

		derXbuf[0] = derXbuf[2];	
		derXbuf[2] = cv::Mat();  //derXbuf[2] must point to other head. otherwise, if derXbuf[2] is changed somewhere else, the derXbuf[1] will also be changed.			
		
		derYbuf[0] = derYbuf[2];
		derYbuf[2] = cv::Mat();

		video>>im; //imPs2 changed if im changed.
	}
	_ivBps = (int)_ivPs[0].size();
	_ivBrt = (int)_ivRt[0].size();
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

	int iCols = (cols+1)*_nbins;
	hist = cv::Mat(rows + 1, iCols, CV_32FC1, cv::Scalar(0.));

	for(int iy = 0; iy < rows; iy++)
	{
		const float *pMag = magnitude.ptr<float>(iy);
		const float *pPhase = phase.ptr<float>(iy);
		const float *pHist0 = hist.ptr<float>(iy);//for integral image, first rows and first cols are zero
		float *pHist = hist.ptr<float>(iy+1); //for integral image, first rows and first cols are zero
		std::vector<float>histSum(_nbins);
		for(int i = 0; i < _nbins; i++) histSum[i]=0.f;
		for(int ix = 0; ix < cols; ix++)
		{
			float bin, weight0, weight1, magnitude0, magnitude1, angle;
			angle = pPhase[ix]>=_fullAng ? pPhase[ix]-_fullAng : pPhase[ix]; 
			int bin0, bin1;
			bin = angle/_anglePerBin;

			bin0 = (int)floorf(bin);
			bin1 = (bin0+1)%_nbins;
			if(bin0>=_nbins && bin0<0 && bin1>=_nbins && bin1<0)
			{
				std::cerr << pPhase[ix] << " " << pMag[ix] << " " << angle << " " << bin << " " << bin0 << " " << bin1 << " " << _nbins << " " << _fullOri << std::endl;
				discoverUO::wait(5);
			}
//CV_Assert(bin0<_nbins && bin0>=0 && bin1<_nbins && bin1>=0);

	//split the magnitude into two adjacent bins
			weight1 = (bin - bin0);
			weight0 = 1 - weight1;
			magnitude0 = pMag[ix]*weight0;
			magnitude1 = pMag[ix]*weight1;
			histSum[bin0] += magnitude0;
			histSum[bin1] += magnitude1;
			for(int n = 0; n < _nbins; n++)
			{
				pHist[(ix+1)*_nbins+n] = pHist0[(ix+1)*_nbins+n] + histSum[n];
			}
		}
	}
	//std::cout<<"done iH. cols ="<<cols<<" rows ="<<rows<<"\n";
}

void IntegralVideo::integralHist(const cv::Mat& dx, const cv::Mat& dy, cv::Mat& hist) const
{
	cv::Mat  magnitude, phase;

	//magnitude(dx, dy, magnitude);
	//phase(dx, dy, phase, true);
	cartToPolar(dx, dy, magnitude, phase, true);

	int cols = dx.cols;
	int rows = dx.rows;

	int iCols = (cols+1)*_nbins;
	hist = cv::Mat(rows + 1, iCols, CV_32FC1, cv::Scalar(0.));

	for(int iy = 0; iy < rows; iy++)
	{
		const float *pMag = magnitude.ptr<float>(iy);
		const float *pPhase = phase.ptr<float>(iy);
		const float *pHist0 = hist.ptr<float>(iy);//for integral image, first rows and first cols are zero
		float *pHist = hist.ptr<float>(iy+1); //for integral image, first rows and first cols are zero
		std::vector<float>histSum(_nbins);
		for(int i = 0; i < _nbins; i++) histSum[i]=0.f;
		for(int ix = 0; ix < cols; ix++)
		{
			float bin, weight0, weight1, magnitude0, magnitude1, angle;
			angle = pPhase[ix]>=_fullAng ? pPhase[ix]-_fullAng : pPhase[ix]; 
			int bin0, bin1;
			bin = angle/_anglePerBin;

			bin0 = (int)floorf(bin);
			bin1 = (bin0+1)%_nbins;
			/*
			if(bin0>=_nbins && bin0<0 && bin1>=_nbins && bin1<0)
			{
				cout<<pPhase[ix]<<" "<<pMag[ix]<<" "<<angle<<" "<<bin<<" "<<bin0<<" "<<bin1<<" "<<_nbins<<" "<<_fullOri<<endl;
				discoverUO::wait();
			}
			*/
//CV_Assert(bin0<_nbins && bin0>=0 && bin1<_nbins && bin1>=0);

	//split the magnitude into two adjacent bins
			weight1 = (bin - bin0);
			weight0 = 1 - weight1;
			magnitude0 = pMag[ix]*weight0;
			magnitude1 = pMag[ix]*weight1;
			histSum[bin0] += magnitude0;
			histSum[bin1] += magnitude1;
			for(int n = 0; n < _nbins; n++)
			{
				pHist[(ix+1)*_nbins+n] = pHist0[(ix+1)*_nbins+n] + histSum[n];
			}
		}
	}
	//std::cout<<"done iH. cols ="<<cols<<" rows ="<<rows<<"\n";
}


void  IntegralVideo::getDesc(const std::vector<cv::Mat>& iv, const cv::Point3i& tlp, const cv::Point3i& whl, std::vector<float>& dst, bool normByArea) const
{
	//for(int i = 0; i < _nbins; i++) desc[i] = 0.f;
	int x = tlp.x*_nbins, y = tlp.y, t = tlp.z;
	int w = whl.x*_nbins, h = whl.y, l = whl.z;
	float area;
	if(normByArea)
		area = (float)((whl.x*h*l)/100.); //divided by 100. to compensate for float accuracy (in case of the value is too small)
	float t0, t1;

	const float *p0[2], *p1[2];
	p1[1] = iv[t+l].ptr<float>(y+h)+x;
	p1[0] = iv[t+l].ptr<float>(y)+x;
	p0[1] = iv[t].ptr<float>(y+h)+x;
	p0[0] = iv[t].ptr<float>(y)+x;
	for (int i = 0; i < _nbins; i++)
	{
		t1 = p1[1][w+i] - p1[1][i] - p1[0][w+i] + p1[0][i];
		t0 = p0[1][w+i] - p0[1][i] - p0[0][w+i] + p0[0][i];

		if(normByArea)
			dst[i] = (t1-t0)/area;
		else
			dst[i] = t1-t0;
	}
	/*
	//non-efficient access:
	for (int i = 0; i < _nbins; i++)
	{
		t1 = iv[t+l].at<float>(y+h, x+w+i) - iv[t+l].at<float>(y+h, x+i) - \
			 iv[t+l].at<float>(y, x+w+i)  + iv[t+l].at<float>(y, x+i);
		t0 = iv[t].at<float>(y+h, x+w+i) - iv[t].at<float>(y+h, x+i) - \
			 iv[t].at<float>(y, x+w+i) + iv[t].at<float>(y, x+i);

		if(normByArea)
			dst[i] = (t1-t0)/area;
		else
			dst[i] = t1-t0;
	}
	*/
}


#ifdef normThreshold
	#undef normThreshold
#endif


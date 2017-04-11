#pragma once
#include <string>
#include <list>
#include <vector>
#include <sequence/IStreamedVideo.h>
#include <sequence/Video.h>
#include <sequence/BackgroundSeparation.h>

typedef std::shared_ptr<sequence::IStreamedVideo> VideoPtr;
typedef std::list<std::shared_ptr<sequence::IStreamedVideo>> VideoList;
typedef std::vector<cv::Mat> Frames;
typedef std::vector<sequence::FBSeparator::Rectangles> RectVect; 
RectVect fb_separation_timing(const Frames& frames);
VideoList load_files(const std::string& path);
Frames convert_videos(const VideoList& vl);
void print_frames_time(const std::vector<double>& frame_time);

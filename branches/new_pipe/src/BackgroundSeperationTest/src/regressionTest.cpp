#include "regressionTest.h"
#include <sequence/BackgroundSeparation.h>
#include <iostream>
#include <utils/Filesystem.h>
#include <sequence/IVideo.h>
#include <sequence/DefaultPacker.h>
#include <sequence/GpuPacker.h>

void regressionTests(const std::string& dirPath)
{
	std::cout << "Regression tests..." << std::endl;
    auto vsqs = utils::Filesystem::getFileList(dirPath, ".vsq");

    //std::map<std::string,
    for (auto& vfile : vsqs) {
        std::shared_ptr<sequence::IVideo> seq = sequence::IVideo::create(vfile);
        GpuPacker gp();

        for (size_t i = 0; i < seq->getNumFrames(); i++) {
            cv::Mat frame = seq->getFrameImage(i);


            //m_FramesQueue->push(std::move(frame));

            //m_Packer->compressFrame(i, frame, seq->getFrameTime(i));
        }


    }

}
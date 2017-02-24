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


void dropCSVInfo(std::ostream& out, const std::string& dirPath)
{
    out << "CSV info..." << std::endl;
    auto csvs = utils::Filesystem::getFileList(dirPath, ".cvs");

    //std::map<std::string,
    for (auto& file : csvs) {
        std::shared_ptr<sequence::IVideo> seq = sequence::IVideo::create(file);
        out << "Processing: " << file << std::endl;
        auto count = seq->getNumFrames();
        out << "\tNumber of frames: " << count << std::endl;
        auto frame1 = seq->getFrameImage(0);
        out << "\tFrame size: " << frame1.size() << std::endl;
        for (int i = 0; i < count; ++i) {
            auto frame = seq->getFrameImage(i);
            out << "\t\tFrame: " << i << std::endl;
            auto crumbles = seq->getFrameCrumbles(i);
            auto time = seq->getFrameTime(i);
            out << "\t\tFrame time: " << time << std::endl;
            out << "\t\tNumber of crumbles: " << crumbles.size() << std::endl;
            for (auto it = crumbles.begin(); it != crumbles.end(); ++it) {
                out << "\t\t\t" << "C " << it->br() << "," << it->tl() << std::endl;
            }
        }
    }
}
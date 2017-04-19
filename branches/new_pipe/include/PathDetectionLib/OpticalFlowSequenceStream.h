/* #pragma once
#ifndef OpticalFlowStream_h__
#define OpticalFlowStream_h__

#include "ISequenceDetector.h"
#include "OpticalFlowPathDetector.h"

#include <opencv2/core.hpp>

#include <config/PathDetection.h>
#include <utils/CvZoom.h>

#include <mutex>

#include <opencv2/cudaoptflow.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>

namespace clustering
{
    class OpticalFlowSequenceStream //: public ISequenceDetector
    {
    public:
        OpticalFlowSequenceStream(bool visualize);
        ~OpticalFlowSequenceDetector();

        static void registerParameters(ProgramOptions& programOptions);
        virtual bool loadParameters(const ProgramOptions& options) override;
        virtual void process(const std::string& inputSequence, const std::string& outputPaths) override;
        virtual void show() override;

    private:
        config::PathDetection& getConfig();

        bool m_Visualize;
        std::mutex m_VisualizeMutex;
        cv::Mat m_VisualizationFrame;
        utils::ZoomObjectCollection m_ZoomObjects;

        cv::Mat m_VideoFrame;

        void visualize();

        OpticalFlowPathDetector m_PathDetector;
    };
}

#endif // OpticalFlowDetector_h__
*/
#ifndef pathFinder_h__
#define pathFinder_h__

#include <vector>
#include <map>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <boost/shared_ptr.hpp>
#include <memory>
#include <map>

namespace utils {
    namespace pathFinder
    {
        enum _PF_STRATEGY_PICK_RESULT
        {
            _PFPICK_OK,
            _PFPICK_ALREADY_REGISTERED,
            _PFPICK_STRATEGY_NOT_FOUND,
            _PFPICK_ALREADY_ACTIVE,
            _PFPICK_ALREADY_PAUSED,
            _PFPICK_UNKNOWN_ERROR
        };

        enum _PF_MATRIX_ORIENTATION
        {
            _PFM_PATH_IN_ROWS,
            _PFM_PATH_IN_COL
        };

        enum _PF_STRATEGY_USE_RESULT
        {
            _PFUSE_OK,
            _PFUSE_FALSE,
            _PFUSE_EXCEPTION_THROWN,
            _PFUSE_NOT_FOUND,
            _PFUSE_NO_STRATEGY_PICKED
        };

        class ITrackSetUpdaterStrategy
        {
            public:
            virtual bool update( const cv::Mat& image )=0;
            virtual ~ITrackSetUpdaterStrategy() {};
            virtual std::string id()=0;
            virtual cv::Mat getPathMatrix( bool activePathsOnly, _PF_MATRIX_ORIENTATION orient )=0;
            virtual std::vector<std::pair<std::vector<cv::Point2f>, bool>> getPathVector( bool activePathOnly )=0;
            protected:
            /*	\brief used to inform if concrete object is ready to use - all parameters are set up.
            */
            virtual bool ready()=0;
        };

        class CTrackSetContext
        {
            public:
            CTrackSetContext() {};
            CTrackSetContext( const std::string& id );

            _PF_STRATEGY_PICK_RESULT useStrategy( const std::string& id );
            _PF_STRATEGY_PICK_RESULT stopStrategy( const std::string& id );
            _PF_STRATEGY_PICK_RESULT pauseStrategy( const std::string& id );
            _PF_STRATEGY_PICK_RESULT activateStrategy( const std::string& id );
            _PF_STRATEGY_USE_RESULT require( const cv::Mat& image );
            _PF_STRATEGY_USE_RESULT lastStepResult( const std::string& id );

            private:

            std::map<std::string, _PF_STRATEGY_USE_RESULT> m_errors;
            std::vector<std::pair<std::shared_ptr<ITrackSetUpdaterStrategy>, bool>> m_strategies;

        };

        class CTSFactory
        {
            public:
            static CTSFactory* getInstance();
            _PF_STRATEGY_PICK_RESULT registerStrategy( const std::shared_ptr<ITrackSetUpdaterStrategy> &prototype );
            std::shared_ptr<ITrackSetUpdaterStrategy> getStrategy( const std::string& id );
            private:
            static CTSFactory* m_instance;
            std::vector <std::shared_ptr<ITrackSetUpdaterStrategy>> m_updaters;
            CTSFactory& operator= ( const CTSFactory& cpy ) {};
            CTSFactory( const CTSFactory& cpy ) {};
            CTSFactory() {};

        };


    }//namespace pathFinder.
}//namespace utils

#endif // pathFinder_h__

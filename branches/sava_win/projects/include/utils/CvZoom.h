/********************************************************************
	created:	2012/09/20
	created:	20:9:2012   21:40
	filename: 	cvUtils.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__CVUTILS_H__
#define HEADER_GUARD_UTILS__CVUTILS_H__

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QtCore/QObject>
#include "CvZoomObjects.h"

namespace utils {

    //! class stores information about current zoom
    class ZoomInfo
    {
        public:
        //! simple constructor
        ZoomInfo();
        //! Constructor
        //! \param originSize size of origin image
        //! \param center center of zoomed part
        //! \param ratio zoom ratio
        ZoomInfo( const cv::Size& originSize, const cv::Point& center, float ratio );

        public:
        //! \return center of zoomed part (in global coordinates)
        cv::Point getCenter() const { return center; }
        //! sets center of zoomed part
        //! \param val center in global coordinates
        void setCenter( cv::Point val );
        //! \return size (width and height) of zoomed region
        cv::Size getSize() const { return size; }
        //! sets size of zoomed region
        //! \param val
        void setSize( cv::Size val );
        //! \return size of origin image
        cv::Size getOrigin() const { return origin; }
        //! sets size of origin image
        //! \param val
        void setOrigin( cv::Size val )
        {
            origin = val;
        }
        //! \return zoom ratio (0 - 1)
        float getRatio() const { return ratio; }
        //! sets zoom ratio
        //! \param val new ratio (0 - 1)
        void setRatio( float val );
        //! \return global coordinates of upper left corner of zoomed image
        cv::Point getShift() const;
        IZoomTransformConstPtr getTransform() const;

        private:
        //! prevens zoomed image to violate boundaries of origin (shifts center)
        void adjust();

        private:
        //! center of zoomed part
        cv::Point center;
        //! size of zoomed region
        cv::Size size;
        //! size of origin image
        cv::Size origin;
        //! zoom ratio (0 - 1)
        float ratio;
        IZoomTransformPtr transform;
    };
    typedef std::shared_ptr<ZoomInfo> ZoomInfoPtr;
    typedef std::shared_ptr<const ZoomInfo> ZoomInfoConstPtr;

    //! Creates zoomed window
    //! \param name name of the window that may be used as a window identifier
    //! \param flags flags of the window
    void zoomedWindow( const std::string& name, int flags = cv::WINDOW_KEEPRATIO );
    //! displays the image in the zoomed window
    //! \param name window identifier
    //! \param mat image to show
    void zoomShow( const std::string& name, cv::InputArray mat, const ZoomObjectCollection& zc = ZoomObjectCollection() );

    //! destroys zoomed window
    //! \param name window identifier
    void zoomDestroyWindow( const std::string& name );
    void zoomDestroyAllWindows();
    //! assigns callback for mouse events
    void zoomMouseCallback( const std::string& name, cv::MouseCallback onMouse, void* userdata = 0 );
    //! \param name window identifier
    //! \return zoom parameters
    ZoomInfoConstPtr getZoomInfo( const std::string& name );
    //! sets zoom parameters
    //! \param name window identifier
    //! \param zoom initialized zoom parameters
    void setZoomInfo( const std::string& name, ZoomInfoPtr zoom );
}

class CvZoomEventFilter : public QObject
{
    Q_OBJECT
public:
    virtual bool eventFilter( QObject *, QEvent * );
};


#endif

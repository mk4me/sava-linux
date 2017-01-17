/********************************************************************
	created:	2012/09/27
	created:	27:9:2012   12:07
	filename: 	CvZoomObjects.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__CVZOOMOBJECTS_H__
#define HEADER_GUARD_UTILS__CVZOOMOBJECTS_H__


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <memory>

namespace utils {

    class IZoomTransform
    {
        public:
        virtual ~IZoomTransform() {}
        public:
        virtual cv::Point globalToZoomed( const cv::Point& global ) const = 0;
        virtual cv::Point zoomedToGlobal( const cv::Point& zoomed ) const = 0;
        virtual float getRatio()  const= 0;
        float getRatioInv() const { return 1.0f / getRatio();}
    };
    typedef std::shared_ptr<IZoomTransform> IZoomTransformPtr;
    typedef std::shared_ptr<const IZoomTransform> IZoomTransformConstPtr;

    class IZoomObject
    {
        public:
        virtual ~IZoomObject() {}
        virtual void draw( cv::Mat& image, IZoomTransformConstPtr transform ) = 0;
    };
    typedef std::shared_ptr<IZoomObject> IZoomObjectPtr;
    typedef std::shared_ptr<const IZoomObject> IZoomObjectConstPtr;
    typedef std::vector<IZoomObjectPtr> ZoomObjectCollection;

    //! draws the line segment (pt1, pt2) in the image
    void zoomLine( ZoomObjectCollection& collection, cv::Point pt1, cv::Point pt2, const cv::Scalar& color,
                   int thickness=1, int lineType=8, int shift=0 );

    //! draws the rectangle outline or a solid rectangle with the opposite corners pt1 and pt2 in the image
    void zoomRectangle( ZoomObjectCollection& collection, cv::Point pt1, cv::Point pt2,
                        const cv::Scalar& color, int thickness=1,
                        int lineType=8, int shift=0 );

    //! draws the rectangle outline or a solid rectangle covering rec in the image
    void zoomRectangle( ZoomObjectCollection& collection, cv::Rect rec,
                        const cv::Scalar& color, int thickness=1,
                        int lineType=8, int shift=0 );

    //! draws the circle outline or a solid circle in the image
    void zoomCircle( ZoomObjectCollection& collection, cv::Point center, int radius,
                     const cv::Scalar& color, int thickness=1,
                     int lineType=8, int shift=0 );

    //! draws an elliptic arc, ellipse sector or a rotated ellipse in the image
    void zoomEllipse( ZoomObjectCollection& collection, cv::Point center, cv::Size axes,
                      double angle, double startAngle, double endAngle,
                      const cv::Scalar& color, int thickness=1,
                      int lineType=8, int shift=0 );

    //! draws a rotated ellipse in the image
    void zoomEllipse( ZoomObjectCollection& collection, const cv::RotatedRect& box, const cv::Scalar& color,
                      int thickness=1, int lineType=8 );

    //! draws a filled convex polygon in the image
    void zoomFillConvexPoly( ZoomObjectCollection& collection, const cv::Point* pts, int npts,
                             const cv::Scalar& color, int lineType=8, int shift=0 );
    void zoomFillConvexPoly( ZoomObjectCollection& collection, const std::vector<cv::Point>& points,
                             const cv::Scalar& color, int lineType=8, int shift=0 );

    //! fills an area bounded by one or more polygons
    void zoomFillPoly( ZoomObjectCollection& collection, const cv::Point** pts,
                       const int* npts, int ncontours,
                       const cv::Scalar& color, int lineType=8, int shift=0,
                       cv::Point offset=cv::Point() );

    void zoomFillPoly( ZoomObjectCollection& collection, const std::vector<std::vector<cv::Point>>& pts,
                       const cv::Scalar& color, int lineType=8, int shift=0,
                       cv::Point offset=cv::Point() );

    //! draws one or more polygonal curves
    void zoomPolylines( ZoomObjectCollection& collection, const cv::Point* const* pts, const int* npts,
                        int ncontours, bool isClosed, const cv::Scalar& color,
                        int thickness=1, int lineType=8, int shift=0 );

    void zoomPolylines( ZoomObjectCollection& collection, const std::vector<std::vector<cv::Point>>& pts,
                        bool isClosed, const cv::Scalar& color,
                        int thickness=1, int lineType=8, int shift=0 );

    void zoomPutText(ZoomObjectCollection& collection, const std::string& text, cv::Point org, int fontFace, 
        double fontScale, cv::Scalar color, int thickness=1, int lineType=8, bool bottomLeftOrigin=false);

    ////! draws contours in the image
    //void zoomDrawContours( ZoomObjectCollection& collection, cv::InputArrayOfArrays contours,
    //    int contourIdx, const cv::Scalar& color,
    //    int thickness=1, int lineType=8,
    //    cv::InputArray hierarchy=cv::noArray(),
    //    int maxLevel=INT_MAX, cv::Point offset=cv::Point() );

}


#endif

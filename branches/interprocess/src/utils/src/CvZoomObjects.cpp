#include "CvZoomObjects.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <boost/thread.hpp>
#include <map>
#include <string>

namespace utils {

    class BaseZoom : public IZoomObject
    {
        protected:
        BaseZoom( const cv::Scalar& color, int thickness, int lineType, int shift ) :
        color( color ),
        thickness( thickness ),
        lineType( lineType ),
        shift( shift )
        {}

        cv::Scalar color;
        int thickness;
        int lineType;
        int shift;
    };

    class CircleZoom : public BaseZoom
    {
        public:
        CircleZoom( const cv::Point& center, int radius, const cv::Scalar& color,
        int thickness=1, int lineType=8, int shift=0 ) :
        BaseZoom( color, thickness, lineType, shift ),
        center( center ),
        radius( radius )
        {

        }
        virtual void draw( cv::Mat& image, IZoomTransformConstPtr transform )
        {
            cv::circle( image, transform->globalToZoomed( center ), static_cast<int>( radius * transform->getRatioInv() ), color, thickness, lineType, shift );
        }
        private:
        cv::Point center;
        int radius;
    };

    class LineZoom : public BaseZoom
    {
        public:
        LineZoom( cv::Point pt1, cv::Point pt2, const cv::Scalar& color,
        int thickness=1, int lineType=8, int shift=0 ) :
        BaseZoom( color, thickness, lineType, shift ),
        pt1( pt1 ),
        pt2( pt2 )
        {
        }

        virtual void draw( cv::Mat& image, IZoomTransformConstPtr t )
        {
            cv::line( image, t->globalToZoomed( pt1 ), t->globalToZoomed( pt2 ), color, thickness, lineType, shift );
        }

        private:
        cv::Point pt1;
        cv::Point pt2;
    };

    class RectZoom : public BaseZoom
    {
        public:
        RectZoom( cv::Point pt1, cv::Point pt2, const cv::Scalar& color,
        int thickness=1, int lineType=8, int shift=0 ) :
        BaseZoom( color, thickness, lineType, shift ),
        pt1( pt1 ),
        pt2( pt2 )
        {
        }

        virtual void draw( cv::Mat& image, IZoomTransformConstPtr t )
        {
            cv::rectangle( image, t->globalToZoomed( pt1 ), t->globalToZoomed( pt2 ), color, thickness, lineType, shift );
        }

        private:
        cv::Point pt1;
        cv::Point pt2;
    };

    class EllipseZoom : public BaseZoom
    {
        public:
        EllipseZoom( cv::Point center, cv::Size axes, double angle, double startAngle, double endAngle,
        const cv::Scalar& color, int thickness=1, int lineType=8, int shift=0 ) :
        BaseZoom( color, thickness, lineType, shift ),
        center( center ),
        axes( axes ),
        angle( angle ),
        startAngle( startAngle ),
        endAngle( endAngle )
        {

        }

        virtual void draw( cv::Mat& image, IZoomTransformConstPtr t )
        {
            cv::Size a( static_cast<int>( axes.width * t->getRatioInv() ),
            static_cast<int>( axes.height * t->getRatioInv() ) );
            cv::ellipse( image, t->globalToZoomed( center ), a, angle, startAngle, endAngle, color, thickness, lineType, shift );
        }

        private:
        cv::Point center;
        cv::Size axes;
        double angle;
        double startAngle;
        double endAngle;
    };

    class FillConvexZoom : public BaseZoom
    {
        public:
        FillConvexZoom( const std::vector<cv::Point>& points,
        const cv::Scalar& color, int lineType=8, int shift=0 ) :
        BaseZoom( color, 1, lineType, shift ),
        points( points )
        {
        }

        virtual void draw( cv::Mat& image, IZoomTransformConstPtr t )
        {
            std::vector<cv::Point> v( points );

            for( auto it = v.begin(); it != v.end(); ++it ) {
                *it = t->globalToZoomed( *it );
            }

            cv::fillConvexPoly( image, v, color, lineType, shift );
        }
        private:
        std::vector<cv::Point> points;
    };

    class FillPolyZoom : public BaseZoom
    {
        public:
        FillPolyZoom( const std::vector<std::vector<cv::Point>>& points,
        const cv::Scalar& color, int lineType=8, int shift=0, cv::Point offset = cv::Point() ) :
        BaseZoom( color, 1, lineType, shift ),
        points( points ),
        offset( offset )
        {
        }

        virtual void draw( cv::Mat& image, IZoomTransformConstPtr t )
        {
            std::vector<std::vector<cv::Point>>  v( points );

            for( auto itX = v.begin(); itX != v.end(); ++itX ) {
                for( auto itY = itX->begin(); itY != itX->end(); ++itY ) {
                    *itY = t->globalToZoomed( *itY );
                }

            }

            cv::fillPoly( image, v, color, lineType, shift, offset );
        }
        private:
        std::vector<std::vector<cv::Point>> points;
        cv::Point offset;
    };

    class PutTextZoom : public BaseZoom
    {
    public:
        PutTextZoom(  const std::string& text, cv::Point org, int fontFace, 
            double fontScale, cv::Scalar color, int thickness=1, int lineType=8, bool bottomLeftOrigin=false)
            :BaseZoom( color, thickness, lineType, 0 ),
            text(text),
            org(org), 
            fontFace(fontFace), 
            fontScale(fontScale),
            bottomLeftOrigin(bottomLeftOrigin)
        {
        }

        virtual void draw( cv::Mat& image, IZoomTransformConstPtr transform )
        {
            cv::putText(image, text, transform->globalToZoomed(org), fontFace, fontScale, color, thickness, lineType, bottomLeftOrigin);
        }

    private:
        cv::Point org;
        int fontFace;
        double fontScale;
        std::string text;
        bool bottomLeftOrigin;
    };

    class PolylineZoom : public BaseZoom
    {
    public:
        PolylineZoom( const std::vector<std::vector<cv::Point>>& pts, bool isClosed, const cv::Scalar& color, int thickness/*=1*/, int lineType/*=8*/, int shift/*=0 */ )
        :BaseZoom( color, thickness, lineType, shift ),
        points( pts ),
        isClosed( isClosed )
        {
        }

        virtual void draw( cv::Mat& image, IZoomTransformConstPtr transform )
        {
            std::vector<std::vector<cv::Point>>  v( points );

            for( auto itX = v.begin(); itX != v.end(); ++itX ) {
                for( auto itY = itX->begin(); itY != itX->end(); ++itY ) {
                    *itY = transform->globalToZoomed( *itY );
                }

            }

            cv::polylines( image, v, isClosed, color, thickness, lineType, shift );
        }

        private:
        std::vector<std::vector<cv::Point>> points;
        bool isClosed;
    };

    void zoomLine( ZoomObjectCollection& collection, cv::Point pt1, cv::Point pt2, const cv::Scalar& color, int thickness/*=1*/, int lineType/*=8*/, int shift/*=0*/ )
    {
        collection.push_back( IZoomObjectPtr( new LineZoom( pt1, pt2, color, thickness, lineType, shift ) ) );
    }

    void zoomCircle( ZoomObjectCollection& collection, cv::Point center, int radius, const cv::Scalar& color, int thickness/*=1*/, int lineType/*=8*/, int shift/*=0*/ )
    {
        collection.push_back( IZoomObjectPtr( new CircleZoom( center, radius, color, thickness, lineType, shift ) ) );
    }

    void zoomRectangle( ZoomObjectCollection& collection, cv::Point pt1, cv::Point pt2, const cv::Scalar& color, int thickness/*=1*/, int lineType/*=8*/, int shift/*=0*/ )
    {
        collection.push_back( IZoomObjectPtr( new RectZoom( pt1, pt2, color, thickness, lineType, shift ) ) );
    }

    void zoomRectangle( ZoomObjectCollection& collection, cv::Rect rec, const cv::Scalar& color, int thickness/*=1*/, int lineType/*=8*/, int shift/*=0*/ )
    {
        cv::Point p1( rec.x, rec.y );
        cv::Point p2( rec.x + rec.width, rec.y + rec.height );
        collection.push_back( IZoomObjectPtr( new RectZoom( p1, p2, color, thickness, lineType, shift ) ) );
    }

    void zoomEllipse( ZoomObjectCollection& collection, cv::Point center, cv::Size axes,
                      double angle, double startAngle, double endAngle, const cv::Scalar& color, int thickness/*=1*/, int lineType/*=8*/, int shift/*=0*/ )
    {
        collection.push_back( IZoomObjectPtr( new EllipseZoom( center, axes, angle, startAngle,endAngle, color, thickness, lineType, shift ) ) );
    }

    void zoomEllipse( ZoomObjectCollection& collection, const cv::RotatedRect& box, const cv::Scalar& color, int thickness/*=1*/, int lineType/*=8*/ )
    {
        collection.push_back( IZoomObjectPtr(
                                  new EllipseZoom( box.center, box.size, box.angle, 0.0, 360.0, color, thickness, lineType ) )
                            );
    }

    void zoomDrawContours( ZoomObjectCollection& collection, cv::InputArrayOfArrays contours, int contourIdx, const cv::Scalar& color, int thickness/*=1*/, int lineType/*=8*/, cv::InputArray hierarchy/*=cv::noArray()*/, int maxLevel/*=INT_MAX*/, cv::Point offset/*=cv::Point() */ )
    {

    }

    void zoomFillConvexPoly( ZoomObjectCollection& collection, const cv::Point* pts, int npts, const cv::Scalar& color, int lineType/*=8*/, int shift/*=0*/ )
    {
        std::vector<cv::Point> points;

        for( int i = 0; i < npts; ++i ) {
            points.push_back( pts[i] );
        }

        zoomFillConvexPoly( collection, points, color, lineType, shift );
    }

    void zoomFillConvexPoly( ZoomObjectCollection& collection, const std::vector<cv::Point>& points, const cv::Scalar& color, int lineType/*=8*/, int shift/*=0*/ )
    {
        collection.push_back( IZoomObjectPtr( new FillConvexZoom( points, color, lineType, shift ) ) );
    }

    void zoomFillPoly( ZoomObjectCollection& collection, const cv::Point** pts, const int* npts, int ncontours, const cv::Scalar& color, int lineType/*=8*/, int shift/*=0*/, cv::Point offset/*=cv::Point() */ )
    {
        std::vector<std::vector<cv::Point>> points;

        for( int contour = 0; contour < ncontours; ++contour ) {
            int npoints = npts[contour];
            std::vector<cv::Point> coll;

            for( int i = 0; i < npoints; ++i ) {
                coll.push_back( pts[contour][i] );
            }

            points.push_back( coll );
        }

        zoomFillPoly( collection, points, color, lineType, shift, offset );
    }

    void zoomFillPoly( ZoomObjectCollection& collection, const std::vector<std::vector<cv::Point>>& pts, const cv::Scalar& color, int lineType/*=8*/, int shift/*=0*/, cv::Point offset/*=cv::Point() */ )
    {
        collection.push_back( IZoomObjectPtr( new FillPolyZoom( pts, color, lineType, shift ) ) );
    }

    void zoomPolylines( ZoomObjectCollection& collection, const cv::Point* const* pts, const int* npts, int ncontours, bool isClosed, const cv::Scalar& color, int thickness/*=1*/, int lineType/*=8*/, int shift/*=0 */ )
    {
        std::vector<std::vector<cv::Point>> points;

        for( int contour = 0; contour < ncontours; ++contour ) {
            int npoints = npts[contour];
            std::vector<cv::Point> coll;

            for( int i = 0; i < npoints; ++i ) {
                coll.push_back( pts[contour][i] );
            }

            points.push_back( coll );
        }

        zoomPolylines( collection, points, isClosed, color, thickness, lineType, shift );
    }

    void zoomPolylines( ZoomObjectCollection& collection, const std::vector<std::vector<cv::Point>>& pts, bool isClosed, const cv::Scalar& color, int thickness/*=1*/, int lineType/*=8*/, int shift/*=0 */ )
    {
        collection.push_back( IZoomObjectPtr( new PolylineZoom( pts, isClosed, color, thickness, lineType, shift ) ) );
    }

    void zoomPutText( ZoomObjectCollection& collection, const std::string& text, cv::Point org, int fontFace, double fontScale, cv::Scalar color, int thickness/*=1*/, int lineType/*=8*/, bool bottomLeftOrigin/*=false*/ )
    {
        collection.push_back( IZoomObjectPtr( new PutTextZoom( text, org, fontFace, fontScale, color, thickness, lineType, bottomLeftOrigin) ) );
    }

}

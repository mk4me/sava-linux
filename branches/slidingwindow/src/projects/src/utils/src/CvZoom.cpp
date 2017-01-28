#include "CvZoom.h"

#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <boost/thread.hpp>
#include <map>
#include <QtWidgets/QWidget>
#include <QtCore/QEvent>
#include <QtGui/QKeyEvent>

namespace utils {

    struct ZoomData
    {
        typedef std::pair<cv::MouseCallback, void*> _callback;
        typedef std::shared_ptr<_callback> _callbackPtr;
        ZoomInfoPtr zoomInfo;
        int ratioTrackbar;
        cv::Mat currentMat;
        _callbackPtr callback;
        ZoomObjectCollection currentCollection;
    };
    typedef std::shared_ptr<ZoomData> ZoomDataPtr;
    typedef std::shared_ptr<const ZoomData> ZoomDataConstPtr;

    //!
    static class windowData {
        public:
        //!
        typedef std::map<std::string, ZoomDataPtr> Name2ZoomDataMap;
        typedef std::map<QWidget*, std::string> Widget2NameMap;
        //!
        Name2ZoomDataMap name2zoom;
        Widget2NameMap widget2name;
        //!
        windowData()
        {

        }

        ~windowData()
        {
            /*std::for_each(name2zoom.begin(), name2zoom.end(), [&](const Name2ZoomDataMap::value_type& pair)
            {
                cv::saveWindowParameters(pair.first);
            });*/
        }
    } windows;


    QWidget* getWidget( const std::string &name )
    {
        auto ptr = cvGetWindowHandle( name.c_str() );
        return reinterpret_cast<QWidget*>( ptr );
    }


    bool tryGetWidgetSize( const std::string& name, cv::Size& size )
    {
        QWidget* wid = utils::getWidget( name );

        if( wid ) {
            auto list = wid->children();
            auto it = list.begin();

            if( it != list.end() ) {
                QWidget* w = qobject_cast<QWidget*>( *it );

                if( w ) {
                    auto name = w->objectName().toStdString();
                    size = cv::Size( w->width(), w->height() );
                    return true;
                }
            }
        }

        return false;
    }

    class ZoomTransform : public IZoomTransform
    {
        public:
        ZoomTransform( ZoomInfo* info ) : info( info ) {}
        virtual cv::Point globalToZoomed( const cv::Point& global ) const
        {
            return ( global - info->getShift() ) * ( 1.0f / info->getRatio() );
        }

        virtual cv::Point zoomedToGlobal( const cv::Point& zoomed ) const
        {
            return zoomed * info->getRatio() + info->getShift();
        }

        virtual float getRatio() const
        {
            return info->getRatio();
        }
        private:
        ZoomInfo* info;

    };

    class ZoomRatio : public IZoomTransform
    {
        public:
        ZoomRatio( IZoomTransformConstPtr zoom, float ratio ) : zoom( zoom ), ratio( ratio ) {}

        virtual cv::Point globalToZoomed( const cv::Point& global ) const
        {
            cv::Point p = zoom->globalToZoomed( global );
            p.x *= ratio;
            p.y *= ratio;
            return p;
        }

        virtual cv::Point zoomedToGlobal( const cv::Point& zoomed ) const
        {
            cv::Point p = zoom->zoomedToGlobal( zoomed );
            p.x *= ratio;
            p.y *= ratio;
            return p;
        }

        virtual float getRatio() const
        {
            return zoom->getRatio();
        }

        private:
        float ratio;
        IZoomTransformConstPtr zoom;
    };

    void _zoomShow( cv::Mat m, ZoomInfoPtr zoomInfo, const ZoomObjectCollection &zoomCollection, const std::string& name )
    {
        if( !m.empty() ) {
            cv::Mat dst;

            try {
                cv::Size widgetSize;
                float widget2imageRatio = 1.0f;
                cv::Size imageSize = zoomInfo->getOrigin();

                if( tryGetWidgetSize( name, widgetSize ) ) {
                    float widgetRatio = ( float )widgetSize.width / widgetSize.height;
                    float imageRatio = ( float( imageSize.width ) ) / imageSize.height;
                    widget2imageRatio = ( float )widgetSize.width / imageSize.width;

                    if( abs( widgetRatio - imageRatio ) > 0.1f ) {
                        widgetSize = cv::Size(imageSize.width * widget2imageRatio, imageSize.height * widget2imageRatio);
                    }

                } else {
                    widgetSize = imageSize;
                }

                getRectSubPix( m, zoomInfo->getSize(), zoomInfo->getCenter(), dst );
                resize( dst, dst, widgetSize ); //zoomInfo->getOrigin());
                //resize(dst, dst, size);

                IZoomTransformPtr t( new ZoomRatio( zoomInfo->getTransform(), widget2imageRatio ) );

                for( auto it = zoomCollection.begin(); it != zoomCollection.end(); ++it ) {
                    ( *it )->draw( dst, t );
                }

                cv::imshow( name, dst );

            } catch( std::exception& ) {
                cv::imshow( name, m );
            }
        }
    }



    ZoomInfo::ZoomInfo() :
    ratio( 1.0f )
    {
        transform = IZoomTransformPtr( new ZoomTransform( this ) );
    }


    ZoomInfo::ZoomInfo( const cv::Size& originSize, const cv::Point& center, float ratio ) :
    origin( originSize ),
            size( originSize )
    {
        setRatio( ratio );
        setCenter( center );
    }



    void ZoomInfo::setRatio( float val )
    {
        ratio = ( std::min )( 1.0f , ( std::max )( val, 0.01f ) );
        auto newSize = origin;
        newSize.width = static_cast<int>( newSize.width * ratio );
        newSize.height = static_cast<int>( newSize.height * ratio );
        setSize( newSize );
        adjust();
    }

    void ZoomInfo::adjust()
    {
        auto dx = center.x - size.width / 2;
        auto dy = center.y - size.height / 2;

        if( dx < 0 ) {
            center.x -= dx;
        }

        if( dy < 0 ) {
            center.y -= dy;
        }

        dx = center.x + size.width / 2 - origin.width;
        dy = center.y + size.height / 2 - origin.height;

        if( dx > 0 ) {
            center.x -= dx;
        }

        if( dy > 0 ) {
            center.y -= dy;
        }
    }

    void ZoomInfo::setSize( cv::Size val )
    {
        size = val;
        adjust();
    }

    void ZoomInfo::setCenter( cv::Point val )
    {
        center = val;
        adjust();
    }

    cv::Point ZoomInfo::getShift() const
    {
        return cv::Point( center.x - size.width / 2, center.y - size.height / 2 );
    }



    utils::IZoomTransformConstPtr ZoomInfo::getTransform() const
    {
        return transform;
    }


    template<class Key, class Val>
            bool getMapKey( const std::map<Key, std::shared_ptr<Val>>& map, Val* val, Key& out )
        {
            for( auto it = map.begin(); it != map.end(); ++it ) {
                if( it->second.get() == val ) {
                    out = it->first;
                    return true;
                }
            }

            return false;
        }

    void refreshGUI( ZoomData* zd )
    {
        std::string name;

        if( getMapKey( windows.name2zoom, zd, name ) ) {
            cv::setTrackbarPos( "Ratio", name, static_cast<int>( zd->zoomInfo->getRatio() * 100.0f ) );
        }
    }

    void updateKeyboard( const std::string& name, int key )
    {
        //int key = cvWaitKey(1);
        if( key >= '0' && key <= '9' ) {
            float ratio = key != '0' ? ( key - '0' ) / 10.0f : 1.0f;
            auto it = windows.name2zoom.find( name );

            if( it != windows.name2zoom.end() ) {
                it->second->zoomInfo->setRatio( ratio );
                auto zd = it->second.get();
                refreshGUI( zd );
                _zoomShow( zd->currentMat, zd->zoomInfo, zd->currentCollection, name );
            }
        }
    }


    bool panning = false;
    int px, py;
    // Implement mouse callback
    void mouseCallback( int event, int x, int y, int flags, void* param )
    {
        ZoomData* zd = ( ZoomData* ) param;
        ZoomInfoPtr zi = zd->zoomInfo;

        switch( event ) {
		case cv::EVENT_LBUTTONDOWN: {
                if( flags & cv::EVENT_FLAG_CTRLKEY ) {
                    panning = true;
                    px = x;
                    py = y;
                    return;
                }

                //auto ratio = zi->getRatio();
                //cv::Point clk(static_cast<int>(x * ratio), static_cast<int>(y * ratio));
                //zi->setCenter(zi->getShift() + clk);
                //zi->setRatio(ratio - 0.1f);
                //refreshGUI(zd);
            }
            break;

		case cv::EVENT_MOUSEMOVE:
                if( ( flags & cv::EVENT_FLAG_CTRLKEY ) && panning ) {
                    cv::Point center = zi->getCenter();
                    center.x -= x - px;
                    center.y -= y - py;
                    px = x;
                    py = y;
                    zi->setCenter( center );
                    std::string name;

                    if( getMapKey( windows.name2zoom, zd, name ) ) {
                        _zoomShow( zd->currentMat, zd->zoomInfo, zd->currentCollection, name );

                    } else {
                        assert( false );
                    }

                    return;

                } else {
                    panning = false;
                }

                break;

		case cv::EVENT_LBUTTONUP:
                panning = false;
                //zi->setRatio(zi->getRatio() + 0.1f);
                //refreshGUI(zd);
                break;
        }

        if( zd->callback ) {
            auto t = zd->zoomInfo->getTransform();
            cv::Point point( x, y );
            point = t->zoomedToGlobal( point );
            zd->callback->first( event, point.x, point.y, flags, zd->callback->second );
        }
    }

    void onTrackbarchange( int pos, void* data )
    {
        ZoomData* zd = ( ZoomData* ) data;
        zd->zoomInfo->setRatio( pos / 100.0f );
        std::string name;

        if( getMapKey( windows.name2zoom, zd, name ) ) {
            _zoomShow( zd->currentMat, zd->zoomInfo, zd->currentCollection, name );

        } else {
            assert( false );
        }
    }

    void createZoomControl( const std::string& name )
    {
        auto it = windows.name2zoom.find( name );
        assert( it != windows.name2zoom.end() );
        cv::createTrackbar( "Ratio",name, &it->second->ratioTrackbar, 100, onTrackbarchange, it->second.get() );
    }

    void zoomedWindow( const std::string& name, int flags )
    {
        cv::namedWindow( name, flags );
        //cv::namedWindow(name, flags | CV_GUI_NORMAL);
        auto it = windows.name2zoom.find( name );

        if( it == windows.name2zoom.end() ) {

            QWidget* widget = getWidget( name );

            widget->installEventFilter( new CvZoomEventFilter() );
            windows.widget2name[widget] = name;

            ZoomInfoPtr zoom( new ZoomInfo );
            ZoomDataPtr data( new ZoomData );
            data->zoomInfo = zoom;
            data->ratioTrackbar = 100;
            windows.name2zoom[name] = data;
            cv::setMouseCallback( name, mouseCallback, ( void* )data.get() );
            cv::loadWindowParameters( name );
            createZoomControl( name );
        }
    }

    ZoomInfoConstPtr getZoomInfo( const std::string& name )
    {
        ZoomInfoPtr zoom;
        auto it = windows.name2zoom.find( std::string( name ) );

        if( it != windows.name2zoom.end() ) {
            zoom = it->second->zoomInfo;
        }

        return zoom;
    }

    void zoomShow( const std::string& name, cv::InputArray mat, const ZoomObjectCollection& zoomCollection )
    {
        cv::Mat m = mat.getMat();
        zoomedWindow( name );
        auto entry = windows.name2zoom.find( std::string( name ) );

        if( entry != windows.name2zoom.end() ) {
            auto size = mat.size();
            ZoomInfoPtr zoomInfo = entry->second->zoomInfo;
            entry->second->currentMat = m;
            entry->second->currentCollection = zoomCollection;

            if( zoomInfo->getOrigin() != size ) {
                zoomInfo->setOrigin( size );
                zoomInfo->setSize( size );
                zoomInfo->setCenter( cv::Point( size.width / 2, size.height / 2 ) );
            }

            _zoomShow( m, zoomInfo, zoomCollection, name );

            //cv::saveWindowParameters(name);

        } else {
            cv::imshow( name, mat );
        }
    }


    void setZoomInfo( const std::string& name, ZoomInfoPtr zoom )
    {
        auto it = windows.name2zoom.find( name );

        if( it != windows.name2zoom.end() ) {
            it->second->zoomInfo = zoom;
            refreshGUI( it->second.get() );

        } else {
            throw std::runtime_error( "zoom : entry not found" );
        }
    }

    void zoomDestroyWindow( const std::string& name )
    {
        auto zname = windows.name2zoom.find( name );

        if( zname != windows.name2zoom.end() ) {
            windows.name2zoom.erase( zname );
        }

        cv::saveWindowParameters( name );
        cv::destroyWindow( name );
    }

    void zoomMouseCallback( const std::string& name, cv::MouseCallback onMouse, void* userdata /*= 0*/ )
    {
        auto entry = windows.name2zoom.find( name );

        if( entry != windows.name2zoom.end() ) {
            entry->second->callback = ZoomData::_callbackPtr( new ZoomData::_callback( onMouse, userdata ) );

        } else {
            assert( false );
        }
    }

    void zoomDestroyAllWindows()
    {
        while( !windows.name2zoom.empty() ) {
            auto entry = windows.name2zoom.begin();
            zoomDestroyWindow( entry->first );
        }

        cv::destroyAllWindows();
    }



}

bool CvZoomEventFilter::eventFilter( QObject *o, QEvent *e )
{
    if( e->type() == QEvent::KeyPress ) {


        auto it = utils::windows.widget2name.find( qobject_cast<QWidget*>( o ) );

        if( it != utils::windows.widget2name.end() ) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>( e );
            utils::updateKeyboard( it->second, keyEvent->key() );
        }
    }

    // always returns false, because we may need normal keyboard events
    return false;
}

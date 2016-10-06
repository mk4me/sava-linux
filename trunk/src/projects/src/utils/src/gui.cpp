#include "gui.h"
#include <map>
namespace utils {
    namespace gui
    {
#ifndef __utils_gui_winelem_eq_operator
#define __utils_gui_winelem_eq_operator
        bool operator==( AWindowElement* lhs, const std::string& rhs )
        {
            return ( *lhs )==rhs;
        }
#endif __utils_gui_winelem_eq_operator

        void onMouse( int event, int x, int y, int flags, void* obj )
        {
            if( !obj ) {
                return;
            }

            CMouseObserver *pObsv=static_cast<CMouseObserver*>( obj );

            if( !pObsv ) {
                return;
            }

            pObsv->m_iMouseX=x;
            pObsv->m_iMouseY=y;

            pObsv->m_iMouseEvent=event;

            //std::cout<<event<<","<< flags <<"\n";
            if( event!=0 ) {
                pObsv->m_bMouseEventRegistered=true;
            }
        }


        const int CMouseObserver::Pressed()
        {
            if( !m_bMouseEventRegistered ) {
                return 0;
            }

            m_bMouseEventRegistered=false;
            //std::cout<<"retrieved!\n";
            return m_iMouseEvent;

        }

        void AMisWindow::init()
        {
            m_mtrxCanvas=cv::Mat( m_iHeight, m_iWidth, CV_8UC3, cv::Scalar( 255,255,255 ) );
            cv::namedWindow( m_sWindowName );
        }

        void AMisWindow::repaint()
        {
            updateMenu();
            updateCanvas( m_mtrxCanvas );

            for( auto it=m_menuAreas.begin(); it!=m_menuAreas.end(); ++it )
                if( ( *it ).second->Visible() ) {
                    ( *it ).second->draw( m_mtrxCanvas );
                }

            cv::imshow( m_sWindowName, m_mtrxCanvas );
            cv::waitKey( 1 );
        }

        AMisWindow::~AMisWindow()
        {
            CWElemHandlerFactory::getInstance()->deleteAllWindowElements( m_sWindowName );
            cv::destroyWindow( m_sWindowName );
        }

        const bool AMisWindow::addArea( AWindowArea* wa, const std::string& name )
        {
            if( m_menuAreas.find( name )!=m_menuAreas.end() ) {
                return false;
            }

            m_menuAreas.insert( std::map<std::string, AWindowArea*>::value_type( name,wa ) );
            return true;
        }

        const bool AMisWindow::removeArea( const std::string& name )
        {
            m_menuAreas.erase( name );
            return true;
        }

        AWindowArea* AMisWindow::getAreaByPoint( int x, int y )
        {
            for( auto it=m_menuAreas.begin(); it!=m_menuAreas.end(); ++it ) {
                AWindowArea* wa=( *it ).second;

                if( wa->contains( x, y ) ) {
                    return wa;
                }
            }

            return NULL;
        }

        AWindowArea* AMisWindow::getArea( const std::string& name )
        {
            auto it=m_menuAreas.find( name );

            if( it==m_menuAreas.end() ) {
                return NULL;
            }

            return ( *it ).second;
        }

		std::vector <AWindowArea*> AMisWindow::getAreasByPoint( int x, int y ){

			std::vector <AWindowArea*> rv;
			for( auto it=m_menuAreas.begin(); it!=m_menuAreas.end(); ++it ) {
				AWindowArea* wa=( *it ).second;

				if( wa->contains( x, y ) ) {
					rv.push_back(wa);
				}
			}

			return rv;
		}

        void AControlerView::serveMouseEvent()
        {
            int pPressed=m_pMouseObserver.Pressed();

            /*if (pPressed!=0)
            	std::cout<< pPressed <<"\n";*/
            switch( pPressed ) {
                case CV_EVENT_LBUTTONDBLCLK: {
                    onMouseDbl( m_pMouseObserver.MouseX(), m_pMouseObserver.MouseY() );
                    break;
                }

                case CV_EVENT_MOUSEMOVE: {
                    onMouseMove( m_pMouseObserver.MouseX(), m_pMouseObserver.MouseY() );
                    break;
                }

                case CV_EVENT_LBUTTONUP: {
                    //AWindowArea* wa=m_pWindow->getAreaByPoint( m_pMouseObserver.MouseX(), m_pMouseObserver.MouseY() );
					std::vector<AWindowArea*> wav;  //window area vector
					wav=m_pWindow->getAreasByPoint(m_pMouseObserver.MouseX(), m_pMouseObserver.MouseY());
                    bool clicked=false;

					for (auto it=wav.begin(); it!=wav.end(); ++it){
						clicked=(*it)->onClick(m_pMouseObserver.MouseX()-(*it)->Rect().x, m_pMouseObserver.MouseY()-(*it)->Rect().y);
						if (clicked)
							break;
					}

					/*if( wa!=NULL ) {
					clicked=wa->onClick( m_pMouseObserver.MouseX()-wa->Rect().x, m_pMouseObserver.MouseY()-wa->Rect().y );

					} else {
					onMouseLeftUp( m_pMouseObserver.MouseX(), m_pMouseObserver.MouseY() );
					}*/

                    if( !clicked ) {
                        onMouseLeftUp( m_pMouseObserver.MouseX(), m_pMouseObserver.MouseY() );
                    }

                    break;
                }

                case CV_EVENT_LBUTTONDOWN: {
                    onMouseLeftDown( m_pMouseObserver.MouseX(), m_pMouseObserver.MouseY() );
                    break;
                }

                case CV_EVENT_RBUTTONUP: {
                    onMouseRightUp( m_pMouseObserver.MouseX(), m_pMouseObserver.MouseY() );
                    break;
                }

                case CV_EVENT_RBUTTONDOWN: {
                    onMouseRightDown( m_pMouseObserver.MouseX(), m_pMouseObserver.MouseY() );
                    break;
                }

				case  CV_EVENT_MBUTTONUP: {
					onMouseMiddleUp(m_pMouseObserver.MouseX(), m_pMouseObserver.MouseY());
					break;
				}

				case  CV_EVENT_MBUTTONDOWN: {
					onMouseMiddleDown(m_pMouseObserver.MouseX(), m_pMouseObserver.MouseY());
					break;
				}
            }
        }


        void AControlerView::update()
        {
            serveMouseEvent();
            updateControlerView();
            m_pWindow->repaint();
        }


        CWElemHandlerFactory* CWElemHandlerFactory::getInstance()
        {
            if( !m_instance ) {
                m_instance=new CWElemHandlerFactory();
            }

            return m_instance;
        }

        CWElemHandlerFactory* CWElemHandlerFactory::m_instance=0;

        bool CWElemHandlerFactory::createElement( const ElementType typeOfElement, const std::string& name, const std::string& windowName/*=""*/, int value/*=0*/, int length/*=100*/, IAction* action/*=NULL*/ )
        {
            if( std::find( m_elementVector.begin(), m_elementVector.end(), name )!= m_elementVector.end() ) {
                return false;
            }

            AWindowElement* newWindowElement;

            if( typeOfElement==WET_TRACKBAR ) {
                newWindowElement=new CTrackBar( name, value, length, windowName );

            } else {
                newWindowElement=new CButton( name, value, typeOfElement, windowName, action );
            }

            m_elementVector.push_back( newWindowElement );

            return true;
        }

        void CWElemHandlerFactory::deleteWinElement( const std::string& name )
        {
            std::remove( m_elementVector.begin(), m_elementVector.end(), name );
        }

        void CWElemHandlerFactory::deleteAllWindowElements( const std::string& winName )
        {
            std::vector<AWindowElement*>::iterator it=m_elementVector.begin();

            while( it!=m_elementVector.end() ) {
                if( ( *it )->WindowName()==winName ) {
                    it = m_elementVector.erase( it );

                } else {
                    ++it;
                }
            }
        }

        AWindowElement* CWElemHandlerFactory::getElement( const std::string& name )
        {
            std::vector<AWindowElement*>::iterator it=std::find( m_elementVector.begin(), m_elementVector.end(), name );

            if( it==m_elementVector.end() ) {
                return NULL;
            }

            //throw std::exception("Window Element Factory: Element with given name doesn't exists! ");

            return *it;
        }

        int CWElemHandlerFactory::getValue( const std::string& name )
        {
            std::vector<AWindowElement*>::iterator it=std::find( m_elementVector.begin(), m_elementVector.end(), name );

            if( it==m_elementVector.end() )
                //TODO co tutaj ma byæ zwracane?
                //return NULL;
            {
                return 0;
            }

            //throw std::exception("Window Element Factory: Element with given name doesn't exists! ");

            return ( *it )->Value();
        }







        AWindowElement::~AWindowElement()
        {
        }

        void AWindowElement::performAction()
        {
            if( m_action!=NULL ) {
                m_action->perform();
            }
        }

        /*void AWindowElement::registerAction( IAction* action )
        {
        	m_action=action;
        }*/


        int CTrackBar::Value() const
        {
            return cv::getTrackbarPos( m_name, m_windowName );
        }

        CTrackBar::CTrackBar( const std::string& name, const int position, const int length, const std::string& windowName/*=""*/ ):AWindowElement( name, windowName ),
        m_length( length ),
        m_position( position )
        {
            cv::createTrackbar( m_name, m_windowName, &m_position, m_length );
        }


        int CButton::Value() const
        {
            return m_value;
        }

        void buttonCallback( int state, void* ptr )
        {
            CButton* buttonCalled= static_cast<CButton*>( ptr );

            buttonCalled->m_value=state;
            buttonCalled->performAction();
        }

        CButton::CButton( const std::string& name, const int startValue, ElementType buttonType, const std::string& windowName/*=""*/ , IAction* action/*=NULL*/ ):AWindowElement( name, windowName,action ),
        m_value( startValue )
        {
            if( buttonType==WET_TRACKBAR ) {
                return;
            }

            //throw std::exception("You are trying to create button but with type <WET_TRACKBAR>");

            if( buttonType==WET_BUTTON ) {
                cv::createButton( m_name, cv::ButtonCallback( buttonCallback ), this, CV_PUSH_BUTTON,  startValue != 0);
            }
        }


        bool AWindowArea::contains( int x, int y ) const
        {
            if( ( x>=m_place.tl().x ) && ( x<=m_place.br().x ) && ( y>=m_place.tl().y ) && ( y<=m_place.br().y ) )
            {
                return true;
            }

            return false;
        }

    }
}

//void test()
//{
//
//}

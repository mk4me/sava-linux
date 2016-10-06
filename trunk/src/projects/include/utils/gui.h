#ifndef ut_gui_h__
#define ut_gui_h__
#include <map>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/highgui.hpp>

#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"

#include <map>


namespace utils {
    namespace gui
    {

        void onMouse( int event, int x, int y, int flags, void* obj );

        class CMouseObserver
        {
            private:
            friend void onMouse( int event, int x, int y, int flags, void* obj );
            int m_iMouseX;

            int m_iMouseY;
            int m_iMouseEvent;
            bool m_bMouseEventRegistered;

            public:
            CMouseObserver() : m_bMouseEventRegistered( false )
            {};

            const int MouseX() const { return m_iMouseX; }
            const int MouseY() const { return m_iMouseY; }
            const int Pressed();
        };
        //////////////////////////////////////////
        class AControlerView;

        class IAction
        {
            protected:
            AControlerView* m_controler;
            public:
            IAction( AControlerView* controler ): m_controler( controler )
            {};

            virtual void perform()=0;
        };

        //////////////////////////////////////////



        class AWindowElement
        {
            private:

            AWindowElement(); //we don't want to create Window element without name - it is holding by GuiHandler;
            AWindowElement( const AWindowElement& cpy ); //we don't want to create copy of this class, because it is connected with existing window
            IAction *m_action;
            protected:
            std::string m_label;

            std::string m_windowName;

            std::string m_name;

            AWindowElement( const std::string& name, const std::string& windowName=std::string(), IAction* action=NULL ):m_name( name ),
            m_windowName( windowName ),
            m_action( action )
            {};

            void performAction();
            public:


            virtual ~AWindowElement();

            //void registerAction(IAction* action);

            virtual int Value() const =0;

            std::string WindowName() const { return m_windowName; }

            std::string Label() const { return m_label; }
            void Label( std::string val )
            {
                m_label = val;
            }


            bool operator== ( const std::string& cpy ) const
            {
                return ( m_name==cpy );
            };

            bool operator == ( const AWindowElement& cpy ) const
            {
                return operator == ( cpy.m_name );
            }

        };


        //************************************
        // Method:    operator== Needed for find in vector<AWindowElement*>
        // FullName:  utils::gui::operator==
        // Access:    public
        // Returns:   bool
        // Qualifier:
        // Parameter: AWindowElement * lhs - Left Hand Side variable
        // Parameter: const std::string & rhs - Right Hand Side variable
        //************************************

        ///////////////////////////////////////////

        class CTrackBar:public AWindowElement
        {
            private:
            const int m_length;
            int m_position;
            public:
            CTrackBar( const std::string& name, int position, const int length, const std::string& windowName=std::string() );

            const int Length() const { return m_length; }

            virtual int Value() const;



        };
        ///////////////////////////////////////////
        void buttonCallback( int state, void* ptr );

        enum ElementType
        {
            WET_BUTTON,
            WET_CHECKBOX,
            WET_RADIO,
            WET_TRACKBAR,
        };

        class CButton:public AWindowElement
        {
            private:
            int m_value;
            friend void buttonCallback( int state, void* ptr );

            public:
            CButton( const std::string& name, const int startValue, ElementType buttonType, const std::string& windowName=std::string(), IAction* action=NULL );

            virtual int Value() const;

        };

        /*	\brief Window Element Handler Singleton (GoF "Singleton" design pattern)
        */
        class CWElemHandlerFactory
        {
            public:

            //************************************
            // Method:    getInstance - method realizing retrieving an instance of GoF singleton design pattern class
            // FullName:  utils::gui::CWElemHandlerFactory::getInstance
            // Access:    public static
            // Returns:   CWElemHandlerFactory*
            // Qualifier:
            //************************************
            static CWElemHandlerFactory* getInstance();

            //************************************
            // Method:    createElement - creates new Windows element such as trackbar, radiobutton, checkbox, push button. It cares of not creating two or more element with the same name - you should always check the return value!
            // FullName:  utils::gui::CWElemHandlerFactory::createElement
            // Access:    public
            // Returns:   bool - true in case of success false else
            // Qualifier:
            // Parameter: const ElementType typeOfElement
            // Parameter: const std::string & name
            // Parameter: const std::string & windowName
            // Parameter: int value - for trackbar: start position of trackbar, for button - unused, for other - start value;
            // Parameter: int length - for trackbar: max position of trackbar, for other - unused.
            //************************************
            bool createElement( const ElementType typeOfElement, const std::string& name, const std::string& windowName=std::string(), int value=0, int length=100, IAction* action=NULL );

            //bool registerAction(const std::string& name, IAction* action);

            //************************************
            // Method:    getElement - returns pointer to Window element with specific name
            // FullName:  utils::gui::CWElemHandlerFactory::getElement
            // Access:    public
            // Returns:   AWindowElement* it is a pointer to window element with specific name
            // Qualifier: const
            // Parameter: const std::string & name - name of element for retrieving
            // Attn: In case of not existing element with given name throws exception so use try ... catch ...
            //************************************
            AWindowElement* getElement( const std::string& name );

            //************************************
            // Method:    getValue - returns value of element with given name
            // FullName:  utils::gui::CWElemHandlerFactory::getValue
            // Access:    public
            // Returns:   int - if window element is push button always returns -1 if win element is trackbar it is a position on trakbar pane else it should be interpreted as boolean value
            // Qualifier: const
            // Parameter: const std::string & name - name of window element which value should be returned;
            // Attn: In case of not existing element with given name throws exception so use try ... catch ...
            //************************************
            int getValue( const std::string& name );

            private:
            #pragma region singleton declaration
            CWElemHandlerFactory() {};

            //disallow copy constructor and assign operator.
            CWElemHandlerFactory( const CWElemHandlerFactory& cpy );
            const CWElemHandlerFactory& operator = ( const CWElemHandlerFactory& rhs );

            static CWElemHandlerFactory* m_instance;
            #pragma endregion

            #pragma region handler declaration
            std::vector <AWindowElement*> m_elementVector;
            #pragma endregion

            friend class AMisWindow; //created window should remove all element that are connected with itself, and nobody else should make a mess in win elements list.

            //************************************
            // Method:    deleteWinElement - deletes one element with given name
            // FullName:  utils::gui::CWElemHandlerFactory::deleteWinElement
            // Access:    public
            // Returns:   void
            // Qualifier:
            // Parameter: const std::string & name - name of element for deleting
            //************************************
            void deleteWinElement( const std::string& name );


            //************************************
            // Method:    deleteAllWindowElements - deletes all elements connected with windows with given window
            // FullName:  utils::gui::CWElemHandlerFactory::deleteAllWindowElements
            // Access:    public
            // Returns:   void
            // Qualifier:
            // Parameter: const std::string & winName - name of window where elements should be deleted from.
            //************************************
            void deleteAllWindowElements( const std::string& winName );

        };

        ///////////////////////////////////////////
        class AWindowArea
        {
            public:
            AWindowArea():m_place( cv::Rect( 0,0,0,0 ) ),
            m_visible( true )
            {};

            AWindowArea( cv::Rect place ):m_place( place ),
                         m_visible( true )
            {};

            virtual ~AWindowArea()
            {};

            virtual void draw( cv::Mat& canvas )=0;

            virtual bool onClick( int x, int y )=0;

            bool contains( int x, int y ) const;

            const cv::Rect & Rect() const {return m_place;};

            bool Visible() const { return m_visible; }
            void Visible( bool val )
            {
                m_visible = val;
            }
            private:
            cv::Rect m_place;

            bool m_visible;

        };

        //////////////////////////////////////////
        class AMisWindow
        {
            private:
            void init();

            cv::Mat m_mtrxCanvas;

            std::map<std::string, AWindowArea*> m_menuAreas;
            protected:
            std::string m_sWindowName;
            int m_iWidth;
            int m_iHeight;
            int m_iMargin;

            virtual void updateCanvas( cv::Mat& mtrxCanvas ) =0;
            virtual void updateMenu()=0;
            public:


            AMisWindow():m_sWindowName( "NoName" ),
            m_iWidth( 820 ),
            m_iHeight( 620 ),
            m_iMargin( 20 )
            //m_mtrxCanvas(m_iHeight, m_iWidth, CV_8UC3, cv::Scalar(255,255,255))
            {
                init();
            };

            AMisWindow( const std::string & sWindowName, int iWidth, int iHeight,int iMargin ):m_sWindowName( sWindowName ),
                        m_iWidth( iWidth ),
                        m_iHeight( iHeight ),
                        m_iMargin( iMargin )
                        //m_mtrxCanvas(m_iHeight, m_iWidth, CV_8UC3, cv::Scalar(255,255,255))
            {
                init();
            };

            virtual ~AMisWindow();

            const std::string & WindowName() const { return m_sWindowName; }

            const int Width() const { return m_iWidth; };
            const int Height() const { return m_iHeight; };
            const int Margin() const {return m_iMargin; };

            const bool addArea( AWindowArea* wa, const std::string& name );
            const bool removeArea( const std::string& name );
            AWindowArea* getArea( const std::string& name );
            AWindowArea* getAreaByPoint( int x, int y );
			std::vector <AWindowArea*> getAreasByPoint(int x, int y);

            void repaint();

        };
        //////////////////////////////////////////

        class AControlerView
        {
            private:
            CMouseObserver m_pMouseObserver;
            AMisWindow *m_pWindow;


            void serveMouseEvent();

            //
            //AControlerView();
            protected:
            AControlerView( const AControlerView& pCpy )
            {
            };
            AMisWindow * Window() const { return m_pWindow; }

            virtual void onMouseLeftUp( int iX, int iY )=0;
            virtual void onMouseLeftDown( int iX, int iY )=0;

            virtual void onMouseRightUp( int iX, int iY )=0;
            virtual void onMouseRightDown( int iX, int iY )=0;

			virtual void onMouseMiddleUp(int iX, int iY) = 0;
			virtual void onMouseMiddleDown(int iX, int iY) = 0;

            virtual void onMouseDbl( int iX, int iY )=0;
            virtual void onMouseMove( int iX, int iY )=0;
            virtual void updateControlerView()=0;
            public:
            AControlerView( AMisWindow *pWindow ):m_pWindow( pWindow )
            {
                cv::setMouseCallback( pWindow->WindowName(), cv::MouseCallback( onMouse ), &m_pMouseObserver );
                //cvSetMouseCallback(pWindow->WindowName().c_str(), CvMouseCallback(onMouse), &m_pMouseObserver);

            };

            virtual ~AControlerView() {};

            void update();
        };


    }
}

#endif // ut_gui_h__

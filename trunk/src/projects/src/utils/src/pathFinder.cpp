#include "pathFinder.h"
#include <stdio.h>
#include <string>
#include <map>
#include <stdexcept>
#include <exception>
namespace utils {
    namespace pathFinder
    {
        CTSFactory* CTSFactory::getInstance()
        {
            if( !m_instance ) {
                m_instance=new CTSFactory();
            }

            return m_instance;
        }

        _PF_STRATEGY_PICK_RESULT CTSFactory::registerStrategy( const std::shared_ptr<ITrackSetUpdaterStrategy>& prototype )
        {
            for( auto it=m_updaters.begin(); it!=m_updaters.end(); ++it ) {
                if( it->get()->id() ==prototype->id() ) {
                    return _PFPICK_ALREADY_REGISTERED;
                }
            }

            m_updaters.push_back( prototype );

            return _PFPICK_OK;
        }

        std::shared_ptr<ITrackSetUpdaterStrategy> CTSFactory::getStrategy( const std::string& id )
        {
            for( auto it=m_updaters.begin(); it!=m_updaters.end(); ++it ) {
                if( it->get()->id() ==id ) {
                    return *it;
                }
            }

            //error handling:
            std::stringstream error;
            error<<"Track Set updater strategy (interface ITrackSetUpdaterStrategy) ["<<id<<"] is not registered in strategy Factory (class CTSFactory),\n use utils::pathFinder::CTSFactory::getInstance()::registerStrategy.\n";
            throw std::invalid_argument( error.str().c_str() );
            return std::shared_ptr<ITrackSetUpdaterStrategy>();
        }

        CTSFactory *CTSFactory::m_instance=0;

        _PF_STRATEGY_PICK_RESULT CTrackSetContext::useStrategy( const std::string& id )
        {
            for( auto it=m_strategies.begin(); it!=m_strategies.end(); ++it ) {
                if( it->first.get()->id() == id ) {
                    return _PFPICK_ALREADY_REGISTERED;
                }
            }

            try {
                m_strategies.push_back( std::pair<std::shared_ptr<ITrackSetUpdaterStrategy>, bool>( CTSFactory::getInstance()->getStrategy( id ), true ) );

            } catch( std::invalid_argument ) {
                return _PFPICK_STRATEGY_NOT_FOUND;

            } catch( ... ) {
                return _PFPICK_UNKNOWN_ERROR;
            }

            return _PFPICK_OK;
        }

        _PF_STRATEGY_PICK_RESULT CTrackSetContext::stopStrategy( const std::string& id )
        {
            for( auto it=m_strategies.begin(); it!=m_strategies.end(); ++it ) {
                if( it->first.get()->id() == id ) {
                    m_strategies.erase( it );
                    return _PFPICK_OK;
                }
            }

            return _PFPICK_STRATEGY_NOT_FOUND;
        }

        _PF_STRATEGY_PICK_RESULT CTrackSetContext::pauseStrategy( const std::string& id )
        {
            for( auto it=m_strategies.begin(); it!=m_strategies.end(); ++it ) {
                if( it->first.get()->id() == id ) {
                    if( !it->second ) {
                        return _PFPICK_ALREADY_PAUSED;
                    }

                    it->second=false;
                    return _PFPICK_OK;
                }
            }

            return _PFPICK_STRATEGY_NOT_FOUND;
        }

        _PF_STRATEGY_PICK_RESULT CTrackSetContext::activateStrategy( const std::string& id )
        {
            for( auto it=m_strategies.begin(); it!=m_strategies.end(); ++it ) {
                if( it->first.get()->id() == id ) {
                    if( it->second ) {
                        return _PFPICK_ALREADY_ACTIVE;
                    }

                    it->second=true;
                    return _PFPICK_OK;
                }
            }

            return _PFPICK_STRATEGY_NOT_FOUND;
        }

        _PF_STRATEGY_USE_RESULT CTrackSetContext::lastStepResult( const std::string& id )
        {
            for( auto it=m_errors.begin(); it!=m_errors.end(); ++it ) {
                if( it->first==id ) {
                    return it->second;
                }
            }

            return _PFUSE_NOT_FOUND;
        }

        _PF_STRATEGY_USE_RESULT CTrackSetContext::require( const cv::Mat& image )
        {
            bool rv=true;

            std::map<std::string, _PF_STRATEGY_USE_RESULT>().swap(m_errors);

            if( m_strategies.begin()==m_strategies.end() ) {
                return _PFUSE_NO_STRATEGY_PICKED;
            }

            for( auto it=m_strategies.begin(); it!=m_strategies.end(); ++it ) {
                if( it->second ) {
                    _PF_STRATEGY_USE_RESULT r;

                    try {
                        if( it->first->update( image ) ) {
                            r=_PFUSE_OK;

                        } else {
                            r=_PFUSE_FALSE;
                        }

                    } catch( ... ) {
                        r=_PFUSE_EXCEPTION_THROWN;
                    }

                    m_errors[it->first->id()]=r;

                    if( r!=_PFUSE_OK ) {
                        rv=false;
                    }
                }
            }

            if( rv ) {
                return _PFUSE_OK;
            }

            return _PFUSE_FALSE;
        }

        CTrackSetContext::CTrackSetContext( const std::string& id )
        {
            useStrategy( id );
        }

    }//namespace pathFinder

}//namespace utils


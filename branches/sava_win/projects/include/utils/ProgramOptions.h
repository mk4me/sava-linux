#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

/// <summary>
/// klasa implementuj¹ca mechanizm parsowania parametrów wejciowych aplikacji.
/// </summary>
class ProgramOptions
{
    public:
    ProgramOptions( const std::string & executable = "" ) : normalOptions( "Options" ), executable( executable )
    {

    }

	/// <summary>
	/// Adds option with given name and description.
	/// </summary>
	/// <param name ="name">Option name.</parameter>
	/// <param name ="desc">Option description.</parameter>
	void add(const std::string & name, const std::string & desc)
	{
		normalOptions.add_options()(name.c_str(), desc.c_str());
	}

    /// <summary>
    /// Adds option with given name and description.
    /// </summary>
    /// <param name ="name">Option name.</parameter>
    /// <param name ="desc">Option description.</parameter>
    template <typename Type>
    void add( const std::string & name, const std::string & desc )
    {
        normalOptions.add_options()( name.c_str(), po::value<Type>(), desc.c_str() );
    }

    /// <summary>
    /// Adds option with given name, description and default value.
    /// </summary>
    /// <param name ="name">Option name.</parameter>
    /// <param name ="desc">Option description.</parameter>
    /// <param name ="def"></parameter>
    /// <returns></returns>
    template <typename Type>
    void add( const std::string & name, const std::string & desc, Type def )
    {
        normalOptions.add_options()( name.c_str(), po::value<Type>()->default_value( def ), desc.c_str() );
    }

    template <typename ElementType>
    void addVector( const std::string & name, const std::string & desc )
    {
        normalOptions.add_options()( name.c_str(), po::value<std::vector<ElementType>>()->multitoken(), desc.c_str() );
    }

    template <typename Type>
    void addPositional( const std::string & name, const std::string & desc )
    {
        normalOptions.add_options()( name.c_str(), po::value<Type>(), desc.c_str() );
        positionalOptions.add( name.c_str(), 1 );
    }

    void parse( int& argc, const char *argv[] )
    {
        po::command_line_parser parser( argc, argv );
        parser.options( normalOptions );
        parser.positional( positionalOptions );
        store( parser.run(), map );
        notify( map );
    }

    const bool exists( const std::string & name ) const
    {
        return ( map.count( name ) > 0 );
    }

    template <typename T>
    const bool get( const std::string & name, T &result ) const
    {
        if( map.count( name ) == 0 ) {
            return false;
        }

        result = map[name].as<T>();
        return true;
    }

    const std::string printUsage() const {
        std::ostringstream out;

        // print command line
        out << executable << " [extra options] ";
        int s = positionalOptions.max_total_count();

        for( int i = 0; i < s; ++i )
        {
            out << positionalOptions.name_for_position( i ) << " ";
        }

        out << std::endl << normalOptions;
        return out.str();
    }

	void setExecutable(const std::string& execTitle) { executable = execTitle; }

    private:
    std::string executable;

    po::variables_map map;
    po::options_description normalOptions;


    po::positional_options_description positionalOptions;
};
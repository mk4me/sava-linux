#pragma once
#include <opencv/cv.h>
#include <boost/unordered_map.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/xml_oarchive.hpp>
//#include <boost/archive/xml_iarchive.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/tracking.hpp>

// collections
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
//#include <boost/serialization/shared_ptr_helper.hpp>
//#include <boost/serialization/shared_ptr_132.hpp>

#include <utils/CvSerialization.h>

#define SERIALIZATION_LOG(msg)				//std::cout << "Serialisation log: " << msg << std::endl
#define SERIALIZATION_NVP(x)				x
#define SERIALIZATION_BASE_OBJECT_NVP(t)	boost::serialization::base_object<t>(*this)

namespace bss = boost::serialization;

/// <summary>
/// This file contains serialization procedures for classes not covered by boost::serialization (std::shared_ptr or cv::_Point).
/// </summary>
namespace boost 
{ 
	namespace serialization 
	{
	/// <summary>
	/// Serializes shared pointer.
	/// </summary>
	/// <param name=archive>Boost serialization archive.</param>
	/// <param name=value>Shared pointer to be serialized.</param>
	/// <param name=version>Archive version.</param>
/*	template<class Archive, class Type>
	void save(Archive & archive, const std::shared_ptr<Type> & value, const unsigned int version)
	{
		Type *data = value.get();
		archive << data;
	}

	/// <summary>
	/// Deserializes shared pointer.
	/// </summary>
	/// <param name=archive>Boost serialization archive.</param>
	/// <param name=value>Shared pointer to be deserialized.</param>
	/// <param name=version>Archive version.</param>
	template<class Archive, class Type>
	void load(Archive & archive, std::shared_ptr<Type> & value, const unsigned int version)
	{
		static boost::unordered_map<void*, std::weak_ptr<Type>> hash;

		Type *data;
		archive >> data;

		if (hash[data].expired()) {
			value = std::shared_ptr<Type>(data);
			hash[data] = value;
		}
		else {
			value = hash[data].lock();
		}
	}

	template<class Archive, class Type>
	void serialize(Archive & archive, std::shared_ptr<Type> & value, const unsigned int version)
	{
		 boost::serialization::split_free(archive, value, version);
	}
*/
	/// <summary>
	/// Serializes and deserializes cv:_Point.
	/// </summary>
	/// <param name=archive>Boost serialization archive.</param>
	/// <param name=value>Point object to serialized/deserialized.</param>
	/// <param name=version>Archive version.</param>
	/*template<class Archive, class Type>
	void serialize(Archive & archive, cv::Point_<Type> & value, const unsigned int version)
	{
		archive & value.x;
		archive & value.y;
	}*/

	}
}
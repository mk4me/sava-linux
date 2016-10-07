#include "Camera.h"

#include "utils/Filesystem.h"
#include "utils/CvSerialization.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/access.hpp>

#include <fstream>

namespace config
{

	Camera::Camera()
	{
		m_EmptyMat = cv::Mat(1080, 1920, CV_8U);
		m_EmptyMat = cv::Scalar(255);
	}

	const std::string Camera::c_EmptyString;

	Camera::Item::Item(const std::string& user, const std::string& password, const std::string& ip, const std::string& name)
		: m_User(user)
		, m_Password(password)
		, m_Ip(ip)
		, m_Name(name)
	{
		m_Mask = cv::Mat(1080, 1920, CV_8U);
		m_Mask = cv::Scalar(255);
	}

	template<class Archive>
	void Camera::Item::serialize(Archive & ar, const unsigned int version)
	{
		ar & m_User;
		ar & m_Password;
		ar & m_Ip;
		ar & m_Name;
		ar & m_MaskData;
		ar & m_Mask;
	}

	Camera& Camera::getInstance()
	{
		static Camera instance;
		return instance;
	}

	bool Camera::load()
	{
		try
		{
			std::ifstream ifs(utils::Filesystem::getConfigPath() + "camera.cfg", std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia & m_Cameras;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Camera::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	bool Camera::save() const
	{
		try
		{
			std::ofstream ofs(utils::Filesystem::getConfigPath() + "camera.cfg", std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa & m_Cameras;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "config::Camera::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	int Camera::addCamera(const std::string& user, const std::string& password, const std::string& ip, const std::string& name)
	{
		if (name.empty() || user.empty() || password.empty() || ip.empty())
			return -1;

		if (std::find_if(m_Cameras.begin(), m_Cameras.end(), [&](const Item& c) { return name == c.m_Name; }) != m_Cameras.end())
			return -1;

		m_Cameras.push_back(Item(user, password, ip, name));
		return (int)m_Cameras.size() - 1;
	}

	bool Camera::removeCamera(int num)
	{
		if (num < 0 || num >= m_Cameras.size())
			return false;

		m_Cameras.erase(m_Cameras.begin() + num);
		return true;
	}

	const std::string& Camera::getUser(int num) const
	{
		if (num < 0 || num >= m_Cameras.size())
			return c_EmptyString;
		return m_Cameras[num].m_User;
	}

	bool Camera::setUser(int num, const std::string& user)
	{
		if (num < 0 || num >= m_Cameras.size() || user.empty())
			return false;
		m_Cameras[num].m_User = user;
		return true;
	}

	const std::string& Camera::getPassword(int num) const
	{
		if (num < 0 || num >= m_Cameras.size())
			return c_EmptyString;
		return m_Cameras[num].m_Password;
	}

	bool Camera::setPassword(int num, const std::string& password)
	{
		if (num < 0 || num >= m_Cameras.size() || password.empty())
			return false;
		m_Cameras[num].m_Password = password;
		return true;
	}

	const std::string& Camera::getIp(int num) const
	{
		if (num < 0 || num >= m_Cameras.size())
			return c_EmptyString;
		return m_Cameras[num].m_Ip;
	}

	bool Camera::setIp(int num, const std::string& ip)
	{
		if (num < 0 || num >= m_Cameras.size() || ip.empty())
			return false;
		m_Cameras[num].m_Ip = ip;
		return true;
	}

	const std::string& Camera::getName(int num) const
	{
		if (num < 0 || num >= m_Cameras.size())
			return c_EmptyString;
		return m_Cameras[num].m_Name;
	}

	bool Camera::setName(int num, const std::string& name)
	{
		if (num < 0 || num >= m_Cameras.size() || name.empty())
			return false;

		auto it = std::find_if(m_Cameras.begin(), m_Cameras.end(), [&](const Item& c) { return name == c.m_Name; });
		if (it != m_Cameras.end() && std::distance(m_Cameras.begin(), it) != num)
			return false;

		m_Cameras[num].m_Name = name;
		return true;
	}

	const std::string& Camera::getMaskData(int num) const
	{
		if (num < 0 || num >= m_Cameras.size())
			return c_EmptyString;
		return m_Cameras[num].m_MaskData;
	}

	bool Camera::setMaskData(int num, const std::string& maskData)
	{
		if (num < 0 || num >= m_Cameras.size() || maskData.empty())
			return false;
		m_Cameras[num].m_MaskData = maskData;
		return true;
	}

	bool Camera::setMask(int num, const cv::Mat& mask)
	{
		if (num < 0 || num >= m_Cameras.size() || mask.empty())
			return false;
		m_Cameras[num].m_Mask = mask;
		return true;
	}

	const cv::Mat& Camera::getMask(const std::string& cameraName) const
	{
		auto camera = std::find_if(m_Cameras.begin(), m_Cameras.end(), [&](const Item& c){ return c.m_Name == cameraName; });
		if (camera == m_Cameras.end())
			return m_EmptyMat;
		else
			return camera->m_Mask;
	}
}

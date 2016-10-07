#pragma once
#ifndef Camera_h__
#define Camera_h__

#include <vector>
#include <opencv/cv.h>

namespace boost
{
	namespace serialization
	{
		class access;
	}
}

namespace config
{
	class Camera
	{
	public:
		static Camera& getInstance();

		bool load();
		bool save() const;

		int getNumCameras() const { return (int)m_Cameras.size(); }

		/// <summary>
		/// Adds the camera.
		/// </summary>
		/// <param name="user">User name.</param>
		/// <param name="password">Password.</param>
		/// <param name="ip">Ip of the camera.</param>
		/// <param name="name">Camera unique name.</param>
		/// <returns>Camera index if success, otherwise -1.</returns>
		int addCamera(const std::string& user, const std::string& password, const std::string& ip, const std::string& name);
		bool removeCamera(int num);
		
		const std::string& getUser(int num) const;
		bool setUser(int num, const std::string& user);

		const std::string& getPassword(int num) const;
		bool setPassword(int num, const std::string& password);

		const std::string& getIp(int num) const;
		bool setIp(int num, const std::string& ip);
		
		const std::string& getName(int num) const;
		bool setName(int num, const std::string& name);

		const std::string& getMaskData(int num) const;
		bool setMaskData(int num, const std::string& maskData);

		bool setMask(int num, const cv::Mat& mask);

		const cv::Mat& getMask(const std::string& cameraName) const;

	private:
		Camera();
		~Camera() { }

		struct Item
		{
			friend class boost::serialization::access;

		public:
			Item() { }
			Item(const std::string& user, const std::string& password, const std::string& ip, const std::string& name);
			~Item() { }

			std::string m_User;
			std::string m_Password;
			std::string m_Ip;
			std::string m_Name;
			std::string m_MaskData;
			cv::Mat m_Mask;

			template<class Archive>
			void serialize(Archive & ar, const unsigned int version);
		};

		static const std::string c_EmptyString;
		
		cv::Mat m_EmptyMat;

		std::vector<Item> m_Cameras;
	};
}

#endif // Camera_h__

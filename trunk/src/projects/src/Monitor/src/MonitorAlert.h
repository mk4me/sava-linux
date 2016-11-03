#pragma once

#include "MonitorVideoManager.h"
#include "MonitorRegion.h"

#include <string>
#include <sequence/IVideo.h>
#include <sequence/Cluster.h>

#include "boost\archive\binary_iarchive.hpp"
#include "boost\archive\binary_oarchive.hpp"
#include "boost\serialization\shared_ptr.hpp"
#include "boost/serialization/export.hpp"
#include <boost/serialization/nvp.hpp>

#include <QPolygonF>

/////////////////////////////////////////////////////////////////////////
class MonitorAlert
{

public:

	typedef sequence::IVideo::Timestamp Timestamp;

	typedef sequence::Cluster Cluster;
	typedef std::shared_ptr<Cluster> ClusterPtr;

	typedef sequence::IVideo Video;
	typedef std::shared_ptr<Video> VideoPtr;

	enum EType
	{
		ACTION,
		REGION,
	};

	enum EState
	{
		NONE,
		STARTED,
		FINISHED_UNACCEPTED,
		FINISHED_ACCEPTED,
	};

	MonitorAlert();
	virtual ~MonitorAlert() {};

	virtual bool load(const std::string& filename) = 0;
	virtual bool save(const std::string& filename) const = 0;

	virtual EType getType() const = 0;

	virtual void start() { m_State = STARTED; }
	virtual void finish() { m_State = FINISHED_UNACCEPTED; }
	virtual void accept() { m_State = FINISHED_ACCEPTED; }

	EState getState() const { return m_State; }

	bool isStarted() const { return m_State >= STARTED; }
	bool isFinished() const { return m_State > STARTED; }
	bool isAccepted() const { return m_State >= FINISHED_ACCEPTED; }

	size_t getId() const { return m_Id; }
	void setId(size_t id) { m_Id = id; }

	std::string getName() const { return m_Name; }
	void setName(const std::string& _name) { m_Name = _name; }

	Timestamp getStartTime() const { return m_StartTime; }
	void setStartTime(const Timestamp& _time) { m_StartTime = _time; }

	Timestamp getEndTime() const { return m_EndTime; }
	void setEndTime(const Timestamp& _time) { m_EndTime = _time; }

	Timestamp getAcceptTime() const { return m_AcceptTime; }
	void setAcceptTime(const Timestamp& _time) { m_AcceptTime = _time; }

	std::vector<std::string> getVideos() const { return m_Videos; }
	bool addVideo(const std::string& _video);

	bool isValid() const;

	virtual QPolygonF getRegion(size_t _frame) { return QPolygonF(); }
	virtual QPolygonF getRegion() { return QPolygonF(); }

	static std::string timeToString(const Timestamp& _time);

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_Id;
		ar & m_Name;
		ar & m_StartTime;
		ar & m_EndTime;
		ar & m_AcceptTime;
		ar & m_State;
		ar & m_Videos;
	};


private:
	size_t m_Id;
	std::string m_Name;
	Timestamp m_StartTime;
	Timestamp m_EndTime;
	Timestamp m_AcceptTime;

	EState m_State;

	std::vector<std::string> m_Videos;

};
typedef std::shared_ptr<MonitorAlert> MonitorAlertPtr;




/////////////////////////////////////////////////////////////////////////
template
<typename T>
class MonitorAlertData : public MonitorAlert
{

public:
	MonitorAlertData() : MonitorAlert() {}
	MonitorAlertData(const T& i_Data) : MonitorAlert(), m_Data(i_Data) {}
	MonitorAlertData(const std::string& filename) : MonitorAlert() { load(filename); }

	virtual ~MonitorAlertData() {}

	virtual bool load(const std::string& filename) override
	{
		try
		{
			std::ifstream ifs(filename, std::ios::binary | std::ios::in);
			boost::archive::binary_iarchive ia(ifs);
			ia >> *this;

			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "MonitorAlert::load() exception: " << e.what() << std::endl;
			return false;
		}
	}

	virtual bool save(const std::string& filename) const override
	{
		try
		{
			std::ofstream ofs(filename, std::ios::binary | std::ios::out);
			boost::archive::binary_oarchive oa(ofs);
			oa << *this;
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "MonitorAlert::save() exception: " << e.what() << std::endl;
			return false;
		}
	}

	virtual void accept() override
	{
		setAcceptTime(boost::posix_time::microsec_clock::local_time());
		MonitorAlert::accept();
	}

	T& getData() { return m_Data; }
	void setData(const T& data) { m_Data = data; }

protected:
	T m_Data;

private:
	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(MonitorAlert);
		ar & m_Data;
	}
};





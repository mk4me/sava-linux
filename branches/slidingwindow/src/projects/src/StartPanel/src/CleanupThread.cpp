#include "CleanupThread.h"

#include "config/Directory.h"

#include "utils/Filesystem.h"

CleanupThread::CleanupThread(QObject *parent)
	: QThread(parent)
{

}

CleanupThread::~CleanupThread()
{

}

void CleanupThread::run()
{
	config::Directory::getInstance().load();
	std::string path = config::Directory::getInstance().getTemporaryPath();
	utils::Filesystem::removeContents(path);
}

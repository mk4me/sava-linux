//
// Created by wojtek on 21.11.16.
//

#include <Filesystem.h>
#include <stdexcept>
#include <QtCore/QProcess>
#include "utils/Spawning.h"

namespace utils {

    std::string Spawning::getProcessFilename(const std::string &processName) {
        #ifdef _WIN32
        #ifdef _DEBUG
                return processName + std::string("d.exe");
        #else
                return processName + std::string(".exe");
        #endif
        #else
                //return processName + std::string("d");
                return processName;
        #endif
    }

    std::string Spawning::getProcessFilePath(const std::string &processName) {
        auto pathBase = utils::Filesystem::getAppPath();
        auto path = pathBase + getProcessFilename(processName + std::string("d"));
        if (!utils::Filesystem::exists(path)) {
            path = pathBase + getProcessFilename(processName);
        }
        return path;
    }

    QString Spawning::getProcessFilePathQ(const QString& processName) {
        return QString::fromStdString(getProcessFilePath(processName.toStdString()));
    }

    void Spawning::spawnProcess(const std::string &processName) {
        auto path = getProcessFilePath(processName);
        bool test = QProcess::startDetached(QString::fromStdString(path));
        if (!test) {
            throw std::runtime_error(std::string("Unable spawn ") + processName);
        }
    }

    bool Spawning::trySpawnProcess(const std::string &processName) {
        try {
            spawnProcess(processName);
        } catch (...) {
            return false;
        }

        return true;
    }

}
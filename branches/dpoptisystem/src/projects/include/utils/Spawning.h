//
// Created by wojtek on 21.11.16.
//

#ifndef EDR_UTILS_SPAWNINGUTILS_H
#define EDR_UTILS_SPAWNINGUTILS_H

namespace utils {
    class Spawning {
    public:
        static std::string getProcessFilename(const std::string &processName);

        static std::string getProcessFilePath(const std::string &processName);
        static QString getProcessFilePathQ(const QString& processName);

        static void spawnProcess(const std::string &processName);

        static bool trySpawnProcess(const std::string &processName);
    };
}


#endif //EDR_UTILS_SPAWNINGUTILS_H

#pragma once

#include "fb_separation.h"
#include <vector>
#include <map>
#include <sequence/PathStream.h>
#include <PathDetectionLib/IPathDetector.h>

typedef	std::map<sequence::PathStream::Id, sequence::PathStream::Path> Paths;
typedef std::vector<Paths> PathsVec;
PathsVec path_detection_timinig(const Frames& frames, const RectVect& rv,  std::shared_ptr<clustering::IPathDetector> m_PathDetector);


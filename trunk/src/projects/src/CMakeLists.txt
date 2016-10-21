ADD_PROJECT(utils "CURL;BOOST;QT;OPENCV;LIBGPUJPEG")
ADD_PROJECT(sequence "BOOST;OPENCV")
ADD_PROJECT(config "BOOST;OPENCV")
ADD_PROJECT(cvgui "BOOST;OPENCV")
ADD_PROJECT(Aquisition "config;sequence;utils")
ADD_PROJECT(PathDetection "config;sequence;utils;OPENCV")
ADD_PROJECT(Compression "config;sequence;utils;PathDetection")
ADD_PROJECT(AxisPerfTool "config;sequence;utils")
ADD_PROJECT(PathAnalysisAlgorithms "BOOST;OPENCV;config;cvgui;sequence;utils") #OPENMP;
ADD_PROJECT(PathAnalysis "BOOST;OPENCV;QT;config;cvgui;sequence;utils;PathAnalysisAlgorithms")


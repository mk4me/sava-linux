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

BEGIN_PROJECTS_GROUP("ACTION")
ADD_PROJECT(GbhPipe "OPENCV;VLFEAT;sequence;config;utils" "Action/Pipe/GbhPipe")
ADD_PROJECT(MbhPipe "OPENCV;VLFEAT;sequence;config;utils" "Action/Pipe/MbhPipe")
ADD_PROJECT(SvmPipe "OPENCV;VLFEAT;sequence;config;utils" "Action/Pipe/SvmPipe")
ADD_PROJECT(GetGmmGBH "OPENCV;VLFEAT;sequence;utils" "Action/Train/GetGmmGBH")
ADD_PROJECT(GetGmmMBH "OPENCV;VLFEAT;sequence;utils" "Action/Train/GetGmmMbh")
ADD_PROJECT(GetTrainTestDataGBH "VLFEAT;sequence;utils" "Action/Train/GetTrainTestDataGBH")
ADD_PROJECT(GetTrainTestDataMBH "VLFEAT;sequence;utils" "Action/Train/GetTrainTestDataMBH")
ADD_PROJECT(MergeData "VLFEAT;config;sequence;utils" "Action/Train/MergeData")
ADD_PROJECT(SplitData "VLFEAT;config;sequence;utils" "Action/Train/SplitData")
ADD_PROJECT(SvmTrain "VLFEAT;config;sequence;utils" "Action/Train/SvmTrain")
ADD_PROJECT(TrainManager "VLFEAT;config;sequence;utils" "Action/Train/TrainManager")
ADD_PROJECT(DatabaseBuilder "VLFEAT;config;sequence;utils" "Action/Train/DatabaseBuilder")
END_PROJECTS_GROUP()


ADD_PROJECT(network "BOOST;sequence;utils")
ADD_PROJECT(StartPanel "BOOST;QT;OPENCV;config;utils;network")
ADD_PROJECT(SplitSetter "BOOST;sequence;utils")
#ADD_PROJECT(Monitor "utils;BOOST;OPENCV;config;sequence;utils;network")



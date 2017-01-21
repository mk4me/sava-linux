#include "svm.h"
#include "chiSquareDist.h"
#include "biostream.h" 
#include "biistream.h" 
#include "formatBinaryStream.h"

#include "utils/Filesystem.h"
#include "utils/Database.h"

#include "config/Action.h"
#include "config/Glossary.h"

#include <opencv/cxcore.h>
#include <opencv/cv.h>

#include <boost/filesystem.hpp>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "omp.h"

#include "dputils.h" 
#include "dplog.h"

using namespace std;
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

//#define EVAL_CPP   //Binary-class Cross Validation with Different Criteria(precision, recall, F-score, AUC...)
#ifdef EVAL_CPP 
	#include "eval.h"
#endif

//DEFAULT PARAMETERS
//THESE BELOW ARE ONLY FOR DISPLAYING
std::string dataNumP = "150000"; //number of total features for finding clusters by kmeans. 
//due to the too many training features(over millions), we normally random-choose "dataNum" from all training features
std::string _maxFramesP = "160"; //maxium frames per video. if the video has more frames, it needs to split the video to do multiple processing to avoid memory overflow
std::string pSamplesP = "100";  //number of features chosen from each clip to train gmm
std::string xSizeP = "2";
std::string ySizeP = "2";
std::string tSizeP = "2";
std::string numBinsP = "8";
std::string rootFactorP = "2";
std::string partFactorP = "8";
std::string _numClustersPGbhP = "128";
std::string _numClustersPMbhP = "128";
std::string samNumP = "10000";
//KERNEL and C are NOT only for displaying
std::string kernel = "LINEAR";
std::string C = "10";
double CE = 10; //C in SVM

static int (*info)(const char *fmt,...) = &printf;
void print_null(const char *s) {}
void read_binary(const string * fileName, const int numClass) ;
void exit_with_help()
{
	printf(
	"Usage: svm-train [options] training_set_file [model_file]\n"
	"options:\n"
	"-s svm_type : set type of SVM (default 0)\n"
	"	0 -- C-SVC		(multi-class classification)\n"
	"	1 -- nu-SVC		(multi-class classification)\n"
	"	2 -- one-class SVM\n"
	"	3 -- epsilon-SVR	(regression)\n"
	"	4 -- nu-SVR		(regression)\n"
	"-t kernel_type : set type of kernel function (default 2)\n"
	"	0 -- linear: u'*v\n"
	"	1 -- polynomial: (gamma*u'*v + coef0)^degree\n"
	"	2 -- radial basis function: exp(-gamma*|u-v|^2)\n"
	"	3 -- sigmoid: tanh(gamma*u'*v + coef0)\n"
	"	4 -- precomputed kernel (kernel values in training_set_file)\n"
	"	5 -- intersection (min): min(u,v)\n"
	"	6 -- chi-squared: 2uv/(u+v)\n"
	"	7 -- Jenson-Shannon's: u/2log((u+v)/u)) + v/2log((u+v)/v))\n"
	"-d degree : set degree in kernel function (default 3)\n"
	"-g gamma : set gamma in kernel function (default 1/num_features)\n"
	"-r coef0 : set coef0 in kernel function (default 0)\n"
	"-c cost : set the parameter C of C-SVC, epsilon-SVR, and nu-SVR (default 1)\n"
	"-n nu : set the parameter nu of nu-SVC, one-class SVM, and nu-SVR (default 0.5)\n"
	"-p epsilon : set the epsilon in loss function of epsilon-SVR (default 0.1)\n"
	"-m cachesize : set cache memory size in MB (default 100)\n"
	"-e epsilon : set tolerance of termination criterion (default 0.001)\n"
	"-h shrinking : whether to use the shrinking heuristics, 0 or 1 (default 1)\n"
	"-b probability_estimates : whether to train a SVC or SVR model for probability estimates, 0 or 1 (default 0)\n"
	"-wi weight : set the parameter C of class i to weight*C, for C-SVC (default 1)\n"
	"-v n: n-fold cross validation mode\n"
	"-q : quiet mode (no outputs)\n"
	"-x : read binary input training file with number of words\n"
	"-f : path to data files\n"
	);
	exit(1);
}

void exit_input_error(int line_num)
{
	fprintf(stderr,"Wrong input format at line %d\n", line_num);
	exit(1);
}

void parse_command_line(int argc, char **argv, char *input_file_name, char *model_file_name, bool& binaryFile, int& numWords, double C = 32.5, std::string kernel = "LINEAR");
void read_problem(const char *filename);


int getNumClasses();

struct svm_parameter param;		// set by parse_command_line
struct svm_problem prob;		// set by read_problem
vector< svm_model *> model;
struct svm_node *x_space;
int *classId;

int nr_fold;
string dataPath;

static char *line0 = NULL;
static int max_line_len;


int main(int argc, char **argv)
{
	//Parse OPP parameters
	std::string coreParams = dp::oppGetParamsFromArgs(argc, argv);

	//Use Dp logs
	dp::dpLog log;
	log.initLogFile(coreParams);

	log.dbg("<cpp>params:");
	log.dbgl(coreParams);

	std::string testNameStr = dp::oppGetValueForKey("testName", coreParams);
	log.dbg("<cpp>testNameStr:");
	log.dbgl(testNameStr);

	std::string mergedataOutDir = dp::oppGetValueForKey("mergedataOutDir", coreParams) + "/";
	log.dbg("<cpp>mergedataOutDir:");
	log.dbgl(mergedataOutDir);

	std::string trainOutDir = dp::oppGetValueForKey("trainOutDir", coreParams) + "/";
	log.dbg("<cpp>trainOutDir:");
	log.dbgl(trainOutDir);

	std::string dbOutDir = dp::oppGetValueForKey("dbOutDir", coreParams) + "/";
	log.dbg("<cpp>dbOutDir:");
	log.dbgl(dbOutDir);

	// parameters 
	if (!dp::oppGetValueForKey("kernel", coreParams).empty())
	{ 
	kernel = dp::oppGetValueForKey("kernel", coreParams);
	log.dbg("<cpp>kernel:");
	log.dbgl(kernel);
	}

	if (!dp::oppGetValueForKey("C", coreParams).empty())
	{ 
	std::string C = dp::oppGetValueForKey("C", coreParams);
	log.dbg("<cpp>C:");
	log.dbgl(C);
	CE = atof(C.c_str());
	}
	//parameters for displaying
	if (!dp::oppGetValueForKey("dataNumP", coreParams).empty())
	{
		dataNumP = dp::oppGetValueForKey("dataNumP", coreParams);
		log.dbg("<cpp>dataNumP:");
		log.dbgl(dataNumP);
	}

	if (!dp::oppGetValueForKey("_maxFramesP", coreParams).empty())
	{ 
		_maxFramesP = dp::oppGetValueForKey("_maxFramesP", coreParams);
		log.dbg("<cpp>_maxFramesP:");
		log.dbgl(_maxFramesP);
	}

	if (!dp::oppGetValueForKey("pSamplesP", coreParams).empty())
	{ 
		pSamplesP = dp::oppGetValueForKey("pSamplesP", coreParams);
		log.dbg("<cpp>pSamplesP:");
		log.dbgl(pSamplesP);
	}

	if (!dp::oppGetValueForKey("xSizeP", coreParams).empty())
	{
		xSizeP = dp::oppGetValueForKey("xSizeP", coreParams);
		log.dbg("<cpp>xSizeP:");
		log.dbgl(xSizeP);
	}

	if (!dp::oppGetValueForKey("ySizeP", coreParams).empty())
	{ 
		ySizeP = dp::oppGetValueForKey("ySizeP", coreParams);
		log.dbg("<cpp>ySizeP:");
		log.dbgl(ySizeP);
	}

	if (!dp::oppGetValueForKey("tSizeP", coreParams).empty())
	{ 
		tSizeP = dp::oppGetValueForKey("tSizeP", coreParams);
		log.dbg("<cpp>tSizeP:");
		log.dbgl(tSizeP);
	}

	if (!dp::oppGetValueForKey("numBinsP", coreParams).empty())
	{ 
		numBinsP = dp::oppGetValueForKey("numBinsP", coreParams);
		log.dbg("<cpp>numBinsP:");
		log.dbgl(numBinsP);
	}

	if (!dp::oppGetValueForKey("_numClustersPGbh", coreParams).empty())
	{ 
		_numClustersPGbhP = dp::oppGetValueForKey("_numClustersPGbh", coreParams);
		log.dbg("<cpp>_numClustersPGbh:");
		log.dbgl(_numClustersPGbhP);
	}

	if (!dp::oppGetValueForKey("_numClustersPMbh", coreParams).empty())
	{ 
		_numClustersPMbhP = dp::oppGetValueForKey("_numClustersPMbh", coreParams);
		log.dbg("<cpp>_numClustersPMbh:");
		log.dbgl(_numClustersPMbhP);
	}
	if (!dp::oppGetValueForKey("rootFactorP", coreParams).empty())
	{ 
		rootFactorP = dp::oppGetValueForKey("rootFactorP", coreParams);
		log.dbg("<cpp>rootFactorP:");
		log.dbgl(rootFactorP);
	}

	if (!dp::oppGetValueForKey("partFactorP", coreParams).empty())
	{ 
		partFactorP = dp::oppGetValueForKey("partFactorP", coreParams);
		log.dbg("<cpp>partFactorP:");
		log.dbgl(partFactorP);
	}

	if (!dp::oppGetValueForKey("samNumP", coreParams).empty())
	{ 
		samNumP = dp::oppGetValueForKey("samNumP", coreParams);
		log.dbg("<cpp>samNumP:");
		log.dbgl(samNumP);
	}
	log.closeLogFile();

	int _numClustersPMbh = atoi(_numClustersPMbhP.c_str());
	int _numClustersPGbh = atoi(_numClustersPGbhP.c_str());
	int xSize = atoi(xSizeP.c_str());
	int ySize = atoi(ySizeP.c_str());
	int tSize = atoi(tSizeP.c_str());
	int numBins = atoi(numBinsP.c_str());
	int rootFactor = atoi(rootFactorP.c_str());
	int partFactor = atoi(partFactorP.c_str());

	std::string databaseOutDir = utils::Filesystem::getAppPath() + dbOutDir;
	utils::Database::setDatabaseDir(databaseOutDir);

	char input_file_name[1024];
	char model_file_name[1024];
	const char *error_msg;
	
	bool binaryFile = 0;  //added by Feng for read binary training file. defualt is "0" (original data format. if arg -x .., read binary file)
	int numW;
	int nCls = getNumClasses();

	parse_command_line(argc, argv, input_file_name, model_file_name, binaryFile, numW, CE, kernel);
	//config::Action::getInstance().load();
	int numWordsGbh = 2 * (((xSize * ySize * tSize * numBins) / rootFactor) + ((8 / partFactor) * (xSize * ySize * tSize * numBins))) * _numClustersPGbh;
	int numWordsMbh = 2 * 2 * (((xSize * ySize * tSize * numBins) / rootFactor) + ((8 / partFactor) * (xSize * ySize * tSize * numBins))) * _numClustersPMbh;
	int numWords = numWordsGbh + numWordsMbh; //config::Action::getInstance().getDescriptorSize();
	std::cout << "numwords" << numWords << std::endl;
	//dataPath = config::Action::getInstance().getDescriptorPath(); //ZMIENIC PODAC SCIEZKE WYJSCIOWA MERGE DATA
	dataPath = utils::Filesystem::getAppPath() + mergedataOutDir;
	string *fileName = new string[nCls];
	if(binaryFile)
	{
		for (int i = 0; i < nCls; i++)
			fileName[i] = dataPath + "pwords" + std::to_string(i) + "_" + std::to_string(numWords) + ".dat";

		read_binary(fileName, nCls);
		std::cout<<"done read binary file pwords."<<std::endl;
		strcpy(model_file_name,"model.dat");
		delete []fileName;
	}
	else
	{
		//read_problem(input_file_name);
		std::cerr<<"Wrong! Current version only support binary training file inputs"<<std::endl;
		cin.get();cin.get();
		exit(2);
	}

	error_msg = svm_check_parameter(&prob,&param);

	if(error_msg)
	{
		fprintf(stderr,"ERROR: %s\n",error_msg);
		exit(1);
	}

	double *values = new double[nCls];
	double maxVal;
	struct svm_node *x;

	model.clear();
	//std::string outPath = utils::Filesystem::getDataPath() + "action/svm/"; //ZMIENIC NA WYJSCIOWA SVM
	std::string outPath = utils::Filesystem::getAppPath() + trainOutDir;
	boost::filesystem::create_directories(outPath);
	//learning the models (number of classes)
	for(int i = 0; i < nCls; i++)
	{
		for(int i0 = 0; i0 <prob.l; i0++)
		{
			if (classId[i0] == i)
				prob.y[i0] = 0;
			else
				prob.y[i0] = 1;
		}

		model.push_back(svm_train(&prob,&param));

		std::string modelFile = outPath + "model_" + std::to_string(i) + ".dat";
		if (svm_save_model(modelFile.c_str(), model[i]))
		{
			fprintf(stderr, "can't save model to file %s\n", modelFile);
			discoverUO::wait(5);
		}
		
		std::cout<<"Done training class "<<i+1<<std::endl;
	}
	free(prob.y);
	free(prob.x);
	free(classId);
	svm_destroy_param(&param);

	
	//done the training!

	fileName = new string[nCls];
	for (int j = 0; j < nCls; j++)
		fileName[j] = dataPath + "bagWord" + std::to_string(j) + "_" + std::to_string(numWords) + ".dat";

	int correct = 0;
	int total = 0;
	double error = 0;
	double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;
	int svm_type=svm_get_svm_type(model[0]);
	using namespace cv;
	cv::Mat pMat;
	BinClusterInStream *pFile;
	cv::Mat *pMatF = new Mat[nCls];
	for (int i = 0; i < nCls; i++)
	{
		pFile = new BinClusterInStream(fileName[i]);
		pFile->read(pMat);
		pMatF[i].create(pMat.rows, pMat.cols, CV_32FC1);
		//label[i] = Mat(pMat.rows, 1, CV_32SC1, Scalar_<int>(i));

		//cout<<label[i].at<int>(200,0)<<"\n";
		if (pMat.type() == CV_32FC1)
			pMat.copyTo(pMatF[i]);
		else
		{
			fprintf(stderr,"Wrong input format: sample_serial_number out of range\n");
			exit(1);
		}

		delete pFile;
		pMat.release();
	}
	delete []fileName;
	x = (struct svm_node *) malloc((pMatF[0].cols+1)*sizeof(struct svm_node));
	FILE *output = fopen((outPath + "results.txt").c_str(),"w");
	fpos_t position;
	fgetpos(output, &position);
	if (svm_type==NU_SVR || svm_type==EPSILON_SVR)
	{
		fprintf(output,"                                                            \n");
		fprintf(output,"                                                            \n");
		fprintf(output,"                                                            \n");
		
	}
	else
	{
		config::Glossary::getInstance().load();
		fprintf(output, "                                                            \n");
		fprintf(output, "                                                            \n");
		fprintf(output, "                                                            \n");
		fprintf(output, "Classes:\n");
		for (int i = 0; i < nCls; ++i)
			fprintf(output, "%d - %s\n", i, config::Glossary::getInstance().getTrainedActionName(i));
		fprintf(output, "\n\n");
		fprintf(output,"tests\ttarget\tpredict\n");
	}

	int target_label, label;
	float *data;
	int j0;

	for(int id = 0; id < nCls; id++)
	{
		target_label = id;
		for(int j = 0; j < pMatF[id].rows; j++)  //number of features
		{
			data = pMatF[id].ptr<float>(j);

			for(j0 = 0; j0 < pMatF[id].cols; j0++)  //feature dimensions
			{
				x[j0].index = j0+1;  //feature id beginning from "1" while j0 beginning from "0"
				x[j0].value = data[j0];
			}
			x[j0].index = -1;

			
			//svm_predict_values(model[0], x, &maxVal);
			svm_predict_values(model[0], x, &values[0]);
			maxVal = values[0];
			label = 0;
			//classify x with nCls models and save the results into values[i]

			//For binary libsvm( before version 3.17), 
			//since internally class labels are ordered by their first occurrence in the training set, 
			//we need to set the classified values to negative after the 1st class
			for(int i = 1; i < nCls; i++)
			{
				svm_predict_values(model[i], x, &values[i]);
				//std::cout<<model[i]->nSV[0]<<" "<<model[i]->nSV[1]<<endl;
				values[i] = -values[i];

				if (maxVal < values[i])
				{
					maxVal = values[i];
					label = i;
				}
			}

	//std::cout<<values[0]<<" "<<values[1]<<" "<<values[2]<<" "<<values[3]<<" "<<maxVal<<" "<<label<<endl;
			fprintf(output,"%d\t%d\t%d",j,id, label);
			if(label == target_label)
				fprintf(output,"\n");
			else
				fprintf(output,"  Wrong!\n");

			if(label == target_label)
				++correct;
			error += (label-target_label)*(label-target_label);
			sump += label;
			sumt += target_label;
			sumpp += label*label;
			sumtt += target_label*target_label;
			sumpt += label*target_label;
			++total;
		}

		std::cout<<"Done predict class "<<id+1<<"!\n";
	}
	double accur = (double)correct / total * 100;
	info("Accuracy = %g%% (%d/%d) (classification)\n",
		accur, correct, total);
		//(double)correct/total*100,correct,total);
	fsetpos (output, &position);
	fprintf(output,"Accuracy = %g%% (%d/%d) (classification)",
		(double)correct/total*100,correct,total);
	delete []pMatF;

	std::cout<<"\nDone testing with kernel = "<<param.kernel_type<<" and C = "<<param.C<<std::endl;
	for (int i = 0; i < model.size(); i++)
		svm_free_and_destroy_model(&model[i]);

	// Write OPP string to result file
	log.initResultFile(coreParams);
	log.addResult("testName=" + testNameStr + ";accuracy=" + std::to_string(accur) + ";_numClustersMbh=" + _numClustersPMbhP + ";_numClustersGbh=" + _numClustersPGbhP + ";kernel=" + kernel + ";C=" + C + ";xSizeP=" + xSizeP + ";ySizeP=" + ySizeP + ";tSizeP" + tSizeP + ";numBinsP=" + numBinsP + ";samNumP=" + samNumP + ";rootFactorP=" + rootFactorP + ";partFactorP=" + partFactorP + ";_maxFramesP=" + _maxFramesP + ";pSamplesP=" + pSamplesP + ";dataNumP=" + dataNumP);
	log.closeResultFile();

	std::cout<<"\nDone clear model "<<model.size()<<std::endl;

	free(x_space);
	//free(x_space);
	free(line0);
	
	//model.clear();
	delete []values;
	return 0;
}


void parse_command_line(int argc, char **argv, char *input_file_name, char *model_file_name, bool& binaryFile, int& numWords, double C, std::string kernel)
{
	int i;
	void (*print_func)(const char*) = NULL;	// default printing to stdout

	// default values
	param.svm_type = C_SVC;
	//param.kernel_type = RBF;
	//param.kernel_type = CHISQUARED;
	std::map<std::string, int> m;
	m["LINEAR"] = static_cast<int>(LINEAR);
	m["POLY"] = static_cast<int>(POLY);
	m["RBF"] = static_cast<int>(RBF);
	m["SIGMOID"] = static_cast<int>(SIGMOID);
	m["INTERSECTION"] = static_cast<int>(INTERSECTION);
	m["CHISQUARED"] = static_cast<int>(CHISQUARED);
	m["JS"] = static_cast<int>(JS);
	param.kernel_type = m.at(kernel);//kernel;
	param.degree = 3;
	param.gamma = 0.00225;	// param.gamma = 0;	// 1/num_features
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = C;		//param.C = 1;
	param.eps = 1e-3;
	//param.eps = 1e-5;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL; //set the parameter C of class i to weight*C, for C-SVC (default 1)

	binaryFile = true;

	// parse options
	for(i=1;i<argc;i++)
	{
		if(argv[i][0] != '-') break;
		if(++i>=argc)
			exit_with_help();
		switch(argv[i-1][1])
		{
			case 'x':				// Added by Feng to read binaryFile, if arg -x ..., read binary file
				binaryFile = true; 
				numWords = atoi(argv[i]);
				break;
			case 's':
				param.svm_type = atoi(argv[i]);
				break;
			case 't':
				param.kernel_type = atoi(argv[i]);
				break;
			case 'd':
				param.degree = atoi(argv[i]);
				break;
			case 'g':
				param.gamma = atof(argv[i]);
				break;
			case 'r':
				param.coef0 = atof(argv[i]);
				break;
			case 'n':
				param.nu = atof(argv[i]);
				break;
			case 'm':
				param.cache_size = atof(argv[i]);
				break;
			case 'c':
				param.C = atof(argv[i]);
				break;
			case 'e':
				param.eps = atof(argv[i]);
				break;
			case 'p':
				param.p = atof(argv[i]);
				break;
			case 'h':
				param.shrinking = atoi(argv[i]);
				break;
			case 'b':
				param.probability = atoi(argv[i]);
				break;
			case 'q':
				print_func = &print_null;
				i--;
				break;
			case 'w':
				++param.nr_weight;
				param.weight_label = (int *)realloc(param.weight_label,sizeof(int)*param.nr_weight);
				param.weight = (double *)realloc(param.weight,sizeof(double)*param.nr_weight);
				param.weight_label[param.nr_weight-1] = atoi(&argv[i-1][2]);
				param.weight[param.nr_weight-1] = atof(argv[i]);
				break;
			case 'f':
				dataPath = utils::Filesystem::unifyPath(argv[i]);
				break;
			default:
				fprintf(stderr,"Unknown option: -%c\n", argv[i-1][1]);
				exit_with_help();
		}
	}

	svm_set_print_string_function(print_func);

	// determine filenames

	if(!binaryFile)
	{
		if(i>=argc)
			exit_with_help();

		strcpy(input_file_name, argv[i]);

		if(i<argc-1)
			strcpy(model_file_name,argv[i+1]);
		else
		{
			char *p = strrchr(argv[i],'/');
			if(p==NULL)
				p = argv[i];
			else
				++p;
			sprintf(model_file_name,"%s.model",p);
		}
	}
}


void read_binary(const string * fileName, const int numClass) 
{
	using namespace cv;
	cv::Mat pMat;
	prob.l = 0;
	BinClusterInStream *pFile;
	cv::Mat *pMatF = new Mat[numClass];

	for (int i = 0; i < numClass; i++)
	{
		pFile = new BinClusterInStream(fileName[i]);
		pFile->read(pMat);
		pMatF[i].create(pMat.rows, pMat.cols, CV_32FC1);

		if (pMat.type() == CV_32FC1)
			pMat.copyTo(pMatF[i]);
		else
		{
			fprintf(stderr,"Wrong input format: sample_serial_number out of range\n");
			exit(1);
		}
		prob.l += pMat.rows;

		delete pFile;
		pMat.release();
	}

	int elements = prob.l * (pMatF[0].cols + 1);
	float *data;

	classId = Malloc(int,prob.l);
	prob.y = Malloc(double,prob.l);
	prob.x = Malloc(struct svm_node *,prob.l);
	x_space = Malloc(struct svm_node,elements);

	int id = 0, jd = 0;
	for (int i = 0; i < numClass; i++)  //class
	{
		for(int j = 0; j < pMatF[i].rows; j++, id++)  //number of features
		{
			prob.x[id] = &x_space[jd];
			//prob.y[id] = i;   //class label
			classId[id] = i;   //class label
			data = pMatF[i].ptr<float>(j);

			for(int j0 = 0; j0 < pMatF[i].cols; j0++, jd++)  //feature dimensions
			{
				x_space[jd].index = j0+1;   //feature id beginning from "1" while j0 beginning from "0"
				x_space[jd].value = data[j0];
			}
			x_space[jd++].index = -1;
		}
	}
	if(param.kernel_type == CHISQUARED)  //compute mean chi-square dist and save it to 1/param.gamma
	{
		for (int i = 0; i < numClass; i++)  //class
			pMat.push_back(pMatF[i]);
		double sumDist = 0;
		int count = 0;
		float *p1, *p2;
		for( int i = 0; i < pMat.rows; i++)
		{
			p1 = pMat.ptr<float>(i);
			for(int j = i+1; j < pMat.rows; j++)
			{
				p2 = pMat.ptr<float>(j);
				sumDist += chiSquareDistMP<float, float>(p1, p2, pMat.cols);
				count++;
			}
		}
		param.gamma = (double)count/(sumDist*0.5);
		std::cout<<param.gamma<<" "<<pMat.rows<<" "<<count<<" "<<sumDist<<std::endl;
	}
	else if(param.gamma == 0 && pMatF[0].cols > 0)
		param.gamma = 1.0/pMatF[0].cols;

	for (int i = 0; i < numClass; i++)
		pMatF[i].release();
	delete []pMatF;
}

int getNumClasses()
{
	utils::Database database;
	if (!database.load())
	{
		std::cerr << "Cant load video database!\n";
		discoverUO::wait(5);
	}
	return (int)database.getNumActions();
}
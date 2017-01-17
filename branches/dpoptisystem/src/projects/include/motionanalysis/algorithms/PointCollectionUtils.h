#pragma once
#include "flann/flann.hpp"


namespace motion_analysis {

template <typename Collection, typename MatType>
void segmentClusterToArray(const Collection& collection, MatType* matrix, int row){
	int i = 0;
	for (auto itPoint = collection.begin(); itPoint != collection.end(); ++itPoint){
		matrix[i++] = (MatType)(*itPoint).x;
		matrix[i++] = (MatType)(*itPoint).y;
	}
}


template <typename Collection, typename MatType>
void collectionToFlannMatrix(const Collection& collection, flann::Matrix<MatType>& matrix, int row) {
	int col=0;
	const int base =collection.begin()->t; //expected time;
	auto itLast=collection.begin();
	float meanX=collection.mean.x;
	float meanY=collection.mean.y;

	for (auto itPoint=collection.begin(); itPoint!=collection.end(); ++itPoint){
		if (itPoint->t>base+col){
			const int gapLength=itPoint->t-base-col;
			float difX=(itPoint->x-itLast->x)/gapLength;
			float difY=(itPoint->y-itLast->y)/gapLength;
			int gapNr=0;
			while (itPoint->t>base+col){
				matrix[row][2*col]=(*itPoint).x-difX*gapNr-meanX;
				matrix[row][2*col+1]=(*itPoint).y-difY*gapNr-meanY;
				col+=1;
				gapNr+=1;
			}
		}
		matrix[row][2*col]=(MatType)(*itPoint).x-(float)meanX;
		matrix[row][2*col+1]=(MatType)(*itPoint).y-(float)meanY;
		col+=1;
		itLast=itPoint;
	}
}



template <typename Collection, typename MatType>
void ungappedCollectionToFlannMatrix(const Collection& collection, flann::Matrix<MatType>& matrix, int row) {
	int col = 0;
	auto itLast = collection.begin();
	float meanX = collection.mean.x;
	float meanY = collection.mean.y;

	for (auto itPoint = collection.begin(); itPoint != collection.end(); ++itPoint){
		matrix[row][2 * col] = (MatType)(*itPoint).x - (float)meanX;
		matrix[row][2 * col + 1] = (MatType)(*itPoint).y - (float)meanY;
		col += 1;
		itLast = itPoint;
	}
}

}
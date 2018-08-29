/*
 * FileSink.cpp
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#include "FileSink.h"
#include "GroupsockHelper.h"
#include "OutputFile.h"
#include "strDup.h"
#include "DP_RTSP_CLIENT_Interface.h"
#include <iostream>
#include <string.h>
using namespace std;

////////// FileSink //////////

FileSink::FileSink(UsageEnvironment& env, FILE* fid, unsigned bufferSize,
		char const* perFrameFileNamePrefix, unsigned short clientID) :
		MediaSink(env), fOutFid(fid), fBufferSize(bufferSize), fSamePresentationTimeCounter(
				0), _cliID(clientID) {
	fBuffer = new unsigned char[bufferSize];
	if (perFrameFileNamePrefix != NULL) {
		fPerFrameFileNamePrefix = strDup(perFrameFileNamePrefix);
		fPerFrameFileNameBuffer =
				new char[strlen(perFrameFileNamePrefix) + 100];
	} else {
		fPerFrameFileNamePrefix = NULL;
		fPerFrameFileNameBuffer = NULL;
	}
	fPrevPresentationTime.tv_sec = ~0;
	fPrevPresentationTime.tv_usec = 0;
}

FileSink::~FileSink() {
	delete[] fPerFrameFileNameBuffer;
	delete[] fPerFrameFileNamePrefix;
	delete[] fBuffer;
	if (fOutFid != NULL)
		fclose(fOutFid);
}

FileSink* FileSink::createNew(UsageEnvironment& env, char const* fileName,
		unsigned bufferSize, unsigned short clientID, Boolean oneFilePerFrame) {
	do {
		FILE* fid;
		char const* perFrameFileNamePrefix;
		if (oneFilePerFrame) {
			// Create the fid for each frame
			fid = NULL;
			perFrameFileNamePrefix = fileName;
		} else {
			// Normal case: create the fid once
			fid = OpenOutputFile(env, fileName);
			if (fid == NULL)
				break;
			perFrameFileNamePrefix = NULL;
		}

		return new FileSink(env, fid, bufferSize, perFrameFileNamePrefix,
				clientID);
	} while (0);

	return NULL;
}

//上面的fAfterGettingFunc为我们自己注册的函数，如果运行的是testProgs中的openRTSP实例，
//则该函数指向下列代码中通过调用getNextFrame（）注册的afterGettingFrame（）函数
Boolean FileSink::continuePlaying() {
	if (fSource == NULL)
		return False;

	//从上面的代码中可以看到getNextFrame()函数的第一个参数为分别在各自类中定义的buffer，
	//我们继续以openRTSP为运行程序来分析，fBuffer为FileSink类里定义的指针：unsigned char* fBuffer
	fSource->getNextFrame(fBuffer, fBufferSize, afterGettingFrame, this,
			onSourceClosure, this);
	cout << "fBuffer:: len :: " << strlen((char*) fBuffer) << " buffer "
			<< fBufferSize << " iD::::::::::continuePlaying :: :" << _cliID
			<< endl;

	return True;
}

void FileSink::afterGettingFrame(void* clientData, unsigned frameSize,
		unsigned numTruncatedBytes, struct timeval presentationTime,
		unsigned /*durationInMicroseconds*/) {
	FileSink* sink = (FileSink*) clientData;
	sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime);
}

void FileSink::addData(unsigned char const* data, unsigned dataSize,
		struct timeval presentationTime) {
//	cout << "addData 1 " << endl;
	if (fPerFrameFileNameBuffer != NULL && fOutFid == NULL) {
		// Special case: Open a new file on-the-fly for this frame
		if (presentationTime.tv_usec == fPrevPresentationTime.tv_usec
				&& presentationTime.tv_sec == fPrevPresentationTime.tv_sec) {
			// The presentation time is unchanged from the previous frame, so we add a 'counter'
			// suffix to the file name, to distinguish them:
			sprintf(fPerFrameFileNameBuffer, "%s-%lu.%06lu-%u",
					fPerFrameFileNamePrefix, presentationTime.tv_sec,
					presentationTime.tv_usec, ++fSamePresentationTimeCounter);
		} else {
			sprintf(fPerFrameFileNameBuffer, "%s-%lu.%06lu",
					fPerFrameFileNamePrefix, presentationTime.tv_sec,
					presentationTime.tv_usec);
			fPrevPresentationTime = presentationTime; // for next time
			fSamePresentationTimeCounter = 0; // for next time
		}
		fOutFid = OpenOutputFile(envir(), fPerFrameFileNameBuffer);
	}

	// Write to our file:
#ifdef TEST_LOSS
	static unsigned const framesPerPacket = 10;
	static unsigned const frameCount = 0;
	static Boolean const packetIsLost;
	if ((frameCount++)%framesPerPacket == 0) {
		packetIsLost = (our_random()%10 == 0); // simulate 10% packet loss #####
	}

	if (!packetIsLost)
#endif
	if (fOutFid != NULL && data != NULL) {
		fwrite(data, 1, dataSize, fOutFid);
		cout << "dataSize : " << dataSize << endl;
	}
}

void FileSink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
		struct timeval presentationTime) {
	if (numTruncatedBytes > 0) {
		envir()
				<< "FileSink::afterGettingFrame(): The input frame data was too large for our buffer size ("
				<< fBufferSize << ").  " << numTruncatedBytes
				<< " bytes of trailing data was dropped!  Correct this by increasing the \"bufferSize\" parameter in the \"createNew()\" call to at least "
				<< fBufferSize + numTruncatedBytes << "\n";
	}

#if 1 //
	///fwrite data 将数据保存到文件中
	addData(fBuffer, frameSize, presentationTime);

	if (fOutFid == NULL || fflush(fOutFid) == EOF) {
		// The output file has closed.  Handle this the same way as if the input source had closed:
		if (fSource != NULL)
			fSource->stopGettingFrames();
		onSourceClosure();
		return;
	}

	if (fPerFrameFileNameBuffer != NULL) {
		if (fOutFid != NULL) {
			fclose(fOutFid);
			fOutFid = NULL;
		}
	}
#endif

	// Then try getting the next frame:
	continuePlaying();
}

void FileSink::afterGettingFrameGetData(unsigned frameSize,
		unsigned numTruncatedBytes, struct timeval presentationTime) {
	if (numTruncatedBytes > 0) {
		envir()
				<< "FileSink::afterGettingFrame(): The input frame data was too large for our buffer size ("
				<< fBufferSize << ").  " << numTruncatedBytes
				<< " bytes of trailing data was dropped!  Correct this by increasing the \"bufferSize\" parameter in the \"createNew()\" call to at least "
				<< fBufferSize + numTruncatedBytes << "\n";
	}

	DP_RTSP_CLIENT_FRAME_DATA_S stFrameData(_cliID);
	stFrameData.pu8Data = new unsigned char[frameSize];
	memset(stFrameData.pu8Data, 0, frameSize);
//	if (video) {
//		stFrameData.pu8Data[0] = 0x00;
//		stFrameData.pu8Data[1] = 0x00;
//		stFrameData.pu8Data[2] = 0x00;
//		stFrameData.pu8Data[3] = 0x01;
//		memcpy(stFrameData.pu8Data + 4, fBuffer, frameSize);
//	}
	DP_RTSP_CLIENT_Client::_mDataQueueSet[_cliID]->DSP_PutData(&stFrameData,
			sizeof(DP_RTSP_CLIENT_FRAME_DATA_S));

#if 0 //
	///fwrite data 将数据保存到文件中
	addData(fBuffer, frameSize, presentationTime);

	if (fOutFid == NULL || fflush(fOutFid) == EOF) {
		// The output file has closed.  Handle this the same way as if the input source had closed:
		if (fSource != NULL)
		fSource->stopGettingFrames();
		onSourceClosure();
		return;
	}

	if (fPerFrameFileNameBuffer != NULL) {
		if (fOutFid != NULL) {
			fclose(fOutFid);
			fOutFid = NULL;
		}
	}
#endif

	// Then try getting the next frame:
	continuePlaying();
}

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
#include "Logger.h"
using namespace FrameWork;
//
#include "Rtsp_server.h"

using namespace std;

////////// FileSink //////////
int flag = 0;
FileSink::FileSink(UsageEnvironment& env, FILE* fid, unsigned bufferSize,
		char const* perFrameFileNamePrefix, unsigned short clientID,
		CommonPlay *cpObj) :
		MediaSink(env, cpObj), fOutFid(fid), fBufferSize(bufferSize), fSamePresentationTimeCounter(
				0), _cliID(clientID), fGetIDRFrame(false), tvStart() {
	fBuffer = new DP_U8[bufferSize];
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
	fStartCode[0] = 0x00;
	fStartCode[1] = 0x00;
	fStartCode[2] = 0x00;
	fStartCode[3] = 0x01;
	////0000000000000
	Logger::GetInstance().Debug(
			"_cliIDZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ %d", clientID);
}

FileSink::~FileSink() {
	delete[] fPerFrameFileNameBuffer;
	delete[] fPerFrameFileNamePrefix;
	delete[] fBuffer;
	if (fOutFid != NULL)
		fclose(fOutFid);
}

FileSink* FileSink::createNew(UsageEnvironment& env, char const* fileName,
		unsigned bufferSize, unsigned short clientID, Boolean oneFilePerFrame,
		CommonPlay *cpObj) {
	////no
	do {
		FILE* fid;
		char const* perFrameFileNamePrefix;
		if (oneFilePerFrame) {
			// Create the fid for each frame
			fid = NULL;
			perFrameFileNamePrefix = fileName;
		} else {
			// Normal case: create the fid once
//			fid = OpenOutputFile(env, fileName);
//			if (fid == NULL)
//				break;
			perFrameFileNamePrefix = NULL;
		}

		return new FileSink(env, fid, bufferSize, perFrameFileNamePrefix,
				clientID, cpObj);
	} while (0);

	return NULL;
}

//上面的fAfterGettingFunc为我们自己注册的函数，如果运行的是testProgs中的openRTSP实例，
//则该函数指向下列代码中通过调用getNextFrame（）注册的afterGettingFrame（）函数
Boolean FileSink::continuePlaying() {
	if (fSource == NULL)
		return False;
//	afterGettingFrameGetData
//	afterGettingFrame
	//从上面的代码中可以看到getNextFrame()函数的第一个参数为分别在各自类中定义的buffer，
	//我们继续以openRTSP为运行程序来分析，fBuffer为FileSink类里定义的指针：DP_U8* fBuffer
	fSource->getNextFrame(fBuffer, fBufferSize, afterGettingFrame, this,
			onSourceClosure, this);
//	cout << "fBuffer:: len :: " << strlen((char*) fBuffer) << " buffer "
//			<< fBufferSize << " iD::::::::::continuePlaying :: :" << _cliID
//			<< endl;
//	unsigned int iPos = 0;
//	for (iPos = 0; iPos < 20; iPos++) {
//		if (iPos != 0 && (fBuffer)[iPos] == 0x00 && (fBuffer)[iPos - 1] == 0x01)
//			break;
//		if (iPos % 10 == 0 && iPos != 0)
//			printf("\n");
//		printf("%02x ", ((DP_U8*) fBuffer)[iPos]);
//	}
//	printf("\n");
//	AA 4A 29 73 BE 91 9E 50
// B2 9F 07 1A 7C C1 5D F4
//	38 55 93 F7 08
//	EC 35 78 FE BB 06 BD B4
#if 1
//	while (iPos != fBufferSize) {
//		if (fBuffer[iPos] == 0xEC) {
//			if (fBuffer[iPos + 1] == 0x35) {
//				if (fBuffer[iPos + 2] == 0x78) {
//					if (fBuffer[iPos + 3] == 0xFE) {
//						if (fBuffer[iPos + 4] == 0xBB) {
//							if (fBuffer[iPos + 5] == 0x06) {
//								if (fBuffer[iPos + 6] == 0xBD) {
//									if (fBuffer[iPos + 7] == 0xB4) {
//										cout
//												<< "Bingoooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo"
//												<< endl;
//										break;
//									}
//								}
//							}
//						}
//					}
//				}
//			}
//		}
//		iPos++;
//	}
#endif
	return True;
}

void FileSink::afterGettingFrame(void* clientData, unsigned frameSize,
		unsigned numTruncatedBytes, struct timeval presentationTime,
		unsigned /*durationInMicroseconds*/) {
	FileSink* sink = (FileSink*) clientData;
//	afterGettingFrame
//	afterGettingFrameGetData
	sink->afterGettingFrameGetData(frameSize, numTruncatedBytes,
			presentationTime);
}

void FileSink::addData(DP_U8 const* data, unsigned dataSize,
		struct timeval presentationTime) {
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
//		DP_U8 start[4] = { 0 };
//		start[0] = 0x00;
//		start[1] = 0x00;
//		start[2] = 0x00;
//		start[3] = 0x01;
//		fwrite(start, 1, 4, fOutFid);
#if 1
		//search the first SPS - PPS - IDR
		if (((data[0] & 0x1F) != 7) && (fGetIDRFrame == false)) {
			return;
		} else if (fGetIDRFrame == true) {
		} else {
			gettimeofday(&tvStart, NULL);
//			cout << " start ! second : " << tvStart.tv_sec << "use : "
//					<< tvStart.tv_usec << endl;
			fGetIDRFrame = true;
			DP_U8 start[4] = { 0 };
			start[0] = 0x00;
			start[1] = 0x00;
			start[2] = 0x00;
			start[3] = 0x01;
			fwrite(start, 1, 4, fOutFid);
		}
#endif
#if 0
		struct timeval tv;
		gettimeofday(&tv, NULL);
//		cout << " start ! second : " << tvStart.tv_sec << "use : "
//				<< tvStart.tv_usec << endl;
//		cout << " second : " << tv.tv_sec << "use : " << tv.tv_usec << endl;

		DP_RTSP_SERVER_MEDIA_STREAM_INFO_S MediaStream;
		memset(&MediaStream, 0, sizeof(DP_RTSP_SERVER_MEDIA_STREAM_INFO_S));
		if (dataSize != 4) {
			MediaStream.enFrameType = DP_RTSP_SERVER_H264_FRAME_I;
			MediaStream.u32TimeStamp = 0;
			MediaStream.s32FrameSize = dataSize + 4;

//			DP_U8 start[4] = { 0 };
//			start[0] = 0x00;
//			start[1] = 0x00;
//			start[2] = 0x00;
//			start[3] = 0x01;
//			DP_U8 buff[dataSize + 4] = { 0 };
//			memcpy(buff, start, 4);
//			memcpy(buff + 4, data, dataSize);

			static DP_U32 combinFramePos = 0;
			static DP_U8 *u8CombinFrame = NULL;
			if ((data[0] & 0x1F) == 7 || ((data[0] & 0x1F) == 8)) {
				u8CombinFrame = (DP_U8*) realloc(u8CombinFrame,
						combinFramePos + dataSize + 4);
				memcpy(u8CombinFrame + combinFramePos, fStartCode, 4);
				memcpy(u8CombinFrame + combinFramePos + 4, data, dataSize);
				combinFramePos += dataSize + 4;
				return;
			} else if ((data[0] & 0x1F) == 5) {
				u8CombinFrame = (DP_U8*) realloc(u8CombinFrame,
						combinFramePos + dataSize + 4);
				memcpy(u8CombinFrame + combinFramePos, fStartCode, 4);
				memcpy(u8CombinFrame + combinFramePos, data, dataSize);
				MediaStream.pu8FrameStream = u8CombinFrame;
			} else {
				DP_U8 aFrame[dataSize + 4] = {0};
				memcpy(aFrame, fStartCode, 4);
				memcpy(aFrame + 4, fBuffer, dataSize);
				MediaStream.pu8FrameStream = aFrame;
				MediaStream.s32FrameSize = dataSize + 4;
			}
//				MediaStream.pu8FrameStream = buff;

			DP_RTSP_SERVER_MediaStreamInput(0, 0, MediaStream);
			if ((data[0] & 0x1F) == 7 || ((data[0] & 0x1F) == 8)
					|| (data[0] & 0x1F) == 5) {
				free(u8CombinFrame);
				u8CombinFrame = NULL;
				combinFramePos = 0;
			}
#endif
#if 0
		unsigned int iPos = 0;
		for (iPos = 0; iPos < 200; iPos++) {
			if (iPos != 0 && (MediaStream.pu8FrameStream)[iPos] == 0x00
					&& (MediaStream.pu8FrameStream)[iPos - 1] == 0x01)
			break;
			if (iPos % 10 == 0 && iPos != 0)
			printf("\n");
			printf("%02x ", ((DP_U8*) MediaStream.pu8FrameStream)[iPos]);
		}
	}
#endif
		fwrite(data, 1, dataSize, fOutFid);
//		cout << "dataSize : " << dataSize << "clid: " << envir()._cliID << endl;
//		if (dataSize == 6)
//			exit(0);
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
	unsigned int iPos = 0;
//	printf("\n");
//	for (iPos = 0; iPos < 50; iPos++) {
//		if (iPos != 0 && (fBuffer)[iPos] == 0x00 && (fBuffer)[iPos - 1] == 0x01)
//			break;
//		if (iPos % 10 == 0 && iPos != 0)
//			printf("\n");
//		printf("%02x ", ((DP_U8*) fBuffer)[iPos]);
//	}
//	printf("\n");
#if 1
//#define h264 1
	do {
//		if (h264) {
		if (fBuffer[0] != 0x00) {
			if (((fBuffer[0] & 0x1F) != 7) && (fGetIDRFrame == false)) {
				break;
			} else if (fGetIDRFrame == true) {
			} else {
//			gettimeofday(&tvStart, NULL);
				//			cout << " start ! second : " << tvStart.tv_sec << "use : "
				//					<< tvStart.tv_usec << endl;
				fGetIDRFrame = true;
			}

			DP_RTSP_CLIENT_FRAME_DATA_S stFrameData(_cliID);
			stFrameData.enFrameType = DP_RTSP_CLINET_CODEC_H264;
			//deal with
			DP_RTSP_SERVER_MEDIA_STREAM_INFO_S MediaStream;
			memset(&MediaStream, 0, sizeof(DP_RTSP_SERVER_MEDIA_STREAM_INFO_S));
			MediaStream.enFrameType = DP_RTSP_SERVER_H264_FRAME_I;
			MediaStream.u32TimeStamp = 0;
//			static DP_U32 _mCombinFrameSize = 0;
//			static DP_U8 *_mCombinFrame = NULL;
			//7-->SPS 8-->PPS 5-->IDR
			if ((fBuffer[0] & 0x1F) == 7 || (fBuffer[0] & 0x1F) == 8) {
				_mCombinFrame[_cliID] = (DP_U8*) realloc(_mCombinFrame[_cliID],
						_mCombinFrameSize[_cliID] + frameSize + 4);
				memcpy(_mCombinFrame[_cliID] + _mCombinFrameSize[_cliID],
						fStartCode, 4);
				memcpy(_mCombinFrame[_cliID] + _mCombinFrameSize[_cliID] + 4,
						fBuffer, frameSize);
				_mCombinFrameSize[_cliID] += frameSize + 4;
				break;
			}
//			if ((fBuffer[0] & 0x1F) == 8) {
//				u8CombinFrame = (DP_U8*) realloc(u8CombinFrame,
//						combinFramePos + frameSize + 4);
//				memcpy(u8CombinFrame + combinFramePos, fStartCode, 4);
//				memcpy(u8CombinFrame + combinFramePos + 4, fBuffer, frameSize);
//				combinFramePos += frameSize + 4;
//				break;
//			}
			else if ((fBuffer[0] & 0x1F) == 5) {
				_mCombinFrame[_cliID] = (DP_U8*) realloc(_mCombinFrame[_cliID],
						_mCombinFrameSize[_cliID] + frameSize + 4);
				memcpy(_mCombinFrame[_cliID] + _mCombinFrameSize[_cliID],
						fStartCode, 4);
				memcpy(_mCombinFrame[_cliID] + _mCombinFrameSize[_cliID] + 4,
						fBuffer, frameSize);
				_mCombinFrameSize[_cliID] += frameSize + 4;
//				MediaStream.pu8FrameStream = u8CombinFrame;
//				MediaStream.s32FrameSize = combinFramePos + frameSize + 4;
			} else {
#if 0
//				DP_U8 aFrame[frameSize + 4] = { 0 };
//				memcpy(aFrame, fStartCode, 4);
//				memcpy(aFrame + 4, fBuffer, frameSize);
//				MediaStream.pu8FrameStream = aFrame;
//				MediaStream.s32FrameSize = frameSize + 4;
//				DP_RTSP_SERVER_MediaStreamInput(0, 0, MediaStream);
#endif
				stFrameData.pu8Data = new DP_U8[frameSize + 4];
				memset(stFrameData.pu8Data, 0, frameSize + 4);
				memcpy(stFrameData.pu8Data, fStartCode, 4);
				memcpy(stFrameData.pu8Data + 4, fBuffer, frameSize);
				stFrameData.u32FrameSize = frameSize + 4;
				DP_S32 ret =
						DP_RTSP_CLIENT_Client::_mDataQueueSet[_cliID]->DP_RTSP_CLIENT_RecvData(
								0, stFrameData.u32FrameSize,
								_mCliRecvFrameSequence[_cliID],
								DP_RTSP_CLINET_CODEC_H264, stFrameData.pu8Data);
				Logger::GetInstance().Info(
						"stFrameData.u32FrameSize :%d ret %d cliID: %d",
						stFrameData.u32FrameSize, ret, _cliID);
				break;
			}
#if 0
//			DP_RTSP_SERVER_MediaStreamInput(0, 0, MediaStream);
#endif

			stFrameData.pu8Data = new DP_U8[_mCombinFrameSize[_cliID]];
			memset(stFrameData.pu8Data, 0, _mCombinFrameSize[_cliID]);
			memcpy(stFrameData.pu8Data, _mCombinFrame[_cliID],
					_mCombinFrameSize[_cliID]);
			DP_U32 rett =
					DP_RTSP_CLIENT_Client::_mDataQueueSet[_cliID]->DP_RTSP_CLIENT_RecvData(
							0, _mCombinFrameSize[_cliID],
							_mCliRecvFrameSequence[_cliID],
							DP_RTSP_CLINET_CODEC_H264, stFrameData.pu8Data);
			//when to free u8CombinFrame?????????????????///delete 7 8
			if ((fBuffer[0] & 0x1F) == 7 || ((fBuffer[0] & 0x1F) == 8)
					|| (fBuffer[0] & 0x1F) == 5) {
				free(_mCombinFrame[_cliID]);
				_mCombinFrame[_cliID] = NULL;
				_mCombinFrameSize[_cliID] = 0;
			}

//			DP_RTSP_CLIENT_FRAME_DATA_S stFrameData(_cliID);
//			stFrameData.pu8Data = new DP_U8[combinFramePos + 4];
//			memset(stFrameData.pu8Data, 0, combinFramePos + 4);
			//	if (video) {
//			stFrameData.pu8Data[0] = 0x00;
//			stFrameData.pu8Data[1] = 0x00;
//			stFrameData.pu8Data[2] = 0x00;
//			stFrameData.pu8Data[3] = 0x01;
//			memcpy(stFrameData.pu8Data + 4, fBuffer, frameSize);
//			DP_RTSP_CLIENT_Client::_mDataQueueSet[_cliID]->DP_RTSP_CLIENT_PutData(
//					&stFrameData, sizeof(DP_RTSP_CLIENT_FRAME_DATA_S));
//				}
			// 265 etc.
//			}
//		} else {
//
		}
	} while (0);
#endif
#if 0 //

	addData(fBuffer, frameSize, presentationTime);
	if (((fBuffer[0] & 0x7E) >> 1) == 19 || ((fBuffer[0] & 0x7E) >> 1) == 21) { //16~21
		///fwrite data 将数据保存到文件中
		if (flag++ == 1)
		exit(0);
	}
#endif
//	if (fOutFid == NULL || fflush(fOutFid) == EOF) {
//		// The output file has closed.  Handle this the same way as if the input source had closed:
//		if (fSource != NULL)
//			fSource->stopGettingFrames();
//		onSourceClosure();
//		return;
//	}

	if (fPerFrameFileNameBuffer != NULL) {
		if (fOutFid != NULL) {
			fclose(fOutFid);
			fOutFid = NULL;

		}
	}

// Then try getting the next frame:
	continuePlaying();
}

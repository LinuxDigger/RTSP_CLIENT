/*
 * H264ParserSPS.h
 *
 *  Created on: Aug 29, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H264PARSERSPS_H_
#define INCLUDE_H264PARSERSPS_H_

/**
 * 解码SPS,获取视频图像宽、高和帧率信息
 *
 * @param buf SPS数据内容
 * @param nLen SPS数据的长度
 * @param width 图像宽度
 * @param height 图像高度
 * @param fps 帧率
 * @成功则返回true , 失败则返回false
 */
//class H264ParserSPS {
//public:
//	H264ParserSPS() {
//	}
//	~H264ParserSPS() {
//	}
bool h264_decode_sps(unsigned char * buf, unsigned int nLen, int &width, int &height,
		int &fps);
//};

#endif /* INCLUDE_H264PARSERSPS_H_ */

/*
 * ParseSPS.h
 *
 *  Created on: Aug 29, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_PARSESPS_H_
#define INCLUDE_PARSESPS_H_

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
bool h264_decode_sps(DP_U8 * buf, unsigned int nLen, int &width,
		int &height, int &fps);

#endif /* INCLUDE_PARSESPS_H_ */

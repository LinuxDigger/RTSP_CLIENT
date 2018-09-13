/*

 * H264ParserSPS.cpp
 *
 *  Created on: Aug 29, 2018
 *      Author: jhb
 */

#include <string.h>
#include <math.h>
#include <iostream>
#include <stdio.h>
#include "H264ParserSPS.h"
using namespace std;

unsigned int Ue(unsigned char *pBuff, unsigned int nLen,
		unsigned int &nStartBit) {
	unsigned int nZeroNum = 0;
	while (nStartBit < nLen * 8) {
		if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8))) //&:按位与，%取余
				{
			break;
		}
		nZeroNum++;
		nStartBit++;
	}
	nStartBit++;

	//计算结果
	unsigned int dwRet = 0;
	for (unsigned int i = 0; i < nZeroNum; i++) {
		dwRet <<= 1;
		if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8))) {
			dwRet += 1;
		}
		nStartBit++;
	}
	return (1 << nZeroNum) - 1 + dwRet;
}

int Se(unsigned char *pBuff, unsigned int nLen, unsigned int &nStartBit) {
	int UeVal = Ue(pBuff, nLen, nStartBit);
	double k = UeVal;
	int nValue = ceil(k / 2); //ceil函数：ceil函数的作用是求不小于给定实数的最小整数。ceil(2)=ceil(1.2)=cei(1.5)=2.00
	if (UeVal % 2 == 0)
		nValue = -nValue;
	return nValue;
}

//ok
unsigned int u(unsigned int BitCount, unsigned char * buf,
		unsigned int &nStartBit) {
	unsigned int dwRet = 0;
	for (unsigned int i = 0; i < BitCount; i++) {
		dwRet <<= 1;
		if (buf[nStartBit / 8] & (0x80 >> (nStartBit % 8))) {
			dwRet += 1;
		}
		nStartBit++;
	}
	return dwRet;
}

/**
 * H264的NAL起始码防竞争机制
 *
 * @param buf SPS数据内容
 *
 * @无返回倿 */
void de_emulation_prevention(unsigned char * buf, unsigned int* buf_size) {
	unsigned int i = 0, j = 0;
	unsigned char * tmp_ptr = NULL;
	unsigned int tmp_buf_size = 0;
	int val = 0;

	tmp_ptr = buf;
	tmp_buf_size = *buf_size;
	for (i = 0; i < (tmp_buf_size - 2); i++) {
		//check for 0x000003
		val = (tmp_ptr[i] ^ 0x00) + (tmp_ptr[i + 1] ^ 0x00)
				+ (tmp_ptr[i + 2] ^ 0x03);
		if (val == 0) {
			//kick out 0x03
			for (j = i + 2; j < tmp_buf_size - 1; j++)
				tmp_ptr[j] = tmp_ptr[j + 1];

			//and so we should devrease bufsize
			(*buf_size)--;
		}
	}
}

const char ff_zigzag_direct[64] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25,
		18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21,
		28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58,
		59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63 };

const char ff_zigzag_scan[16 + 1] = { 0 + 0 * 4, 1 + 0 * 4, 0 + 1 * 4, 0
		+ 2 * 4, 1 + 1 * 4, 2 + 0 * 4, 3 + 0 * 4, 2 + 1 * 4, 1 + 2 * 4, 0
		+ 3 * 4, 1 + 3 * 4, 2 + 2 * 4, 3 + 1 * 4, 3 + 2 * 4, 2 + 3 * 4, 3
		+ 3 * 4, };

static int decode_scaling_list(unsigned char *data, char *factors, int size,
		const char *jvt_list, const char *fallback_list, int nLen,
		unsigned int &nStartBit) {
	int i, last = 8, next = 8;
	const char *scan = size == 16 ? ff_zigzag_scan : ff_zigzag_direct;
	if (!u(1, data, nStartBit)) /* matrix not written, we use the predicted one */
		memcpy(factors, fallback_list, size * sizeof(char));
	else
		for (i = 0; i < size; i++) {
			if (next) {
				int v = Se(data, nLen, nStartBit);
				if (v < -128 || v > 127) {

					return -1;
				}
				next = (last + v) & 0xff;
			}
			if (!i && !next) { /* matrix not written, we use the preset one */
				memcpy(factors, jvt_list, size * sizeof(char));
				break;
			}
			last = factors[scan[i]] = next ? next : last;
		}
	return 0;
}

static const char default_scaling4[2][16] = { { 6, 13, 20, 28, 13, 20, 28, 32,
		20, 28, 32, 37, 28, 32, 37, 42 }, { 10, 14, 20, 24, 14, 20, 24, 27, 20,
		24, 27, 30, 24, 27, 30, 34 } };

static const char default_scaling8[2][64] = { { 6, 10, 13, 16, 18, 23, 25, 27,
		10, 11, 16, 18, 23, 25, 27, 29, 13, 16, 18, 23, 25, 27, 29, 31, 16, 18,
		23, 25, 27, 29, 31, 33, 18, 23, 25, 27, 29, 31, 33, 36, 23, 25, 27, 29,
		31, 33, 36, 38, 25, 27, 29, 31, 33, 36, 38, 40, 27, 29, 31, 33, 36, 38,
		40, 42 }, { 9, 13, 15, 17, 19, 21, 22, 24, 13, 13, 17, 19, 21, 22, 24,
		25, 15, 17, 19, 21, 22, 24, 25, 27, 17, 19, 21, 22, 24, 25, 27, 28, 19,
		21, 22, 24, 25, 27, 28, 30, 21, 22, 24, 25, 27, 28, 30, 32, 22, 24, 25,
		27, 28, 30, 32, 33, 24, 25, 27, 28, 30, 32, 33, 35 } };

/* returns non zero if the provided SPS scaling matrix has been filled */
static int decode_scaling_matrices(unsigned char *data, int is_sps,
		char (*scaling_matrix4)[16], char (*scaling_matrix8)[64],
		int chroma_format_idc, int nlen, unsigned int &nStartBit) {
	int ret = 0;
	char fallback[4][16];
	if (u(1, data, nStartBit)) {
		ret |= decode_scaling_list(data, scaling_matrix4[0], 16,
				default_scaling4[0], fallback[0], nlen, nStartBit);  // Intra, Y
		ret |= decode_scaling_list(data, scaling_matrix4[1], 16,
				default_scaling4[0], scaling_matrix4[0], nlen, nStartBit); // Intra, Cr
		ret |= decode_scaling_list(data, scaling_matrix4[2], 16,
				default_scaling4[0], scaling_matrix4[1], nlen, nStartBit); // Intra, Cb
		ret |= decode_scaling_list(data, scaling_matrix4[3], 16,
				default_scaling4[1], fallback[1], nlen, nStartBit);  // Inter, Y
		ret |= decode_scaling_list(data, scaling_matrix4[4], 16,
				default_scaling4[1], scaling_matrix4[3], nlen, nStartBit); // Inter, Cr
		ret |= decode_scaling_list(data, scaling_matrix4[5], 16,
				default_scaling4[1], scaling_matrix4[4], nlen, nStartBit); // Inter, Cb
		if (is_sps) {
			ret |= decode_scaling_list(data, scaling_matrix8[0], 64,
					default_scaling8[0], fallback[2], nlen, nStartBit); // Intra, Y
			ret |= decode_scaling_list(data, scaling_matrix8[3], 64,
					default_scaling8[1], fallback[3], nlen, nStartBit); // Inter, Y
			if (chroma_format_idc == 3) {
				ret |= decode_scaling_list(data, scaling_matrix8[1], 64,
						default_scaling8[0], scaling_matrix8[0], nlen,
						nStartBit); // Intra, Cr
				ret |= decode_scaling_list(data, scaling_matrix8[4], 64,
						default_scaling8[1], scaling_matrix8[3], nlen,
						nStartBit); // Inter, Cr
				ret |= decode_scaling_list(data, scaling_matrix8[2], 64,
						default_scaling8[0], scaling_matrix8[1], nlen,
						nStartBit); // Intra, Cb
				ret |= decode_scaling_list(data, scaling_matrix8[5], 64,
						default_scaling8[1], scaling_matrix8[4], nlen,
						nStartBit); // Inter, Cb
			}
		}
		if (!ret)
			ret = is_sps;
	}

	return ret;
}

bool h264_decode_sps(unsigned char * buf, unsigned int nLen, int &width,
		int &height, int &fps) {
	unsigned int StartBit = 0;
	fps = 0;
	char scaling_matrix4[6][16];
	char scaling_matrix8[6][64];
	de_emulation_prevention(buf, &nLen);
	int forbidden_zero_bit = u(1, buf, StartBit);
	int nal_ref_idc = u(2, buf, StartBit);
	int nal_unit_type = u(5, buf, StartBit);
	if (nal_unit_type == 7) { ////////7
		int profile_idc = u(8, buf, StartBit);
		int constraint_set0_flag = u(1, buf, StartBit); //(buf[1] & 0x80)>>7;
		int constraint_set1_flag = u(1, buf, StartBit); //(buf[1] & 0x40)>>6;
		int constraint_set2_flag = u(1, buf, StartBit); //(buf[1] & 0x20)>>5;
		int constraint_set3_flag = u(1, buf, StartBit); //(buf[1] & 0x10)>>4;
		int reserved_zero_4bits = u(4, buf, StartBit);
		int level_idc = u(8, buf, StartBit);
		int seq_parameter_set_id = Ue(buf, nLen, StartBit);

		if (profile_idc == 100 ||  // High profile
				profile_idc == 110 ||  // High10 profile
				profile_idc == 122 ||  // High422 profile
				profile_idc == 244 ||  // High444 Predictive profile
				profile_idc == 44 ||  // Cavlc444 profile
				profile_idc == 83 || // Scalable Constrained High profile s(SVC)
				profile_idc == 86 ||  // Scalable High Intra profile (SVC)
				profile_idc == 118 ||  // Stereo High profile (MVC)
				profile_idc == 128 ||  // Multiview High profile (MVC)
				profile_idc == 138 ||  // Multiview Depth High profile (MVCD)
				profile_idc == 144) {
			int chroma_format_idc = Ue(buf, nLen, StartBit);
			printf("chroma_format_idc :%d,profile_idc:%d \n", chroma_format_idc,
					profile_idc);
			if (chroma_format_idc == 3)
				int residual_colour_transform_flag = u(1, buf, StartBit);

			int bit_depth_luma_minus8 = Ue(buf, nLen, StartBit);
			int bit_depth_chroma_minus8 = Ue(buf, nLen, StartBit);
			int qpprime_y_zero_transform_bypass_flag = u(1, buf, StartBit);
			decode_scaling_matrices(buf, 1, scaling_matrix4, scaling_matrix8,
					chroma_format_idc, nLen, StartBit);
		}

		int log2_max_frame_num_minus4 = Ue(buf, nLen, StartBit);

		int pic_order_cnt_type = Ue(buf, nLen, StartBit);

		if (pic_order_cnt_type == 0)
			int log2_max_pic_order_cnt_lsb_minus4 = Ue(buf, nLen, StartBit);
		else if (pic_order_cnt_type == 1) {
			int delta_pic_order_always_zero_flag = u(1, buf, StartBit);
			int offset_for_non_ref_pic = Se(buf, nLen, StartBit);
			int offset_for_top_to_bottom_field = Se(buf, nLen, StartBit);
			int num_ref_frames_in_pic_order_cnt_cycle = Ue(buf, nLen, StartBit);

			int *offset_for_ref_frame =
					new int[num_ref_frames_in_pic_order_cnt_cycle];
			for (int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
				offset_for_ref_frame[i] = Se(buf, nLen, StartBit);
			delete[] offset_for_ref_frame;
		}

		int num_ref_frames = Ue(buf, nLen, StartBit);
		int gaps_in_frame_num_value_allowed_flag = u(1, buf, StartBit);
		int pic_width_in_mbs_minus1 = Ue(buf, nLen, StartBit);
		int pic_height_in_map_units_minus1 = Ue(buf, nLen, StartBit);

		width = (pic_width_in_mbs_minus1 + 1) * 16;
		height = (pic_height_in_map_units_minus1 + 1) * 16;

		int frame_mbs_only_flag = u(1, buf, StartBit);
		if (!frame_mbs_only_flag)
			int mb_adaptive_frame_field_flag = u(1, buf, StartBit);

		int direct_8x8_inference_flag = u(1, buf, StartBit);
		int frame_cropping_flag = u(1, buf, StartBit);
		if (frame_cropping_flag) {
			int frame_crop_left_offset = Ue(buf, nLen, StartBit);
			int frame_crop_right_offset = Ue(buf, nLen, StartBit);
			int frame_crop_top_offset = Ue(buf, nLen, StartBit);
			int frame_crop_bottom_offset = Ue(buf, nLen, StartBit);
		}
		int vui_parameter_present_flag = u(1, buf, StartBit);

		if (vui_parameter_present_flag) {
			int aspect_ratio_info_present_flag = u(1, buf, StartBit);
			if (aspect_ratio_info_present_flag) {
				int aspect_ratio_idc = u(8, buf, StartBit);
				if (aspect_ratio_idc == 255) {
					int sar_width = u(16, buf, StartBit);
					int sar_height = u(16, buf, StartBit);
				}
			}
			int overscan_info_present_flag = u(1, buf, StartBit);
			if (overscan_info_present_flag)
				int overscan_appropriate_flagu = u(1, buf, StartBit);
			int video_signal_type_present_flag = u(1, buf, StartBit);
			if (video_signal_type_present_flag) {
				int video_format = u(3, buf, StartBit);
				int video_full_range_flag = u(1, buf, StartBit);
				int colour_description_present_flag = u(1, buf, StartBit);
				if (colour_description_present_flag) {
					int colour_primaries = u(8, buf, StartBit);
					int transfer_characteristics = u(8, buf, StartBit);
					int matrix_coefficients = u(8, buf, StartBit);
				}
			}
			int chroma_loc_info_present_flag = u(1, buf, StartBit);
			if (chroma_loc_info_present_flag) {
				int chroma_sample_loc_type_top_field = Ue(buf, nLen, StartBit);
				int chroma_sample_loc_type_bottom_field = Ue(buf, nLen,
						StartBit);
			}
			int timing_info_present_flag = u(1, buf, StartBit);
			if (timing_info_present_flag) {
				int num_units_in_tick = u(32, buf, StartBit);
				int time_scale = u(32, buf, StartBit);
				fps = time_scale / num_units_in_tick;
				int fixed_frame_rate_flag = u(1, buf, StartBit);
				if (fixed_frame_rate_flag) {
					fps = fps / 2;
				}
			}
		}
		return true;
	} else {
		return false;
	}
}

bool h265_decode_sps(unsigned char * buf, unsigned int nLen, int &width, int &height,
		int &fps) {
	unsigned int StartBit = 0;
	fps = 0;
	de_emulation_prevention(buf, &nLen);
	int forbidden_zero_bit = u(1, buf, StartBit);
	int nal_ref_idc = u(2, buf, StartBit);
	int nal_unit_type = u(5, buf, StartBit);
	//int forbidden_zero_bit = u(1, buf, StartBit);
	nal_ref_idc = u(4, buf, StartBit);
	int max_sub_layers = u(3, buf, StartBit);
	int temporal_id_nesting_flag = u(1, buf, StartBit);
	nal_unit_type = Ue(buf, nLen, StartBit);
	int chroma_format_idc = Ue(buf, nLen, StartBit);
	if (chroma_format_idc == 3) {
		int separate_colour_plane_flag = u(1, buf, StartBit);
		if (separate_colour_plane_flag)
			chroma_format_idc = 0;
	}
	width = Ue(buf, nLen, StartBit);
	height = Ue(buf, nLen, StartBit);
	return true;
}

//int main(void)
//{
//	int width = 0;
//	int height = 0;
//	int fps = 0;
//	char buffer[28] = {0x67,0x64,0x00,0x2A,0xAD,0x84,0x01,0x0C,0x20,0x08,0x61,0x00,0x43,0x08,0x02,0x18,0x40,0x10,0xC2,0x00,0x84,0x3B,0x50,0x3C,0x01,0x13,0xF2,0xA0};
//	char buffer1[128] = {0x67,0x64,0x00,0x29,0xAC,0xE6,0x01,0xE0,0x08,0x9F,0x97,0x01,0x10,0x00,0x00,0x03,0x00,0x10,0x00,0x00,0x03,0x00,0xA8,0xF1,0x83,0x11,0xA0};
//	char buffer2[128] = {0x42 ,0x01 ,0x01 ,0x01 ,0x60 ,0x00 ,0x00 ,0x03 ,0x00 ,0x90 ,0x00 ,0x00 ,0x03 ,0x00 ,0x00 ,0x03 ,0x00 ,0x78 ,0xA0 ,0x03 ,0xC0 ,0x80 ,0x10 ,0xE5 ,0x96 ,0xEA ,0x49 ,0x32 ,0xB8 ,0x04 ,0x00 ,0x00 ,0x03 ,0x00 ,0x04 ,0x00 ,0x00 ,0x03 ,0x00 ,0x64 ,0x20};
//	h264_decode_sps((char *)buffer,28,width,height,fps);
//	printf(" == [%d,%d,%d] == \n",width,height,fps);
//	h264_decode_sps((char *)buffer1,27,width,height,fps);
//	printf(" == [%d,%d,%d] == \n",width,height,fps);
//	h265_decode_sps((char *)buffer2,41,width,height,fps);
//	printf(" == [%d,%d,%d] == \n",width,height,fps);
//	return 0;
//}


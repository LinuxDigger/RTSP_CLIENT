/*
 * H265ParserSPS.cpp
 *
 *  Created on: Aug 29, 2018
 *      Author: jhb
 */

#include "H265ParserSPS.h"

bool ParseSequenceParameterSet(unsigned char * data, int size,
		vc_params_t& params) {
	if (size < 20) {
		return false;
	}

	NALBitstream bs(data, size);

// seq_parameter_set_rbsp()
	bs.GetWord(4); // sps_video_parameter_set_id
	int sps_max_sub_layers_minus1 = bs.GetWord(3); // "The value of sps_max_sub_layers_minus1 shall be in the range of 0 to 6, inclusive."
	if (sps_max_sub_layers_minus1 > 6) {
		return false;
	}
	bs.GetWord(1); // sps_temporal_id_nesting_flag
// profile_tier_level( sps_max_sub_layers_minus1 )
	{
		bs.GetWord(2); // general_profile_space
		bs.GetWord(1); // general_tier_flag
		params.profile = bs.GetWord(5); // general_profile_idc
		bs.GetWord(32); // general_profile_compatibility_flag[32]
		bs.GetWord(1); // general_progressive_source_flag
		bs.GetWord(1); // general_interlaced_source_flag
		bs.GetWord(1); // general_non_packed_constraint_flag
		bs.GetWord(1); // general_frame_only_constraint_flag
		bs.GetWord(44); // general_reserved_zero_44bits
		params.level = bs.GetWord(8); // general_level_idc
		uint8 sub_layer_profile_present_flag[6] = { 0 };
		uint8 sub_layer_level_present_flag[6] = { 0 };
		for (int i = 0; i < sps_max_sub_layers_minus1; i++) {
			sub_layer_profile_present_flag[i] = bs.GetWord(1);
			sub_layer_level_present_flag[i] = bs.GetWord(1);
		}
		if (sps_max_sub_layers_minus1 > 0) {
			for (int i = sps_max_sub_layers_minus1; i < 8; i++) {
				uint8 reserved_zero_2bits = bs.GetWord(2);
			}
		}
		for (int i = 0; i < sps_max_sub_layers_minus1; i++) {
			if (sub_layer_profile_present_flag[i]) {
				bs.GetWord(2); // sub_layer_profile_space[i]
				bs.GetWord(1); // sub_layer_tier_flag[i]
				bs.GetWord(5); // sub_layer_profile_idc[i]
				bs.GetWord(32); // sub_layer_profile_compatibility_flag[i][32]
				bs.GetWord(1); // sub_layer_progressive_source_flag[i]
				bs.GetWord(1); // sub_layer_interlaced_source_flag[i]
				bs.GetWord(1); // sub_layer_non_packed_constraint_flag[i]
				bs.GetWord(1); // sub_layer_frame_only_constraint_flag[i]
				bs.GetWord(44); // sub_layer_reserved_zero_44bits[i]
			}
			if (sub_layer_level_present_flag[i]) {
				bs.GetWord(8); // sub_layer_level_idc[i]
			}
		}
	}
	unsigned int sps_seq_parameter_set_id = bs.GetUE(); // "The  value  of sps_seq_parameter_set_id shall be in the range of 0 to 15, inclusive."
	if (sps_seq_parameter_set_id > 15) {
		return false;
	}
	unsigned int chroma_format_idc = bs.GetUE(); // "The value of chroma_format_idc shall be in the range of 0 to 3, inclusive."
	if (sps_seq_parameter_set_id > 3) {
		return false;
	}
	if (chroma_format_idc == 3) {
		bs.GetWord(1); // separate_colour_plane_flag
	}
	params.width = bs.GetUE(); // pic_width_in_luma_samples
	params.height = bs.GetUE(); // pic_height_in_luma_samples
	if (bs.GetWord(1)) { // conformance_window_flag
		bs.GetUE(); // conf_win_left_offset
		bs.GetUE(); // conf_win_right_offset
		bs.GetUE(); // conf_win_top_offset
		bs.GetUE(); // conf_win_bottom_offset
	}
	unsigned int bit_depth_luma_minus8 = bs.GetUE();
	unsigned int bit_depth_chroma_minus8 = bs.GetUE();
	if (bit_depth_luma_minus8 != bit_depth_chroma_minus8) {
		return false;
	}
//...

	return true;
}


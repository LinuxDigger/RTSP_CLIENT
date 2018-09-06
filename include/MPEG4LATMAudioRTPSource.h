/*
 * MPEG4LATMAudioRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG4LATMAUDIORTPSOURCE_H_
#define INCLUDE_MPEG4LATMAUDIORTPSOURCE_H_

#include "MultiFramedRTPSource.h"

class MPEG4LATMAudioRTPSource: public MultiFramedRTPSource {
public:
	static MPEG4LATMAudioRTPSource*
	createNew(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat, unsigned rtpTimestampFrequency);

	// By default, the LATM data length field is included at the beginning of each
	// returned frame.  To omit this field, call the following:
	void omitLATMDataLengthField();

	Boolean returnedFrameIncludesLATMDataLengthField() const {
		return fIncludeLATMDataLengthField;
	}

protected:
	virtual ~MPEG4LATMAudioRTPSource();

private:
	MPEG4LATMAudioRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
			unsigned rtpTimestampFrequency);
	// called only by createNew()

private:
	// redefined virtual functions:
	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	virtual char const* MIMEtype() const;

private:
	Boolean fIncludeLATMDataLengthField;
};

// A utility for parsing a "StreamMuxConfig" string
Boolean
parseStreamMuxConfigStr(char const* configStr,
		// result parameters:
		Boolean& audioMuxVersion, Boolean& allStreamsSameTimeFraming,
		DP_U8& numSubFrames, DP_U8& numProgram,
		DP_U8& numLayer, DP_U8*& audioSpecificConfig,
		unsigned& audioSpecificConfigSize);
// Parses "configStr" as a sequence of hexadecimal digits, representing
// a "StreamMuxConfig" (as defined in ISO.IEC 14496-3, table 1.21).
// Returns, in "audioSpecificConfig", a binary representation of
// the enclosed "AudioSpecificConfig" structure (of size
// "audioSpecificConfigSize" bytes).  The memory for this is allocated
// dynamically by this function; the caller is responsible for
// freeing it.  Other values, that precede "AudioSpecificConfig",
// are returned in the other parameters.
// Returns True iff the parsing succeeds.
// IMPORTANT NOTE: The implementation of this function currently assumes
// that everything after the first "numLayer" field is an
// "AudioSpecificConfig".  Therefore, it will not work properly if
// "audioMuxVersion" != 0, "numProgram" > 0, or "numLayer" > 0.
// Also, any 'other data' or CRC info will be included at
// the end of "audioSpecificConfig".

DP_U8* parseStreamMuxConfigStr(char const* configStr,
// result parameter:
		unsigned& audioSpecificConfigSize);
// A variant of the above that returns just the "AudioSpecificConfig" data
// (or NULL) if the parsing failed, without bothering with the other
// result parameters.

DP_U8* parseGeneralConfigStr(char const* configStr,
// result parameter:
		unsigned& configSize);
// A routine that parses an arbitrary config string, returning
// the result in binary form.

#endif /* INCLUDE_MPEG4LATMAUDIORTPSOURCE_H_ */

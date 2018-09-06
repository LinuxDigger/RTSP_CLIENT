/*
 * MPEG2TransportStreamMultiplexor.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG2TRANSPORTSTREAMMULTIPLEXOR_H_
#define INCLUDE_MPEG2TRANSPORTSTREAMMULTIPLEXOR_H_

#include "FramedSource.h"
#include "MPEG1or2Demux.h" // for SCR

#define PID_TABLE_SIZE 256

class MPEG2TransportStreamMultiplexor: public FramedSource {
public:
	Boolean canDeliverNewFrameImmediately() const {
		return fInputBufferBytesUsed < fInputBufferSize;
	}
	// Can be used by a downstream reader to test whether the next call to "doGetNextFrame()"
	// will deliver data immediately).

protected:
	MPEG2TransportStreamMultiplexor(UsageEnvironment& env, CommonPlay *cpObj);
	virtual ~MPEG2TransportStreamMultiplexor();

	virtual void awaitNewBuffer(DP_U8* oldBuffer) = 0;
	// implemented by subclasses

	void handleNewBuffer(DP_U8* buffer, unsigned bufferSize,
			int mpegVersion, MPEG1or2Demux::SCR scr, int16_t PID = -1);
	// called by "awaitNewBuffer()"
	// Note: For MPEG-4 video, set "mpegVersion" to 4; for H.264 video, set "mpegVersion" to 5.
	// The buffer is assumed to be a PES packet, with a proper PES header.
	// If "PID" is not -1, then it (currently, only the low 8 bits) is used as the stream's PID,
	// otherwise the "stream_id" in the PES header is reused to be the stream's PID.

private:
	// Redefined virtual functions:
	virtual void doGetNextFrame();

private:
	void deliverDataToClient(u_int8_t pid, DP_U8* buffer,
			unsigned bufferSize, unsigned& startPositionInBuffer);

	void deliverPATPacket();
	void deliverPMTPacket(Boolean hasChanged);

	void setProgramStreamMap(unsigned frameSize);

protected:
	Boolean fHaveVideoStreams;

private:
	unsigned fOutgoingPacketCounter;
	unsigned fProgramMapVersion;
	u_int8_t fPreviousInputProgramMapVersion, fCurrentInputProgramMapVersion;
	// These two fields are used if we see "program_stream_map"s in the input.
	struct {
		unsigned counter;
		u_int8_t streamType; // for use in Program Maps
	} fPIDState[PID_TABLE_SIZE];
	u_int8_t fPCR_PID, fCurrentPID;
	// Note: We map 8-bit stream_ids directly to PIDs
	MPEG1or2Demux::SCR fPCR;
	DP_U8* fInputBuffer;
	unsigned fInputBufferSize, fInputBufferBytesUsed;
	Boolean fIsFirstAdaptationField;
};

// The CRC calculation function that Transport Streams use.  We make this function public
// here in case it's useful elsewhere:
u_int32_t calculateCRC(u_int8_t const* data, unsigned dataLength,
		u_int32_t initialValue = 0xFFFFFFFF);

#endif /* INCLUDE_MPEG2TRANSPORTSTREAMMULTIPLEXOR_H_ */

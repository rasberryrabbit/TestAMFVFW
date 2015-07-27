/******************************************************************************
Copyright (C) 2015 by jackun <jack.un@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once
#ifndef TESTAMFVFW_H
#define TESTAMFVFW_H

#include "DeviceDX11.h"
#include "DeviceOCL.h"
#include "Log.h"
#include "Colorspace.h"

/* Registry */
#define OVE_REG_KEY    HKEY_CURRENT_USER
#define OVE_REG_PARENT L"Software"
#define OVE_REG_CHILD  L"TestAMFVFW"
#define OVE_REG_CLASS  L"config"

extern HMODULE hmoduleVFW;
extern CRITICAL_SECTION lockCS;

#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#define MAX(a, b) (((a)>(b)) ? (a) : (b))
#define CLIP(v, a, b) (((v)<(a)) ? (a) : ((v)>(b)) ? (b) : (v))

/* Settings */
#define S_LOG         "Log"
#define S_CABAC       "CABAC"
#define S_RCM         "RateControlMethod"
#define S_PRESET      "Preset"
#define S_PROFILE     "Profile"
#define S_LEVEL       "Level"
#define S_IDR         "IDRPeriod"
#define S_GOP         "GOPSize"
#define S_BITRATE     "Bitrate"
#define S_QPI         "QPI"
#define S_QPP         "QPP"
#define S_QPB         "QPB"
#define S_QP_MIN      "QPMin"
#define S_QP_MAX      "QPMax"
#define S_QPBD        "QPBDelta"
#define S_COLORPROF   "ColorProfile" //AMF_VIDEO_CONVERTER_COLOR_PROFILE_ENUM
#define S_DEVICEIDX   "DeviceIndex"
#define S_FPS_NUM     "FPSnum"
#define S_FPS_DEN     "FPSden"
#define S_FPS_ENABLED "FPSEnabled"

#define S_INSTALL     "InstallPath"

enum RCM {
	RCM_CQP = 0,
	RCM_CBR,
	RCM_PCVBR,
	RCM_LCVBR,
};

amf_pts AMF_STD_CALL amf_high_precision_clock();
void AMF_STD_CALL amf_increase_timer_precision();

class CodecInst {
private:
	Logger *mLog;
	int started;	//if the codec has been properly initalized yet
	HMODULE hModCore, hModVCE;

	DeviceOCL mDeviceCL;
	amf::AMFContextPtr   mContext;
	amf::AMFComponentPtr mEncoder;
	amf::AMFComponentPtr mConverter;
	amf::AMF_MEMORY_TYPE mNativeMemType;
	amf::AMF_SURFACE_FORMAT mFmtIn;
	amf_pts mFrameDuration;

	unsigned int mLength;
	LONG mWidth;
	LONG mHeight;
	unsigned int mFormat;	//input format for compressing, output format for decompression. Also the bitdepth.
	unsigned int mCompression;
	bool mHasIDR;
	UINT32 mIDRPeriod;
	UINT32 mCompressedSize;

	/* ICM_COMPRESS_FRAMES_INFO params */
	int frame_total;
	UINT32 fps_num;
	UINT32 fps_den;

	bool FindDLLs();
	void PrintProps(amf::AMFPropertyStorage *props);

public:
	DeviceDX11 mDeviceDX11;
	std::map<std::string, int32_t> mConfigTable;
	bool mDialogUpdated;

	bool SaveRegistry();
	bool ReadRegistry();
	void InitSettings();

	void LogMsg(bool msgBox, const wchar_t *psz_fmt, ...);

	CodecInst();
	~CodecInst();

	DWORD GetState(LPVOID pv, DWORD dwSize);
	DWORD SetState(LPVOID pv, DWORD dwSize);
	DWORD Configure(HWND hwnd);
	DWORD GetInfo(ICINFO* icinfo, DWORD dwSize);

	DWORD CompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD CompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD CompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD CompressGetSize(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD Compress(ICCOMPRESS* icinfo, DWORD dwSize);
	DWORD CompressEnd();
	DWORD CompressFramesInfo(ICCOMPRESSFRAMES *);

	DWORD DecompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD DecompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD DecompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD Decompress(ICDECOMPRESS* icinfo, DWORD dwSize);
	DWORD DecompressGetPalette(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD DecompressEnd();

	BOOL QueryConfigure();
};

CodecInst* Open(ICOPEN* icinfo);
DWORD Close(CodecInst* pinst);
#endif
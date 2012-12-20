////////////////////////////////////////////////////////
//            Read Only infomation registers          //
////////////////////////////////////////////////////////
//      id					addr	  data  bias type
__REG(ABORT_CODE,	  0x1000, 0x00, 0, RO, B)
__REG(FV_LO,	      0x1001, 0x00, 0, RO, B)
__REG(FV_HI,	      0x1002, 0x00, 0, RO, B)
__REG(VV_LO,	      0x1003, 0x00, 0, RO, B)
__REG(VV_HI,	      0x1004, 0x00, 0, RO, B)
__REG(CHIP_VERSION,	  0x1005, 0x00, 0, RO, B)
__REG(RELEASE_TYPE,	  0x1006, 0x00, 0, RO, B)
//__REG(GPIO_CNTL,	  0x1007, 0x00, 0, RW, B)
//__REG(GPIO_DIR,	    0x1008, 0x00, 0, RW, B)

/////////////////////////////////////////////////////////////
//                 AEC related register                    //
/////////////////////////////////////////////////////////////
#if 0
{
#endif

__REG(OutCTL,	0x1019, b_11000000, 0, CI, B)   // For playback  **
/* except bit7, if any bits changed, NEWC should be called. */
#ifndef _MACRO_DEFINED_
#define OUT_CTRL_AUTO			  b_10000000	// Automatic FW Control base on Jack Sense and DAC enables, 1= Auto, 0= Manual
#define OUT_CTRL_SUB_DIFF		  b_01000000	// Sub Differential control, 1=Differential, 0=Single Ended
#define OUT_CTRL_LO_DIFF		  b_00100000	// Line Out Differential control, 1=Differential, 0=Single Ended
#define OUT_CTRL_CLASSD_PWM		  b_00010000	// ClassD Output, 1=PWM, 0=Speakers
#define OUT_CTRL_CLASSD_MONO	  b_00001000	// ClassD Mono, 1=Mono, 0=Stereo
#define OUT_CTRL_CLASSD_EN		  b_00000100	// If OutCTL[7]=0, 1=Enable ClassD Speakers, 0=Disable ClassD Speakers
#define OUT_CTRL_LO_EN			  b_00000010	// If OutCTL[7]=0, 1=Enable Line Out, 0=Disable Line Out
#define OUT_CTRL_HP_EN			  b_00000001	// If OutCTL[7]=0, 1=Enable Headphone, 0=Disable Headphone
#endif

__REG(LineCTL, 0x101A, b_00000001, 0, CI, B)
#ifndef _MACRO_DEFINED_
#define IN_CTRL_AUTO			  b_10000000	// Automatic FW Control base on Jack Sense and ADC enables, 1=Auto, 0=Manual
#define IN_CTRL_L1_DIFF			  b_00001000	// Line In 1 Differential control, 1=Differential, 0=Single Ended
#define IN_CTRL_L3_EN			  b_00000100	// If LineCTL[7]=0, 1=Enable Line In 3, 0=Disable Line In 3
#define IN_CTRL_L2_EN			  b_00000010	// If LineCTL[7]=0, 1=Enable Line In 2, 0=Disable Line In 2
#define IN_CTRL_L1_EN			  b_00000001	// If LineCTL[7]=0, 1=Enable Line In 1, 0=Disable Line In 1
#endif

__REG(MicCTL,  0x101E, b_00000110, 0, CI, B)  // For Capture **, 0x06
/* reset value: b_00000110 */
#ifndef _MACRO_DEFINED_
#define MIC_POWER_ALWAYS	      b_00010000	// 1 = leave microphone and bias always on to avoid pops (burns power), 0 = microphone powered up as needed, mute for 400ms to remove pops
#define	MIC_BIAS_SELECT			  b_00001000	// 1= 80%, 0= 50%
#define	MIC_BOOST_MASK			  b_00000111	// 2:0 MicCTL [2:0]	Microphone Boost in 6dB Steps, 0= 0dB, 7= +42dB
#endif

__REG(EQ_GainL,  0x10D7,	b_00000000, 0, CI, B)
__REG(EQ_GainH,  0x10D8,	b_00001100, 0, CI, B)

/* DRC RELATED */
/* When AEC_En = 0 and DRC_En = 1 */
/*
__REG(DRC_RatioL,   0x10DB,	b_00000000, 0, WC, B) // 1:1
__REG(DRC_RatioH,   0x10DC,	b_00000000, 0, WC, B)
__REG(DRC_MaxAmplL, 0x10DD,	b_00000000, 0, WC, B)
__REG(DRC_MaxAmplH, 0x10DE,	b_00000000, 0, WC, B)
*/
__REG(DRC_ThreshL,  0x10DF,	b_00000000, 0, CI, B)
__REG(DRC_ThreshH,  0x10E0,	b_00010001, 0, CI, B)
__REG(DRC_AttackL,  0x10E1,	b_00110010, 0, CI, B)
__REG(DRC_AttackH,  0x10E2,	b_00000000, 0, CI, B)

/*
__REG(DRC_ReleaseL, 0x10E3,	b_00000000, 0, WC, B)
__REG(DRC_ReleaseH, 0x10E4,	b_00000000, 0, WC, B)
__REG(DRC_BoostL,   0x10E5,	b_00000000, 0, WC, B)
__REG(DRC_BoostH,   0x10E6,	b_00000000, 0, WC, B)
*/

////////////////////////////////////////////////////////
//           Linear Acoustic Echo Canceller           //
////////////////////////////////////////////////////////
/*
__REG(AECAdptL,        0x10EC,	b_00000000, 0, RW, B)
__REG(AECAdptH,        0x10ED,	b_00000000, 0, RW, B)

__REG(AECNLPGainL,     0x10EE,	b_00000000, 0, RW, B)
__REG(AECNLPGainH,     0x10EF,	b_00000000, 0, RW, B)
*/
__REG(AECDblTalkL,     0x10F0,	b_00000000, 0, CI, B)
__REG(AECDblTalkH,     0x10F1,	b_00000000, 0, CI, B)

__REG(AECConvPeriodL,  0x10F2,	b_00000010, 0, CI, B)
__REG(AECConvPeriodH,  0x10F3,	b_00000000, 0, CI, B)

__REG(FAR_AEC_THDLL,   0x10F4,	b_11000100, 0, CI, B)  //0xc4
__REG(FAR_AEC_THDLH,   0x10F5,	b_00001001, 0, CI, B)  //0x09

__REG(FAR_AEC_THDHL,   0x10F6,	b_10000000, 0, CI, B)  //0x80
__REG(FAR_AEC_THDHH,   0x10F7,	b_00000000, 0, CI, B)  //0x00

__REG(HAEC_ERLL,       0x10F8,	b_00000000, 0, CI, B)  //0x00
__REG(HAEC_ERLH,       0x10F9,	b_00010110, 0, CI, B)  //0x16

__REG(AECBulkDlyGainL, 0x10FA,	b_10100110, 0, CI, B)  //0xa6
__REG(AECBulkDlyGainH, 0x10FB,	b_00000101, 0, CI, B)  //0x05

__REG(HTXDLY0,         0x10FC,	b_00000001, 0, CI, B)  //0x01
//__REG(HTXDLY1,         0x10FD,	b_00000000, 0, CI, B)  //0x00

__REG(AECDRC_RatioL,   0x10FE,	b_00000000, 0, CI, B)  //0x00
__REG(AECDRC_RatioH,   0x10FF,	b_00000100, 0, CI, B)  //0x04


__REG(AECDRC_MaxAmplL, 0x1100,	b_00000000, 0, CI, B)  //0x00
__REG(AECDRC_MaxAmplH, 0x1101,	b_00001110, 0, CI, B)  //0x0e

__REG(AECDRC_ThreshL,  0x1102,	b_00000000, 0, CI, B)  //0x00
__REG(AECDRC_ThreshH,  0x1103,	b_00001101, 0, CI, B)  //0x0d

/*
__REG(AECDRC_AttackL,  0x1104,	b_00000000, 0, RW, B)
__REG(AECDRC_AttackH,  0x1105,	b_00000000, 0, RW, B)

__REG(AECDRC_ReleaseL, 0x1106,	b_00000000, 0, RW, B)
__REG(AECDRC_ReleaseH, 0x1107,	b_00000000, 0, RW, B)

__REG(AECDRC_BoostL,   0x1108,	b_00000000, 0, RW, B)
__REG(AECDRC_BoostH,   0x1109,	b_00000000, 0, RW, B)
*/

__REG(NRTotalSNRL,     0x110A,	b_11001101, 0, CI, B)   //0xcd
__REG(NRTotalSNRH,     0x110B,	b_00000101, 0, CI, B)   //0x05

/*
__REG(NRSubSNRL,       0x110C,	b_00000000, 0, RW, B)
__REG(NRSubSNRH,       0x110D,	b_00000000, 0, RW, B)
__REG(NR_MinGainL,     0x110E,	b_00000000, 0, RW, B)
__REG(NR_MinGainH,     0x110F,	b_00000000, 0, RW, B)
*/

__REG(CNI_AttenuationL,0x1110,	b_00000000, 0, CI, B)   //0x00
__REG(CNI_AttenuationH,0x1111,	b_00011000, 0, CI, B)   //0x18

/*
__REG(LECAdptL,        0x1112,	b_00000000, 0, RW, B)
__REG(LECAdptH,        0x1113,	b_00000000, 0, RW, B)
__REG(LECNLPGainL,     0x1114,	b_00000000, 0, RW, B)
__REG(LECNLPGainH,     0x1115,	b_00000000, 0, RW, B)
__REG(LECDblTalkL,     0x1116,	b_00000000, 0, RW, B)
__REG(LECDblTalkH,     0x1117,	b_00000000, 0, RW, B)
__REG(LECConvPeriodL,  0x1118,	b_00000000, 0, RW, B)
__REG(LECConvPeriodH,  0x1119,	b_00000000, 0, RW, B)
__REG(FAR_LEC_THDLL,   0x111A,	b_00000000, 0, RW, B)
__REG(FAR_LEC_THDLH,   0x111B,	b_00000000, 0, RW, B)
__REG(FAR_LEC_THDHL,   0x111C,	b_00000000, 0, RW, B)
__REG(FAR_LEC_THDHH,   0x111D,	b_00000000, 0, RW, B)
__REG(HLEC_ERLL,       0x111E,	b_00000000, 0, RW, B)
__REG(HLEC_ERLH,       0x111F,	b_00000000, 0, RW, B)
*/

__REG(LECBulkDlyGainL, 0x1120,	b_00000000, 0, CI, B)  //0x00
__REG(LECBulkDlyGainH, 0x1121,	b_00000100, 0, CI, B)  //0x04
/*
__REG(HRXDLY0,         0x1122,	b_00000000, 0, RW, B)
__REG(HRXDLY1,         0x1123,	b_00000000, 0, RW, B)
*/
__REG(IBNRTotalSNRL,   0x1124,	b_11001101, 0, CI, B)  //0xcd
__REG(IBNRTotalSNRH,   0x1125,	b_00000101, 0, CI, B)  //0x05
/*
__REG(IBNRSubSNRL,     0x1126,	b_00000000, 0, RW, B)
__REG(IBNRSubSNRH,     0x1127,	b_00000000, 0, RW, B)
__REG(IBNR_MinGainL,   0x1128,	b_00000000, 0, RW, B)
__REG(IBNR_MinGainH,   0x1129,	b_00000000, 0, RW, B)
*/

////////////////////////////////////////////////////////
//                Microphone AGC Related              //
////////////////////////////////////////////////////////
__REG(AGCLevelL,        0x112A,	b_10000000, 0, CI, B)   //0x80
__REG(AGCLevelH,        0x112B,	b_11110100, 0, CI, B)   //0xf4
/*
__REG(AGCGainIncrL,     0x112C,	b_00000000, 0, RW, B)
__REG(AGCGainIncrH,     0x112D,	b_00000000, 0, RW, B)
__REG(AGCGainDecrL,     0x112E,	b_00000000, 0, RW, B)
__REG(AGCGainDecrH,     0x112F,	b_00000000, 0, RW, B)
__REG(AGCChkUpdL,       0x1130,	b_00000000, 0, RW, B)
__REG(AGCChkUpdH,       0x1131,	b_00000000, 0, RW, B)
__REG(AGCMaxLinOutL,    0x1132,	b_00000000, 0, RW, B)
__REG(AGCMaxLinOutH,    0x1133,	b_00000000, 0, RW, B)
__REG(AGCMaxOutpLimitL, 0x1134,	b_00000000, 0, RW, B)
__REG(AGCMaxOutpLimitH, 0x1135,	b_00000000, 0, RW, B)
*/
__REG(RecOutpGainL,     0x1136,	b_00000000, 0, CI, B)  //0x00
__REG(RecOutpGainH,     0x1137,	b_00001000, 0, CI, B)  //0x08

/* Microphone EQ */
/*
__REG(MicEQ0L,  0x1138,	b_00000000, 0, RW, B)
__REG(MicEQ0H,  0x1139,	b_00000000, 0, RW, B)
__REG(MicEQ1L,  0x113A,	b_00000000, 0, RW, B)
__REG(MicEQ1H,  0x113B,	b_00000000, 0, RW, B)
__REG(MicEQ2L,  0x113C,	b_00000000, 0, RW, B)
__REG(MicEQ2H,  0x113D,	b_00000000, 0, RW, B)
__REG(MicEQ3L,  0x113E,	b_00000000, 0, RW, B)
__REG(MicEQ3H,  0x113F,	b_00000000, 0, RW, B)
__REG(MicEQ4L,  0x1140,	b_00000000, 0, RW, B)
__REG(MicEQ4H,  0x1141,	b_00000000, 0, RW, B)
__REG(MicEQ5L,  0x1142,	b_00000000, 0, RW, B)
__REG(MicEQ5H,  0x1143,	b_00000000, 0, RW, B)
__REG(MicEQ6L,  0x1144,	b_00000000, 0, RW, B)
__REG(MicEQ6H,  0x1145,	b_00000000, 0, RW, B)
__REG(MicEQ7L,  0x1146,	b_00000000, 0, RW, B)
__REG(MicEQ7H,  0x1147,	b_00000000, 0, RW, B)
__REG(MicEQ8L,  0x1148,	b_00000000, 0, RW, B)
__REG(MicEQ8H,  0x1149,	b_00000000, 0, RW, B)
__REG(MicEQ9L,  0x114A,	b_00000000, 0, RW, B)
__REG(MicEQ9H,  0x114B,	b_00000000, 0, RW, B)
__REG(MicEQ10L, 0x114C,	b_00000000, 0, RW, B)
__REG(MicEQ10H, 0x114D,	b_00000000, 0, RW, B)
__REG(MicEQ11L, 0x114E,	b_00000000, 0, RW, B)
__REG(MicEQ11H, 0x114F,	b_00000000, 0, RW, B)

__REG(SideToneGainL,   0x1150,	b_00000000, 0, RW, B)
__REG(SideToneGainH,   0x1151,	b_00000000, 0, RW, B)

__REG(BF_MicDistanceL, 0x1152,	b_00000000, 0, RW, B)
__REG(BF_MicDistanceH, 0x1153,	b_00000000, 0, RW, B)
__REG(BF_BeamWidth,    0x1154,	b_00000000, 0, RW, B)
__REG(BFCtrl,          0x1155,	b_00000000, 0, RW, B)

__REG(LA_RefLevelMinL, 0x1156,	b_00000000, 0, RW, B)
__REG(LA_RefLevelMinH, 0x1157,	b_00000000, 0, RW, B)
__REG(LA_RefLevelMaxL, 0x1158,	b_00000000, 0, RW, B)
__REG(LA_RefLevelMaxH, 0x1159,	b_00000000, 0, RW, B)
__REG(LA_PowerMarginL, 0x115A,	b_00000000, 0, RW, B)
__REG(LA_PowerMarginH, 0x115B,	b_00000000, 0, RW, B)
__REG(LA_PwrDynRangeL, 0x115C,	b_00000000, 0, RW, B)
__REG(LA_PwrDynRangeH, 0x115D,	b_00000000, 0, RW, B)
__REG(LA_StepUpL,      0x115E,	b_00000000, 0, RW, B)
__REG(LA_StepUpH,      0x115F,	b_00000000, 0, RW, B)
__REG(LA_StepDownL,    0x1160,	b_00000000, 0, RW, B)
__REG(LA_StepDownH,    0x1161,	b_00000000, 0, RW, B)
__REG(LA_UpdateInBlocksL,  0x1162,	b_00000000, 0, RW, B)
__REG(LA_UpdateInBlocksH,  0x1163,	b_00000000, 0, RW, B)
__REG(LA_NoiseAvgScaleL,   0x1164,	b_00000000, 0, RW, B)
__REG(LA_NoiseAvgScaleH,   0x1165,	b_00000000, 0, RW, B)
__REG(LA_RefStableCountsL, 0x1166,	b_00000000, 0, RW, B)
__REG(LA_RefStableCountsH, 0x1167,	b_00000000, 0, RW, B)
__REG(LA_StabilityCountsL, 0x1168,	b_00000000, 0, RW, B)
__REG(LA_StabilityCountsH, 0x1169,	b_00000000, 0, RW, B)
*/

__REG(S1ADCInpRate,     0x116A,	b_00100111, 0, CI, B)  //0x27
__REG(S8OutpRate,       0x1175,	b_00000010, 0, CI, B)  //0x02
__REG(MiscRoute,        0x1176,	b_00000000, 0, CI, B)  //0x00
#ifndef _MACRO_DEFINED_
#define ScaleOutRate_16K 0x02
#endif

__REG(SPDIF_CTL,    0x1179,	b_00100000, 0, CI, B)  //0x20

__REG(DSPEn1,       0x117A,	b_00100111, 0, CI, B)  //0x27
/* reset value:           b_00010101 */
/* only effective in DSP mode */
#ifndef _MACRO_DEFINED_
#define	SideTone_En		  b_10000000
#define	UseRightMic		  b_01000000  //effective when MicBF_En not set
#define	IBNR_En           b_00100000  //depend on LEC_En
#define	MicAGC_En		  b_00010000
#define	MicBF_En		  b_00001000
#define	NR_En			  b_00000100
#define	LEC_En			  b_00000010  //depend on AEC_En
#define	AEC_En			  b_00000001
#endif

__REG(POWER,	0x117E,	0x00, 0, CI, B)
/* reset value:             b_00100000 */
#ifndef _MACRO_DEFINED_
#define SleepOn             b_10000000
#define DeepSleep           b_01000000   //Deep Sleep (analog power off) when SleepOn set.
#define DSPStall            b_00100000
#endif

__REG(S8DACSubFrom, 0x1180,	b_00000110, 0, CI, B)   //0x06

////////////////////////////////////////////////////////////
//                    AEC NLP related                     //
////////////////////////////////////////////////////////////

__REG(AEC_NLP_4KL,	0x11AD,	b_00000000, 0, CI, B)  //0x00
__REG(AEC_NLP_4KH,	0x11AE,	b_01000000, 0, CI, B)  //0x40
__REG(LEC_NLP_4KL,	0x11AF,	b_00000000, 0, CI, B)  //0x00
__REG(LEC_NLP_4KH,	0x11B0,	b_01000000, 0, CI, B)  //0x40

#if 0
}
#endif
//////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
//        DSP related registers, must be NEWC         //
////////////////////////////////////////////////////////
__REG(DSPInit0,	0x117C,	0x00, 0, CI, B)
/* reset value:             b_10000000 */
/* write on initiating */
#ifndef _MACRO_DEFINED_
#define BypassDSP         b_10000000  //disable DSP mode
#define S8MonoDACEnP      b_00000001  //Mono DAC,DSP mode only
#endif

/* initial value = b_10101100 */
__REG(DSPInit1,	0x117D,	  b_10101000, 0, WC, B)   // **
/* reset value:           b_00000000 */
#ifndef _MACRO_DEFINED_
#define S7STDACEnP        b_10000000  //Stereo DAC DSP mode only
#define S6DPRT2EnC        b_01000000  //Digital Port2 I2S-4 Wire Record/DSP mode only
#define S5DPRT1EnC        b_00100000  //Digital Port1 I2S-5 Wire Record/DSP mode only
#define S4I2SEnP          b_00010000  //Digital Port1, Digital Port2,USB
#define S3I2SEnP          b_00001000  //Digital Port1, Digital Port2
#define S2MicADCEnC       b_00000100  //Mic ADC
#define S1LinADCEnC       b_00000010  //Line In ADC
#define NEWC              b_00000001
#endif
/*
NEWC - New Confiuration. Act/Process changes to host API. 
This bit will clear when update is done.Once this bit is set, 
the host should read this bit to verify that it has cleared 
before modifying any other API registers!
*/

__REG(DSPEn2, 0x117B,	b_01000000, 0, WC, B)
/* reset value:             b_00000000 */
/* not need to write on initiating */
#ifndef _MACRO_DEFINED_
#define	S7STDACMono			  b_01000000	// Class-D mono OutCTL [3]=1 should be used. **
#define	FuncGen_En			  b_00100000  // DSP mode only
#define	LA_En			      b_00010000	// DSP mode only
#define	Proc3D_En			  b_00001000	// DSP mode only
#define	DRC_En		          b_00000100	// DSP mode only
#define	SubWooferCrossover    b_00000010	// DSP mode only
#define	EQ_En				  b_00000001	// 1=Enable, 0=Disable
#endif

////////////////////////////////////////////////////////
//               DSP Routing registers                //
////////////////////////////////////////////////////////
#ifndef _MACRO_DEFINED_
#define S1OUT       0x01
#define S2OUT       0x02
#define S3OUT       0x03
#define S4OUT       0x04
#define ScaleOut    0x05
#define VoiceOut0   0x06
#define VoiceOut1   0x07
#define FunGenOut   0x08
#define Mix1Out     0x09
#endif

__REG(S7STDACFrom,	0x117F,	0x00, 0, WC, B)   // reset value: 0x05 - ScaleOut *

__REG(S5Dp1OutFrom, 0x1181,	0x00, 0, WC, B)   // reset value: 0x06 - VoiceOut0*
__REG(S6Dp2OutFrom, 0x1182,	0x00, 0, WC, B)   // reset value: 0x06 - VoiceOut0
__REG(S9USBOutFrom,	0x1183,	0x00, 0, WC, B)   // reset value: 0x06 - VoiceOut0

__REG(Mix0In0From,	0x1184,	0x00, 0, WC, B)   // reset value: 0x01 - S1OUT
__REG(Mix0In1From,	0x1185,	0x00, 0, WC, B)   // reset value: 0x03 - S3OUT    *
__REG(Mix0In2From,	0x1186,	0x00, 0, WC, B)   // reset value: 0x04 - S4OUT
__REG(Mix0In3From,	0x1187,	0x00, 0, WC, B)   // reset value: 0x00 - None (no connect)

__REG(Mix1In0From,	0x1188,	0x00, 0, WC, B)   // reset value: 0x00 - None (no connect)
__REG(Mix1In1From,	0x1189,	0x00, 0, WC, B)   // reset value: 0x00 - None (no connect)
__REG(Mix1In2From,	0x118A,	0x00, 0, WC, B)   // reset value: 0x00 - None (no connect)
__REG(Mix1In3From,	0x118B,	0x00, 0, WC, B)   // reset value: 0x00 - None (no connect)

__REG(VoiceIn0From,	0x118C,	0x00, 0, WC, B)   // reset value: 0x02 - S2OUT    *

#ifndef _MACRO_DEFINED_
#define ByPassToDPort1   0x00
#define ByPassToDPort2   0x01
#define ByPassToDAC      0x02
#define ByPassToDACSub   0x03
#define ByPassToSPDIF    0x04
#define ByPassToUSB      0x05

#define S3S4FromDPort1   0x00
#define S3S4FromDPort2   0x01
#define S4FromUSB        0x02
#endif

__REG(S1S2RouteTo,      0x116C,	0x00, 0, WC, B) //FOR BYPASS MODE(NOT DSP MODE),reset value:to DAC.   *(FOR TEST)
__REG(S3ROUTE,          0x116E,	0x00, 0, WC, B) //reset value:from digital port1,to DAC(BYPASS MODE). *
__REG(S4ROUTE,          0x1170,	0x00, 0, WC, B)

__REG(VoiceRcdProcRate,	0x11C8,	0x00, 0, WC, B)
#ifndef _MACRO_DEFINED_
#define VoiceModeMask   0x03
#endif
//__REG(S5OutpRateSRC,	  0x11C9,	0x00, 0, WC, B) //no use
//__REG(S6OutpRateSRC,	  0x11CA,	0x00, 0, WC, B)

////////////////////////////////////////////////////////
//            Stream Rate related registers           //
////////////////////////////////////////////////////////
#ifndef _MACRO_DEFINED_
#define SAMPLE_ALAW_8BITS    0x00
#define SAMPLE_ULAW_8BITS    0x01
#define SAMPLE_LINE_16BITS   0x02
#define SAMPLE_LINE_24BITS   0x03
#endif

__REG(S2MicInpRate,     0x116B,	b_00100010, 0, CI, B) //reset value:b_10100010,stero,16bit,16K.        *
#ifndef _MACRO_DEFINED_
#define MIC_RATE_16K    0x02
#define MIC_RATE_32K    0x05
#define MIC_RATE_48K    0x07
#endif

__REG(S3InpRate,        0x116D,	b_00000000, 0, WC, B) //reset value: b_10100111,configure on runtime.  *
#ifndef _MACRO_DEFINED_
#define S3SRC_8k_48K    0x00
#define S3SRC_11k_48K   0x01
#define S3SRC_16k_48K   0x02
#define S3SRC_22k_48K   0x03
#define S3SRC_24k_48K   0x04
#define S3SRC_32k_48K   0x05
#define S3SRC_44k_48K   0x06
#define S3SRC_48k_48K   0x07
#define S3SRC_88k_48K   0x08
#define S3SRC_96k_48K   0x09
#define S3SRC_8k_16K    0x0A
#define S3SRC_16k_16K   0x0B
#endif
__REG(S4InpRate,        0x116F,	b_00000000, 0, WC, B)

/*
 Note: The above digital stream information is used only for the firmware and DMA setup.
 The host still needs to configure the I2S and PCM setup registers 0xF50, 
 0xF5E ¨C 0xF68. Also USB control will automatically change the above Stream4 registers
 as needed. USB will override Stream4 because it has priority over it.
*/
__REG(S5OutpRate,       0x1171,	b_00100000, 0, CI, B)
// reset value: b_00100111 16bit,16K->48K         *
#ifndef _MACRO_DEFINED_
#define S5SRC_16k_8K    0x00
#define S5SRC_16k_11K   0x01
#define S5SRC_16k_16K   0x02
#define S5SRC_16k_22K   0x03
#define S5SRC_16k_24K   0x04
#define S5SRC_16k_32K   0x05
#define S5SRC_16k_44K   0x06
#define S5SRC_16k_48K   0x07
#define S5SRC_16k_88K   0x08
#define S5SRC_16k_96K   0x09
#define S5SRC_48k_24K   0x0A
#define S5SRC_48k_32K   0x0B
#define S5SRC_48k_44K   0x0C
#define S5SRC_48k_48K   0x0D
#endif

__REG(S6OutpRate,       0x1172,	b_00000000, 0, WC, B)
__REG(S7OutpRate,       0x1173,	b_00000000, 0, WC, B)
// reset value: b_00000111,48K. *
#ifndef _MACRO_DEFINED_
#define S7OutRate_16K    0x02
#define S7OutRate_48K    0x07
#endif

//__REG(DACRoute,         0x1174,	b_00000000, 0, WC, B) // no use


//__REG(USBROUTE,         0x1177,	b_00000000, 0, WC, B)

/*
__REG(SPDIF_CODE,   0x1178,	b_00000000, 0, RW, B)
*/

////////////////////////////////////////////////////////
//              Digital Port registers                //
////////////////////////////////////////////////////////
//      id					addr	data     bias type
__REG(CLOCK_DIVIDER,	0x0F50,	b_11111111, 0, WI, B)
#ifndef _MACRO_DEFINED_
/*
#define PORT2_DIV_SEL_6_144MHz				b_00000000	// 0x0 = 6.144 MHz
#define PORT2_DIV_SEL_4_096MHz				b_00010000	// 0x1 = 4.096 MHz
#define PORT2_DIV_SEL_3_072MHz				b_00100000	// 0x2 = 3.072 MHz
#define PORT2_DIV_SEL_2_048MHz				b_00110000	// 0x3 = 2.048 MHz
#define PORT2_DIV_SEL_1_536MHz				b_01000000	// 0x4 = 1.536 MHz
#define PORT2_DIV_SEL_1_024MHz				b_01010000	// 0x5 = 1.024 MHz
#define PORT2_DIV_SEL_768kHz				  b_01100000	// 0x6 = 768kHz
#define PORT2_DIV_SEL_512kHz				  b_01110000	// 0x7 = 512 kHz
#define PORT2_DIV_SEL_384kHz				  b_10000000	// 0x8 = 384 kHz
#define PORT2_DIV_SEL_256kHz				  b_10010000	// 0x9 = 256 kHz
#define PORT2_DIV_SEL_5_644MHz				b_10100000	// 0xa = 5.644 MHz
#define PORT2_DIV_SEL_2_822MHz				b_10110000	// 0xb = 2.822 MHz
#define PORT2_DIV_SEL_1_411MHz				b_11000000	// 0xc = 1.411 MHz
#define PORT2_DIV_SEL_705kHz				  b_11010000	// 0xd = 705 kHz
#define PORT2_DIV_SEL_352kHz				  b_11100000	// 0xe = 352 kHz
#define PORT2_DIV_SEL_EXT				      b_11110000	// 0xf = external clock enabled
*/
#define PORT1_DIV_SEL_6_144MHz				b_00000000	// 0x0 = 6.144 MHz
#define PORT1_DIV_SEL_4_096MHz				b_00000001	// 0x1 = 4.096 MHz
#define PORT1_DIV_SEL_3_072MHz				b_00000010	// 0x2 = 3.072 MHz
#define PORT1_DIV_SEL_2_048MHz				b_00000011	// 0x3 = 2.048 MHz
#define PORT1_DIV_SEL_1_536MHz				b_00000100	// 0x4 = 1.536 MHz
#define PORT1_DIV_SEL_1_024MHz				b_00000101	// 0x5 = 1.024 MHz
#define PORT1_DIV_SEL_768kHz				b_00000110	// 0x6 = 768kHz
#define PORT1_DIV_SEL_512kHz				b_00000111	// 0x7 = 512 kHz
#define PORT1_DIV_SEL_384kHz				b_00001000	// 0x8 = 384 kHz
#define PORT1_DIV_SEL_256kHz				b_00001001	// 0x9 = 256 kHz
#define PORT1_DIV_SEL_5_644MHz				b_00001010	// 0xa = 5.644 MHz
#define PORT1_DIV_SEL_2_822MHz				b_00001011	// 0xb = 2.822 MHz
#define PORT1_DIV_SEL_1_411MHz				b_00001100	// 0xc = 1.411 MHz
#define PORT1_DIV_SEL_705kHz				b_00001101	// 0xd = 705 kHz
#define PORT1_DIV_SEL_352kHz				b_00001110	// 0xe = 352 kHz
#define PORT1_DIV_SEL_EXT				    b_00001111	// 0xf = external clock enabled
#endif

__REG(PORT1_CTL1,	0x0F51,	b_10110000, 0, WI,B)
#ifndef _MACRO_DEFINED_
#define PORT1_DELAY					          b_10000000	// 1=Data delayed 1 bit (I2S standard), 0=no delay (sony mode)
#define PORT1_JUSTR_LSBF				      b_01000000	// [1/0]=Right/Left Justify (I2S) or LSB/MSB First (PCM)
#define PORT1_RX_EN					          b_00100000	// 1=RX Clock Enable, 0=RX Clock Disabled
#define PORT1_TX_EN					          b_00010000	// 1=TX Clock Enable, 0=TX Clock Disabled
#define PORT1_BITCLK_POL				      b_00000100	// 0=Normal clock, 1=Inverted clock
#define PORT1_WS_POL					      b_00000010	// 0=Rising Edge Active for Word Strobe, 1=Falling Edge Active for Word Strobe
#define PORT1_PCM_MODE					      b_00000001	// 0=I2S mode, 1=PCM Mode
#endif

__REG(PORT1_TX_CLOCKS_PER_FRAME_PHASE,	0x0F52,	b_00000111, 0, WI,B)  // clocks/frame=(N+1)*8
__REG(PORT1_RX_CLOCKS_PER_FRAME_PHASE,	0x0F53,	b_00000111, 0, WI,B)  // clocks/frame=(N+1)*8
__REG(PORT1_TX_SYNC_WIDTH, 0x0F54,	b_00011111, 0, WI,B)  // clocks=(N+1)
__REG(PORT1_RX_SYNC_WIDTH, 0x0F55,	b_00011111, 0, WI,B)  // clocks=(N+1)


__REG(PORT1_CTL2,	0x0F56,	b_00000101, 0, WI,B)
#ifndef _MACRO_DEFINED_
#define PORT1_CTRL_TX_PT				    b_00100000	// Tx passthrough mode,  0=off, 1=on
#define PORT1_CTRL_RX_PT				    b_00010000	// Rx passthrough mode,  0=off, 1=on
#define PORT1_CTRL_RX_SIZE_8				b_00000000	// RX Sample Size, 00=8 bits
#define PORT1_CTRL_RX_SIZE_16				b_00000100	// RX Sample Size, 01=16 bit
#define PORT1_CTRL_RX_SIZE_24T			    b_00001000	// RX Sample Size, 10=24 bit truncated to 16 bits
#define PORT1_CTRL_RX_SIZE_24				b_00001100	// RX Sample Size, 11=24 bit
#define PORT1_CTRL_TX_SIZE_8				b_00000000	// TX Sample Size, 00=8 bits
#define PORT1_CTRL_TX_SIZE_16				b_00000001	// TX Sample Size, 01=16 bit
#define PORT1_CTRL_TX_SIZE_24T			    b_00000010	// TX Sample Size, 10=24 bit truncated to 16 bits
#define PORT1_CTRL_TX_SIZE_24				b_00000011	// TX Sample Size, 11=24 bit
#endif

__REG(PORT1_RX_STR1_SLOT_CTL,	0x0F57,	b_00100000, 0, WI,B)
__REG(PORT1_RX_STR2_SLOT_CTL,	0x0F58,	b_00100100, 0, WI,B)

__REG(PORT1_TX_STR1_SLOT_CTL,	0x0F5A,	b_00100000, 0, WI,B)
__REG(PORT1_TX_STR2_SLOT_CTL,	0x0F5B,	b_00100100, 0, WI,B)

////////////////////////////////////////////////////////
//              gain related registers                //
////////////////////////////////////////////////////////
__REG(DAC1L,	        0x100D, 0x00, 0x4A, RW, B)  // **
__REG(DAC2R,            0x100E, 0x00, 0x4A, RW, B)  // **
__REG(DSPMaxVol,        0x100F, 0x4F, 0,    RW, B)  // *

/*
 When in DSP mode the gains are controlled using DSP scaling instead of hardware DAC.
 The hardware DAC is controlled by set the DSPDAC [6:0] variable. 
 The volume scaling is derived from the DACL and DACR setting.
 This can be switch the other way around if DSPDAC[7]=1.
 If DINT0[7]=0
 0 = hardcode hardware DAC gain to DSPDAC[6:0] and use DSP scaling control volume.
 1 = hardcode DSP scaling to DSPDAC[6:0] and use hardware DAC gain to control volume.
*/
__REG(DSPDAC,	          0x1010, 0x4A, 0,    RW, B)
__REG(ClassDGain,	      0x1011, b_00000000, 0, WI, B)  // enable SSC *
#ifndef _MACRO_DEFINED_
#define CLASS_D_GAIN_2W8				b_00000000	// 2.8W
#define CLASS_D_GAIN_2W6				b_00000001	// 2.6W
#define CLASS_D_GAIN_2W5				b_00000010	// 2.5W
#define CLASS_D_GAIN_2W4				b_00000011	// 2.4W
#define CLASS_D_GAIN_2W3				b_00000100	// 2.3W
#define CLASS_D_GAIN_2W2				b_00000101	// 2.2W
#define CLASS_D_GAIN_2W1				b_00000110	// 2.1W
#define CLASS_D_GAIN_2W0				b_00000111	// 2.0W
#define CLASS_D_GAIN_1W3				b_00001000	// 1.3W
#define CLASS_D_GAIN_1W25				b_00001001	// 1.25W
#define CLASS_D_GAIN_1W2				b_00001010	// 1.2W
#define CLASS_D_GAIN_1W15				b_00001011	// 1.15W
#define CLASS_D_GAIN_1W1				b_00001100	// 1.1W
#define CLASS_D_GAIN_1W05				b_00001101	// 1.05W
#define CLASS_D_GAIN_1W0				b_00001110	// 1.0W
#define CLASS_D_GAIN_0W9				b_00001111	// 0.9W
#endif

__REG(DAC3Sub,      0x1012, 0x00, 0x4A, RW, B)
__REG(ADC1L,	    0x1013,	0x00, 0x4A, RW, B)
__REG(ADC1R,        0x1014,	0x00, 0x4A, RW, B)
__REG(MicADC2GainL, 0x1015,	0x00, 0x4A, RW, B)
__REG(MicADC2GainR, 0x1016,	0x00, 0x4A, RW, B)
__REG(DSPMaxMic,    0x1017,	0x00, 0,    RW, B)

__REG(Mute,	        0x1018,	0x00, 0,    RW, B)  // **
#ifndef _MACRO_DEFINED_
#define LEFT_AUX_MUTE			  b_01000000
#define RIGH_AUX_MUTE			  b_00100000
#define LEFT_MIC_MUTE			  b_00010000
#define RIGH_MIC_MUTE			  b_00001000
#define SUB_SPEAKER_MUTE		  b_00000100
#define LEFT_SPEAKER_MUTE		  b_00000010
#define RIGH_SPEAKER_MUTE		  b_00000001
#define VOLUME_MUTE_ALL			  b_01111111
#endif

__REG(L1Gain,	 0x101B, 0x17, 0, RW,B)
#ifndef _MACRO_DEFINED_
#define LINE1_MUTE				  b_10000000	// 1=mute, 0=unmute
#define LINE1_GAIN_MASK			  b_00011111	// range 0x00-0x1F (-35.5dB to +12dB)
#endif

__REG(L2Gain,	 0x101C, 0x17, 0, RW,B)
#ifndef _MACRO_DEFINED_
#define LINE2_MUTE				  b_10000000	// 1=mute, 0=unmute
#define LINE2_GAIN_MASK			  b_00011111	// range 0x00-0x1F (-35.5dB to +12dB)
#endif

__REG(L3Gain,	 0x101D, 0x17, 0, RW,B)
#ifndef _MACRO_DEFINED_
#define LINE3_MUTE			      b_10000000	// 1=mute, 0=unmute
#define LINE3_GAIN_MASK			  b_00011111	// range 0x00-0x1F (-35.5dB to +12dB)
#endif

/* MIXER GAINE */
__REG(Mix0In0Gain, 0x101F,	b_00000000, 0, RW, B)
__REG(Mix0In1Gain, 0x1020,	b_00000000, 0, RW, B) //reset value:b_00000000,unmute and 0dB. For playback  **
__REG(Mix0In2Gain, 0x1021,	b_00000000, 0, RW, B)
__REG(Mix0In3Gain, 0x1022,	b_00000000, 0, RW, B)


__REG(Mix1In0Gain, 0x1023,	b_00000000, 0, RW, B)
__REG(Mix1In1Gain, 0x1024,	b_00000000, 0, RW, B)
__REG(Mix1In2Gain, 0x1025,	b_00000000, 0, RW, B)
__REG(Mix1In3Gain, 0x1026,	b_00000000, 0, RW, B)


#if 0
/////////////////////////////////////////////////////////
//            Function Generator related               //
/////////////////////////////////////////////////////////

__REG(FG_Left_Ctl,	0x1191,	0x00, 0, WC, B)
__REG(FG_Left_Time,	0x1192,	0x00, 0, WC, B)
__REG(FG_Left_A0L,	0x1193,	0x00, 0, WC, B)
__REG(FG_Left_A0H,	0x1194,	0x00, 0, WC, B)
__REG(FG_Left_A1L,	0x1195,	0x00, 0, WC, B)
__REG(FG_Left_A1H,	0x1196,	0x00, 0, WC, B)
__REG(FG_Left_A2L,	0x1197,	0x00, 0, WC, B)
__REG(FG_Left_A2H,	0x1198,	0x00, 0, WC, B)
__REG(FG_Left_A3L,	0x1199,	0x00, 0, WC, B)
__REG(FG_Left_A3H,	0x119A,	0x00, 0, WC, B)
__REG(FG_Left_A4L,	0x119B,	0x00, 0, WC, B)
__REG(FG_Left_A4H,	0x119C,	0x00, 0, WC, B)
__REG(FG_Left_A5L,	0x119D,	0x00, 0, WC, B)
__REG(FG_Left_A5H,	0x119E,	0x00, 0, WC, B)

__REG(FG_Right_Ctl,	0x119F,	0x00, 0, WC, B)
__REG(FG_Right_Time,0x11A0,	0x00, 0, WC, B)
__REG(FG_Right_A0L,	0x11A1,	0x00, 0, WC, B)
__REG(FG_Right_A0H,	0x11A2,	0x00, 0, WC, B)
__REG(FG_Right_A1L,	0x11A3,	0x00, 0, WC, B)
__REG(FG_Right_A1H,	0x11A4,	0x00, 0, WC, B)
__REG(FG_Right_A2L,	0x11A5,	0x00, 0, WC, B)
__REG(FG_Right_A2H,	0x11A6,	0x00, 0, WC, B)
__REG(FG_Right_A3L,	0x11A7,	0x00, 0, WC, B)
__REG(FG_Right_A3H,	0x11A8,	0x00, 0, WC, B)
__REG(FG_Right_A4L,	0x11A9,	0x00, 0, WC, B)
__REG(FG_Right_A4H,	0x11AA,	0x00, 0, WC, B)
__REG(FG_Right_A5L,	0x11AB,	0x00, 0, WC, B)
__REG(FG_Right_A5H,	0x11AC,	0x00, 0, WC, B)


////////////////////////////////////////////////////////////
//              AGC and VoiceOut related                  //
////////////////////////////////////////////////////////////
__REG(VoiceOutCtl,	0x11B1,	0x00, 0, CI, B)  // 16K, For Capture   **
/* reset value: b_10000000 */
#ifndef _MACRO_DEFINED_
#define VoiceOutRateMask  b_11000000
#endif

__REG(MicAGCCtl,	  0x11B2,	0x00, 0, WC, B)
__REG(AGCInitGainL,	0x11B3,	0x00, 0, WC, B)
__REG(AGCInitGainH,	0x11B4,	0x00, 0, WC, B)

////////////////////////////////////////////////////////////
//            AEC/LEC Adjustable Tail Lengths             //
////////////////////////////////////////////////////////////
__REG(AEC1TailLength,	0x11B5,	0x00, 0, WC, B)
__REG(AEC2TailLength,	0x11B6,	0x00, 0, WC, B)
__REG(AECCNVGTHL,	    0x11B7,	0x00, 0, WC, B)
__REG(AECCNVGTHH,	    0x11B8,	0x00, 0, WC, B)
__REG(LECTailLength,	0x11B9,	0x00, 0, WC, B)
__REG(LECCNVGTHL,	    0x11BA,	0x00, 0, WC, B)
__REG(LECCNVGTHH,	    0x11BB,	0x00, 0, WC, B)
__REG(DIALAGCL,	      0x11BC,	0x00, 0, WC, B)
__REG(DIALAGCH,	      0x11BD,	0x00, 0, WC, B)
__REG(AECNLPCtl,	    0x11BE,	0x00, 0, WC, B)

__REG(APIButtonCtl,	  0x11BF,	0x00, 0, WC, B)

__REG(NRAttnFloorL,	  0x11C0,	0x00, 0, WC, B)
__REG(NRAttnFloorH,	  0x11C1,	0x00, 0, WC, B)

__REG(NRMaxPowerL,	  0x11C2,	0x00, 0, WC, B)
__REG(NRMaxPowerH,	  0x11C3,	0x00, 0, WC, B)

__REG(IBNRAttnFloorL,	0x11C4,	0x00, 0, WC, B)
__REG(IBNRAttnFloorH,	0x11C5,	0x00, 0, WC, B)

__REG(IBNRMaxPowerL,	0x11C6,	0x00, 0, WC, B)
__REG(IBNRMaxPowerH,	0x11C7,	0x00, 0, WC, B)


__REG(MicBiquad1_KL,	0x11CB,	0x00, 0, WC, B)
__REG(MicBiquad1_KH,	0x11CC,	0x00, 0, WC, B)
__REG(MicBiquad1_b0L,	0x11CD,	0x00, 0, WC, B)
__REG(MicBiquad1_b0H,	0x11CE,	0x00, 0, WC, B)
__REG(MicBiquad1_b1L,	0x11CF,	0x00, 0, WC, B)
__REG(MicBiquad1_b1H,	0x11D0,	0x00, 0, WC, B)
__REG(MicBiquad1_b2L,	0x11D1,	0x00, 0, WC, B)
__REG(MicBiquad1_b2H,	0x11D2,	0x00, 0, WC, B)
__REG(MicBiquad1_a1L,	0x11D3,	0x00, 0, WC, B)
__REG(MicBiquad1_a1H,	0x11D4,	0x00, 0, WC, B)
__REG(MicBiquad1_a2L,	0x11D5,	0x00, 0, WC, B)
__REG(MicBiquad1_a2H,	0x11D6,	0x00, 0, WC, B)

__REG(MicBiquad2_KL,	0x11D7,	0x00, 0, WC, B)
__REG(MicBiquad2_KH,	0x11D8,	0x00, 0, WC, B)
__REG(MicBiquad2_b0L,	0x11D9,	0x00, 0, WC, B)
__REG(MicBiquad2_b0H,	0x11DA,	0x00, 0, WC, B)
__REG(MicBiquad2_b1L,	0x11DB,	0x00, 0, WC, B)
__REG(MicBiquad2_b1H,	0x11DC,	0x00, 0, WC, B)
__REG(MicBiquad2_b2L,	0x11DD,	0x00, 0, WC, B)
__REG(MicBiquad2_b2H,	0x11DE,	0x00, 0, WC, B)
__REG(MicBiquad2_a1L,	0x11DF,	0x00, 0, WC, B)
__REG(MicBiquad1_a1H,	0x11E0,	0x00, 0, WC, B)
__REG(MicBiquad2_a2L,	0x11E1,	0x00, 0, WC, B)
__REG(MicBiquad2_a2H,	0x11E2,	0x00, 0, WC, B)


/////////////////////////////////////////////////////////
//               EEPROM UPDATING RELATED               //
/////////////////////////////////////////////////////////
__REG(UpdateAL,	  0x02FC,	0x00, 0, RW, B)
__REG(UpdateAM,	  0x02FD,	0x00, 0, RW, B)
__REG(UpdateAH,	  0x02FE,	0x00, 0, RW, B)

__REG(UpdateLen,  0x02FF,	0x00, 0, RW, B)
__REG(UpdateBuff, 0x0300,	0x00, 0, RW, B)

__REG(UpdateCtr,  0x0400,	0x00, 0, RW, B)

/*
This control register allows API defaults to be stored and retrieved 
if the device has an EERPOM. When the API defaults are stored in the EEPROM
they can be recalled using this register. 
The last active profile will also be recalled on device power up.
Saving Profile 1
1) Write 0xC1 or 0xE1 to API Profile Control
2) Wait until ProfileAccessStart = 0
Restoring Profile 0
1) Write 0x80 to API Profile Control
2) Wait until ProfileAccessStart = 0
Restoring Profile 1
1) Write 0x81 to API Profile Control
2) Wait until ProfileAccessStart = 0
*/
__REG(APIProfileCtr, 0x040A,	0x00, 0, RW, B)
#endif

#ifndef _MACRO_DEFINED_
#define _MACRO_DEFINED_
#endif

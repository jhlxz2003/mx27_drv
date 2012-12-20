#ifndef CX20707_MACROS_H
#define CX20707_MACROS_H

#define cx20707_dsp_mode(codec)  \
    cx20707_update_register((codec), DSPInit0, 0, BypassDSP)

#define cx20707_writew_newc(codec,reg,val)  \
    do { \
        cx20707_real_write((codec), (reg), ((val)&0xff)); \
        cx20707_real_write((codec), (reg)+1, (((val)>>8)&0xff)); \
        cx20707_dsp_newc(codec); \
    } while (0)

#define cx20707_set_bit_newc(codec,reg,val) \
    do {  \
        if (cx20707_update_register((codec), (reg), (val), (val)) > 0) {\
            cx20707_dsp_newc(codec); \
        } \
    } while (0)

#define cx20707_clear_bit_newc(codec,reg,val) \
    do {  \
        if (cx20707_update_register((codec), (reg), 0, (val)) > 0) {\
            cx20707_dsp_newc(codec); \
        } \
    } while (0)

//////////////////////////////////////////////
//         dsp routing configuration        //
//////////////////////////////////////////////
#if 0
{
#endif

#define cx20707_mixer0_from_config(codec,pin,source) cx20707_update_register((codec), Mix0In0From + (pin), (source), 0xff)
#define cx20707_mixer1_from_config(codec,pin,source) cx20707_update_register((codec), Mix1In0From + (pin), (source), 0xff)
#define cx20707_voice_from_config(codec,source)      cx20707_update_register((codec), VoiceIn0From, (source), 0xff)
#define cx20707_stream7_from_config(codec,source)    cx20707_update_register((codec), S7STDACFrom, (source), 0xff)
#define cx20707_stream8_from_config(codec,source)    cx20707_update_register((codec), S8DACSubFrom, (source), 0xff)
#define cx20707_stream5_from_config(codec,source)    cx20707_update_register((codec), S5Dp1OutFrom, (source), 0xff)
#define cx20707_stream6_from_config(codec,source)    cx20707_update_register((codec), S6Dp2OutFrom, (source), 0xff)

#if 0
}
#endif

//////////////////////////////////////////////
//           streams configuration          //
//////////////////////////////////////////////
#if 0
{
#endif

/* stream1 setting */
#define cx20707_stream1_enable(codec)           cx20707_update_register((codec), DSPInit1, S1LinADCEnC, S1LinADCEnC)
#define cx20707_stream1_disable(codec)          cx20707_update_register((codec), DSPInit1, 0, S1LinADCEnC)
#define cx20707_stream1_input_rate_16k(codec)   cx20707_update_register((codec), S1ADCInpRate, 0x02, 0x0f)
#define cx20707_stream1_input_rate_48k(codec)   cx20707_update_register((codec), S1ADCInpRate, 0x07, 0x0f)
#define cx20707_stream1_mono_left(codec)        cx20707_update_register((codec), S1ADCInpRate, 0, (3<<6))
#define cx20707_stream1_mono_right(codec)       cx20707_update_register((codec), S1ADCInpRate, (1<<6), (3<<6))
#define cx20707_stream1_stero(codec)            cx20707_update_register((codec), S1ADCInpRate, (2<<6), (3<<6))

/* stream2 setting */
#define cx20707_stream2_enable(codec)           cx20707_update_register((codec), DSPInit1, S2MicADCEnC, S2MicADCEnC)
#define cx20707_stream2_disable(codec)          cx20707_update_register((codec), DSPInit1, 0, S2MicADCEnC)
#define cx20707_stream2_input_rate_16k(codec)   cx20707_update_register((codec), S2MicInpRate, 0x02, 0x0f)
#define cx20707_stream2_input_rate_48k(codec)   cx20707_update_register((codec), S2MicInpRate, 0x07, 0x0f)
#define cx20707_stream2_mono_left(codec)        cx20707_update_register((codec), S2MicInpRate, 0, (3<<6))
#define cx20707_stream2_mono_right(codec)       cx20707_update_register((codec), S2MicInpRate, (1<<6), (3<<6))
#define cx20707_stream2_stero(codec)            cx20707_update_register((codec), S2MicInpRate, (2<<6), (3<<6))

/* stream3 setting */
#define cx20707_stream3_enable(codec)           cx20707_update_register((codec), DSPInit1, S3I2SEnP, S3I2SEnP)
#define cx20707_stream3_disable(codec)          cx20707_update_register((codec), DSPInit1, 0, S3I2SEnP)
#define cx20707_stream3_src_config(codec,val)   cx20707_update_register((codec), S3InpRate, (val), 0x0f)
#define cx20707_stream3_mono_left(codec)        cx20707_update_register((codec), S3InpRate, 0, (3<<6))
#define cx20707_stream3_mono_right(codec)       cx20707_update_register((codec), S3InpRate, (1<<6), (3<<6))
#define cx20707_stream3_stero(codec)            cx20707_update_register((codec), S3InpRate, (2<<6), (3<<6))

/* stream4 setting */
#define cx20707_stream4_enable(codec)           cx20707_update_register((codec), DSPInit1, S4I2SEnP, S4I2SEnP)
#define cx20707_stream4_disable(codec)          cx20707_update_register((codec), DSPInit1, 0, S4I2SEnP)
#define cx20707_stream4_src_config(codec,val)   cx20707_update_register((codec), S4InpRate, (val), 0x0f)
#define cx20707_stream4_mono_left(codec)        cx20707_update_register((codec), S4InpRate, 0, (3<<6))
#define cx20707_stream4_mono_right(codec)       cx20707_update_register((codec), S4InpRate, (1<<6), (3<<6))
#define cx20707_stream4_stero(codec)            cx20707_update_register((codec), S4InpRate, (2<<6), (3<<6))

/* stream5 setting */
#define cx20707_stream5_enable(codec)           cx20707_update_register((codec), DSPInit1, S5DPRT1EnC, S5DPRT1EnC)
#define cx20707_stream5_disable(codec)          cx20707_update_register((codec), DSPInit1, 0, S5DPRT1EnC)
#define cx20707_stream5_src_config(codec, val)  cx20707_update_register((codec), S5OutpRate, (val), 0x0f)

/* stream6 setting */
#define cx20707_stream6_enable(codec)           cx20707_update_register((codec), DSPInit1, S6DPRT2EnC, S6DPRT2EnC)
#define cx20707_stream6_disable(codec)          cx20707_update_register((codec), DSPInit1, 0, S6DPRT2EnC)
#define cx20707_stream6_src_config(codec, val)  cx20707_update_register((codec), S6OutpRate, (val), 0x0f)

/* stream7 setting */
#define cx20707_stream7_enable(codec)           cx20707_update_register((codec), DSPInit1, S7STDACEnP, S7STDACEnP)
#define cx20707_stream7_disable(codec)          cx20707_update_register((codec), DSPInit1, 0, S7STDACEnP)
#define cx20707_stream7_output_rate_16k(codec)  cx20707_update_register((codec), S7OutpRate, 0x02, 0x0f)
#define cx20707_stream7_output_rate_48k(codec)  cx20707_update_register((codec), S7OutpRate, 0x07, 0x0f)

/* stream8 setting */
#define cx20707_stream8_enable(codec)           cx20707_update_register((codec), DSPInit0, S8MonoDACEnP, S8MonoDACEnP)
#define cx20707_stream8_disable(codec)          cx20707_update_register((codec), DSPInit0, 0, S8MonoDACEnP)
#define cx20707_stream8_output_rate_16k(codec)  cx20707_update_register((codec), S8OutpRate, 0x02, 0x0f)
#define cx20707_stream8_output_rate_48k(codec)  cx20707_update_register((codec), S8OutpRate, 0x07, 0x0f)

#if 0
}
#endif

///////////////////////////////////////////////////////////////
//                      Output Control                       //
///////////////////////////////////////////////////////////////
#if 0
{
#endif

#define cx20707_outctl_auto(codec)         cx20707_update_register((codec), OutCTL, OUT_CTRL_AUTO, OUT_CTRL_AUTO)
#define cx20707_outctl_manual(codec)       cx20707_update_register((codec), OutCTL, 0, OUT_CTRL_AUTO)

/* from stream7 */
#define cx20707_classD_enable(codec)       cx20707_set_bit_newc(codec, OutCTL, OUT_CTRL_CLASSD_EN)
#define cx20707_classD_disable(codec)      cx20707_clear_bit_newc(codec, OutCTL, OUT_CTRL_CLASSD_EN)

#define cx20707_classD_mono(codec)         cx20707_set_bit_newc(codec, OutCTL, OUT_CTRL_CLASSD_MONO)
#define cx20707_classD_stero(codec)        cx20707_clear_bit_newc(codec, OutCTL, OUT_CTRL_CLASSD_MONO)

#define cx20707_classD_pwm_mode(codec)     cx20707_set_bit_newc(codec, OutCTL, OUT_CTRL_CLASSD_PWM)
#define cx20707_classD_normal_mode(codec)  cx20707_clear_bit_newc(codec, OutCTL, OUT_CTRL_CLASSD_PWM)

#define cx20707_outp_muteL(codec)          cx20707_update_register((codec), Mute, LEFT_SPEAKER_MUTE, LEFT_SPEAKER_MUTE)
#define cx20707_outp_unmuteL(codec)        cx20707_update_register((codec), Mute, 0, LEFT_SPEAKER_MUTE)

#define cx20707_outp_muteR(codec)          cx20707_update_register((codec), Mute, RIGH_SPEAKER_MUTE, RIGH_SPEAKER_MUTE)
#define cx20707_outp_unmuteR(codec)        cx20707_update_register((codec), Mute, 0, RIGH_SPEAKER_MUTE)

#define cx20707_lineout_enable(codec)      cx20707_set_bit_newc(codec, OutCTL, OUT_CTRL_LO_EN)
#define cx20707_lineout_disable(codec)     cx20707_clear_bit_newc(codec, OutCTL, OUT_CTRL_LO_EN)

#define cx20707_lineout_diff(codec)        cx20707_set_bit_newc(codec, OutCTL, OUT_CTRL_LO_DIFF)
#define cx20707_lineout_single_end(codec)  cx20707_clear_bit_newc(codec, OutCTL, OUT_CTRL_LO_DIFF)

#define cx20707_headphone_enable(codec)    cx20707_set_bit_newc(codec, OutCTL, OUT_CTRL_HP_EN)
#define cx20707_headphone_disable(codec)   cx20707_clear_bit_newc(codec, OutCTL, OUT_CTRL_HP_EN)

/* from stream8 */
#define cx20707_lineout_sub_diff(codec)        cx20707_set_bit_newc(codec, OutCTL, OUT_CTRL_SUB_DIFF)
#define cx20707_lineout_sub_single_end(codec)  cx20707_clear_bit_newc(codec, OutCTL, OUT_CTRL_SUB_DIFF)

#if 0
}
#endif

///////////////////////////////////////////////////////////////
//                      input Control                        //
///////////////////////////////////////////////////////////////
#if 0
{
#endif

/* To Stream2 */
#define cx20707_mic_always_on(codec)  cx20707_update_register((codec), MicCTL, MIC_POWER_ALWAYS, MIC_POWER_ALWAYS)
#define cx20707_mic_needed_on(codec)  cx20707_update_register((codec), MicCTL, 0, MIC_POWER_ALWAYS)

#define cx20707_mic_bias_80p(codec)   cx20707_update_register((codec), MicCTL, MIC_BIAS_SELECT, MIC_BIAS_SELECT)
#define cx20707_mic_bias_50p(codec)   cx20707_update_register((codec), MicCTL, 0, MIC_BIAS_SELECT)

#define cx20707_mic_boost(codec,val)  cx20707_update_register((codec), MicCTL, ((val)&0x07), MIC_BOOST_MASK)

/* Microphone Gain and Mute */
#define cx20707_mic_gainL(codec,val)  cx20707_update_register((codec), MicADC2GainL, (val), 0xff)
#define cx20707_mic_gainR(codec,val)  cx20707_update_register((codec), MicADC2GainR, (val), 0xff)

#define cx20707_mic_muteL(codec)      cx20707_update_register((codec), Mute, LEFT_MIC_MUTE, LEFT_MIC_MUTE)
#define cx20707_mic_unmuteL(codec)    cx20707_update_register((codec), Mute, 0, LEFT_MIC_MUTE)

#define cx20707_mic_muteR(codec)      cx20707_update_register((codec), Mute, RIGH_MIC_MUTE, RIGH_MIC_MUTE)
#define cx20707_mic_unmuteR(codec)    cx20707_update_register((codec), Mute, 0, RIGH_MIC_MUTE)

/* Microphone SideTone Setting */
#define cx20707_mic_sidetone_enable(codec)   cx20707_update_register((codec), DSPEn1, SideTone_En, SideTone_En)
#define cx20707_mic_sidetone_disable(codec)  cx20707_update_register((codec), DSPEn1, 0, SideTone_En)
#define cx20707_mic_sidetone_gain(codec,val) cx20707_writew_newc(codec, SideToneGainL, val)

#define cx20707_mic_comfort_noise_level(codec,val) cx20707_writew_newc(codec, CNI_AttenuationL, val)

/* Microphone EQ undefiined */

/* LineIn Settiing To Stream1 */
#define cx20707_linctl_auto(codec)   cx20707_update_register((codec), LineCTL, IN_CTRL_AUTO, IN_CTRL_AUTO)
#define cx20707_linctl_manual(codec) cx20707_update_register((codec), LineCTL, 0, IN_CTRL_AUTO)

#define cx20707_lin_adc_gainL(codec,val) cx20707_update_register((codec), ADC1L, (val), 0xff)
#define cx20707_lin_adc_gainR(codec,val) cx20707_update_register((codec), ADC1R, (val), 0xff)

#define cx20707_lin_adc_muteL(codec)   cx20707_update_register((codec), Mute, LEFT_AUX_MUTE, LEFT_AUX_MUTE)
#define cx20707_lin_adc_muteR(codec)   cx20707_update_register((codec), Mute, RIGH_AUX_MUTE, RIGH_AUX_MUTE)

#define cx20707_lin_adc_unmuteL(codec) cx20707_update_register((codec), Mute, 0, LEFT_AUX_MUTE)
#define cx20707_lin_adc_unmuteR(codec) cx20707_update_register((codec), Mute, 0, RIGH_AUX_MUTE)

/* LineIn1 Setting */
#define cx20707_lin1_enable(codec)      cx20707_set_bit_newc(codec,LineCTL,IN_CTRL_L1_EN)
#define cx20707_lin1_disable(codec)     cx20707_clear_bit_newc(codec,LineCTL,IN_CTRL_L1_EN)
#define cx20707_lin1_diff(codec)        cx20707_set_bit_newc(codec,LineCTL,IN_CTRL_L1_DIFF)
#define cx20707_lin1_single_end(codec)  cx20707_clear_bit_newc(codec,LineCTL,IN_CTRL_L1_DIFF)
#define cx20707_lin1_gain(codec, val)   cx20707_update_register((codec), L1Gain, (val), LINE1_GAIN_MASK)
#define cx20707_lin1_mute(codec)        cx20707_update_register((codec), L1Gain, LINE1_MUTE, LINE1_MUTE)
#define cx20707_lin1_unmute(codec)      cx20707_update_register((codec), L1Gain, 0, LINE1_MUTE)

/* LineIn2 Setting */
#define cx20707_lin2_enable(codec)   cx20707_set_bit_newc(codec,LineCTL,IN_CTRL_L2_EN)
#define cx20707_lin2_disable(codec)  cx20707_clear_bit_newc(codec,LineCTL,IN_CTRL_L2_EN)
#define cx20707_lin2_gain(codec,val) cx20707_update_register((codec), L2Gain, (val), LINE2_GAIN_MASK)
#define cx20707_lin2_mute(codec)     cx20707_update_register((codec), L2Gain, LINE2_MUTE, LINE2_MUTE)
#define cx20707_lin2_unmute(codec)   cx20707_update_register((codec), L2Gain, 0, LINE2_MUTE)

/* Mixer Setting */
#define cx20707_mix0_mute(codec,pin)   cx20707_update_register((codec), (Mix0In0Gain+(pin)), 0x80, 0x80)
#define cx20707_mix0_unmute(codec,pin) cx20707_update_register((codec), (Mix0In0Gain+(pin)), 0, 0x80)
#define cx20707_mix1_mute(codec,pin)   cx20707_update_register((codec), (Mix1In0Gain+(pin)), 0x80, 0x80)
#define cx20707_mix1_unmute(codec,pin) cx20707_update_register((codec), (Mix1In0Gain+(pin)), 0, 0x80)

#define cx20707_mix0_gain(codec,pin,val) cx20707_update_register((codec), (Mix0In0Gain+(pin)), ((val)&0x7f), 0x7f)
#define cx20707_mix1_gain(codec,pin,val) cx20707_update_register((codec), (Mix1In0Gain+(pin)), ((val)&0x7f), 0x7f)

#if 0
}
#endif

///////////////////////////////////////////////////////////////
//                  Sound Effects Processing                 //
///////////////////////////////////////////////////////////////
#if 0
{
#endif
/* DRC Setting, AEC should be disabled */
#define cx20707_DRC_enable(codec)   cx20707_update_register((codec), DSPEn2, DRC_En, DRC_En)
#define cx20707_DRC_disable(codec)  cx20707_update_register((codec), DSPEn2, 0, DRC_En)

#define cx20707_DRC_ratio(codec,val)        cx20707_writew_newc(codec,DRC_RatioL,val)
#define cx20707_DRC_threshold(codec,val)    cx20707_writew_newc(codec,DRC_ThreshL,val)
#define cx20707_DRC_MaxAmpl(codec,val)      cx20707_writew_newc(codec,DRC_MaxAmplL,val)
#define cx20707_DRC_boost(codec,val)        cx20707_writew_newc(codec,DRC_BoostL,val)
#define cx20707_DRC_attack_time(codec,val)  cx20707_writew_newc(codec,DRC_AttackL,val)
#define cx20707_DRC_release_time(codec,val) cx20707_writew_newc(codec,DRC_ReleaseL,val)

/* In Bound Noise Reduction, LEC should be enabled */
#define cx20707_inbound_noise_reduction_enable(codec)   cx20707_update_register((codec), DSPEn1, IBNR_En, IBNR_En)
#define cx20707_inbound_noise_reduction_disable(codec)  cx20707_update_register((codec), DSPEn1, 0, IBNR_En)

#define cx20707_inbound_noise_reduction_SNR(codec,val)      cx20707_writew_newc(codec,IBNRTotalSNRL,val)
#define cx20707_inbound_noise_reduction_SNR_sub(codec,val)  cx20707_writew_newc(codec,IBNRSubSNRL,val)
#define cx20707_inbound_noise_reduction_min_gain(codec,val) cx20707_writew_newc(codec,IBNR_MinGainL,val)

#if 0
}
#endif

///////////////////////////////////////////////////////////////
//                     Voice Processing                      //
///////////////////////////////////////////////////////////////
#if 0
{
#endif

/* AEC Setting */
#define cx20707_AEC_enable(codec)  cx20707_update_register((codec), DSPEn1, AEC_En, AEC_En)
#define cx20707_AEC_disable(codec) cx20707_update_register((codec), DSPEn1, 0, AEC_En)

#define cx20707_AEC_adapt_coef(codec,val)                 cx20707_writew_newc(codec,AECAdptL,val)
#define cx20707_AEC_FDNLP_gain_for_initial_convergence(codec,val)  cx20707_writew_newc(codec,AECNLPGainL,val)
#define cx20707_AEC_FDNLP_gain_for_DBLTalk(codec,val)     cx20707_writew_newc(codec,AECDblTalkL,val)
#define cx20707_AEC_FDNLP_gain_for_conv_period(codec,val) cx20707_writew_newc(codec,AECConvPeriodL,val)
#define cx20707_AEC_far_end_low_threshold(codec,val)      cx20707_writew_newc(codec,FAR_AEC_THDLL,val)
#define cx20707_AEC_far_end_high_threshold(codec,val)     cx20707_writew_newc(codec,FAR_AEC_THDHL,val)
#define cx20707_AEC_ERLE_threshold(codec,val)             cx20707_writew_newc(codec,HAEC_ERLL,val)
#define cx20707_AEC_bulk_delay_gain(codec,val)            cx20707_writew_newc(codec,AECBulkDlyGainL,val)

#define cx20707_AEC_bulk_delay_LEC_off(codec, val) cx20707_update_register((codec), HTXDLY0, (val), 0xff)
#define cx20707_AEC_bulk_delay_LEC_on(codec, val)  cx20707_update_register((codec), HTXDLY1, (val), 0xff)

/* AEC DRC Setting */
#define cx20707_AEC_DRC_ratio(codec,val)        cx20707_writew_newc(codec,AECDRC_RatioL,val)
#define cx20707_AEC_DRC_threshold(codec,val)    cx20707_writew_newc(codec,AECDRC_ThreshL,val)
#define cx20707_AEC_DRC_MaxAmpl(codec,val)      cx20707_writew_newc(codec,AECDRC_MaxAmplL,val)
#define cx20707_AEC_DRC_boost(codec,val)        cx20707_writew_newc(codec,AECDRC_BoostL,val)
#define cx20707_AEC_DRC_attack_time(codec,val)  cx20707_writew_newc(codec,AECDRC_AttackL,val)
#define cx20707_AEC_DRC_release_time(codec,val) cx20707_writew_newc(codec,AECDRC_ReleaseL,val)

/* OutBound Noise Reduction Setting */
#define cx20707_outbound_mic_noise_reduction_enable(codec)  \
    cx20707_update_register((codec), DSPEn1, NR_En, NR_En)

#define cx20707_outbound_mic_noise_reduction_disable(codec) \
    cx20707_update_register((codec), DSPEn1, 0, NR_En)

#define cx20707_outbound_mic_noise_reduction_SNR(codec,val)      cx20707_writew_newc(codec,NRTotalSNRL,val)
#define cx20707_outbound_mic_noise_reduction_SNR_sub(codec,val)  cx20707_writew_newc(codec,NRSubSNRL,val)
#define cx20707_outbound_mic_noise_reduction_min_gain(codec,val) cx20707_writew_newc(codec,NR_MinGainL,val)

/* Mic AGC Setting */
#define cx20707_MicAGC_enable(codec)     cx20707_update_register(codec, DSPEn1, MicAGC_En, MicAGC_En)
#define cx20707_MicAGC_disable(codec)    cx20707_update_register(codec, DSPEn1, 0, MicAGC_En)

#define cx20707_MicAGC_level(codec,val)            cx20707_writew_newc(codec,AGCLevelL,val)
#define cx20707_MicAGC_gain_incr(codec,val)        cx20707_writew_newc(codec,AGCGainIncrL,val)
#define cx20707_MicAGC_gain_decr(codec,val)        cx20707_writew_newc(codec,AGCGainDecrL,val)
#define cx20707_MicAGC_update_rate(codec,val)      cx20707_writew_newc(codec,AGCChkUpdL,val)
#define cx20707_MicAGC_max_linout_level(codec,val) cx20707_writew_newc(codec,AGCMaxLinOutL,val)
#define cx20707_MicAGC_output_limit(codec,val)     cx20707_writew_newc(codec,AGCMaxOutpLimitL,val)

#if 0
}
#endif


#endif

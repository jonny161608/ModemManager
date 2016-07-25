/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details:
 *
 * Copyright (C) 2011 Red Hat, Inc.
 * Copyright (C) 2011 Google, Inc.
 */

#ifndef _MODEMMANAGER_ENUMS_H_
#define _MODEMMANAGER_ENUMS_H_

#if !defined (__MODEM_MANAGER_H_INSIDE__)
#error "Only <ModemManager.h> can be included directly."
#endif

/**
 * SECTION:mm-enums
 * @short_description: Common enumerations and types in the API.
 *
 * This section defines enumerations and types that are used in the
 * ModemManager interface.
 **/

/**
 * MMModemCapability:
 * @MM_MODEM_CAPABILITY_NONE: Modem has no capabilities.
 * @MM_MODEM_CAPABILITY_POTS: Modem supports the analog wired telephone network (ie 56k dialup) and does not have wireless/cellular capabilities.
 * @MM_MODEM_CAPABILITY_CDMA_EVDO: Modem supports at least one of CDMA 1xRTT, EVDO revision 0, EVDO revision A, or EVDO revision B.
 * @MM_MODEM_CAPABILITY_GSM_UMTS: Modem supports at least one of GSM, GPRS, EDGE, UMTS, HSDPA, HSUPA, or HSPA+ packet switched data capability.
 * @MM_MODEM_CAPABILITY_LTE: Modem has LTE data capability.
 * @MM_MODEM_CAPABILITY_LTE_ADVANCED: Modem has LTE Advanced data capability.
 * @MM_MODEM_CAPABILITY_IRIDIUM: Modem has Iridium capabilities.
 * @MM_MODEM_CAPABILITY_ANY: Mask specifying all capabilities.
 *
 * Flags describing one or more of the general access technology families that a
 * modem supports.
 */
typedef enum { /*< underscore_name=mm_modem_capability >*/
    MM_MODEM_CAPABILITY_NONE         = 0,
    MM_MODEM_CAPABILITY_POTS         = 1 << 0,
    MM_MODEM_CAPABILITY_CDMA_EVDO    = 1 << 1,
    MM_MODEM_CAPABILITY_GSM_UMTS     = 1 << 2,
    MM_MODEM_CAPABILITY_LTE          = 1 << 3,
    MM_MODEM_CAPABILITY_LTE_ADVANCED = 1 << 4,
    MM_MODEM_CAPABILITY_IRIDIUM      = 1 << 5,
    MM_MODEM_CAPABILITY_ANY          = 0xFFFFFFFF
} MMModemCapability;

/**
 * MMModemLock:
 * @MM_MODEM_LOCK_UNKNOWN: Lock reason unknown.
 * @MM_MODEM_LOCK_NONE: Modem is unlocked.
 * @MM_MODEM_LOCK_SIM_PIN: SIM requires the PIN code.
 * @MM_MODEM_LOCK_SIM_PIN2: SIM requires the PIN2 code.
 * @MM_MODEM_LOCK_SIM_PUK: SIM requires the PUK code.
 * @MM_MODEM_LOCK_SIM_PUK2: SIM requires the PUK2 code.
 * @MM_MODEM_LOCK_PH_SP_PIN: Modem requires the service provider PIN code.
 * @MM_MODEM_LOCK_PH_SP_PUK: Modem requires the service provider PUK code.
 * @MM_MODEM_LOCK_PH_NET_PIN: Modem requires the network PIN code.
 * @MM_MODEM_LOCK_PH_NET_PUK: Modem requires the network PUK code.
 * @MM_MODEM_LOCK_PH_SIM_PIN: Modem requires the PIN code.
 * @MM_MODEM_LOCK_PH_CORP_PIN: Modem requires the corporate PIN code.
 * @MM_MODEM_LOCK_PH_CORP_PUK: Modem requires the corporate PUK code.
 * @MM_MODEM_LOCK_PH_FSIM_PIN: Modem requires the PH-FSIM PIN code.
 * @MM_MODEM_LOCK_PH_FSIM_PUK: Modem requires the PH-FSIM PUK code.
 * @MM_MODEM_LOCK_PH_NETSUB_PIN: Modem requires the network subset PIN code.
 * @MM_MODEM_LOCK_PH_NETSUB_PUK: Modem requires the network subset PUK code.
 *
 * Enumeration of possible lock reasons.
 */
typedef enum { /*< underscore_name=mm_modem_lock >*/
    MM_MODEM_LOCK_UNKNOWN        = 0,
    MM_MODEM_LOCK_NONE           = 1,
    MM_MODEM_LOCK_SIM_PIN        = 2,
    MM_MODEM_LOCK_SIM_PIN2       = 3,
    MM_MODEM_LOCK_SIM_PUK        = 4,
    MM_MODEM_LOCK_SIM_PUK2       = 5,
    MM_MODEM_LOCK_PH_SP_PIN      = 6,
    MM_MODEM_LOCK_PH_SP_PUK      = 7,
    MM_MODEM_LOCK_PH_NET_PIN     = 8,
    MM_MODEM_LOCK_PH_NET_PUK     = 9,
    MM_MODEM_LOCK_PH_SIM_PIN     = 10,
    MM_MODEM_LOCK_PH_CORP_PIN    = 11,
    MM_MODEM_LOCK_PH_CORP_PUK    = 12,
    MM_MODEM_LOCK_PH_FSIM_PIN    = 13,
    MM_MODEM_LOCK_PH_FSIM_PUK    = 14,
    MM_MODEM_LOCK_PH_NETSUB_PIN  = 15,
    MM_MODEM_LOCK_PH_NETSUB_PUK  = 16
} MMModemLock;

/**
 * MMModemState:
 * @MM_MODEM_STATE_FAILED: The modem is unusable.
 * @MM_MODEM_STATE_UNKNOWN: State unknown or not reportable.
 * @MM_MODEM_STATE_INITIALIZING: The modem is currently being initialized.
 * @MM_MODEM_STATE_LOCKED: The modem needs to be unlocked.
 * @MM_MODEM_STATE_DISABLED: The modem is not enabled and is powered down.
 * @MM_MODEM_STATE_DISABLING: The modem is currently transitioning to the @MM_MODEM_STATE_DISABLED state.
 * @MM_MODEM_STATE_ENABLING: The modem is currently transitioning to the @MM_MODEM_STATE_ENABLED state.
 * @MM_MODEM_STATE_ENABLED: The modem is enabled and powered on but not registered with a network provider and not available for data connections.
 * @MM_MODEM_STATE_SEARCHING: The modem is searching for a network provider to register with.
 * @MM_MODEM_STATE_REGISTERED: The modem is registered with a network provider, and data connections and messaging may be available for use.
 * @MM_MODEM_STATE_DISCONNECTING: The modem is disconnecting and deactivating the last active packet data bearer. This state will not be entered if more than one packet data bearer is active and one of the active bearers is deactivated.
 * @MM_MODEM_STATE_CONNECTING: The modem is activating and connecting the first packet data bearer. Subsequent bearer activations when another bearer is already active do not cause this state to be entered.
 * @MM_MODEM_STATE_CONNECTED: One or more packet data bearers is active and connected.
 *
 * Enumeration of possible modem states.
 */
typedef enum { /*< underscore_name=mm_modem_state >*/
    MM_MODEM_STATE_FAILED        = -1,
    MM_MODEM_STATE_UNKNOWN       = 0,
    MM_MODEM_STATE_INITIALIZING  = 1,
    MM_MODEM_STATE_LOCKED        = 2,
    MM_MODEM_STATE_DISABLED      = 3,
    MM_MODEM_STATE_DISABLING     = 4,
    MM_MODEM_STATE_ENABLING      = 5,
    MM_MODEM_STATE_ENABLED       = 6,
    MM_MODEM_STATE_SEARCHING     = 7,
    MM_MODEM_STATE_REGISTERED    = 8,
    MM_MODEM_STATE_DISCONNECTING = 9,
    MM_MODEM_STATE_CONNECTING    = 10,
    MM_MODEM_STATE_CONNECTED     = 11
} MMModemState;

/**
 * MMModemStateFailedReason:
 * @MM_MODEM_STATE_FAILED_REASON_NONE: No error.
 * @MM_MODEM_STATE_FAILED_REASON_UNKNOWN: Unknown error.
 * @MM_MODEM_STATE_FAILED_REASON_SIM_MISSING: SIM is required but missing.
 * @MM_MODEM_STATE_FAILED_REASON_SIM_ERROR: SIM is available, but unusable (e.g. permanently locked).
 *
 * Enumeration of possible errors when the modem is in @MM_MODEM_STATE_FAILED.
 */
typedef enum { /*< underscore_name=mm_modem_state_failed_reason >*/
    MM_MODEM_STATE_FAILED_REASON_NONE        = 0,
    MM_MODEM_STATE_FAILED_REASON_UNKNOWN     = 1,
    MM_MODEM_STATE_FAILED_REASON_SIM_MISSING = 2,
    MM_MODEM_STATE_FAILED_REASON_SIM_ERROR   = 3,
} MMModemStateFailedReason;

/**
 * MMModemPowerState:
 * @MM_MODEM_POWER_STATE_UNKNOWN: Unknown power state.
 * @MM_MODEM_POWER_STATE_OFF: Off.
 * @MM_MODEM_POWER_STATE_LOW: Low-power mode.
 * @MM_MODEM_POWER_STATE_ON: Full power mode.
 *
 * Power state of the modem.
 */
typedef enum { /*< underscore_name=mm_modem_power_state >*/
    MM_MODEM_POWER_STATE_UNKNOWN = 0,
    MM_MODEM_POWER_STATE_OFF     = 1,
    MM_MODEM_POWER_STATE_LOW     = 2,
    MM_MODEM_POWER_STATE_ON      = 3
} MMModemPowerState;

/**
 * MMModemStateChangeReason:
 * @MM_MODEM_STATE_CHANGE_REASON_UNKNOWN: Reason unknown or not reportable.
 * @MM_MODEM_STATE_CHANGE_REASON_USER_REQUESTED: State change was requested by an interface user.
 * @MM_MODEM_STATE_CHANGE_REASON_SUSPEND: State change was caused by a system suspend.
 * @MM_MODEM_STATE_CHANGE_REASON_FAILURE: State change was caused by an unrecoverable error.
 *
 * Enumeration of possible reasons to have changed the modem state.
 */
typedef enum { /*< underscore_name=mm_modem_state_change_reason >*/
    MM_MODEM_STATE_CHANGE_REASON_UNKNOWN        = 0,
    MM_MODEM_STATE_CHANGE_REASON_USER_REQUESTED = 1,
    MM_MODEM_STATE_CHANGE_REASON_SUSPEND        = 2,
    MM_MODEM_STATE_CHANGE_REASON_FAILURE        = 3,
} MMModemStateChangeReason;

/**
 * MMModemAccessTechnology:
 * @MM_MODEM_ACCESS_TECHNOLOGY_UNKNOWN: The access technology used is unknown.
 * @MM_MODEM_ACCESS_TECHNOLOGY_POTS: Analog wireline telephone.
 * @MM_MODEM_ACCESS_TECHNOLOGY_GSM: GSM.
 * @MM_MODEM_ACCESS_TECHNOLOGY_GSM_COMPACT: Compact GSM.
 * @MM_MODEM_ACCESS_TECHNOLOGY_GPRS: GPRS.
 * @MM_MODEM_ACCESS_TECHNOLOGY_EDGE: EDGE (ETSI 27.007: "GSM w/EGPRS").
 * @MM_MODEM_ACCESS_TECHNOLOGY_UMTS: UMTS (ETSI 27.007: "UTRAN").
 * @MM_MODEM_ACCESS_TECHNOLOGY_HSDPA: HSDPA (ETSI 27.007: "UTRAN w/HSDPA").
 * @MM_MODEM_ACCESS_TECHNOLOGY_HSUPA: HSUPA (ETSI 27.007: "UTRAN w/HSUPA").
 * @MM_MODEM_ACCESS_TECHNOLOGY_HSPA: HSPA (ETSI 27.007: "UTRAN w/HSDPA and HSUPA").
 * @MM_MODEM_ACCESS_TECHNOLOGY_HSPA_PLUS: HSPA+ (ETSI 27.007: "UTRAN w/HSPA+").
 * @MM_MODEM_ACCESS_TECHNOLOGY_1XRTT: CDMA2000 1xRTT.
 * @MM_MODEM_ACCESS_TECHNOLOGY_EVDO0: CDMA2000 EVDO revision 0.
 * @MM_MODEM_ACCESS_TECHNOLOGY_EVDOA: CDMA2000 EVDO revision A.
 * @MM_MODEM_ACCESS_TECHNOLOGY_EVDOB: CDMA2000 EVDO revision B.
 * @MM_MODEM_ACCESS_TECHNOLOGY_LTE: LTE (ETSI 27.007: "E-UTRAN")
 * @MM_MODEM_ACCESS_TECHNOLOGY_ANY: Mask specifying all access technologies.
 *
 * Describes various access technologies that a device uses when registered with
 * or connected to a network.
 */
typedef enum { /*< underscore_name=mm_modem_access_technology >*/
    MM_MODEM_ACCESS_TECHNOLOGY_UNKNOWN     = 0,
    MM_MODEM_ACCESS_TECHNOLOGY_POTS        = 1 << 0,
    MM_MODEM_ACCESS_TECHNOLOGY_GSM         = 1 << 1,
    MM_MODEM_ACCESS_TECHNOLOGY_GSM_COMPACT = 1 << 2,
    MM_MODEM_ACCESS_TECHNOLOGY_GPRS        = 1 << 3,
    MM_MODEM_ACCESS_TECHNOLOGY_EDGE        = 1 << 4,
    MM_MODEM_ACCESS_TECHNOLOGY_UMTS        = 1 << 5,
    MM_MODEM_ACCESS_TECHNOLOGY_HSDPA       = 1 << 6,
    MM_MODEM_ACCESS_TECHNOLOGY_HSUPA       = 1 << 7,
    MM_MODEM_ACCESS_TECHNOLOGY_HSPA        = 1 << 8,
    MM_MODEM_ACCESS_TECHNOLOGY_HSPA_PLUS   = 1 << 9,
    MM_MODEM_ACCESS_TECHNOLOGY_1XRTT       = 1 << 10,
    MM_MODEM_ACCESS_TECHNOLOGY_EVDO0       = 1 << 11,
    MM_MODEM_ACCESS_TECHNOLOGY_EVDOA       = 1 << 12,
    MM_MODEM_ACCESS_TECHNOLOGY_EVDOB       = 1 << 13,
    MM_MODEM_ACCESS_TECHNOLOGY_LTE         = 1 << 14,
    MM_MODEM_ACCESS_TECHNOLOGY_ANY         = 0xFFFFFFFF,
} MMModemAccessTechnology;

/**
 * MMModemMode:
 * @MM_MODEM_MODE_NONE: None.
 * @MM_MODEM_MODE_CS: CSD, GSM, and other circuit-switched technologies.
 * @MM_MODEM_MODE_2G: GPRS, EDGE.
 * @MM_MODEM_MODE_3G: UMTS, HSxPA.
 * @MM_MODEM_MODE_4G: LTE.
 * @MM_MODEM_MODE_ANY: Any mode can be used (only this value allowed for POTS modems).
 *
 * Bitfield to indicate which access modes are supported, allowed or
 * preferred in a given device.
 */
typedef enum { /*< underscore_name=mm_modem_mode >*/
    MM_MODEM_MODE_NONE = 0,
    MM_MODEM_MODE_CS   = 1 << 0,
    MM_MODEM_MODE_2G   = 1 << 1,
    MM_MODEM_MODE_3G   = 1 << 2,
    MM_MODEM_MODE_4G   = 1 << 3,
    MM_MODEM_MODE_ANY  = 0xFFFFFFFF
} MMModemMode;

/**
 * MMModemBand:
 * @MM_MODEM_BAND_UNKNOWN: Unknown or invalid band.
 * @MM_MODEM_BAND_EGSM: GSM/GPRS/EDGE 900 MHz.
 * @MM_MODEM_BAND_DCS: GSM/GPRS/EDGE 1800 MHz.
 * @MM_MODEM_BAND_PCS: GSM/GPRS/EDGE 1900 MHz.
 * @MM_MODEM_BAND_G850: GSM/GPRS/EDGE 850 MHz.
 * @MM_MODEM_BAND_G450: GSM/GPRS/EDGE 450 MHz.
 * @MM_MODEM_BAND_G480: GSM/GPRS/EDGE 480 MHz.
 * @MM_MODEM_BAND_G750: GSM/GPRS/EDGE 750 MHz.
 * @MM_MODEM_BAND_G380: GSM/GPRS/EDGE 380 MHz.
 * @MM_MODEM_BAND_G410: GSM/GPRS/EDGE 410 MHz.
 * @MM_MODEM_BAND_G710: GSM/GPRS/EDGE 710 MHz.
 * @MM_MODEM_BAND_G810: GSM/GPRS/EDGE 810 MHz.
 * @MM_MODEM_BAND_UTRAN_1: UMTS 2100 MHz (IMT, UTRAN band 1).
 * @MM_MODEM_BAND_UTRAN_2: UMTS 1900 MHz (PCS A-F, UTRAN band 2).
 * @MM_MODEM_BAND_UTRAN_3: UMTS 1800 MHz (DCS, UTRAN band 3).
 * @MM_MODEM_BAND_UTRAN_4: UMTS 1700 MHz (AWS A-F, UTRAN band 4).
 * @MM_MODEM_BAND_UTRAN_5: UMTS 850 MHz (CLR, UTRAN band 5).
 * @MM_MODEM_BAND_UTRAN_6: UMTS 800 MHz (UTRAN band 6).
 * @MM_MODEM_BAND_UTRAN_7: UMTS 2600 MHz (IMT-E, UTRAN band 7).
 * @MM_MODEM_BAND_UTRAN_8: UMTS 900 MHz (E-GSM, UTRAN band 8).
 * @MM_MODEM_BAND_UTRAN_9: UMTS 1700 MHz (UTRAN band 9).
 * @MM_MODEM_BAND_UTRAN_10: UMTS 1700 MHz (EAWS A-G, UTRAN band 10).
 * @MM_MODEM_BAND_UTRAN_11: UMTS 1500 MHz (LPDC, UTRAN band 11).
 * @MM_MODEM_BAND_UTRAN_12: UMTS 700 MHz (LSMH A/B/C, UTRAN band 12).
 * @MM_MODEM_BAND_UTRAN_13: UMTS 700 MHz (USMH C, UTRAN band 13).
 * @MM_MODEM_BAND_UTRAN_14: UMTS 700 MHz (USMH D, UTRAN band 14).
 * @MM_MODEM_BAND_UTRAN_19: UMTS 800 MHz (UTRAN band 19).
 * @MM_MODEM_BAND_UTRAN_20: UMTS 800 MHz (EUDD, UTRAN band 20).
 * @MM_MODEM_BAND_UTRAN_21: UMTS 1500 MHz (UPDC, UTRAN band 21).
 * @MM_MODEM_BAND_UTRAN_22: UMTS 3500 MHz (UTRAN band 22).
 * @MM_MODEM_BAND_UTRAN_25: UMTS 1900 MHz (EPCS A-G, UTRAN band 25).
 * @MM_MODEM_BAND_UTRAN_26: UMTS 850 MHz (ECLR, UTRAN band 26).
 * @MM_MODEM_BAND_UTRAN_32: UMTS 1500 MHz (L-band, UTRAN band 32).
 * @MM_MODEM_BAND_EUTRAN_1: E-UTRAN band 1.
 * @MM_MODEM_BAND_EUTRAN_2: E-UTRAN band 2.
 * @MM_MODEM_BAND_EUTRAN_3: E-UTRAN band 3.
 * @MM_MODEM_BAND_EUTRAN_4: E-UTRAN band 4.
 * @MM_MODEM_BAND_EUTRAN_5: E-UTRAN band 5.
 * @MM_MODEM_BAND_EUTRAN_6: E-UTRAN band 6.
 * @MM_MODEM_BAND_EUTRAN_7: E-UTRAN band 7.
 * @MM_MODEM_BAND_EUTRAN_8: E-UTRAN band 8.
 * @MM_MODEM_BAND_EUTRAN_9: E-UTRAN band 9.
 * @MM_MODEM_BAND_EUTRAN_10: E-UTRAN band 10.
 * @MM_MODEM_BAND_EUTRAN_11: E-UTRAN band 11.
 * @MM_MODEM_BAND_EUTRAN_12: E-UTRAN band 12.
 * @MM_MODEM_BAND_EUTRAN_13: E-UTRAN band 13.
 * @MM_MODEM_BAND_EUTRAN_14: E-UTRAN band 14.
 * @MM_MODEM_BAND_EUTRAN_17: E-UTRAN band 17.
 * @MM_MODEM_BAND_EUTRAN_18: E-UTRAN band 18.
 * @MM_MODEM_BAND_EUTRAN_19: E-UTRAN band 19.
 * @MM_MODEM_BAND_EUTRAN_20: E-UTRAN band 20.
 * @MM_MODEM_BAND_EUTRAN_21: E-UTRAN band 21.
 * @MM_MODEM_BAND_EUTRAN_22: E-UTRAN band 22.
 * @MM_MODEM_BAND_EUTRAN_23: E-UTRAN band 23.
 * @MM_MODEM_BAND_EUTRAN_24: E-UTRAN band 24.
 * @MM_MODEM_BAND_EUTRAN_25: E-UTRAN band 25.
 * @MM_MODEM_BAND_EUTRAN_26: E-UTRAN band 26.
 * @MM_MODEM_BAND_EUTRAN_27: E-UTRAN band 27.
 * @MM_MODEM_BAND_EUTRAN_28: E-UTRAN band 28.
 * @MM_MODEM_BAND_EUTRAN_29: E-UTRAN band 29.
 * @MM_MODEM_BAND_EUTRAN_30: E-UTRAN band 30.
 * @MM_MODEM_BAND_EUTRAN_31: E-UTRAN band 31.
 * @MM_MODEM_BAND_EUTRAN_32: E-UTRAN band 32.
 * @MM_MODEM_BAND_EUTRAN_33: E-UTRAN band 33.
 * @MM_MODEM_BAND_EUTRAN_34: E-UTRAN band 34.
 * @MM_MODEM_BAND_EUTRAN_35: E-UTRAN band 35.
 * @MM_MODEM_BAND_EUTRAN_36: E-UTRAN band 36.
 * @MM_MODEM_BAND_EUTRAN_37: E-UTRAN band 37.
 * @MM_MODEM_BAND_EUTRAN_38: E-UTRAN band 38.
 * @MM_MODEM_BAND_EUTRAN_39: E-UTRAN band 39.
 * @MM_MODEM_BAND_EUTRAN_40: E-UTRAN band 40.
 * @MM_MODEM_BAND_EUTRAN_41: E-UTRAN band 41.
 * @MM_MODEM_BAND_EUTRAN_42: E-UTRAN band 42.
 * @MM_MODEM_BAND_EUTRAN_43: E-UTRAN band 43.
 * @MM_MODEM_BAND_EUTRAN_44: E-UTRAN band 44.
 * @MM_MODEM_BAND_EUTRAN_45: E-UTRAN band 45.
 * @MM_MODEM_BAND_EUTRAN_46: E-UTRAN band 46.
 * @MM_MODEM_BAND_EUTRAN_47: E-UTRAN band 47.
 * @MM_MODEM_BAND_EUTRAN_48: E-UTRAN band 48.
 * @MM_MODEM_BAND_EUTRAN_65: E-UTRAN band 65.
 * @MM_MODEM_BAND_EUTRAN_66: E-UTRAN band 66.
 * @MM_MODEM_BAND_EUTRAN_67: E-UTRAN band 67.
 * @MM_MODEM_BAND_EUTRAN_68: E-UTRAN band 68.
 * @MM_MODEM_BAND_EUTRAN_69: E-UTRAN band 69.
 * @MM_MODEM_BAND_EUTRAN_70: E-UTRAN band 70.
 * @MM_MODEM_BAND_EUTRAN_71: E-UTRAN band 71.
 * @MM_MODEM_BAND_CDMA_BC0: CDMA Band Class 0 (US Cellular 850MHz).
 * @MM_MODEM_BAND_CDMA_BC1: CDMA Band Class 1 (US PCS 1900MHz).
 * @MM_MODEM_BAND_CDMA_BC2: CDMA Band Class 2 (UK TACS 900MHz).
 * @MM_MODEM_BAND_CDMA_BC3: CDMA Band Class 3 (Japanese TACS).
 * @MM_MODEM_BAND_CDMA_BC4: CDMA Band Class 4 (Korean PCS).
 * @MM_MODEM_BAND_CDMA_BC5: CDMA Band Class 5 (NMT 450MHz).
 * @MM_MODEM_BAND_CDMA_BC6: CDMA Band Class 6 (IMT2000 2100MHz).
 * @MM_MODEM_BAND_CDMA_BC7: CDMA Band Class 7 (Cellular 700MHz).
 * @MM_MODEM_BAND_CDMA_BC8: CDMA Band Class 8 (1800MHz).
 * @MM_MODEM_BAND_CDMA_BC9: CDMA Band Class 9 (900MHz).
 * @MM_MODEM_BAND_CDMA_BC10: CDMA Band Class 10 (US Secondary 800).
 * @MM_MODEM_BAND_CDMA_BC11: CDMA Band Class 11 (European PAMR 400MHz).
 * @MM_MODEM_BAND_CDMA_BC12: CDMA Band Class 12 (PAMR 800MHz).
 * @MM_MODEM_BAND_CDMA_BC13: CDMA Band Class 13 (IMT2000 2500MHz Expansion).
 * @MM_MODEM_BAND_CDMA_BC14: CDMA Band Class 14 (More US PCS 1900MHz).
 * @MM_MODEM_BAND_CDMA_BC15: CDMA Band Class 15 (AWS 1700MHz).
 * @MM_MODEM_BAND_CDMA_BC16: CDMA Band Class 16 (US 2500MHz).
 * @MM_MODEM_BAND_CDMA_BC17: CDMA Band Class 17 (US 2500MHz Forward Link Only).
 * @MM_MODEM_BAND_CDMA_BC18: CDMA Band Class 18 (US 700MHz Public Safety).
 * @MM_MODEM_BAND_CDMA_BC19: CDMA Band Class 19 (US Lower 700MHz).
 * @MM_MODEM_BAND_ANY: For certain operations, allow the modem to select a band automatically.
 *
 * Radio bands supported by the device when connecting to a mobile network.
 */
typedef enum { /*< underscore_name=mm_modem_band >*/
    MM_MODEM_BAND_UNKNOWN = 0,
    /* GSM/UMTS bands */
    MM_MODEM_BAND_EGSM    = 1,
    MM_MODEM_BAND_DCS     = 2,
    MM_MODEM_BAND_PCS     = 3,
    MM_MODEM_BAND_G850    = 4,
    MM_MODEM_BAND_UTRAN_1 = 5,
    MM_MODEM_BAND_UTRAN_3 = 6,
    MM_MODEM_BAND_UTRAN_4 = 7,
    MM_MODEM_BAND_UTRAN_6 = 8,
    MM_MODEM_BAND_UTRAN_5 = 9,
    MM_MODEM_BAND_UTRAN_8 = 10,
    MM_MODEM_BAND_UTRAN_9 = 11,
    MM_MODEM_BAND_UTRAN_2 = 12,
    MM_MODEM_BAND_UTRAN_7 = 13,
    MM_MODEM_BAND_G450    = 14,
    MM_MODEM_BAND_G480    = 15,
    MM_MODEM_BAND_G750    = 16,
    MM_MODEM_BAND_G380    = 17,
    MM_MODEM_BAND_G410    = 18,
    MM_MODEM_BAND_G710    = 19,
    MM_MODEM_BAND_G810    = 20,
    /* LTE bands */
    MM_MODEM_BAND_EUTRAN_1  = 31,
    MM_MODEM_BAND_EUTRAN_2  = 32,
    MM_MODEM_BAND_EUTRAN_3  = 33,
    MM_MODEM_BAND_EUTRAN_4  = 34,
    MM_MODEM_BAND_EUTRAN_5  = 35,
    MM_MODEM_BAND_EUTRAN_6  = 36,
    MM_MODEM_BAND_EUTRAN_7  = 37,
    MM_MODEM_BAND_EUTRAN_8  = 38,
    MM_MODEM_BAND_EUTRAN_9  = 39,
    MM_MODEM_BAND_EUTRAN_10 = 40,
    MM_MODEM_BAND_EUTRAN_11 = 41,
    MM_MODEM_BAND_EUTRAN_12 = 42,
    MM_MODEM_BAND_EUTRAN_13 = 43,
    MM_MODEM_BAND_EUTRAN_14 = 44,
    MM_MODEM_BAND_EUTRAN_17 = 47,
    MM_MODEM_BAND_EUTRAN_18 = 48,
    MM_MODEM_BAND_EUTRAN_19 = 49,
    MM_MODEM_BAND_EUTRAN_20 = 50,
    MM_MODEM_BAND_EUTRAN_21 = 51,
    MM_MODEM_BAND_EUTRAN_22 = 52,
    MM_MODEM_BAND_EUTRAN_23 = 53,
    MM_MODEM_BAND_EUTRAN_24 = 54,
    MM_MODEM_BAND_EUTRAN_25 = 55,
    MM_MODEM_BAND_EUTRAN_26 = 56,
    MM_MODEM_BAND_EUTRAN_27 = 57,
    MM_MODEM_BAND_EUTRAN_28 = 58,
    MM_MODEM_BAND_EUTRAN_29 = 59,
    MM_MODEM_BAND_EUTRAN_30 = 60,
    MM_MODEM_BAND_EUTRAN_31 = 61,
    MM_MODEM_BAND_EUTRAN_32 = 62,
    MM_MODEM_BAND_EUTRAN_33 = 63,
    MM_MODEM_BAND_EUTRAN_34 = 64,
    MM_MODEM_BAND_EUTRAN_35 = 65,
    MM_MODEM_BAND_EUTRAN_36 = 66,
    MM_MODEM_BAND_EUTRAN_37 = 67,
    MM_MODEM_BAND_EUTRAN_38 = 68,
    MM_MODEM_BAND_EUTRAN_39 = 69,
    MM_MODEM_BAND_EUTRAN_40 = 70,
    MM_MODEM_BAND_EUTRAN_41 = 71,
    MM_MODEM_BAND_EUTRAN_42 = 72,
    MM_MODEM_BAND_EUTRAN_43 = 73,
    MM_MODEM_BAND_EUTRAN_44 = 74,
    MM_MODEM_BAND_EUTRAN_45 = 75,
    MM_MODEM_BAND_EUTRAN_46 = 76,
    MM_MODEM_BAND_EUTRAN_47 = 77,
    MM_MODEM_BAND_EUTRAN_48 = 78,
    MM_MODEM_BAND_EUTRAN_65 = 95,
    MM_MODEM_BAND_EUTRAN_66 = 96,
    MM_MODEM_BAND_EUTRAN_67 = 97,
    MM_MODEM_BAND_EUTRAN_68 = 98,
    MM_MODEM_BAND_EUTRAN_69 = 99,
    MM_MODEM_BAND_EUTRAN_70 = 100,
    MM_MODEM_BAND_EUTRAN_71 = 101,
    /* CDMA Band Classes (see 3GPP2 C.S0057-C) */
    MM_MODEM_BAND_CDMA_BC0  = 128,
    MM_MODEM_BAND_CDMA_BC1  = 129,
    MM_MODEM_BAND_CDMA_BC2  = 130,
    MM_MODEM_BAND_CDMA_BC3  = 131,
    MM_MODEM_BAND_CDMA_BC4  = 132,
    MM_MODEM_BAND_CDMA_BC5  = 134,
    MM_MODEM_BAND_CDMA_BC6  = 135,
    MM_MODEM_BAND_CDMA_BC7  = 136,
    MM_MODEM_BAND_CDMA_BC8  = 137,
    MM_MODEM_BAND_CDMA_BC9  = 138,
    MM_MODEM_BAND_CDMA_BC10 = 139,
    MM_MODEM_BAND_CDMA_BC11 = 140,
    MM_MODEM_BAND_CDMA_BC12 = 141,
    MM_MODEM_BAND_CDMA_BC13 = 142,
    MM_MODEM_BAND_CDMA_BC14 = 143,
    MM_MODEM_BAND_CDMA_BC15 = 144,
    MM_MODEM_BAND_CDMA_BC16 = 145,
    MM_MODEM_BAND_CDMA_BC17 = 146,
    MM_MODEM_BAND_CDMA_BC18 = 147,
    MM_MODEM_BAND_CDMA_BC19 = 148,
    /* Additional UMTS bands:
     *  15-18 reserved
     *  23-24 reserved
     *  27-31 reserved
     */
    MM_MODEM_BAND_UTRAN_10 = 210,
    MM_MODEM_BAND_UTRAN_11 = 211,
    MM_MODEM_BAND_UTRAN_12 = 212,
    MM_MODEM_BAND_UTRAN_13 = 213,
    MM_MODEM_BAND_UTRAN_14 = 214,
    MM_MODEM_BAND_UTRAN_19 = 219,
    MM_MODEM_BAND_UTRAN_20 = 220,
    MM_MODEM_BAND_UTRAN_21 = 221,
    MM_MODEM_BAND_UTRAN_22 = 222,
    MM_MODEM_BAND_UTRAN_25 = 225,
    MM_MODEM_BAND_UTRAN_26 = 226,
    MM_MODEM_BAND_UTRAN_32 = 232,
    /* All/Any */
    MM_MODEM_BAND_ANY = 256
} MMModemBand;

/**
 * MMModemPortType:
 * @MM_MODEM_PORT_TYPE_UNKNOWN: Unknown.
 * @MM_MODEM_PORT_TYPE_NET: Net port.
 * @MM_MODEM_PORT_TYPE_AT: AT port.
 * @MM_MODEM_PORT_TYPE_QCDM: QCDM port.
 * @MM_MODEM_PORT_TYPE_GPS: GPS port.
 * @MM_MODEM_PORT_TYPE_QMI: QMI port.
 * @MM_MODEM_PORT_TYPE_MBIM: MBIM port.
 *
 * Type of modem port.
 */
typedef enum { /*< underscore_name=mm_modem_port_type >*/
    MM_MODEM_PORT_TYPE_UNKNOWN = 1,
    MM_MODEM_PORT_TYPE_NET     = 2,
    MM_MODEM_PORT_TYPE_AT      = 3,
    MM_MODEM_PORT_TYPE_QCDM    = 4,
    MM_MODEM_PORT_TYPE_GPS     = 5,
    MM_MODEM_PORT_TYPE_QMI     = 6,
    MM_MODEM_PORT_TYPE_MBIM    = 7
} MMModemPortType;

/**
 * MMModemContactsStorage:
 * @MM_MODEM_CONTACTS_STORAGE_UNKNOWN: Unknown location.
 * @MM_MODEM_CONTACTS_STORAGE_ME: Device's local memory.
 * @MM_MODEM_CONTACTS_STORAGE_SM: Card inserted in the device (like a SIM/RUIM).
 * @MM_MODEM_CONTACTS_STORAGE_MT: Combined device/ME and SIM/SM phonebook.
 *
 * Specifies different storage locations for contact information.
 */
typedef enum { /*< underscore_name=mm_modem_contacts_storage >*/
    MM_MODEM_CONTACTS_STORAGE_UNKNOWN = 0,
    MM_MODEM_CONTACTS_STORAGE_ME      = 1,
    MM_MODEM_CONTACTS_STORAGE_SM      = 2,
    MM_MODEM_CONTACTS_STORAGE_MT      = 3,
} MMModemContactsStorage;

/**
 * MMBearerIpMethod:
 * @MM_BEARER_IP_METHOD_UNKNOWN: Unknown method.
 * @MM_BEARER_IP_METHOD_PPP: Use PPP to get IP addresses and DNS information.
 * For IPv6, use PPP to retrieve the 64-bit Interface Identifier, use the IID to
 * construct an IPv6 link-local address by following RFC 5072, and then run
 * DHCP over the PPP link to retrieve DNS settings.
 * @MM_BEARER_IP_METHOD_STATIC: Use the provided static IP configuration given
 * by the modem to configure the IP data interface.  Note that DNS servers may
 * not be provided by the network or modem firmware.
 * @MM_BEARER_IP_METHOD_DHCP: Begin DHCP or IPv6 SLAAC on the data interface to
 * obtain any necessary IP configuration details that are not already provided
 * by the IP configuration.  For IPv4 bearers DHCP should be used.  For IPv6
 * bearers SLAAC should be used, and the IP configuration may already contain
 * a link-local address that should be assigned to the interface before SLAAC
 * is started to obtain the rest of the configuration.
 *
 * Type of IP method configuration to be used in a given Bearer.
 */
typedef enum { /*< underscore_name=mm_bearer_ip_method >*/
    MM_BEARER_IP_METHOD_UNKNOWN = 0,
    MM_BEARER_IP_METHOD_PPP     = 1,
    MM_BEARER_IP_METHOD_STATIC  = 2,
    MM_BEARER_IP_METHOD_DHCP    = 3,
} MMBearerIpMethod;

/**
 * MMBearerIpFamily:
 * @MM_BEARER_IP_FAMILY_NONE: None or unknown.
 * @MM_BEARER_IP_FAMILY_IPV4: IPv4.
 * @MM_BEARER_IP_FAMILY_IPV6: IPv6.
 * @MM_BEARER_IP_FAMILY_IPV4V6: IPv4 and IPv6.
 * @MM_BEARER_IP_FAMILY_ANY: Mask specifying all IP families.
 *
 * Type of IP family to be used in a given Bearer.
 */
typedef enum { /*< underscore_name=mm_bearer_ip_family >*/
    MM_BEARER_IP_FAMILY_NONE    = 0,
    MM_BEARER_IP_FAMILY_IPV4    = 1 << 0,
    MM_BEARER_IP_FAMILY_IPV6    = 1 << 1,
    MM_BEARER_IP_FAMILY_IPV4V6  = 1 << 2,
    MM_BEARER_IP_FAMILY_ANY     = 0xFFFFFFFF
} MMBearerIpFamily;

/**
 * MMBearerAllowedAuth:
 * @MM_BEARER_ALLOWED_AUTH_UNKNOWN: Unknown.
 * @MM_BEARER_ALLOWED_AUTH_NONE: None.
 * @MM_BEARER_ALLOWED_AUTH_PAP: PAP.
 * @MM_BEARER_ALLOWED_AUTH_CHAP: CHAP.
 * @MM_BEARER_ALLOWED_AUTH_MSCHAP: MS-CHAP.
 * @MM_BEARER_ALLOWED_AUTH_MSCHAPV2: MS-CHAP v2.
 * @MM_BEARER_ALLOWED_AUTH_EAP: EAP.
 *
 * Allowed authentication methods when authenticating with the network.
 */
typedef enum { /*< underscore_name=mm_bearer_allowed_auth >*/
    MM_BEARER_ALLOWED_AUTH_UNKNOWN  = 0,
    /* bits 0..4 order match Ericsson device bitmap */
    MM_BEARER_ALLOWED_AUTH_NONE     = 1 << 0,
    MM_BEARER_ALLOWED_AUTH_PAP      = 1 << 1,
    MM_BEARER_ALLOWED_AUTH_CHAP     = 1 << 2,
    MM_BEARER_ALLOWED_AUTH_MSCHAP   = 1 << 3,
    MM_BEARER_ALLOWED_AUTH_MSCHAPV2 = 1 << 4,
    MM_BEARER_ALLOWED_AUTH_EAP      = 1 << 5,
} MMBearerAllowedAuth;

/**
 * MMModemCdmaRegistrationState:
 * @MM_MODEM_CDMA_REGISTRATION_STATE_UNKNOWN: Registration status is unknown or the device is not registered.
 * @MM_MODEM_CDMA_REGISTRATION_STATE_REGISTERED: Registered, but roaming status is unknown or cannot be provided by the device. The device may or may not be roaming.
 * @MM_MODEM_CDMA_REGISTRATION_STATE_HOME: Currently registered on the home network.
 * @MM_MODEM_CDMA_REGISTRATION_STATE_ROAMING: Currently registered on a roaming network.
 *
 * Registration state of a CDMA modem.
 */
typedef enum { /*< underscore_name=mm_modem_cdma_registration_state >*/
    MM_MODEM_CDMA_REGISTRATION_STATE_UNKNOWN    = 0,
    MM_MODEM_CDMA_REGISTRATION_STATE_REGISTERED = 1,
    MM_MODEM_CDMA_REGISTRATION_STATE_HOME       = 2,
    MM_MODEM_CDMA_REGISTRATION_STATE_ROAMING    = 3,
} MMModemCdmaRegistrationState;

/**
 * MMModemCdmaActivationState:
 * @MM_MODEM_CDMA_ACTIVATION_STATE_UNKNOWN: Unknown activation state.
 * @MM_MODEM_CDMA_ACTIVATION_STATE_NOT_ACTIVATED: Device is not activated
 * @MM_MODEM_CDMA_ACTIVATION_STATE_ACTIVATING: Device is activating
 * @MM_MODEM_CDMA_ACTIVATION_STATE_PARTIALLY_ACTIVATED: Device is partially activated; carrier-specific steps required to continue.
 * @MM_MODEM_CDMA_ACTIVATION_STATE_ACTIVATED: Device is ready for use.
 *
 * Activation state of a CDMA modem.
 */
typedef enum { /*< underscore_name=mm_modem_cdma_activation_state >*/
    MM_MODEM_CDMA_ACTIVATION_STATE_UNKNOWN             = 0,
    MM_MODEM_CDMA_ACTIVATION_STATE_NOT_ACTIVATED       = 1,
    MM_MODEM_CDMA_ACTIVATION_STATE_ACTIVATING          = 2,
    MM_MODEM_CDMA_ACTIVATION_STATE_PARTIALLY_ACTIVATED = 3,
    MM_MODEM_CDMA_ACTIVATION_STATE_ACTIVATED           = 4,
} MMModemCdmaActivationState;

/**
 * MMModemCdmaRmProtocol:
 * @MM_MODEM_CDMA_RM_PROTOCOL_UNKNOWN: Unknown protocol.
 * @MM_MODEM_CDMA_RM_PROTOCOL_ASYNC: Asynchronous data or fax.
 * @MM_MODEM_CDMA_RM_PROTOCOL_PACKET_RELAY: Packet data service, Relay Layer Rm interface.
 * @MM_MODEM_CDMA_RM_PROTOCOL_PACKET_NETWORK_PPP: Packet data service, Network Layer Rm interface, PPP.
 * @MM_MODEM_CDMA_RM_PROTOCOL_PACKET_NETWORK_SLIP: Packet data service, Network Layer Rm interface, SLIP.
 * @MM_MODEM_CDMA_RM_PROTOCOL_STU_III: STU-III service.
 *
 * Protocol of the Rm interface in modems with CDMA capabilities.
 */
typedef enum { /*< underscore_name=mm_modem_cdma_rm_protocol >*/
    MM_MODEM_CDMA_RM_PROTOCOL_UNKNOWN             = 0,
    MM_MODEM_CDMA_RM_PROTOCOL_ASYNC               = 1,
    MM_MODEM_CDMA_RM_PROTOCOL_PACKET_RELAY        = 2,
    MM_MODEM_CDMA_RM_PROTOCOL_PACKET_NETWORK_PPP  = 3,
    MM_MODEM_CDMA_RM_PROTOCOL_PACKET_NETWORK_SLIP = 4,
    MM_MODEM_CDMA_RM_PROTOCOL_STU_III             = 5,
} MMModemCdmaRmProtocol;

/**
 * MMModem3gppRegistrationState:
 * @MM_MODEM_3GPP_REGISTRATION_STATE_IDLE: Not registered, not searching for new operator to register.
 * @MM_MODEM_3GPP_REGISTRATION_STATE_HOME: Registered on home network.
 * @MM_MODEM_3GPP_REGISTRATION_STATE_SEARCHING: Not registered, searching for new operator to register with.
 * @MM_MODEM_3GPP_REGISTRATION_STATE_DENIED: Registration denied.
 * @MM_MODEM_3GPP_REGISTRATION_STATE_UNKNOWN: Unknown registration status.
 * @MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING: Registered on a roaming network.
 * @MM_MODEM_3GPP_REGISTRATION_STATE_HOME_SMS_ONLY: Registered for "SMS only", home network (applicable only when on LTE).
 * @MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING_SMS_ONLY: Registered for "SMS only", roaming network (applicable only when on LTE).
 * @MM_MODEM_3GPP_REGISTRATION_STATE_EMERGENCY_ONLY: Emergency services only.
 * @MM_MODEM_3GPP_REGISTRATION_STATE_HOME_CSFB_NOT_PREFERRED: Registered for "CSFB not preferred", home network (applicable only when on LTE).
 * @MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING_CSFB_NOT_PREFERRED: Registered for "CSFB not preferred", roaming network (applicable only when on LTE).
 *
 * GSM registration code as defined in 3GPP TS 27.007.
 */
typedef enum { /*< underscore_name=mm_modem_3gpp_registration_state >*/
    MM_MODEM_3GPP_REGISTRATION_STATE_IDLE                       = 0,
    MM_MODEM_3GPP_REGISTRATION_STATE_HOME                       = 1,
    MM_MODEM_3GPP_REGISTRATION_STATE_SEARCHING                  = 2,
    MM_MODEM_3GPP_REGISTRATION_STATE_DENIED                     = 3,
    MM_MODEM_3GPP_REGISTRATION_STATE_UNKNOWN                    = 4,
    MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING                    = 5,
    MM_MODEM_3GPP_REGISTRATION_STATE_HOME_SMS_ONLY              = 6,
    MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING_SMS_ONLY           = 7,
    MM_MODEM_3GPP_REGISTRATION_STATE_EMERGENCY_ONLY             = 8,
    MM_MODEM_3GPP_REGISTRATION_STATE_HOME_CSFB_NOT_PREFERRED    = 9,
    MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING_CSFB_NOT_PREFERRED = 10,
} MMModem3gppRegistrationState;

/**
 * MMModem3gppFacility:
 * @MM_MODEM_3GPP_FACILITY_NONE: No facility.
 * @MM_MODEM_3GPP_FACILITY_SIM: SIM lock.
 * @MM_MODEM_3GPP_FACILITY_FIXED_DIALING: Fixed dialing (PIN2) SIM lock.
 * @MM_MODEM_3GPP_FACILITY_PH_SIM: Device is locked to a specific SIM.
 * @MM_MODEM_3GPP_FACILITY_PH_FSIM: Device is locked to first SIM inserted.
 * @MM_MODEM_3GPP_FACILITY_NET_PERS: Network personalization.
 * @MM_MODEM_3GPP_FACILITY_NET_SUB_PERS: Network subset personalization.
 * @MM_MODEM_3GPP_FACILITY_PROVIDER_PERS: Service provider personalization.
 * @MM_MODEM_3GPP_FACILITY_CORP_PERS: Corporate personalization.
 *
 * A bitfield describing which facilities have a lock enabled, i.e.,
 * requires a pin or unlock code. The facilities include the
 * personalizations (device locks) described in 3GPP spec TS 22.022,
 * and the PIN and PIN2 locks, which are SIM locks.
 */
typedef enum { /*< underscore_name=mm_modem_3gpp_facility >*/
    MM_MODEM_3GPP_FACILITY_NONE          = 0,
    MM_MODEM_3GPP_FACILITY_SIM           = 1 << 0,
    MM_MODEM_3GPP_FACILITY_FIXED_DIALING = 1 << 1,
    MM_MODEM_3GPP_FACILITY_PH_SIM        = 1 << 2,
    MM_MODEM_3GPP_FACILITY_PH_FSIM       = 1 << 3,
    MM_MODEM_3GPP_FACILITY_NET_PERS      = 1 << 4,
    MM_MODEM_3GPP_FACILITY_NET_SUB_PERS  = 1 << 5,
    MM_MODEM_3GPP_FACILITY_PROVIDER_PERS = 1 << 6,
    MM_MODEM_3GPP_FACILITY_CORP_PERS     = 1 << 7,
} MMModem3gppFacility;

/**
 * MMModem3gppNetworkAvailability:
 * @MM_MODEM_3GPP_NETWORK_AVAILABILITY_UNKNOWN: Unknown availability.
 * @MM_MODEM_3GPP_NETWORK_AVAILABILITY_AVAILABLE: Network is available.
 * @MM_MODEM_3GPP_NETWORK_AVAILABILITY_CURRENT: Network is the current one.
 * @MM_MODEM_3GPP_NETWORK_AVAILABILITY_FORBIDDEN: Network is forbidden.
 *
 * Network availability status as defined in 3GPP TS 27.007 section 7.3
 */
typedef enum { /*< underscore_name=mm_modem_3gpp_network_availability >*/
    MM_MODEM_3GPP_NETWORK_AVAILABILITY_UNKNOWN   = 0,
    MM_MODEM_3GPP_NETWORK_AVAILABILITY_AVAILABLE = 1,
    MM_MODEM_3GPP_NETWORK_AVAILABILITY_CURRENT   = 2,
    MM_MODEM_3GPP_NETWORK_AVAILABILITY_FORBIDDEN = 3,
} MMModem3gppNetworkAvailability;

/**
 * MMModem3gppSubscriptionState:
 * @MM_MODEM_3GPP_SUBSCRIPTION_STATE_UNKNOWN: The subscription state is unknown.
 * @MM_MODEM_3GPP_SUBSCRIPTION_STATE_UNPROVISIONED: The account is unprovisioned.
 * @MM_MODEM_3GPP_SUBSCRIPTION_STATE_PROVISIONED: The account is provisioned and has data available.
 * @MM_MODEM_3GPP_SUBSCRIPTION_STATE_OUT_OF_DATA: The account is provisioned but there is no data left.
 *
 * Describes the current subscription status of the SIM.  This value is only available after the
 * modem attempts to register with the network.
 */
typedef enum { /*< underscore_name=mm_modem_3gpp_subscription_state >*/
    MM_MODEM_3GPP_SUBSCRIPTION_STATE_UNKNOWN       = 0,
    MM_MODEM_3GPP_SUBSCRIPTION_STATE_UNPROVISIONED = 1,
    MM_MODEM_3GPP_SUBSCRIPTION_STATE_PROVISIONED   = 2,
    MM_MODEM_3GPP_SUBSCRIPTION_STATE_OUT_OF_DATA   = 3,
} MMModem3gppSubscriptionState;

/**
 * MMModem3gppUssdSessionState:
 * @MM_MODEM_3GPP_USSD_SESSION_STATE_UNKNOWN: Unknown state.
 * @MM_MODEM_3GPP_USSD_SESSION_STATE_IDLE: No active session.
 * @MM_MODEM_3GPP_USSD_SESSION_STATE_ACTIVE: A session is active and the mobile is waiting for a response.
 * @MM_MODEM_3GPP_USSD_SESSION_STATE_USER_RESPONSE: The network is waiting for the client's response.
 *
 * State of a USSD session.
 */
typedef enum { /*< underscore_name=mm_modem_3gpp_ussd_session_state >*/
    MM_MODEM_3GPP_USSD_SESSION_STATE_UNKNOWN       = 0,
    MM_MODEM_3GPP_USSD_SESSION_STATE_IDLE          = 1,
    MM_MODEM_3GPP_USSD_SESSION_STATE_ACTIVE        = 2,
    MM_MODEM_3GPP_USSD_SESSION_STATE_USER_RESPONSE = 3,
} MMModem3gppUssdSessionState;

/**
 * MMFirmwareImageType:
 * @MM_FIRMWARE_IMAGE_TYPE_UNKNOWN: Unknown firmware type.
 * @MM_FIRMWARE_IMAGE_TYPE_GENERIC: Generic firmware image.
 * @MM_FIRMWARE_IMAGE_TYPE_GOBI: Firmware image of Gobi devices.
 *
 * Type of firmware image.
 */
typedef enum { /*< underscore_name=mm_firmware_image_type >*/
    MM_FIRMWARE_IMAGE_TYPE_UNKNOWN = 0,
    MM_FIRMWARE_IMAGE_TYPE_GENERIC = 1,
    MM_FIRMWARE_IMAGE_TYPE_GOBI    = 2,
} MMFirmwareImageType;

#endif /*  _MODEMMANAGER_ENUMS_H_ */

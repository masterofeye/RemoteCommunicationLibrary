#pragma once

#include <QObject>
#include "remotecommunicationlibrary_global.h"
namespace RW{
    namespace COM{
        REMOTECOMMUNICATIONLIBRARY_EXPORT Q_NAMESPACE

			enum class MessageDescription
			{
				EX_CanEasyStartApplication,
				EX_CanEasyLoadWorkspace,
				EX_CanEasyCloseApplication,
				EX_CanEasyStartSimulation,
				EX_CanEasyStopSimulation,
				EX_MKSLogin,
				EX_MKSStartDownload,
				EX_MKSCreateSandBox,
				EX_MKSDropSandbox,
				EX_MKSClose,
				EX_FHostSPStartFHost,
				EX_FHostSPLoadFlashFile,
				EX_FHostSPCloseFHost,
				EX_FHostSPGetState,
				EX_FHostSPAbortSequence,
				EX_FHostSPCloseSequence,
				EX_FHostSPStartFlash,
				EX_FHostSPGetProgress,
				EX_PortalInfoFittingAC,
				EX_PortalInfoAcByIcom,
				EX_PortalInfoReleases,
				EX_PortalInfoSoftwareById,
				EX_PortalInfoShowDialog,
				EX_PortalInfoCloseDialog,
				EX_FileUtilUnZip,
				EX_FileUtilDelete,
				EX_UsbHidLoaderFlashFile,
				EX_PrintDebugInformation,
				IN_ToggleCl30Fast,
				IN_ToggleCl30Slow,
				EX_StartInactivityTimer,
                EX_ProcessLogout,
				EX_StopInactivityTimer,
                EX_LogoutImmediately,
                IN_Notifier,
                EX_ShowPopUp,
                EX_ShowPopUpWithTimeout,
                IN_StartInactivityObserver,
                IN_StopInactivityObserver,
                IN_StopShutdownHandler,
                IN_StartShutdownHandler,
                IN_LogoutImmediately,
                IN_HARDWARE_START,
                IN_IO,
                IN_RELAY,
                IN_USB,
                IN_FHOST,
                IN_ADC,
                IN_DAC,
                IN_UART,
                IN_I2C,
                IN_POWERSTRIPE,
                IN_POWERSUPPLY,
                IN_HARDWARE_STOP,
                EX_WELCOME,
                IN_SESSIONSTARTSERVER,
                IN_SESSIONCONNECT,
                IN_SESSIONDISCONNECT,
                IN_SESSIONLOGON,
                IN_SESSIONLOGOFF,
                IN_SESSIONSHUTDOWN,
				Amount
			};
            Q_ENUM_NS(MessageDescription)

			enum class ErrorDecscription
			{
                Success,
                Error,
                Busy,
                ErrorCanEasyComServerMissing,
                ErrorCanEasyStartSimulation,
                ErrorCanEasySimulationIsRunningFailed,
                ErrorCanEasyStopSimulation,
                ErrorCanEasyWorkspaceNotFound,
                ErrorCanEasyWorkspaceNotLoaded,
                ErrorCanEasyApplication,
                ErrorCanEasyCoCreateInstance,
                ErrorCanEasyGetApplication,
                ErrorCanEasyGetAppWindow,
                ErrorCanEasyDeInit,
                ErrorCanEasyStop,
                ErrorMKSLogin,
                ErrorMKSLocationMissing,
                ErrorMKSError,
                ErrorMKSSandBoxCreation,
                ErrorMKSSanbBoxDrop,
                ErrorMKSCloseFailed,
                ErrorMKSMissingParameter,
                ErrorFHostSPStartApplication,
                ErrorFHostSPCoCreateInstance,
                ErrorFHostSPSequenceStop,
                ErrorFHostSPSequenceStart,
                ErrorFHostSPSequenceClose,
                ErrorFHostSPLoadFlashfileStatusFailed,
                ErrorFHostSPLoadFlashfileFailed,
                ErrorFHostSPFlashfileNotExits,
                ErrorFHostSPGetProgress,
                ErrorFHostSPCloseApplication,
                ErrorFHostSPStillAlive,
                ErrorFHostSPGetStateFailed,
                ErrorFHostSPAbortFailed,
                ErrorFHostSPStateFailed,
                ErrorPortalInfoFinalRegexCheck,
                ErrorPortalInfoProjectnameCount,
                ErrorPortalInfoProjectCount,
                ErrorPortalInfoSamplePhaseAndReleaseCount,
                ErrorPortalInfoSamplePhaseCount,
                ErrorPortalInfoPrepareReleaseInformation,
                ErrorPortalInfoReleaseCount,
                ErrorFileUtilMissingZipFile,
                ErrorFileUtilNoSevenZip,
                ErrorFileUtilFailed,
                ErrorFileUtilDirectoryDontExists,
                ErrorFileUtilDeleteFailed,
                ErrorFileUsbHidLoaderFileDontExists,
                ErrorFileUsbHidLoaderFailed,
                ErrorFileUsbHidLoaderHIDState,
                ErrorFileUsbHidLoaderTimeOut,
                ErrorFileUsbHidLoaderChecksum,
                ErrorFileUsbHidLiaderUnknownError,
                ErrorLogOutNotPossible,
                ErrorLogOutQuerySessionFailed,
			};

            Q_ENUM_NS(ErrorDecscription)

			enum class IdentifierFlag
			{
				RemoteService,
				RemoteApp,
				RemoteView
			};
            Q_ENUM_NS(IdentifierFlag)
    }

}
Q_DECLARE_METATYPE(RW::COM::MessageDescription);
Q_DECLARE_METATYPE(RW::COM::ErrorDecscription);
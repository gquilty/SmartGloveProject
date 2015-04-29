using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using vxlapi_NET;

namespace EuclidianDistanceClassifier
{
    public class XLInterface
    {
        private XLDriver driver;
        private XLClass.xl_driver_config driverConfig;

        private XLDefine.XL_HardwareType hwType = XLDefine.XL_HardwareType.XL_HWTYPE_CANCASEXL;
        private uint hwIndex = 0;
        private uint hwChannel = 0;
        private int portHandle = -1;
        private int eventHandle = -1;
        private UInt64 accessMask = 0;
        private UInt64 permissionMask = 0;
        private UInt64 txMask = 0;
        private int channelIndex = 0;
        private string appName = "Glove";

        // Message types
        public Dictionary<int, string> MessageTypes;
        private const int FULL_THROTTLE_ON = 1;
        private const int FULL_THROTTLE_OFF = 2;
        private const int FULL_BRAKE_ON = 3;
        private const int FULL_BRAKE_OFF = 4;

        public XLInterface()
        {
            MessageTypes = new Dictionary<int, string>();
            MessageTypes.Add(FULL_THROTTLE_ON, "Throttle On");
            MessageTypes.Add(FULL_THROTTLE_OFF, "Throttle Off");
            MessageTypes.Add(FULL_BRAKE_ON, "Brake On");
            MessageTypes.Add(FULL_BRAKE_OFF, "Brake Off");

            XLDefine.XL_Status status;
            driver = new XLDriver();
            driverConfig = new XLClass.xl_driver_config();
            driver.XL_GetDriverConfig(ref driverConfig);
            
            
            // If the application name cannot be found in VCANCONF...
            if ((driver.XL_GetApplConfig(appName, 0, ref hwType, ref hwIndex, ref hwChannel, XLDefine.XL_BusTypes.XL_BUS_TYPE_CAN) != XLDefine.XL_Status.XL_SUCCESS))
            {
                //...create the item with two CAN channels
                status = driver.XL_SetApplConfig(appName, 0, XLDefine.XL_HardwareType.XL_HWTYPE_CANCASEXL, 0, 0, XLDefine.XL_BusTypes.XL_BUS_TYPE_CAN);
                Console.WriteLine(status);
            }


            // Read setting of CAN1
            driver.XL_GetApplConfig(appName, 0, ref hwType, ref hwIndex, ref hwChannel, XLDefine.XL_BusTypes.XL_BUS_TYPE_CAN);

            // Notify user if no channel is assigned to this application 
            if (hwType == XLDefine.XL_HardwareType.XL_HWTYPE_NONE) PrintAssignError();

            accessMask = driver.XL_GetChannelMask(hwType, (int)hwIndex, (int)hwChannel);
            txMask = accessMask; // this channel is used for Tx
            PrintConfig();
            

            permissionMask = accessMask;

            // Open port
            status = driver.XL_OpenPort(ref portHandle, appName, accessMask, ref permissionMask, 1024, XLDefine.XL_InterfaceVersion.XL_INTERFACE_VERSION, XLDefine.XL_BusTypes.XL_BUS_TYPE_CAN);
            Console.WriteLine("\n\nOpen Port             : " + status);
            if (status != XLDefine.XL_Status.XL_SUCCESS) PrintFunctionError();

            // Check port
            status = driver.XL_CanRequestChipState(portHandle, accessMask);
            Console.WriteLine("Can Request Chip State: " + status);
            if (status != XLDefine.XL_Status.XL_SUCCESS) PrintFunctionError();

            // Activate channel
            status = driver.XL_ActivateChannel(portHandle, accessMask, XLDefine.XL_BusTypes.XL_BUS_TYPE_CAN, XLDefine.XL_AC_Flags.XL_ACTIVATE_NONE);
            Console.WriteLine("Activate Channel      : " + status);
            if (status != XLDefine.XL_Status.XL_SUCCESS) PrintFunctionError();

            // Reset time stamp clock
            status = driver.XL_ResetClock(portHandle);
            Console.WriteLine("Reset Clock           : " + status + "\n\n");
            if (status != XLDefine.XL_Status.XL_SUCCESS) PrintFunctionError();

            
        }



        public void SendMessage(int type)
        {


            XLDefine.XL_Status txStatus;
            
            XLClass.xl_event driverEvent = new XLClass.xl_event();

            if (type == FULL_THROTTLE_ON)
            {
                Console.WriteLine("Full Throttle On");
                // FULL THROTTLE ON
                driverEvent.tagData.can_Msg.id = 0x103;
                driverEvent.tagData.can_Msg.dlc = 2;
                driverEvent.tagData.can_Msg.data[0] = 19;
                driverEvent.tagData.can_Msg.data[1] = 136;
                driverEvent.tag = XLDefine.XL_EventTags.XL_TRANSMIT_MSG;
            }

            if (type == FULL_THROTTLE_OFF)
            {
                Console.WriteLine("Full Throttle Off");
                // FULL THROTTLE OFF
                driverEvent.tagData.can_Msg.id = 0x103;
                driverEvent.tagData.can_Msg.dlc = 2;
                driverEvent.tagData.can_Msg.data[0] = 0;
                driverEvent.tagData.can_Msg.data[1] = 0;
                driverEvent.tag = XLDefine.XL_EventTags.XL_TRANSMIT_MSG;
            }

            if (type == FULL_BRAKE_ON)
            {
                Console.WriteLine("Full Brake On");
                // FULL BRAKE ON
                driverEvent.tagData.can_Msg.id = 0x5;
                driverEvent.tagData.can_Msg.dlc = 8;
                driverEvent.tagData.can_Msg.data[0] = 0;
                driverEvent.tagData.can_Msg.data[1] = 100;
                driverEvent.tagData.can_Msg.data[2] = 0;
                driverEvent.tagData.can_Msg.data[3] = 0;
                driverEvent.tagData.can_Msg.data[4] = 0;
                driverEvent.tagData.can_Msg.data[5] = 0;
                driverEvent.tagData.can_Msg.data[6] = 0;
                driverEvent.tagData.can_Msg.data[7] = 0;
                driverEvent.tag = XLDefine.XL_EventTags.XL_TRANSMIT_MSG;
            }

            if (type == FULL_BRAKE_OFF)
            {
                Console.WriteLine("Full Brake Off");
                // FULL BRAKE OFF
                driverEvent.tagData.can_Msg.id = 0x100;
                driverEvent.tagData.can_Msg.dlc = 8;
                driverEvent.tagData.can_Msg.data[0] = 0;
                driverEvent.tagData.can_Msg.data[1] = 0;
                driverEvent.tagData.can_Msg.data[2] = 0;
                driverEvent.tagData.can_Msg.data[3] = 0;
                driverEvent.tagData.can_Msg.data[4] = 0;
                driverEvent.tagData.can_Msg.data[5] = 0;
                driverEvent.tagData.can_Msg.data[6] = 0;
                driverEvent.tagData.can_Msg.data[7] = 0;
                driverEvent.tag = XLDefine.XL_EventTags.XL_TRANSMIT_MSG;
            }



            // Transmit events
            txStatus = driver.XL_CanTransmit(portHandle, txMask, driverEvent);
            Console.WriteLine(txStatus);

        }



        private int PrintFunctionError()
        {
            Console.WriteLine("\nERROR: Function call failed!\nPress any key to close this application...");
            Console.ReadKey();
            return -1;
        }
        // -----------------------------------------------------------------------------------------------




        // -----------------------------------------------------------------------------------------------
        /// <summary>
        /// Displays the Vector Hardware Configuration.
        /// </summary>
        // -----------------------------------------------------------------------------------------------
        private void PrintConfig()
        {
            channelIndex = driver.XL_GetChannelIndex(hwType, (int)hwIndex, (int)hwChannel);

            if (channelIndex > -1)
            {
                Console.WriteLine("\n\nAPPLICATION CONFIGURATION");
                Console.WriteLine("-------------------------------------------------------------------\n");
                Console.WriteLine("Configured Hardware Channel : " + driverConfig.channel[channelIndex].name);
                Console.WriteLine("Hardware Driver Version     : " + driver.VersionToString(driverConfig.channel[channelIndex].driverVersion));
                Console.WriteLine("Used Transceiver            : " + driverConfig.channel[channelIndex].transceiverName);
                Console.WriteLine("Baudrate                    : " + driverConfig.channel[channelIndex].busParams.bitrate);
                Console.WriteLine("-------------------------------------------------------------------\n");
            }
        }
        // -----------------------------------------------------------------------------------------------




        // -----------------------------------------------------------------------------------------------
        /// <summary>
        /// Error message if channel assignment is not valid.
        /// </summary>
        // -----------------------------------------------------------------------------------------------
        private void PrintAssignError()
        {
            Console.WriteLine("\nPlease check application settings of \"" + appName + " CAN1/CAN2\" \nand assign it to an available hardware channel and restart application.");
            driver.XL_PopupHwConfig();
            Console.ReadKey();
        }
        // -----------------------------------------------------------------------------------------------



    } // class
}// namespace

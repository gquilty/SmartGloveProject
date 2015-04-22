using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using vxlapi_NET;

namespace EuclidianDistanceClassifier
{
    class XLInterface
    {
        private XLDriver xl;
        private XLClass.xl_driver_config driverConfig;

        private XLDefine.XL_HardwareType hwType = XLDefine.XL_HardwareType.XL_HWTYPE_NONE;
        private uint hwIndex = 0;
        private uint hwChannel = 0;
        private int portHandle = -1;
        private int eventHandle = -1;
        private UInt64 accessMask = 0;
        private UInt64 permissionMask = 0;
        private UInt64 txMask = 0;
        private int channelIndex = 0;

        public XLInterface()
        {
            xl = new XLDriver();
            driverConfig = new XLClass.xl_driver_config();
            //xl.XL_OpenDriver(ref driverConfig);
        }



        public void SendMessage()
        {

            XLDefine.XL_Status txStatus;
            
            // Create an event collection with 2 messages (events)
            XLClass.xl_event_collection xlEventCollection = new XLClass.xl_event_collection(2);

            // event 1
            xlEventCollection.xlEvent[0].tagData.can_Msg.id      = 0x100;
            xlEventCollection.xlEvent[0].tagData.can_Msg.dlc     = 8;
            xlEventCollection.xlEvent[0].tagData.can_Msg.data[0] = 1;
            xlEventCollection.xlEvent[0].tagData.can_Msg.data[1] = 2;
            xlEventCollection.xlEvent[0].tagData.can_Msg.data[2] = 3;
            xlEventCollection.xlEvent[0].tagData.can_Msg.data[3] = 4;
            xlEventCollection.xlEvent[0].tagData.can_Msg.data[4] = 5;
            xlEventCollection.xlEvent[0].tagData.can_Msg.data[5] = 6;
            xlEventCollection.xlEvent[0].tagData.can_Msg.data[6] = 7;
            xlEventCollection.xlEvent[0].tagData.can_Msg.data[7] = 8;
            xlEventCollection.xlEvent[0].tag = XLDefine.XL_EventTags.XL_TRANSMIT_MSG;

            // event 2
            xlEventCollection.xlEvent[1].tagData.can_Msg.id      = 0x200;
            xlEventCollection.xlEvent[1].tagData.can_Msg.dlc     = 8;
            xlEventCollection.xlEvent[1].tagData.can_Msg.data[0] = 9;
            xlEventCollection.xlEvent[1].tagData.can_Msg.data[1] = 10;
            xlEventCollection.xlEvent[1].tagData.can_Msg.data[2] = 11;
            xlEventCollection.xlEvent[1].tagData.can_Msg.data[3] = 12;
            xlEventCollection.xlEvent[1].tagData.can_Msg.data[4] = 13;
            xlEventCollection.xlEvent[1].tagData.can_Msg.data[5] = 14;
            xlEventCollection.xlEvent[1].tagData.can_Msg.data[6] = 15;
            xlEventCollection.xlEvent[1].tagData.can_Msg.data[7] = 16;
            xlEventCollection.xlEvent[1].tag = XLDefine.XL_EventTags.XL_TRANSMIT_MSG;


            // Transmit events
            txStatus = xl.XL_CanTransmit(portHandle, txMask, xlEventCollection);


        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Bluetooth;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Java.IO;
using Java.Util;

namespace BlueToothTest
{
    class BluetoothManager
    {
        //Unique ID which helps us connect to any device 
        private const string UuidUniverseProfile = "00001101-0000-1000-8000-00805f9b34fb";
        //Represent bluetooth data coming from UART
        private BluetoothDevice result;
        //get input/output stream of this comunication
        private BluetoothSocket mSocket;
        //convert byte[] to readable strings
        private BufferedReader reader;
        private BufferedWriter writer;//----------------------
        private System.IO.Stream mStream;
        private InputStreamReader mReader;

        private System.IO.Stream oStream;
        private OutputStreamWriter oWriter;//-------------------
        


        public BluetoothManager()
        {
            reader = null;
            writer = null;//---------------------------

        }



        public UUID getUUIDFromString()
        {
            return UUID.FromString(UuidUniverseProfile);
        }



        private void close(IDisposable aConnectableObject)
        {
            if (aConnectableObject == null) return;
            try
            {
                aConnectableObject.Dispose();
            }
            catch(Exception)
            {
                throw;
            }
            aConnectableObject = null;
        }


        public int getDataFromDevice()
        {
			//byte[] inPutBuffer = new byte[32];
			//mStream.Read(inPutBuffer, 0, 4);
			//return inPutBuffer[0];
			return mStream.ReadByte();
        }



		//public String getDataFromDevice()
		//{
			//return reader.ReadLine();
			
		//}


		public void sendDataToDevice(String data)//---------------
        {
			int intData = 0;
			int.TryParse(data,out intData);
			byte byteData = (byte)intData;
			
			//byte[] buffer = Encoding.ASCII.GetBytes(data);
		
			//oStream.Write(buffer,0,buffer.Length);//It works!!
			//oStream.Flush();
			
			oStream.WriteByte(byteData);

			
		}


        private void openDeviceConnection(BluetoothDevice btDevice)
        {
            try
            {
                //Getting socket from specific device
                mSocket = btDevice.CreateRfcommSocketToServiceRecord(getUUIDFromString());
                //blocking operation
                mSocket.Connect();
                //input stream
                mStream = mSocket.InputStream;
                //output stream
                //mSocket.OutputStream;
                oStream = mSocket.OutputStream;//----------------------------------

				
				

				oWriter = new OutputStreamWriter(oStream);//--------------------------
				
                //writer = new BufferedWriter(oWriter);//-------------------------------
				////BufferedOutputStream()
                mReader = new InputStreamReader(mStream);
                reader = new BufferedReader(mReader);
            }
            catch(IOException e)
            {
                //Close all
                close(mSocket);
                close(mStream);
                close(mReader);
                close(oStream);
                close(oWriter);//----------------------------------
                throw e;
            }
        }



        public void getAllPairedDevices()
        {
            //your android phone bluetooth device
            BluetoothAdapter btAdapter = BluetoothAdapter.DefaultAdapter;

            var devices = btAdapter.BondedDevices;
            if(devices != null && devices.Count > 0)
            {
                foreach (BluetoothDevice mDevice in devices)
                {
                    //search threw all paired devices
                    //mDevice.Name.Split(' ');
					
                    openDeviceConnection(mDevice);
                }
            }
        }
    }
}
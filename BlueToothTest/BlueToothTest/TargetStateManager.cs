using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace BlueToothTest
{
	public class TargetStateManager
	{
		
		public bool InGameMode { get; set; }
		enum StateEnum { waitForConnection, sendGameMode, gameMode};

		public int State { get; set; }

		public TargetStateManager()
		{
			State = (int)StateEnum.waitForConnection;
			InGameMode = false;
		}

		public void updateState(byte dataByte)
		{
			if (!InGameMode)
			{
				if ((dataByte & 0b00001111) == 15 && (int)StateEnum.waitForConnection == State)
				{
					State = (int)StateEnum.sendGameMode;
				}
				else if ((dataByte & 0b11111111) == 255 && (int)StateEnum.sendGameMode == State)
				{
					State = (int)StateEnum.gameMode;
					//InGameMode = true;
				}
			}
			
		}
	}
}